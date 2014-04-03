/*!
 * \file
 * <!--
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Test for compute, save and load ramps for stepper motors (implementation)
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
#include <cfg/test.h>


static bool ramp_test_single(uint32_t minFreq, uint32_t maxFreq, uint32_t length)
{
	struct Ramp r;
	uint16_t cur, old;
	uint32_t clock;
	uint32_t oldclock;

	ramp_setup(&r, length, minFreq, maxFreq);

	cur = old = r.clocksMaxWL;
	clock = 0;
	oldclock = 0;

	kprintf("testing ramp: (length=%lu, min=%lu, max=%lu)\n", (unsigned long)length, (unsigned long)minFreq, (unsigned long)maxFreq);
	kprintf("              [length=%lu, max=%04x, min=%04x]\n", (unsigned long)r.clocksRamp, r.clocksMaxWL, r.clocksMinWL);

	int i = 0;
	int nonbyte = 0;

	while (clock + cur < r.clocksRamp)
	{
		oldclock = clock;
		old = cur;

		clock += cur;
		cur = ramp_evaluate(&r, clock);

		if (old < cur)
		{
			uint16_t t1 = FIX_MULT32(oldclock >> RAMP_CLOCK_SHIFT_PRECISION, r.precalc.inv_total_time);
			uint16_t t2 = FIX_MULT32(clock >> RAMP_CLOCK_SHIFT_PRECISION,    r.precalc.inv_total_time);
			uint16_t denom1 = FIX_MULT32((uint16_t)((~t1) + 1), r.precalc.max_div_min) + t1;
			uint16_t denom2 = FIX_MULT32((uint16_t)((~t2) + 1), r.precalc.max_div_min) + t2;

			kprintf("    Failed: %04x @ %lu   -->   %04x @ %lu\n", old, (unsigned long)oldclock, cur, (unsigned long)clock);
			kprintf("    T:     %04x -> %04x\n", t1, t2);
			kprintf("    DENOM: %04x -> %04x\n", denom1, denom2);

			cur = ramp_evaluate(&r, clock);
			return false;
		}
		i++;
		if ((old-cur) >= 256)
			nonbyte++;
	}



	kprintf("Test finished: %04x @ %lu [min=%04x, totlen=%lu, numsteps:%d, nonbyte:%d]\n", cur, (unsigned long)clock, r.clocksMinWL, (unsigned long)r.clocksRamp, i, nonbyte);

	return true;
}

int ramp_testSetup(void)
{
	kdbg_init();
	return 0;
}

int ramp_testTearDown(void)
{
	return 0;
}

int ramp_testRun(void)
{
	#define TEST_RAMP(min, max, len) do { \
		if (!ramp_test_single(min, max, len)) \
			return -1; \
	} while(0)

	TEST_RAMP(200,  5000, 3000000);
	TEST_RAMP(1000, 2000, 1000000);

	return 0;
}

TEST_MAIN(ramp);
