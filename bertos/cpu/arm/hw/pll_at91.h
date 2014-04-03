/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \author Giovanni Bajo <rasky@develer.com>
 *
 * \brief AT91SAM7S256 CRT.
 */

#include <io/arm.h>
#include <cfg/macros.h>

#if 1
// Versione completamente unrollata

#define CREATE_PLL_STEP(xtal, cpufreq)

#define PLL_STEP(xtal, cpufreq, d, k) \
    do { \
        uint32_t m = (((cpufreq * (1LL << k) * (d))  + (xtal / 2)) / xtal) - 1; \
        if (m < 2048) \
        { \
            uint32_t pll = (d != 0) ? (xtal * (m + 1)) / (d) : 0; \
            if (80000000 <= pll && pll <= 160000000) \
            { \
		uint32_t err = ABS((int32_t)((pll >> k) - cpufreq)); \
                if (err < best_err) \
                { \
                    best_err=err; best_m=m; best_k=k; best_d=d; \
                } \
            } \
        } \
    } while (0)

#else

// Versione con funzione nestata (GCC only)
// Tempo di compilazione più rapido

#define CREATE_PLL_STEP(xtal, cpufreq) \
    void __attribute__((always_inline)) PLL_STEP(int u1, int u2, int32_t d, long long k)  \
    { \
        (void)u1; (void)u2; \
        uint32_t m = ((((cpufreq * (1LL << k) * (d))  + (xtal / 2)) / xtal) - 1; \
        if (m < 2048) \
        { \
            uint32_t pll = (xtal * (m + 1)) / (d); \
            if (80000000UL <= pll && pll <= 160000000UL) \
            { \
                uint32_t err = ABS((int32_t)((pll >> k) - cpufreq)); \
                if (err < best_err) \
                { \
                    best_err=err; best_m=m; best_k=k; best_d=d; \
                } \
            } \
        } \
    }

#endif

#define PLL_MACRO_STEP(xtal, cpufreq, d) \
    do { \
        if ((d) > 0 && (d) <= 255 && \
            (d) <= xtal / 1000000 && (d) >= xtal / 32000000) \
        { \
            PLL_STEP(xtal, cpufreq, d, 0); \
            PLL_STEP(xtal, cpufreq, d, 1); \
            PLL_STEP(xtal, cpufreq, d, 2); \
            PLL_STEP(xtal, cpufreq, d, 3); \
            PLL_STEP(xtal, cpufreq, d, 4); \
            PLL_STEP(xtal, cpufreq, d, 5); \
            PLL_STEP(xtal, cpufreq, d, 6); \
        } \
    } while (0)

#define PLL_ITERATION_4(xtal, cpufreq, d) \
    PLL_MACRO_STEP(xtal, cpufreq, d)

#define PLL_ITERATION_3(xtal, cpufreq, d) \
    PLL_ITERATION_4(xtal, cpufreq, (d)*4+0); \
    PLL_ITERATION_4(xtal, cpufreq, (d)*4+1); \
    PLL_ITERATION_4(xtal, cpufreq, (d)*4+2); \
    PLL_ITERATION_4(xtal, cpufreq, (d)*4+3);

#define PLL_ITERATION_2(xtal, cpufreq, d) \
    PLL_ITERATION_3(xtal, cpufreq, (d)*4+0); \
    PLL_ITERATION_3(xtal, cpufreq, (d)*4+1); \
    PLL_ITERATION_3(xtal, cpufreq, (d)*4+2); \
    PLL_ITERATION_3(xtal, cpufreq, (d)*4+3);

#define PLL_ITERATION_1(xtal, cpufreq, d) \
    PLL_ITERATION_2(xtal, cpufreq, (d)*4+0); \
    PLL_ITERATION_2(xtal, cpufreq, (d)*4+1); \
    PLL_ITERATION_2(xtal, cpufreq, (d)*4+2); \
    PLL_ITERATION_2(xtal, cpufreq, (d)*4+3);

#define PLL_ITERATION(xtal, cpufreq) \
    PLL_ITERATION_1(xtal, cpufreq, 0); \
    PLL_ITERATION_1(xtal, cpufreq, 1); \
    PLL_ITERATION_1(xtal, cpufreq, 2); \
    PLL_ITERATION_1(xtal, cpufreq, 3)

#define PLL_CALC(xtal, cpufreq, m, d, k) do \
    { \
        uint32_t best_err=cpufreq, best_m, best_k, best_d; \
        CREATE_PLL_STEP(xtal, cpufreq) \
        PLL_ITERATION(xtal, cpufreq); \
        *(m)=best_m; *(d)=best_d; *(k)=best_k; \
    } while (0)

#if 0
int main(int argc, char *argv[])
{
    int32_t m, d, k;
    PLL_CALC(18432000, 48054857, &m, &d, &k);

    if (__builtin_constant_p(m) && __builtin_constant_p(k) && __builtin_constant_p(d))
        printf("SUCCESS -- compile time evaluation\n");
    else
        printf("FAILURE -- run time evaluation\n");

    printf("M:%d D:%d K:%d\n", m, d, k);
}
#endif
