/**
 * \file
 * <!--
 * Copyright 2004, 2008 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Driver to control stepper motor
 *
 *
 * \author Francesco Michelini <francesco.michelini@seacfi.com>
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Simone Zinanni <s.zinanni@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "stepper"
 * $WIZ$ module_depends = "timer", "ramp"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_stepper.h"
 * $WIZ$ module_hw = "bertos/hw/hw_stepper.h"
 * $WIZ$ module_supports = "not avr"
 */

#ifndef DRV_STEPPER_H
#define DRV_STEPPER_H

#include <cfg/compiler.h>

#include <algo/ramp.h>

// Forward declaration
struct Stepper;

///< Special value for steps to move the motor continuously
#define STEPS_INFINITE_POSITIVE        ((int16_t)0xFFFF)
#define STEPS_INFINITE_NEGATIVE        ((int16_t)0x8FFF)

///< Maximum value for stepper steps
#define MAX_STEPS                      0x7FFF

///< Default value -> no level sensor associated to the motor
#define MOTOR_NO_LEVEL_SENSOR          0xFFFF

///< Default value -> no home sensor associated to the motor
#define MOTOR_NO_HOME_SENSOR           0xFFFF

///< Default value for deafsteps in normal movement (no level sensor)
#define DEAFSTEPS_DEFAULT              MAX_STEPS

///< Out-of-band values for speed
//\{
#define SPEED_STOPPED                  0xFFFF    ///< motor is stopped
#define SPEED_HOMING                   0xFFFE    ///< motor is homing
//\}

// default values for steps inside and outside home sensor
#define MOTOR_INSIDE_HOME_STEPS        10
#define MOTOR_OUTSIDE_HOME_STEPS       40

// default value for home sensor tolerance
#define MOTOR_TOLERANCE_HOME_STEPS      2

// default value for consecutive error
#define MOTOR_CONSECUTIVE_ERROR_STEPS   3

// values for the home control enabling
enum MotorHomeSensorCheck
{
	MOTOR_HOMESENSOR_NOCHECK = 0,
	MOTOR_HOMESENSOR_INCHECK,
	MOTOR_HOMESENSOR_OUTCHECK
};

// default value in ms for home procedure timeout
#define MOTOR_TIMEOUT_HOME              20000

/**
 * Motor direction
 */
enum MotorDirection
{
	DIR_POSITIVE = 1,       ///< moving away from zero (which is the home)
	DIR_NONE = 0,           ///< no movement
	DIR_NEGATIVE = -1       ///< moving towards towards zero (which is the home)
};

#define STEPPER_MAX_STATES    32


/**
 * Stepper state-machine conditions
 */
enum StepperState
{
	MSTS_UNINIT,        ///< stepper_init() not yet called
	MSTS_RUN,           ///< running
	MSTS_IDLE,          ///< waiting for a command
	MSTS_PREIDLE,       ///< waiting before going low-current
	MSTS_PRERUN,        ///< waiting after high-current

// Home procedure
	MSTS_PREINIT,       ///< preparing to initialize ;-)
	MSTS_INIT,          ///< initializing home procedure
	MSTS_ENTERING,      ///< entering home sensor
	MSTS_LEAVING,       ///< moving away from home (inside the sensor)
	MSTS_OUTHOME,       ///< moving away from home (outside the sensor)

	MSTS_ERROR,         ///< error status

	///< Dummy entry to guarantee the right underlying size for the enum
	MSTS_DUMMY_ALIGN = STEPPER_MAX_STATES - 1
};

///< Pointer to a function handling a state of the FSM driving the motor
typedef enum StepperState (*fsm_state)(struct Stepper* );

///< Pointer to a isr stepper function
typedef void (*stepper_isr_t)(struct Stepper* );

///< Time for steppers motor
typedef uint16_t stepper_time_t;

/**
 * Stepper configuration
 */
struct StepperConfig
{
	struct Ramp ramp;             ///< Acceleration ramp
	uint16_t pulse;               ///< (clocks) Length of the clock pulse used to drive the motor

	fsm_state states[STEPPER_MAX_STATES];  ///< Custom FSM states (or NULL for default handling)

