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
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Driver to control stepper motor
 *
 * \author Francesco Michelini <francesco.michelini@seacfi.com>
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Simone Zinanni <s.zinanni@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 */

#include "stepper.h"

#include "hw/hw_stepper.h"
#include "hw/hw_sensor.h"

#include "cfg/cfg_stepper.h"
#include <cfg/debug.h>

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL   STEPPER_LOG_LEVEL
#define LOG_FORMAT  STEPPER_LOG_FORMAT
#include <cfg/log.h>

#include <kern/proc.h>

#include <algo/ramp.h>

#include CPU_HEADER(stepper)

#include <string.h>  // memset

/**
 * \name Motor timings
 * \{
 */
#define MOTOR_SWITCH_TICKS      60000   ///< Timer ticks to wait for 10ms
#define MOTOR_SWITCH_COUNT          5   ///< Number of intervals, long 10ms, to wait before/after switching current off/on
#define MOTOR_HOME_MAX_STEPS    30000   ///< Steps before giving up when trying to reach home
#define MOTOR_CURRENT_TICKS      6000   ///< Number of intervals, long 10ms, to mantain high current
// \}

///< Stepper motors
static struct Stepper all_motors[CONFIG_NUM_STEPPER_MOTORS];

///< General FSM states (or NULL if state is not handled)
static fsm_state general_states[STEPPER_MAX_STATES];

// IRQ functions for stepper motors
static void stepper_interrupt(struct Stepper *motor);

static void stepper_accel(struct Stepper *motor);
static void stepper_decel(struct Stepper *motor);

static bool stepper_isState(struct Stepper *motor, enum StepperState state);
INLINE void stepper_changeState(struct Stepper *motor, enum StepperState newState);

static void stepper_enableCheckHome(struct Stepper *motor, bool bDirPositive);

#define MOTOR_INDEX(motor)     (motor->index)

//------------------------------------------------------------------------

INLINE bool setLowCurrent(struct Stepper* motor)
{
	if (motor->power == motor->cfg->powerIdle)
		return false;

	motor->power = motor->cfg->powerIdle;
	STEPPER_SET_POWER_CURRENT(MOTOR_INDEX(motor), motor->cfg->powerIdle);

	return true;
}

INLINE bool setHighCurrent(struct Stepper* motor)
{
	if (motor->power == motor->cfg->powerRun)
		return false;

	motor->power = motor->cfg->powerRun;
	STEPPER_SET_POWER_CURRENT(MOTOR_INDEX(motor), motor->cfg->powerRun);
	return true;
}

INLINE void setCheckSensor(struct Stepper* motor, enum MotorHomeSensorCheck value)
{
	motor->enableCheckHome = value;
}

INLINE int8_t getCheckSensor(struct Stepper* motor)
{
	return motor->enableCheckHome;
}

INLINE void setDirection(struct Stepper* motor, enum MotorDirection dir)
{
	ASSERT(dir == DIR_POSITIVE || dir == DIR_NEGATIVE);
	motor->dir = dir;

	if (!motor->cfg->flags.axisInverted)
	{
		STEPPER_SET_DIRECTION(MOTOR_INDEX(motor), (dir == DIR_POSITIVE));
	}
	else
	{
		STEPPER_SET_DIRECTION(MOTOR_INDEX(motor), (dir != DIR_POSITIVE));
	}
}

/**
 * Schedule a new stepper IRQ to happen after \a delay (number of clocks),
 * and optionally doing a step at the same time (if \a do_step is true).
 */
INLINE void FAST_FUNC stepper_schedule_irq(struct Stepper* motor, stepper_time_t delay, bool do_step)
{

	if (do_step)
	{
		// Record the step we just did
		motor->step += motor->dir;
		stepper_tc_doPulse(motor->timer);
	}
	else
		stepper_tc_skipPulse(motor->timer);

	stepper_tc_setDelay(motor->timer, delay);
}


static void stepper_accel(struct Stepper *motor)
{
	DB(uint16_t old_val = motor->rampValue;)
	DB(uint32_t old_clock = motor->rampClock;)

	const struct Ramp *ramp = &motor->cfg->ramp;

	ASSERT(motor->rampClock != 0);

	motor->rampValue = ramp_evaluate(ramp, motor->rampClock);
	motor->rampClock += motor->rampValue;
	motor->rampStep++;

	DB(if (old_val && motor->rampValue > old_val)
	{
		LOG_ERR("Runtime ramp error: (max=%x, min=%x)\n", ramp->clocksMaxWL, ramp->clocksMinWL);
		LOG_ERR("    %04x @ %lu   -->   %04x @ %lu\n", old_val, old_clock, motor->rampValue, motor->rampClock);
	})

}

