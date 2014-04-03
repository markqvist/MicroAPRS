/*!
 * \file
 * <!--
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Compute, save and load ramps for stepper motors (implementation)
 *
 *
 * \author Simone Zinanni <s.zinanni@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 *
 * The formula used by the ramp is the following:
 *
 * <pre>
 *            a * b
 * f(t) = -------------
 *         lerp(a,b,t)
 * </pre>
 *
 * Where <code>a</code> and <code>b</code> are the maximum and minimum speed
 * respectively (minimum and maximum wavelength respectively), and <code>lerp</code>
 * is a linear interpolation with a factor:
 *
 * <pre>
 * lerp(a,b,t) =  a + t * (b - a)  =  (a * (1 - t)) + (b * t)
 * </pre>
 *
 * <code>t</code> must be in the [0,1] interval. It is easy to see that the
 * following holds true:
 *
 * <pre>
 * f(0) = b,   f(1) = a
 * </pre>
 *
 * And that the function is monotonic. So, the function effectively interpolates
 * between the maximum and minimum speed through its domain ([0,1] -> [b,a]).
 *
 * The curve drawn by this function is similar to 1 / (sqrt(n)), so it is slower
 * than a linear acceleration (which would be 1/n).
 *
 * The floating point version uses a slightly modified function which accepts
 * the parameter in the domain [0, MT] (where MT is maxTime, the length of the
 * ramp, which is a setup parameter for the ramp). This is done to reduce the
 * number of operations per step. The formula looks like this:
 *
 * <pre>
 *               a * b * MT
 * g(t) = ----------------------------
 *           (a * MT) + t * (b - a)
 * </pre>
 *
 * It can be shown that this <code>g(t) = f(t * MT)</code>. The denominator
 * is a linear interpolation in the range [b*MT, a*MT], as t moves in the
 * interval [0, MT]. So the interpolation interval of the function is again
 * [b, a]. The implementation caches the value of the numerator and parts
 * of the denominator, so that the formula becomes:
 *
 * <pre>
 * alpha = a * b * MT
 * beta = a * MT
 * gamma = b - a
 *
 *                alpha
 * g(t) = ----------------------
 *           beta + t * gamma
 * </pre>
 *
 * and <code>t</code> is exactly the parameter that ramp_evaluate() gets,
 * that is the current time (in range [0, MT]). The operations performed
 * for each step are just an addition, a multiplication and a division.
 *
 * The fixed point version of the formula instead transforms the original
 * function as follows:
 *
 * <pre>
 *                   a * b                         a
 *  f(t) =  -------------------------  =  --------------------
 *                 a                         a
 *           b * ( - * (1 - t) + t )         - * (1 - t) + t
 *                 b                         b
 * </pre>
 *
 * <code>t</code> must be computed by dividing the current time (24 bit integer)
 * by the maximum time (24 bit integer). This is done by precomputing the
 * reciprocal of the maximum time as a 0.32 fixed point number, and multiplying
 * it to the current time. Multiplication is performed 8-bits a time by
 * FIX_MULT32(), so that we end up with a 0.16 fixed point number for
 * <code>t</code> (and <code>1-t</code> is just its twos-complement negation).
 * <code>a/b</code> is in the range [0,1] (because a is always less than b,
 * being the minimum wavelength), so it is precomputed as a 0.16 fixed point.
 * The final step is then computing the denominator and executing the division
 * (32 cycles using the 1-step division instruction in the DSP).
 *
 * The assembly implementation is needed for efficiency, but a C version of it
 * can be easily written, in case it is needed in the future.
 *
 */

#include "ramp.h"
#include <cfg/debug.h>

#include <string.h> // memcpy()

void ramp_compute(struct Ramp *ramp, uint32_t clocksRamp, uint16_t clocksMinWL, uint16_t clocksMaxWL)
{
	ASSERT(clocksMaxWL >= clocksMinWL);

	// Save values in ramp struct
	ramp->clocksRamp = clocksRamp;
	ramp->clocksMinWL = clocksMinWL;
	ramp->clocksMaxWL = clocksMaxWL;

#if RAMP_USE_FLOATING_POINT
	ramp->precalc.gamma = ramp->clocksMaxWL - ramp->clocksMinWL;
	ramp->precalc.beta = (float)ramp->clocksMinWL * (float)ramp->clocksRamp;
	ramp->precalc.alpha = ramp->precalc.beta * (float)ramp->clocksMaxWL;

#else
    ramp->precalc.max_div_min = ((uint32_t)clocksMinWL << 16) / (uint32_t)clocksMaxWL;

    /* Calcola 1/total_time in fixed point .32. Assumiamo che la rampa possa al
     * massimo avere 25 bit (cioé valore in tick fino a 2^25, che con il
     * prescaler=3 sono circa 7 secondi). Inoltre, togliamo qualche bit di precisione
     * da destra (secondo quanto specificato in RAMP_CLOCK_SHIFT_PRECISION).
     */
    ASSERT(ramp->clocksRamp < (1UL << (24 + RAMP_CLOCK_SHIFT_PRECISION)));
    ramp->precalc.inv_total_time = 0xFFFFFFFFUL / (ramp->clocksRamp >> RAMP_CLOCK_SHIFT_PRECISION);
    ASSERT(ramp->precalc.inv_total_time < 0x1000000UL);

#endif
}


void ramp_setup(struct Ramp* ramp, uint32_t length, uint32_t minFreq, uint32_t maxFreq)
{
	uint32_t minWL, maxWL;

	minWL = TIME2CLOCKS(FREQ2MICROS(maxFreq));
	maxWL = TIME2CLOCKS(FREQ2MICROS(minFreq));

	ASSERT2(minWL < 65536UL, "Maximum frequency too high");
	ASSERT2(maxWL < 65536UL, "Minimum frequency too high");
	ASSERT(maxFreq > minFreq);

	ramp_compute(
		ramp,
		TIME2CLOCKS(length),
		TIME2CLOCKS(FREQ2MICROS(maxFreq)),
		TIME2CLOCKS(FREQ2MICROS(minFreq))
	);
}

void ramp_default(struct Ramp *ramp)
{
	ramp_setup(ramp, RAMP_DEF_TIME, RAMP_DEF_MINFREQ, RAMP_DEF_MAXFREQ);
}

#if RAMP_USE_FLOATING_POINT

float ramp_evaluate(const struct Ramp* ramp, float curClock)
{
	return ramp->precalc.alpha / (curClock * ramp->precalc.gamma + ramp->precalc.beta);
}

#else

INLINE uint32_t fix_mult32(uint32_t m1, uint32_t m2)
{
	uint32_t accum = 0;
 	accum += m1 * ((m2 >> 0) & 0xFF);
 	accum >>= 8;
 	accum += m1 * ((m2 >> 8) & 0xFF);
 	accum >>= 8;
 	accum += m1 * ((m2 >> 16) & 0xFF);
 	return accum;
}

//   a*b >> 16
INLINE uint16_t fix_mult16(uint16_t a, uint32_t b)
{
	return (b*(uint32_t)a) >> 16;
}

uint16_t FAST_FUNC ramp_evaluate(const struct Ramp* ramp, uint32_t curClock)
{
	uint16_t t = FIX_MULT32(curClock >> RAMP_CLOCK_SHIFT_PRECISION, ramp->precalc.inv_total_time);
	uint16_t denom =  fix_mult16((uint16_t)~t + 1, ramp->precalc.max_div_min) + t;
	uint16_t cur_delta = ((uint32_t)ramp->clocksMinWL << 16) / denom;

	return cur_delta;
}

#endif


