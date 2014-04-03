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
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Stepper hardware-specific definitions
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#ifndef HW_STEPPER_H
#define HW_STEPPER_H

#include <hw/hw_cpufreq.h>

#include <cfg/macros.h>

#include <drv/timer.h>

#warning TODO:This is an example implentation, you must implement it!

#define STEPPER_STROBE_INIT \
do { \
		/* put init code for strobe */ \
} while (0)


#define STEPPER_STROBE_ON       do { /* Implement me! */ } while(0)
#define STEPPER_STROBE_OFF      do { /* Implement me! */ } while(0)

/**
 * CPU clock frequency is divided by 2^STEPPER_PRESCALER_LOG2 to
 * obtain stepper clock.
 */
#define STEPPER_PRESCALER_LOG2    1

/**
 * Stepper timer clock frequency.
 */
#define STEPPER_CLOCK ((CPU_FREQ) >> STEPPER_PRESCALER_LOG2)

/**
 * us delay to reset a stepper motor.
 * This is the time neccessary to reset
 * the stepper controll chip. (see datasheet for more detail).
 */
#define STEPPER_RESET_DELAY 1

/*
 * Pins define for each stepper
 */
#define STEPPER_1_CW_CCW_PIN             0
#define STEPPER_1_HALF_FULL_PIN          0
#define STEPPER_1_CONTROL_PIN            0
#define STEPPER_1_ENABLE_PIN             0
#define STEPPER_1_RESET_PIN              0

/* put here other stepper motor */

#define STEPPER_1_SET            do { /* Implement me! */ } while(0)
/* add here the set for other stepper motor */

#define STEPPER_1_CLEAR          do { /* Implement me! */ } while(0)
/* add here the clear for other stepper motor */

/*
 * Generic macro definition
 */

/*
 * Stepper init macro
 */
#define STEPPER_PIN_INIT_MACRO(port, index) do { \
		/* Add here init pin code */ \
	} while (0)

/*
 * Stepper commands macros
 */
#define STEPPER_SET_CW(index)             do { /* Implement me! */ } while (0)
#define STEPPER_SET_CCW(index)            do { /* Implement me! */ } while (0)
#define STEPPER_SET_HALF(index)           do { /* Implement me! */ } while (0)
#define STEPPER_SET_FULL(index)           do { /* Implement me! */ } while (0)
#define STEPPER_SET_CONTROL_LOW(index)    do { /* Implement me! */ } while (0)
#define STEPPER_SET_CONTROL_HIGHT(index)  do { /* Implement me! */ } while (0)
#define STEPPER_SET_ENABLE(index)         do { /* Implement me! */ } while (0)
#define STEPPER_SET_DISABLE(index)        do { /* Implement me! */ } while (0)
#define STEPPER_SET_RESET_ENABLE(index)   do { /* Implement me! */ } while (0)
#define STEPPER_SET_RESET_DISABLE(index)  do { /* Implement me! */ } while (0)


/*
 * Reset stepper macro
 */

#define STEPPER_RESET_MACRO(index) do { \
		STEPPER_SET_RESET_ENABLE(index); \
		timer_udelay(STEPPER_RESET_DELAY); \
		STEPPER_SET_RESET_DISABLE(index); \
	} while (0)

/*
 * Set half or full step macro
 */
#define STEPPER_SET_STEP_MODE_MACRO(index, flag) do { \
		if (flag) \
			STEPPER_SET_HALF(index); \
		else \
			STEPPER_SET_FULL(index); \
	} while (0)

/*
 * Set control status macro
 */
#warning TODO: This macro is not implemented (see below)

#define STEPPER_SET_CONTROL_BIT_MACRO(index, flag) do { \
		/* if (flag) */ \
			/* WARNING This macros not implemented */ \
		/* else */ \
			/* WARNING This macros not implemented */ \
	} while (0)

/*
 * Set current power macro
 */
#warning TODO: This macro is not implemented (see below)

#define STEPPER_SET_POWER_CURRENT_MACRO(index, flag) do { \
		/* if (flag) */ \
			/* WARNING This macrois not implemented */ \
		/* else */ \
			/* WARNING This macrois not implemented */ \
	} while (0)

/*
 * Set rotation of stepper motor
 * - dir = 1: positive rotation
 * - dir = 0: no motor moviment
 * - dir = -1: negative rotation
 *
 */
#define STEPPER_SET_DIRECTION_MACRO(index, dir) do { \
		switch (dir) \
		{ \
		case 1: \
			STEPPER_SET_CW(index); \
			break; \
		case -1: \
			STEPPER_SET_CCW(index); \
			break; \
		case 0: \
			break; \
		} \
	} while (0)


/*
 * Define macros for manage low level of stepper.
 */

#define STEPPER_INIT()  do { \
		STEPPER_PIN_INIT_MACRO(A, 1); \
		/* Add here code for other stepper motor */ \
	} while (0)


/*
 * Enable select stepper motor
 */
#define STEPPER_ENABLE(index) do { \
		switch (index) \
		{ \
		case 1: \
			STEPPER_SET_ENABLE(1); \
			break; \
			/* Add here code for other stepper motor */ \
		} \
	} while (0)

/*
 * Enable all stepper connect to micro
 */
#define STEPPER_ENABLE_ALL() do { \
		STEPPER_SET_ENABLE(1); \
		/* Add here code for other stepper motor */ \
	} while (0)

/*
 * Disable select stepper motor
 */
#define STEPPER_DISABLE(index) do { \
		switch (index) \
		{ \
		case 1: \
			STEPPER_SET_DISABLE(1); \
			break; \
			/* Add here code for other stepper motor */ \
		} \
	} while (0)

/*
 * Disable all stepper connect to micro
 */
#define STEPPER_DISABLE_ALL() do { \
		STEPPER_SET_DISABLE(1); \
		/* Add here code for other stepper motor */ \
	} while (0)

/*
 * Reset selected stepper motor
 */
#define STEPPER_RESET(index) do { \
		switch (index) \
		{ \
		case 1: \
			STEPPER_RESET_MACRO(1); \
			break; \
			/* Add here code for other stepper motor */ \
		} \
	} while (0)

/*
 * Reset all stepper motor
 */
#define STEPPER_RESET_ALL() do { \
		STEPPER_RESET_MACRO(1) \
		/* Add here code for other stepper motor */ \
	} while (0)

// Set half/full step macros
#define STEPPER_SET_HALF_STEP(index, flag) do { \
		switch (index) \
		{ \
		case 1: \
			STEPPER_SET_STEP_MODE_MACRO(1, flag); \
			break; \
			/* Add here code for other stepper motor */ \
		} \
	} while (0)


// Control status
#define STEPPER_SET_CONTROL_BIT(index, flag) do { \
		switch (index) \
		{ \
		case 1: \
			STEPPER_SET_CONTROL_BIT_MACRO(1, flag); \
			break; \
			/* Add here code for other stepper motor */ \
		} \
	} while (0)


// Set stepper power current
#define STEPPER_SET_POWER_CURRENT(index, flag) do { \
		switch (index) \
		{ \
		case 1: \
			STEPPER_SET_POWER_CURRENT_MACRO(1, flag); \
			break; \
			/* Add here code for other stepper motor */ \
		} \
	} while (0)

// Set rotation dirction of stepper motor
#define STEPPER_SET_DIRECTION(index, dir) do { \
		switch (index) \
		{ \
		case 1: \
			STEPPER_SET_DIRECTION_MACRO(1, dir); \
			break; \
			/* Add here code for other stepper motor */ \
		} \
	} while (0)

#endif /* HW_STEPPER_H */