static void stepper_decel(struct Stepper *motor)
{
	const struct Ramp *ramp = &motor->cfg->ramp;
	DB(uint16_t old_val = motor->rampValue;)

	motor->rampClock -= motor->rampValue;
	ASSERT(motor->rampClock != 0);
	motor->rampValue = ramp_evaluate(ramp, motor->rampClock);
	motor->rampStep--;
	DB(ASSERT(!old_val || motor->rampValue >= old_val););
}

INLINE void stepper_enable_irq(struct Stepper* motor)
{
	stepper_tc_irq_enable(motor->timer);
}

INLINE void stepper_disable_irq(struct Stepper* motor)
{
	stepper_tc_irq_disable(motor->timer);
}

// the home sensor can be in the standard home list or in the digital
// sensor list
bool stepper_readHome(struct Stepper* motor)
{
	return (motor->cfg->homeSensorIndex < NUM_HOME_SENSORS) ?
		hw_home_sensor_read(motor->cfg->homeSensorIndex) :
		bld_hw_sensor_read(motor->cfg->homeSensorIndex - NUM_HOME_SENSORS);
}

bool stepper_readLevel(struct Stepper* motor)
{
	return hw_level_sensor_read(motor->cfg->levelSensorIndex);
}

/************************************************************************/
/* Finite-state machine to drive stepper logic from IRQ                 */
/************************************************************************/

INLINE void stepper_changeState(struct Stepper* motor, enum StepperState newState)
{
	ASSERT(newState < STEPPER_MAX_STATES);

	motor->state = motor->cfg->states[newState];
	if (!motor->state)
		motor->state = general_states[newState];
	ASSERT(motor->state);
}

static bool stepper_isState(struct Stepper* motor, enum StepperState state)
{
	return (motor->cfg->states[state]
	        ? motor->cfg->states[state] == motor->state
	        : general_states[state] == motor->state);
}

static bool stepper_checkHomeErrors(struct Stepper* motor)
{
	bool home;

	home = stepper_readHome(motor);

	if (motor->enableCheckHome == MOTOR_HOMESENSOR_INCHECK && home
		&& (!motor->stepCircular || motor->step < motor->stepCircular / 2))
		/*
		* if home Sensor check enabled in movement to 0 position and
		* the motor is in home increase the counter
		* for rotating motor we include the check that the motor is
		* inside the last "lap" (FIXME: check it better)
		*/
		motor->stepsErrorHome++;
	else if (motor->enableCheckHome == MOTOR_HOMESENSOR_OUTCHECK && !home)
		/*
		* if home Sensor check enabled in movement from 0 position and
		* the motor is not in home increase the counter
		*/
		motor->stepsErrorHome++;
	else
		// clear error steps counter
		motor->stepsErrorHome = 0;

	// if this is the last consecutive position in which the motor is in/out home ...
	ASSERT(motor->stepsErrorHome <= MOTOR_CONSECUTIVE_ERROR_STEPS);
	if (motor->stepsErrorHome >= MOTOR_CONSECUTIVE_ERROR_STEPS)
	{
		// if the position at which the motor first saw/didn't see the home
		// is out of tolerance -> breakmotor -> ERROR
		if (motor->step > motor->stepsTollMax || motor->step < motor->stepsTollMin )
		{
			// break motor and error
			motor->speed = SPEED_STOPPED;
			motor->stepToReach = motor->step;

			stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
			motor->skipIrqs = MOTOR_SWITCH_COUNT;
			return false;
		}

		// the motor reached the home crossing -> disable error check
		setCheckSensor(motor, MOTOR_HOMESENSOR_NOCHECK);
	}

	return true;
}

static void stepper_checkLevelSensor(struct Stepper* motor)
{
	// level sensor check
	if (motor->step > motor->stepsDeaf)
	{
		if (stepper_readLevel(motor))
		{
			// record current position, disable check and stop motor
			motor->stepsDeaf = DEAFSTEPS_DEFAULT;
			motor->stepsLevel = motor->step;
                        //motor->stepToReach = motor->step + motor->rampStep * motor->dir;

			motor->stepToReach = motor->step;
			motor->rampClock = motor->cfg->ramp.clocksMaxWL;
			motor->rampValue = motor->cfg->ramp.clocksMaxWL;
		}
	}
}