	int16_t stepsInHome;          ///< Additional steps to do after home detection
    int16_t stepsOutHome;         ///< Additional steps to do leaving sensor in home procedure
	uint16_t clocksHome;          ///< Clock ticks for steps done when searching home

    int16_t stepsTollOutHome;     ///< tolerance steps leaving home sensor control while moving
    int16_t stepsTollInHome;      ///< tolerance steps leaving home sensor control while moving

    int16_t timeoutHome;          ///< timeout in ms in home procedure

	uint8_t powerRun;             ///< Vref voltage when motor runs (0-255)
	uint8_t powerIdle;            ///< Vref voltage when motor is idle (0-255)

	uint16_t homeSensorIndex;     ///< Home Sensor index in the sensor list
	uint16_t levelSensorIndex;    ///< Level Sensor index in the sensor list

	struct
	{
		bool homeInverted : 1;    ///< True for inverted home sensor
		bool halfStep : 1;        ///< True for half-step mode
		bool axisInverted : 1;    ///< True if the CW/CCW are inverted from default
		bool levelInverted : 1;   ///< True for inverted level sensor
		bool controlBit : 1;      ///< Control bit status
		bool controlMoveBit : 1;  ///< Control bit status in movement
		bool highcurrentBit : 1;  ///< Mantain high current bit status
	} flags;
};


/**
 * Motor context structure
 */
struct Stepper
{
	const struct StepperConfig *cfg; ///< Configuration of this stepper
	fsm_state state;                 ///< Motor FSM state function

	struct TimerCounter *timer;   ///< HW timer bound to this motor
	uint16_t index;               ///< Index of the motor

	volatile int16_t step;        ///< Steps counter (used in interrupt)
	volatile int16_t rampStep;    ///< Current position in acceleration ramp (used in intrrupt)
#if RAMP_USE_FLOATING_POINT
	float rampValue;              ///< Nr of Ticks for current step in ramp
	float rampClock;              ///< Cumulative nr of ticks for current step in ramp
#else
	uint16_t rampValue;
	uint32_t rampClock;
#endif

	enum MotorDirection dir;      ///< Current direction
	uint8_t power;                ///< Current power

	uint16_t speed;               ///< Timer compare value to reach
	int16_t stepToReach;          ///< Final position to reach when running

	int16_t skipIrqs;            ///< Counter used to skip IRQs (delay state changes)
	int16_t changeCurrentIrqs;   ///< Counter used to change current level (delay state changes)

    int8_t  enableCheckHome;      ///< enable the home sensor control during movement
    int8_t  stepsErrorHome;       ///< number of consecutive steps in error
    int16_t stepsTollMax;         ///< home sensor out max position
    int16_t stepsTollMin;         ///< home sensor in max position

	int16_t stepsDeaf;            ///< Position after which start the level check
	int16_t stepsLevel;           ///< Position of level contact

	int16_t stepCircular;         ///< Steps corresponding to 360 degrees (rotating motor)
};


void stepper_init(void);
void stepper_end(void);
struct Stepper *stepper_setup(int index, struct StepperConfig *cfg);
void stepper_disable(void);
void stepper_reset(struct Stepper *motor);
void stepper_home(struct Stepper *motor);
void stepper_setStep(struct Stepper *motor, int16_t step);
int16_t stepper_getStep(struct Stepper *motor);
int16_t stepper_move(struct Stepper *motor, int16_t step, uint16_t speed, int16_t deafstep);
void stepper_stop(struct Stepper *motor);
void stepper_break(struct Stepper *motor, enum StepperState state);
bool stepper_idle(struct Stepper *motor);
bool stepper_readHome(struct Stepper *motor);
bool stepper_readLevel(struct Stepper *motor);
void stepper_updateHalfStep(struct Stepper *motor);
void stepper_updateControlBit(struct Stepper *motor);
void stepper_updateControlMoveBit(struct Stepper *motor);
bool stepper_error(struct Stepper *motor);
bool stepper_inhome(struct Stepper *motor);
int16_t stepper_getLevelStep(struct Stepper *motor);
void stepper_set_stepCircular(struct Stepper *motor, int16_t steps);
int16_t stepper_get_stepCircular(struct Stepper *motor);
int16_t	stepper_scaleSteps(struct Stepper *motor, int16_t dir);

#endif /* DRV_STEPPER_H */