static enum StepperState FAST_FUNC FSM_run(struct Stepper *motor)
{
	uint16_t distance;

	if (!stepper_checkHomeErrors(motor))
		return MSTS_ERROR;

	stepper_checkLevelSensor(motor);

	if ((motor->stepToReach != STEPS_INFINITE_POSITIVE) &&
	    (motor->stepToReach != STEPS_INFINITE_NEGATIVE ))
	{
		// Calculate (always positive) distance between current position and destination step
		distance = (uint16_t)((motor->stepToReach - motor->step) * motor->dir);
	}
	else
	{
		// We're at a very long distance ;-)
		distance = 0xFFFF;
		// if the motor is rotating and it has just ran a complete round
		// the position is set to 0
		if(motor->step == motor->stepCircular)
			motor->step = 0;
	}

	if (distance == 0)
		// Position reached - stop motor
		//motor->speed = SPEED_STOPPED;
		motor->rampStep = -1;
		//motor->rampClock = motor->ramp->clocksMaxWL;
		//motor->rampValue = 0;
		//motor->rampClock = motor->rampValue = motor->ramp->clocksMaxWL;

	else if (distance <= (uint16_t)motor->rampStep)
		stepper_decel(motor);

	// check whether the velocity must be changed
	else if (motor->speed < (uint16_t)motor->rampValue)
	{
		stepper_accel(motor);
		if (motor->speed > (uint16_t)motor->rampValue)
			motor->speed = (uint16_t)motor->rampValue;
	}
	else if (motor->speed > (uint16_t)motor->rampValue)
		stepper_decel(motor);

	// If rampStep == -1, leave output pin high and wait for low current
	if (motor->rampStep < 0)
	{
		// Wait before switching to low current
		motor->speed = SPEED_STOPPED;

		stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
 		motor->skipIrqs = MOTOR_SWITCH_COUNT;

		/*
		 * If there was a home sensor check activated, and the check has not
		 * been done yet, it means that we reached the end position without
		 * finding the home (or exiting from it). This is bad!
		 */
		if (motor->enableCheckHome != MOTOR_HOMESENSOR_NOCHECK)
			return MSTS_ERROR;

		// check if the motor has to stay in high current
		if(motor->cfg->flags.highcurrentBit)
		{
			motor->changeCurrentIrqs = MOTOR_CURRENT_TICKS;
			return MSTS_IDLE;
		}

		return MSTS_PREIDLE;
	}

	// Wait for high->low transition
	ASSERT(motor->rampValue > motor->cfg->pulse);
	stepper_schedule_irq(motor, motor->rampValue, true);

	return MSTS_RUN;
}

static enum StepperState FSM_idle(struct Stepper* motor)
{
	stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);

	if (motor->speed == SPEED_STOPPED)
	{
		// check if it's time to switch to low current
		if(motor->changeCurrentIrqs > 0)
		{
			if(--motor->changeCurrentIrqs == 0)
				setLowCurrent(motor);
		}
		return MSTS_IDLE;
	}

	// Switch to high current and wait for stabilization
	// (if the motor is in low current)
	if(motor->changeCurrentIrqs == 0)
	{
		setHighCurrent(motor);
		motor->skipIrqs = MOTOR_SWITCH_COUNT;
	}

	return MSTS_PRERUN;
}

static enum StepperState FSM_preidle(struct Stepper* motor)
{
	// Normal operation mode
	motor->changeCurrentIrqs = 0;
	setLowCurrent(motor);
	stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
	return MSTS_IDLE;
}

static enum StepperState FSM_error(struct Stepper* motor)
{
	// Error condition mode
	setLowCurrent(motor);
	stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
	return MSTS_ERROR;
}

static enum StepperState FSM_prerun(struct Stepper* motor)
{
	enum MotorDirection dir;

	// distance != 0?
	if ((motor->stepToReach != motor->step) ||
	    (motor->stepToReach == STEPS_INFINITE_POSITIVE) ||
	    (motor->stepToReach == STEPS_INFINITE_NEGATIVE)  )
	{
		// Setup for first step
		motor->rampStep = 0;

		// Setup Direction
		if(motor->stepToReach == STEPS_INFINITE_POSITIVE)
			dir = DIR_POSITIVE;
		else if(motor->stepToReach == STEPS_INFINITE_NEGATIVE)
			 dir = DIR_NEGATIVE;
		else if(motor->stepToReach > motor->step)
			dir = DIR_POSITIVE;
		else
			 dir = DIR_NEGATIVE;

		setDirection(motor, dir);

		// Enable of the home sensor control, if necessary
		// (before calling this function set the motor direction as above)
		stepper_enableCheckHome(motor, (dir == DIR_POSITIVE));

		// if the movement is infinite negative set the sw direction positive
		// (not the hw: see below) to count the steps
		if(motor->stepToReach == STEPS_INFINITE_NEGATIVE) motor->dir = DIR_POSITIVE;

		stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
		return MSTS_RUN;
	}
	else
	{
		/*
		 * If we are here we should do at least one step.
		 *  anyway ....
		 */
		stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
		motor->skipIrqs = MOTOR_SWITCH_COUNT;
		return MSTS_PREIDLE;
	}
}

static enum StepperState FSM_preinit(struct Stepper* motor)
{
	// Set current high, and wait for stabilization
	if (setHighCurrent(motor))
	{
		motor->skipIrqs = MOTOR_SWITCH_COUNT;
		return MSTS_PREINIT;
	}

	/*
	 * This state is used when initializing the motor, to bring back
	 * to the home. The idea is that we do not know where the motor
	 * is at this point, so there can be two possibilities:
	 *
	 * - The motor is already in home. We do not know how much into the
	 *   home we are. So we need to get out of the home (MSTS_LEAVING)
	 *   and then get back into it of the desired number of steps.
	 *
	 * - The motor is not in home: we need to look for it (MSTS_INIT).
	 *   We can safely assume that we will find the home in the negative
	 *   direction. For circular motors, any direction would do. For
	 *   other motors, the home is set at zero, so the current position
	 *   has to be a positive value.
	 *
	 */
	if (stepper_readHome(motor))
	{
		setDirection(motor, DIR_POSITIVE);
		stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
		return MSTS_LEAVING;
	}

	setDirection(motor, DIR_NEGATIVE);
	stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
	return MSTS_INIT;
}


static enum StepperState FSM_init(struct Stepper* motor)
{
	// If we are not in home, keep looking
	if (!stepper_readHome(motor))
	{
		stepper_schedule_irq(motor, motor->cfg->clocksHome, true);
		return MSTS_INIT;
	}

	/*
	 * Home! We still need to enter the home of the specified number of steps.
	 * That will be our absolute zero.
	 */

	motor->step = motor->cfg->stepsInHome - 1; // start counting down steps in home
	motor->stepToReach = 0;

	stepper_schedule_irq(motor, motor->cfg->clocksHome, true);
	return MSTS_ENTERING;
}

static enum StepperState FSM_entering(struct Stepper* motor)
{
	// We must be in home
	//ASSERT(stepper_readHome(motor));

	// if while entering the sensor we are no more in home we reset the steps
	// counter (optical sensor)
	if(!stepper_readHome(motor))
		motor->step = motor->cfg->stepsInHome - 1;

	// Current Position must be non-negative
	ASSERT(motor->step >= 0);

	if(motor->step == 0)
	{
		// reach the final target inside home sensor
		motor->step = 0;
		return MSTS_PREIDLE;
	}

	// keep doing steps
	stepper_schedule_irq(motor, motor->cfg->clocksHome, true);
	return MSTS_ENTERING;
}

static enum StepperState FSM_leaving(struct Stepper* motor)
{
	ASSERT(motor->dir == DIR_POSITIVE);

	motor->step = 0;
	if (!stepper_readHome(motor))
	{
		// we are out of home : change state and going far from sensor
		stepper_schedule_irq(motor, motor->cfg->clocksHome, true);
		return MSTS_OUTHOME;
	}
	else
	{
		// Still at home. Just wait here and keep doing steps
		stepper_schedule_irq(motor, motor->cfg->clocksHome, true);
		return MSTS_LEAVING;
	}
}

static enum StepperState FSM_outhome(struct Stepper* motor)
{
    ASSERT(motor->dir == DIR_POSITIVE);

    // We must be out of home: once we are no more in home
    // we just need to move away, even if not very precide (optical sensor)
    // ASSERT(!stepper_readHome(motor));

    if(motor->step >= motor->cfg->stepsOutHome)
    {
        // reach the final target outside home sensor
        motor->step = 0;

        // start home entering procedure (delay in executing step)
        setDirection(motor, DIR_NEGATIVE);
        stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
		motor->skipIrqs = MOTOR_SWITCH_COUNT;
        return MSTS_INIT;
    }

    // keep doing steps
    stepper_schedule_irq(motor, motor->cfg->clocksHome, true);
    return MSTS_OUTHOME;
}

static void FAST_FUNC stepper_interrupt(struct Stepper *motor)
{
	enum StepperState newState;

	// Check if we need to skip a certain number of IRQs
	if (motor->skipIrqs)
	{
		--motor->skipIrqs;
		stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
		return;
	}

	ASSERT(motor->state);
	newState = motor->state(motor);
	stepper_changeState(motor, newState);
}




/************************************************************************/
/* Public API                                                           */
/************************************************************************/

/**
 * Initialize the stepper module
 */
void stepper_init(void)
{
	STEPPER_INIT();

	// before starting the power all the stepper enable must be surely low
	stepper_disable();

	// Bind functions to general states
	memset(general_states, 0, sizeof(general_states));
	general_states[MSTS_IDLE] = FSM_idle;
	general_states[MSTS_PREIDLE] = FSM_preidle;
	general_states[MSTS_PRERUN] = FSM_prerun;
	general_states[MSTS_RUN] = FSM_run;
	general_states[MSTS_PREINIT] = FSM_preinit;
	general_states[MSTS_INIT] = FSM_init;
	general_states[MSTS_ENTERING] = FSM_entering;
	general_states[MSTS_LEAVING]= FSM_leaving;
	general_states[MSTS_OUTHOME]= FSM_outhome;
	general_states[MSTS_ERROR]= FSM_error;
}

void stepper_end(void)
{
	// Disable all stepper timer interrupt to stop motors
	for (int i = 0; i < CONFIG_NUM_STEPPER_MOTORS; i++)
		stepper_disable_irq(&all_motors[i]);
}

/**
 * Apply a setup config to motor structure context
 */
struct Stepper* stepper_setup(int index, struct StepperConfig *cfg)
{
	struct Stepper* motor;

	ASSERT(index < CONFIG_NUM_STEPPER_MOTORS);

	motor = &all_motors[index];
	motor->index = index;
	motor->cfg = cfg;

	//Register timer to stepper, and enable irq
	stepper_tc_setup(motor->index, &stepper_interrupt, motor);

	stepper_reset(motor);

	stepper_enable_irq(motor);

	return motor;
}

/**
 * Set the enable for all the motors to 0 before switching on the power
 */
void stepper_disable(void)
{
	STEPPER_DISABLE_ALL();
}

/**
 * Reset the motor
 */
void stepper_reset(struct Stepper *motor)
{
	/*
	 * To stop motor diable stepper irq.
	 */
	stepper_disable_irq(motor);

	//Disable a stepper motor
	STEPPER_DISABLE(MOTOR_INDEX(motor));

	// Setup context variables
	motor->power = 0;
	motor->step = 0;
	motor->rampStep = -1;
	// We cannot set the clock at zero at start because of a limit in the fixed point ramp
	motor->rampClock = motor->cfg->ramp.clocksMaxWL;
	motor->rampValue = motor->cfg->ramp.clocksMaxWL;
	motor->speed = SPEED_STOPPED;
	motor->stepToReach = 0;
	motor->skipIrqs = 0;
	motor->stepCircular = 0;
	setDirection(motor, DIR_POSITIVE);
	setLowCurrent(motor);

	motor->changeCurrentIrqs = 0;

	// default value (disable level sensor check)
	motor->stepsDeaf = DEAFSTEPS_DEFAULT;

	STEPPER_SET_HALF_STEP(MOTOR_INDEX(motor), motor->cfg->flags.halfStep);
	STEPPER_SET_CONTROL_BIT(MOTOR_INDEX(motor), motor->cfg->flags.controlBit);

	if (motor->cfg->homeSensorIndex < NUM_HOME_SENSORS)
		hw_home_sensor_set_inverted(motor->cfg->homeSensorIndex, motor->cfg->flags.homeInverted);

	if (motor->cfg->levelSensorIndex != MOTOR_NO_LEVEL_SENSOR)
		hw_level_sensor_set_inverted(motor->cfg->levelSensorIndex, motor->cfg->flags.levelInverted);

	stepper_changeState(motor, MSTS_IDLE);

	// Reset stepper timer counter
	stepper_tc_resetTimer(motor->timer);

	// reset hw to the stepper motor
	STEPPER_RESET(MOTOR_INDEX(motor));
	STEPPER_ENABLE(MOTOR_INDEX(motor));
}


void stepper_updateHalfStep(struct Stepper *motor)
{
	STEPPER_SET_HALF_STEP(MOTOR_INDEX(motor), motor->cfg->flags.halfStep);
}

void stepper_updateControlBit(struct Stepper *motor)
{
	STEPPER_SET_CONTROL_BIT(MOTOR_INDEX(motor), motor->cfg->flags.controlBit);
}

void stepper_updateControlMoveBit(struct Stepper *motor)
{
	STEPPER_SET_CONTROL_BIT(MOTOR_INDEX(motor), motor->cfg->flags.controlMoveBit);
}

/**
 * Find the home of a \a motor assuming no current knowledge about its position.
 *
 * This must be done when the motor is desynchronized with the firmware and
 * we do not know anymore where it is.
 *
 * In normal operation mode, to go back to the home, it is sufficient to use
 * move to step #0 with stepper_move, since the home is always at step #0.
 */
void stepper_home(struct Stepper *motor)
{

	// Begin home procedure
	stepper_disable_irq(motor);

	// disable home sensor check (default)
	setCheckSensor(motor, MOTOR_HOMESENSOR_NOCHECK);
	// deafult value (disable level sensor check)
	motor->stepsDeaf = DEAFSTEPS_DEFAULT;

	setDirection(motor, DIR_POSITIVE);
	stepper_schedule_irq(motor, MOTOR_SWITCH_TICKS, false);
	stepper_changeState(motor, MSTS_PREINIT);

	stepper_enable_irq(motor);
}


void stepper_setStep(struct Stepper *motor, int16_t step)
{
	motor->step = step;
}


int16_t stepper_getStep(struct Stepper *motor)
{
	return motor->step;
}

int16_t stepper_getLevelStep(struct Stepper *motor)
{
	return motor->stepsLevel;
}

void stepper_set_stepCircular(struct Stepper *motor, int16_t steps)
{
	motor->stepCircular = steps;
}

int16_t stepper_get_stepCircular(struct Stepper *motor)
{
	return motor->stepCircular;
}

int16_t stepper_scaleSteps(struct Stepper *motor, int16_t dir)
{
	int16_t steps;

	// scale the current position inside the motor lap
	if(!motor->stepCircular) return 0;

	// to be sure ....
	while(motor->step > motor->stepCircular) motor->step -= motor->stepCircular;

	if(dir == DIR_NEGATIVE)
	{
		steps = ((motor->stepCircular - motor->step) % motor->stepCircular);
		motor->step = steps;
	}
	/*
	else
		steps = (motor->step % motor->stepCircular);
	motor->step = steps;
	*/
	return motor->step;
}

static void stepper_enableCheckHome(struct Stepper *motor, bool bDirPositive)
{
	enum MotorHomeSensorCheck value = MOTOR_HOMESENSOR_NOCHECK; // default

	motor->stepsTollMin = 0;

    if((motor->stepToReach != STEPS_INFINITE_POSITIVE) &&
		(motor->stepToReach != STEPS_INFINITE_NEGATIVE)  )
	{
		if(bDirPositive) // else if(motor->dir == DIR_POSITIVE)
		{
			/* if the direction is positive (movement from 0 position),
			 * if the starting position is inside home and the target position
			 * is outside home -> the motor has to cross the home sensor -> enable the control
			 */
			if (motor->step < motor->cfg->stepsInHome - motor->cfg->stepsTollOutHome &&
				motor->stepToReach > motor->cfg->stepsInHome + motor->cfg->stepsTollOutHome)
			{
				value = MOTOR_HOMESENSOR_OUTCHECK;
				// home sensor out max position
				motor->stepsTollMax = motor->cfg->stepsInHome + motor->cfg->stepsTollOutHome + MOTOR_CONSECUTIVE_ERROR_STEPS;
				// home sensor in max position
				if(motor->cfg->stepsInHome + MOTOR_CONSECUTIVE_ERROR_STEPS > motor->cfg->stepsTollOutHome)
					motor->stepsTollMin = motor->cfg->stepsInHome + MOTOR_CONSECUTIVE_ERROR_STEPS - motor->cfg->stepsTollOutHome;
			}
		}
		else // if(motor->dir == DIR_NEGATIVE)
		{
			/*
			 * if the direction is negative (movement to 0 position),
			 * if the starting position is far from home and the target position
			 * is inside home -> the motor has to cross the home sensor -> enable the control
			 */
			if (motor->step > motor->cfg->stepsInHome + motor->cfg->stepsTollInHome &&
			    motor->stepToReach < motor->cfg->stepsInHome - motor->cfg->stepsTollInHome)
			{
				value = MOTOR_HOMESENSOR_INCHECK;
				// home sensor out max position
				motor->stepsTollMax = motor->cfg->stepsInHome + motor->cfg->stepsTollInHome - MOTOR_CONSECUTIVE_ERROR_STEPS;
				// home sensor in max position
				if(motor->cfg->stepsInHome > motor->cfg->stepsTollInHome + MOTOR_CONSECUTIVE_ERROR_STEPS)
					motor->stepsTollMin = motor->cfg->stepsInHome - (motor->cfg->stepsTollInHome + MOTOR_CONSECUTIVE_ERROR_STEPS);
			}
		}
	}
	setCheckSensor(motor, value);
}

/**
 * Move motor to absolute position at specified speed
 *
 * \arg steps position to reach in steps
 * \arg speed speed in timer ticks (use TIME2CLOCKS() to convert)
 */
int16_t stepper_move(struct Stepper *motor, int16_t steps, uint16_t speed, int16_t deafstep)
{
	// if the stepper already is in the desired position -> nothing to do
	if (motor->step == steps)
		return 0;

	stepper_disable_irq(motor);

	// final position
	motor->stepToReach = steps;

	// clear error steps
	motor->stepsErrorHome = 0;

	// position to start level check
	motor->stepsDeaf = deafstep;

	// clear level position
	motor->stepsLevel = 0;

	if (speed < motor->cfg->ramp.clocksMinWL)
	{
		ASSERT2(0, "speed too fast (small number)");
		speed = motor->cfg->ramp.clocksMinWL;
	}

	motor->rampClock = motor->cfg->ramp.clocksMaxWL;
	motor->rampValue = motor->cfg->ramp.clocksMaxWL;

	// TODO: find the exact value for motor->speed searching  in the ramp array.
	motor->speed = speed;

	stepper_enable_irq(motor);

	return 0;
}


/**
 * Stop motor gracefully
 */
void stepper_stop(struct Stepper *motor)
{
	/*
	 * The best way is to set the target of the movement to the minimum
	 * distance needed to decelerate. The logic in FSM_run will do the rest.
	 */
	if(stepper_idle(motor))
		return;

	stepper_disable_irq(motor);
	motor->stepToReach = motor->step + motor->rampStep * motor->dir;
	stepper_enable_irq(motor);
}


/**
 * Stop motor immediately, changing the status
 */
void stepper_break(struct Stepper *motor, enum StepperState state)
{
	// The best way to abort any operation is to go back to pre-idle mode
	stepper_disable_irq(motor);

	// Set of Speed disabled and Steps reached so that the function
	// stepper_idle() succeeds
	motor->speed = SPEED_STOPPED;
	motor->stepToReach = motor->step;
	stepper_changeState(motor, state);
	stepper_enable_irq(motor);
}

///< Returns true if the stepper is in idle at the final position or in error:
//  this means anyway that the motor is not moving
bool stepper_idle(struct Stepper *motor)
{
	return (stepper_isState(motor, MSTS_ERROR) ||
	        (stepper_isState(motor, MSTS_IDLE) && motor->step == motor->stepToReach) );
}

///< Returns true if the stepper is in error mode
bool stepper_error(struct Stepper *motor)
{
	return (stepper_isState(motor, MSTS_ERROR));
}

///< check the home sensor in zero position
bool stepper_inhome(struct Stepper *motor)
{
	return(stepper_getStep(motor) == 0 &&
	  	   !stepper_readHome(motor) );
}
