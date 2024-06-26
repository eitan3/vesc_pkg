/*
    Copyright 2019 - 2022 Mitch Lustig
	Copyright 2022 Benjamin Vedder	benjamin@vedder.se

	This file is part of the VESC firmware.

	The VESC firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vesc_c_if.h"

#include "conf/datatypes.h"
#include "conf/confparser.h"
#include "conf/confxml.h"
#include "conf/buffer.h"

#include "balance_filter.h"

#include <math.h>
#include <string.h>

// Acceleration average
#define ACCEL_ARRAY_SIZE 40

HEADER

// Return the sign of the argument. -1.0 if negative, 1.0 if zero or positive.
#define SIGN(x)				(((x) < 0.0) ? -1.0 : 1.0)

#define DEG2RAD_f(deg)		((deg) * (float)(M_PI / 180.0))
#define RAD2DEG_f(rad) 		((rad) * (float)(180.0 / M_PI))

// Data type (Values 5 and 10 were removed, and can be reused at a later date, but i wanted to preserve the current value's numbers for UIs)
typedef enum {
	STARTUP = 0,
	RUNNING = 1,
	RUNNING_TILTBACK_DUTY = 2,
	RUNNING_TILTBACK_HIGH_VOLTAGE = 3,
	RUNNING_TILTBACK_LOW_VOLTAGE = 4,
	RUNNING_TILTBACK_TEMP = 5,
	FAULT_ANGLE_PITCH = 6,
	FAULT_ANGLE_ROLL = 7,
	FAULT_SWITCH_HALF = 8,
	FAULT_SWITCH_FULL = 9,
	FAULT_STARTUP = 11,
	FAULT_REVERSE = 12,
	FAULT_QUICKSTOP = 13,
	RUNNING_WHEELSLIP = 14
} BalanceState;

typedef enum {
	CENTERING = 0,
	REVERSESTOP,
	TILTBACK_DUTY,
	TILTBACK_HV,
	TILTBACK_LV,
	TILTBACK_TEMP,
	TILTBACK_NONE
} SetpointAdjustmentType;

typedef enum {
	OFF = 0,
	HALF,
	ON
} SwitchState;

typedef struct{
	float a0, a1, a2, b1, b2;
	float z1, z2;
} Biquad;

typedef enum {
	BQ_LOWPASS,
	BQ_HIGHPASS
} BiquadType;

// This is all persistent state of the application, which will be allocated in init. It
// is put here because variables can only be read-only when this program is loaded
// in flash without virtual memory in RAM (as all RAM already is dedicated to the
// main firmware and managed from there). This is probably the main limitation of
// loading applications in runtime, but it is not too bad to work around.
typedef struct {
	lib_thread thread; // Balance Thread

	balance_config balance_conf;

	// Config values
	float loop_time_seconds;
	float startup_step_size;
	float tiltback_duty_step_size, tiltback_hv_step_size, tiltback_lv_step_size, tiltback_return_step_size;
	float torquetilt_on_step_size, torquetilt_off_step_size;
	float tiltback_variable, tiltback_variable_max_erpm, noseangling_step_size;
	float tunea_transition_step_size, tuneb_transition_step_size, tunec_transition_step_size;

	// Runtime values read from elsewhere
	float pitch_angle, last_pitch_angle, roll_angle, abs_roll_angle, abs_roll_angle_sin, last_gyro_y;
    float true_pitch_angle;
	float gyro[3];
	float duty_cycle, abs_duty_cycle;
	float erpm, abs_erpm, last_erpm, smooth_erpm;
	Biquad smooth_erpm_biquad;
	float motor_current;
	float motor_position;
	float adc1, adc2;
	SwitchState switch_state;

	// Rumtime state values
	BalanceState state;
	bool running;
	float proportional, integral, proportional2;
	float integral_b, integral_c;
	float current_request;
	float setpoint, setpoint_target, setpoint_target_interpolated;
	float noseangling_interpolated;
	float torquetilt_filtered_current, torquetilt_target, torquetilt_interpolated;
	Biquad torquetilt_current_biquad;
	SetpointAdjustmentType setpointAdjustmentType;
	float current_time, last_time, diff_time, loop_overshoot; // Seconds
	float filtered_loop_overshoot, loop_overshoot_alpha, filtered_diff_time;
	float fault_angle_pitch_timer, fault_angle_roll_timer, fault_switch_timer, fault_switch_half_timer; // Seconds
	float motor_timeout_seconds;
	float brake_timeout; // Seconds
	float tuneA_booster_current;
	float tuneB_booster_current;
	float tuneC_booster_current;
	bool traction_control;

	float mc_max_current;
	float mc_brake_max_current;

	bool braking;
	float tb_highvoltage_timer;

	float max_duty_with_margin;
	float wheelslip_timer, wheelslip_end_timer;
	float acceleration;
	int accelidx;
	float accelhist[ACCEL_ARRAY_SIZE];

    // IMU data for the balancing filter
    BalanceFilterData balance_filter;

	// Temp tiltback
	float mc_fet_start_temp;
	float mc_mot_start_temp;
	float temp_tiltback_step_size;

	// Reverse Stop
	float reverse_stop_step_size, reverse_tolerance, reverse_total_erpm;
	float reverse_timer;

	// Total turntilt
	float total_turntilt_interpolated;

	// Roll turntilt
	float roll_turntilt_target, roll_turntilt_interpolated;
	float roll_turntilt_step_size;

	// Yaw Turntilt
	float last_yaw_angle, yaw_angle, abs_yaw_change, last_yaw_change, yaw_change, yaw_aggregate, yaw_aggregate_target;
	float yaw_turntilt_target, yaw_turntilt_interpolated;
	float yaw_turntilt_step_size;

	// Startup Clicks
	unsigned int start_counter_clicks, start_counter_clicks_max, start_click_current;

	// Asymmetric Tune
	float tuneA_current;
	float tuneB_current;
	float tuneB_weight;
	float last_tuneB_weight_target;
	float asym_max_accel_b;
	uint32_t asym_max_erpm_b;
	float tuneC_current;
	float tuneC_weight;
	float last_tuneC_weight_target;
	float asym_max_accel_c;
	uint32_t asym_max_erpm_c;

	// Soft Start
	float softstart_pid_limit;
	float softstart_ramp_step_size;

	// Odometer
	float odo_timer;
	int odometer_dirty;
	uint64_t odometer;

	// Feature: RC Move (control via app while idle)
	int rc_steps;
	int rc_counter;
	float rc_current_target;
	float rc_current;

	// Debug values
	int debug_render_1, debug_render_2;
	int debug_sample_field, debug_sample_count, debug_sample_index;
	int debug_experiment_1, debug_experiment_2, debug_experiment_3, debug_experiment_4, debug_experiment_5, debug_experiment_6;
} data;

// Function Prototypes
static void set_current(data *d, float current);
static void configure(data *d);

// Utility Functions
static float biquad_process(Biquad *biquad, float in) {
    float out = in * biquad->a0 + biquad->z1;
    biquad->z1 = in * biquad->a1 + biquad->z2 - biquad->b1 * out;
    biquad->z2 = in * biquad->a2 - biquad->b2 * out;
    return out;
}

static void biquad_config(Biquad *biquad, BiquadType type, float Fc) {
	float K = tanf(M_PI * Fc);	// -0.0159;
	float Q = 0.707; // maximum sharpness (0.5 = maximum smoothness)
	float norm = 1 / (1 + K / Q + K * K);
	if (type == BQ_LOWPASS) {
		biquad->a0 = K * K * norm;
		biquad->a1 = 2 * biquad->a0;
		biquad->a2 = biquad->a0;
	}
	else if (type == BQ_HIGHPASS) {
		biquad->a0 = 1 * norm;
		biquad->a1 = -2 * biquad->a0;
		biquad->a2 = biquad->a0;
	}
	biquad->b1 = 2 * (K * K - 1) * norm;
	biquad->b2 = (1 - K / Q + K * K) * norm;
}

static void biquad_reset(Biquad *biquad) {
	biquad->z1 = 0;
	biquad->z2 = 0;
}

static void configure(data *d) {
	// Set calculated values from config
	d->loop_time_seconds = (float)1.0 / (float)d->balance_conf.hertz;

	d->motor_timeout_seconds = d->loop_time_seconds * 20; // Times 20 for a nice long grace period

	d->startup_step_size = d->balance_conf.startup_speed / d->balance_conf.hertz;
	d->tiltback_duty_step_size = d->balance_conf.tiltback_duty_speed / d->balance_conf.hertz;
	d->tiltback_hv_step_size = d->balance_conf.tiltback_hv_speed / d->balance_conf.hertz;
	d->tiltback_lv_step_size = d->balance_conf.tiltback_lv_speed / d->balance_conf.hertz;
	d->tiltback_return_step_size = d->balance_conf.tiltback_return_speed / d->balance_conf.hertz;
	d->torquetilt_on_step_size = d->balance_conf.torquetilt_on_speed / d->balance_conf.hertz;
	d->torquetilt_off_step_size = d->balance_conf.torquetilt_off_speed / d->balance_conf.hertz;
	d->roll_turntilt_step_size = d->balance_conf.roll_turntilt_speed / d->balance_conf.hertz;
	d->yaw_turntilt_step_size = d->balance_conf.yaw_turntilt_speed / d->balance_conf.hertz;
	d->noseangling_step_size = d->balance_conf.noseangling_speed / d->balance_conf.hertz;
	d->tunea_transition_step_size = d->balance_conf.tunea_transition_speed / d->balance_conf.hertz;
	d->tuneb_transition_step_size = d->balance_conf.tuneb_transition_speed / d->balance_conf.hertz;
	d->tunec_transition_step_size = d->balance_conf.tunec_transition_speed / d->balance_conf.hertz;

	// Overwrite App CFG Mahony KP to pkg Value
	if (VESC_IF->get_cfg_float(CFG_PARAM_IMU_mahony_kp) != d->balance_conf.mahony_kp) {
		VESC_IF->set_cfg_float(CFG_PARAM_IMU_mahony_kp, d->balance_conf.mahony_kp);
	}

	// Maximum amps allow
	d->mc_max_current = VESC_IF->get_cfg_float(CFG_PARAM_l_current_max);
	d->mc_brake_max_current = fabsf(VESC_IF->get_cfg_float(CFG_PARAM_l_current_min));

	d->max_duty_with_margin = VESC_IF->get_cfg_float(CFG_PARAM_l_max_duty) - 0.1;

	// Init Filters
	if (d->balance_conf.loop_time_filter > 0) {
		d->loop_overshoot_alpha = 2.0 * M_PI * d->loop_time_seconds *
				d->balance_conf.loop_time_filter / (2.0 * M_PI * d->loop_time_seconds *
						d->balance_conf.loop_time_filter + 1.0);
	}

	if (d->balance_conf.torquetilt_filter > 0) { // Torquetilt Current Biquad
		float Fc = d->balance_conf.torquetilt_filter / d->balance_conf.hertz;
		biquad_config(&d->torquetilt_current_biquad, BQ_LOWPASS, Fc);
	}

	// Smooth erpm biquad
	biquad_config(&d->smooth_erpm_biquad, BQ_LOWPASS, 90.0 / d->balance_conf.hertz);

	// Variable nose angle adjustment / tiltback (setting is per 1000erpm, convert to per erpm)
	d->tiltback_variable = d->balance_conf.tiltback_variable / 1000;
	if (d->tiltback_variable > 0) {
		d->tiltback_variable_max_erpm = fabsf(d->balance_conf.tiltback_variable_max / d->tiltback_variable);
		d->tiltback_variable_max_erpm += d->balance_conf.tiltback_variable_start_erpm;
	} else {
		d->tiltback_variable_max_erpm = 100000;
	}

	// Reset loop time variables
	d->last_time = 0.0;
	d->filtered_loop_overshoot = 0.0;

	// Reverse Stop
	d->reverse_tolerance = 50000;
	d->reverse_stop_step_size = 100.0 / d->balance_conf.hertz;

	// Temp Tiltback
	d->mc_fet_start_temp = VESC_IF->get_cfg_float(CFG_PARAM_l_temp_fet_start) - d->balance_conf.temp_tiltback_start_offset;
	d->mc_mot_start_temp = VESC_IF->get_cfg_float(CFG_PARAM_l_temp_motor_start) - d->balance_conf.temp_tiltback_start_offset;
	d->temp_tiltback_step_size = d->balance_conf.temp_tiltback_speed / d->balance_conf.hertz;

	// Yaw Turntilt
	d->yaw_aggregate_target = fmaxf(50, d->balance_conf.yaw_turntilt_aggregate);

	// Startup Clicks
	d->start_click_current = d->balance_conf.startup_click_current;
	d->start_counter_clicks_max = 3;

	// Asymmetric Tune
	if (d->balance_conf.asym_max_accel_b < d->balance_conf.asym_min_accel_b)
		d->asym_max_accel_b = 0.1;
	else 
		d->asym_max_accel_b = d->balance_conf.asym_max_accel_b - d->balance_conf.asym_min_accel_b;

	if (d->balance_conf.asym_max_erpm_b < d->balance_conf.asym_min_erpm_b)
		d->asym_max_erpm_b = 1000;
	else 
		d->asym_max_erpm_b = d->balance_conf.asym_max_erpm_b - d->balance_conf.asym_min_erpm_b;
	
	if (d->balance_conf.asym_max_accel_c < d->balance_conf.asym_min_accel_c)
		d->asym_max_accel_c = 0.1;
	else 
		d->asym_max_accel_c = d->balance_conf.asym_max_accel_c - d->balance_conf.asym_min_accel_c;

	if (d->balance_conf.asym_max_erpm_c < d->balance_conf.asym_min_erpm_c)
		d->asym_max_erpm_c = 1000;
	else 
		d->asym_max_erpm_c = d->balance_conf.asym_max_erpm_c - d->balance_conf.asym_min_erpm_c;

	// Soft Start
	if (d->balance_conf.softstart_speed > 0) {
		d->softstart_ramp_step_size = (10.0 - d->balance_conf.softstart_speed) * 100.0 / d->balance_conf.hertz;
	}
	else {
		d->softstart_ramp_step_size = d->mc_max_current;
	}

	// Odometer
	d->odometer_dirty = 0;
	d->odometer = VESC_IF->mc_get_odometer();

	balance_filter_configure(&d->balance_filter, &d->balance_conf);
}

static void reset_vars(data *d) {
	d->running = false;
	// Clear accumulated values.
	d->integral = 0;
	d->integral_b = 0;
	d->integral_c = 0;
	// Set values for startup
	d->setpoint = d->pitch_angle;
	d->setpoint_target_interpolated = d->pitch_angle;
	d->setpoint_target = 0;
	d->noseangling_interpolated = 0;
	d->torquetilt_target = 0;
	d->torquetilt_interpolated = 0;
	d->torquetilt_filtered_current = 0;
	biquad_reset(&d->torquetilt_current_biquad);
	d->setpointAdjustmentType = CENTERING;
	d->state = RUNNING;
	d->current_time = 0;
	d->last_time = 0;
	d->diff_time = 0;
	d->brake_timeout = 0;
	d->current_request = 0;
	biquad_reset(&d->smooth_erpm_biquad);
	d->smooth_erpm = 0;
	d->softstart_pid_limit = 0;

	// Asymmetric Tune
	d->tuneA_current = 0;
	d->tuneB_current = 0;
	d->tuneB_weight = 0;
	d->last_tuneB_weight_target = 0;
	d->tuneA_booster_current = 0;
	d->tuneB_booster_current = 0;
	d->tuneC_booster_current = 0;

	// Acceleration:
	d->acceleration = 0;
	d->accelidx = 0;
	for (int i = 0; i < ACCEL_ARRAY_SIZE; i++)
		d->accelhist[i] = 0;

	// Total Turntilt:
	d->total_turntilt_interpolated = 0;

	// Roll Turntilt:
	d->roll_turntilt_target = 0;
	d->roll_turntilt_interpolated = 0;

	// Yaw Turntilt:
	d->yaw_turntilt_target = 0;
	d->yaw_turntilt_interpolated = 0;
	d->last_yaw_angle = 0;
	d->yaw_aggregate = 0;

	// Startup Clicks
	d->start_counter_clicks = d->start_counter_clicks_max;
	if (d->start_click_current == 0)
		d->start_counter_clicks = 0;

	// Wheel Slip & Traction Control
	d->wheelslip_timer = 0;
	d->wheelslip_end_timer = 0;
	d->traction_control = false;

	// RC Move:
	d->rc_steps = 0;
	d->rc_current = 0;
}

/**
 *	check_odometer: see if we need to write back the odometer during fault state
 */
static void check_odometer(data *d)
{
	// Make odometer persistent if we've gone 200m or more
	if (d->odometer_dirty > 0) {
		if (VESC_IF->mc_get_odometer() > d->odometer + 200) {
			if (d->odometer_dirty == 1) {
				// Wait 10 seconds before writing to avoid writing if immediately continuing to ride
				d->odo_timer = d->current_time;
				d->odometer_dirty++;
			}
			else if ((d->current_time - d->odo_timer) > 10) {
				VESC_IF->store_backup_data();
				d->odometer = VESC_IF->mc_get_odometer();
				d->odometer_dirty = 0;
			}
		}
	}
}

// Read ADCs and determine switch state
static SwitchState check_adcs(data *d) {
	SwitchState sw_state;

	if (d->balance_conf.fault_adc1 == 0 && d->balance_conf.fault_adc2 == 0){ // No Switch
		sw_state = ON;
	} else if (d->balance_conf.fault_adc2 == 0) { // Single switch on ADC1
		if (d->adc1 > d->balance_conf.fault_adc1) {
			sw_state = ON;
		} else {
			sw_state = OFF;
		}
	} else if (d->balance_conf.fault_adc1 == 0) { // Single switch on ADC2
		if (d->adc2 > d->balance_conf.fault_adc2) {
			sw_state = ON;
		} else {
			sw_state = OFF;
		}
	} else { // Double switch
		if (d->adc1 > d->balance_conf.fault_adc1 && d->adc2 > d->balance_conf.fault_adc2) {
			sw_state = ON;
		} else if (d->adc1 > d->balance_conf.fault_adc1 || d->adc2 > d->balance_conf.fault_adc2) {
			if (d->balance_conf.fault_is_single_switch) {
				sw_state = ON;
			} else {
				sw_state = HALF;
			}
		} else {
			sw_state = OFF;
		}
	}
	return sw_state;
}

// Fault checking order does not really matter. From a UX perspective, switch should be before angle.
static bool check_faults(data *d, bool ignoreTimers){
	// Check switch
	// Switch fully open
	if (d->switch_state == OFF) {
		if((1000.0 * (d->current_time - d->fault_switch_timer)) > d->balance_conf.fault_delay_switch_full || ignoreTimers){
			d->state = FAULT_SWITCH_FULL;
			return true;
		}
		
		// low speed (below 6 x half-fault threshold speed):
		else if ((d->abs_erpm < d->balance_conf.fault_adc_half_erpm * 6)
			   && (1000.0 * (d->current_time - d->fault_switch_timer) > d->balance_conf.fault_delay_switch_half)){
			d->state = FAULT_SWITCH_FULL;
			return true;
		}

		// QUICK STOP
		else if (d->balance_conf.enable_quickstop && 
				 d->abs_erpm < d->balance_conf.quickstop_erpm && 
				 fabsf(d->true_pitch_angle) > d->balance_conf.quickstop_angle && 
				 SIGN(d->true_pitch_angle) == SIGN(d->erpm)) {
			d->state = FAULT_QUICKSTOP;
			return true;
		}
	} else {
		d->fault_switch_timer = d->current_time;
	}

	// Feature: Reverse-Stop
	if(d->setpointAdjustmentType == REVERSESTOP){
		//  Taking your foot off entirely while reversing? Ignore delays
		if (d->switch_state == OFF) {
			d->state = FAULT_SWITCH_FULL;
			return true;
		}
		if (fabsf(d->true_pitch_angle) > 15) {
			d->state = FAULT_REVERSE;
			return true;
		}
		// Above 10 degrees for a half a second? Switch it off
		if ((fabsf(d->true_pitch_angle) > 10) && (d->current_time - d->reverse_timer > 500)) {
			d->state = FAULT_REVERSE;
			return true;
		}
		// Above 5 degrees for a full second? Switch it off
		if ((fabsf(d->true_pitch_angle) > 5) && (d->current_time - d->reverse_timer > 1000)) {
			d->state = FAULT_REVERSE;
			return true;
		}
		if (d->reverse_total_erpm > d->reverse_tolerance * 3) {
			d->state = FAULT_REVERSE;
			return true;
		}
		if (fabsf(d->true_pitch_angle) < 5) {
			d->reverse_timer = d->current_time;
		}
	}

	// Switch partially open and stopped
	if(!d->balance_conf.fault_is_single_switch) {
		if((d->switch_state == HALF || d->switch_state == OFF) && d->abs_erpm < d->balance_conf.fault_adc_half_erpm){
			if ((1000.0 * (d->current_time - d->fault_switch_half_timer)) > d->balance_conf.fault_delay_switch_half || ignoreTimers){
				d->state = FAULT_SWITCH_HALF;
				return true;
			}
		} else {
			d->fault_switch_half_timer = d->current_time;
		}
	}

	// Check roll angle
	if (fabsf(d->roll_angle) > d->balance_conf.fault_roll) {
		if ((1000.0 * (d->current_time - d->fault_angle_roll_timer)) > d->balance_conf.fault_delay_roll || ignoreTimers) {
			d->state = FAULT_ANGLE_ROLL;
			return true;
		}
	} else {
		d->fault_angle_roll_timer = d->current_time;
	}

	// Check pitch angle
	if (fabsf(d->true_pitch_angle) > d->balance_conf.fault_pitch) {
		if ((1000.0 * (d->current_time - d->fault_angle_pitch_timer)) > d->balance_conf.fault_delay_pitch || ignoreTimers) {
			d->state = FAULT_ANGLE_PITCH;
			return true;
		}
	} else {
		d->fault_angle_pitch_timer = d->current_time;
	}

	return false;
}

// Get the step size relative to each state
static float get_setpoint_adjustment_step_size(data *d) {
	switch(d->setpointAdjustmentType){
		case (CENTERING):
			return d->startup_step_size;
		case (TILTBACK_DUTY):
			return d->tiltback_duty_step_size;
		case (TILTBACK_TEMP):
			return d->temp_tiltback_step_size;
		case (TILTBACK_HV):
			return d->tiltback_hv_step_size;
		case (TILTBACK_LV):
			return d->tiltback_lv_step_size;
		case (TILTBACK_NONE):
			return d->tiltback_return_step_size;
		case (REVERSESTOP):
			return d->reverse_stop_step_size;
		default:
			;
	}
	return 0;
}

// Calculate current state and initial setpoint
static void calculate_state_and_initial_setpoint(data *d) {
	float input_voltage = VESC_IF->mc_get_input_voltage_filtered();
	if (input_voltage < d->balance_conf.tiltback_hv) {
		d->tb_highvoltage_timer = d->current_time;
	}

	if (d->setpointAdjustmentType == CENTERING && 
		d->setpoint_target_interpolated != d->setpoint_target) {
		// Ignore tiltback during centering sequence
		d->state = RUNNING;
	} 
	// Entering wheelslip
	else if ((fabs(d->acceleration) > 10) &&					// this isn't normal, either wheelslip or wheel getting stuck
			 (SIGN(d->acceleration) == SIGN(d->erpm)) &&		// we only act on wheelslip, not when the wheel gets stuck
			 (d->abs_duty_cycle > 0.3) &&
			 (d->abs_erpm > 1500))								// acceleration can jump a lot at very low speeds
	{
		d->state = RUNNING_WHEELSLIP;
		d->setpointAdjustmentType = TILTBACK_NONE;
		d->wheelslip_timer = d->current_time;
		d->traction_control = true;
	} 
	// Exit wheelslip
	else if (d->state == RUNNING_WHEELSLIP) {
		if (fabsf(d->acceleration) < 10) {
			// acceleration is slowing down, traction control seems to have worked
			d->traction_control = false;
		}

		// Remain in wheelslip state for at least 500ms to avoid any overreactions
		if (d->abs_duty_cycle > d->max_duty_with_margin) {
			d->wheelslip_timer = d->current_time;
			d->setpoint_target = 0;
		}
		else if (d->current_time - d->wheelslip_timer > 0.5 && 
				 d->abs_duty_cycle < 0.7) // Leave wheelslip state only if duty < 70%
		{
			d->state = RUNNING;
			d->traction_control = false;
		}
	}
	// Reverse stop
	else if (d->setpointAdjustmentType == REVERSESTOP) {
		// accumalete erpms:
		d->reverse_total_erpm += d->erpm;
		if (fabsf(d->reverse_total_erpm) > d->reverse_tolerance) {
			// tilt down by 10 degrees after 50k aggregate erpm
			d->setpoint_target = 10 * (fabsf(d->reverse_total_erpm) - d->reverse_tolerance) / 50000;
		}
		else {
			if (fabsf(d->reverse_total_erpm) <= d->reverse_tolerance/2) {
				if (d->erpm >= 0){
					d->setpointAdjustmentType = TILTBACK_NONE;
					d->reverse_total_erpm = 0;
					d->setpoint_target = 0;
					d->integral = 0;
					d->integral_b = 0;
					d->integral_c = 0;
				}
			}
		}
	} 
	// Go to reverse stop state
	else if (d->balance_conf.enable_reverse_stop && (d->erpm < -200)) {
		d->setpointAdjustmentType = REVERSESTOP;
		d->reverse_timer = d->current_time;
		d->reverse_total_erpm = 0;
		d->setpoint_target = 0;
		d->state = RUNNING;
	} 
	// Duty cycle tiltback
	else if (d->abs_duty_cycle > d->balance_conf.tiltback_duty) {
		if (d->erpm > 0) {
			d->setpoint_target = d->balance_conf.tiltback_duty_angle;
		} else {
			d->setpoint_target = -d->balance_conf.tiltback_duty_angle;
		}
		d->setpointAdjustmentType = TILTBACK_DUTY;
		d->state = RUNNING_TILTBACK_DUTY;
	} 
	// High voltage tiltback
	else if (d->abs_duty_cycle > 0.05 && input_voltage > d->balance_conf.tiltback_hv) {
		if (((d->current_time - d->tb_highvoltage_timer) > .5) ||
		   (input_voltage > d->balance_conf.tiltback_hv + 1)) {
			// 500ms have passed or voltage is another volt higher, time for some tiltback
			if (d->erpm > 0){
				d->setpoint_target = d->balance_conf.tiltback_hv_angle;
			} else {
				d->setpoint_target = -d->balance_conf.tiltback_hv_angle;
			}

			d->setpointAdjustmentType = TILTBACK_HV;
			d->state = RUNNING_TILTBACK_HIGH_VOLTAGE;
		}
		else {
			// Was possibly just a short spike
			d->setpointAdjustmentType = TILTBACK_NONE;
			d->state = RUNNING;
		}
	} 
	// Low voltage tiltback
	else if (d->abs_duty_cycle > 0.05 && input_voltage < d->balance_conf.tiltback_lv) {
		float abs_motor_current = fabsf(d->motor_current);
		float vdelta = d->balance_conf.tiltback_lv - input_voltage;
		float ratio = vdelta * 20 / abs_motor_current;
		// When to do LV tiltback:
		// a) we're 2V below lv threshold
		// b) motor current is small (we cannot assume vsag)
		// c) we have more than 20A per Volt of difference (we tolerate some amount of vsag)
		if ((vdelta > 2) || (abs_motor_current < 5) || (ratio > 1)) {
			if (d->erpm > 0) {
				d->setpoint_target = d->balance_conf.tiltback_lv_angle;
			} else {
				d->setpoint_target = -d->balance_conf.tiltback_lv_angle;
			}

			d->setpointAdjustmentType = TILTBACK_LV;
			d->state = RUNNING_TILTBACK_LOW_VOLTAGE;
		}
		else {
			d->setpointAdjustmentType = TILTBACK_NONE;
			d->setpoint_target = 0;
			d->state = RUNNING;
		}
	}
	// Temp (fet) tiltback
	else if(VESC_IF->mc_temp_fet_filtered() > d->mc_fet_start_temp){
		if(d->erpm > 0){
			d->setpoint_target = d->balance_conf.temp_tiltback_angle;
		} else {
			d->setpoint_target = -d->balance_conf.temp_tiltback_angle;
		}
		d->setpointAdjustmentType = TILTBACK_TEMP;
		d->state = RUNNING_TILTBACK_TEMP;
	}
	// Temp (motor) tiltback
	else if(VESC_IF->mc_temp_motor_filtered() > d->mc_mot_start_temp){
		if(d->erpm > 0){
			d->setpoint_target = d->balance_conf.temp_tiltback_angle;
		} else {
			d->setpoint_target = -d->balance_conf.temp_tiltback_angle;
		}
		d->setpointAdjustmentType = TILTBACK_TEMP;
		d->state = RUNNING_TILTBACK_TEMP;
	}
	// Normal run
	else {
		d->setpointAdjustmentType = TILTBACK_NONE;
		d->setpoint_target = 0;
		d->state = RUNNING;
	}

	// Calculate setpoint interpolation
	if (d->setpoint_target_interpolated != d->setpoint_target) {
		float step_size = get_setpoint_adjustment_step_size(d);
		// If we are less than one step size away, go all the way
		if (fabsf(d->setpoint_target - d->setpoint_target_interpolated) < step_size) {
			d->setpoint_target_interpolated = d->setpoint_target;
		} else if (d->setpoint_target - d->setpoint_target_interpolated > 0) {
			d->setpoint_target_interpolated += step_size;
		} else {
			d->setpoint_target_interpolated -= step_size;
		}
	}
}

// Noseangling calculation
static void calc_noseangling_interpolation(data *d){
	// Nose angle adjustment, add variable then constant tiltback
	float noseangling_target = 0;
	if (d->abs_erpm > d->tiltback_variable_max_erpm) {
		noseangling_target = fabsf(d->balance_conf.tiltback_variable_max) * SIGN(d->erpm);
	} else if (d->abs_erpm > d->balance_conf.tiltback_variable_start_erpm){
		float erpm = d->abs_erpm - d->balance_conf.tiltback_variable_start_erpm;
		erpm = erpm * SIGN(d->erpm);
		noseangling_target = d->tiltback_variable * erpm;
	}

	if (d->erpm > d->balance_conf.tiltback_constant_erpm) {
		noseangling_target += d->balance_conf.tiltback_constant;
	} else if (d->erpm < -d->balance_conf.tiltback_constant_erpm){
		noseangling_target += -d->balance_conf.tiltback_constant;
	}

	if (fabsf(noseangling_target - d->noseangling_interpolated) < d->noseangling_step_size) {
		d->noseangling_interpolated = noseangling_target;
	} else if (noseangling_target - d->noseangling_interpolated > 0) {
		d->noseangling_interpolated += d->noseangling_step_size;
	} else {
		d->noseangling_interpolated -= d->noseangling_step_size;
	}
}

// Torquetilt calculation
static void calc_torquetilt_interpolation(data *d) {
	// Wat is this line O_o
	// Take abs motor current, subtract start offset, and take the max of that with 0 to get the current above our start threshold (absolute).
	// Then multiply it by "power" to get our desired angle, and min with the limit to respect boundaries.
	// Finally multiply it by sign motor current to get directionality back
	float torquetilt_strength = d->braking == false ? d->balance_conf.torquetilt_strength : d->balance_conf.torquetilt_strength_regen;
	float torquetilt_start_current = d->braking == false ? d->balance_conf.torquetilt_start_current : d->balance_conf.torquetilt_start_current_b;
	d->torquetilt_target = fminf(fmaxf(fabsf(d->torquetilt_filtered_current) - torquetilt_start_current, 0) *
			torquetilt_strength, d->balance_conf.torquetilt_angle_limit) * SIGN(d->torquetilt_filtered_current);

	float step_size;
	if ((d->torquetilt_interpolated - d->torquetilt_target > 0 && d->torquetilt_target > 0) ||
			(d->torquetilt_interpolated - d->torquetilt_target < 0 && d->torquetilt_target < 0)) {
		step_size = d->torquetilt_off_step_size;
	} else {
		step_size = d->torquetilt_on_step_size;
	}

	// when slow then erpm data is especially choppy, causing fake spikes in acceleration
	// mellow down the reaction to reduce noticeable oscillations
	if (d->abs_erpm < 500) {
		step_size /= 2;
	}

	if (fabsf(d->torquetilt_target - d->torquetilt_interpolated) < step_size) {
		d->torquetilt_interpolated = d->torquetilt_target;
	} else if (d->torquetilt_target - d->torquetilt_interpolated > 0) {
		d->torquetilt_interpolated += step_size;
	} else {
		d->torquetilt_interpolated -= step_size;
	}
}

// Rool turntilt calculation
static void calc_roll_turntilt_interpolation(data *d) {
	if (d->balance_conf.roll_turntilt_strength == 0) {
		d->roll_turntilt_interpolated = 0;
		return;
	}

	// Apply cutzone
	if ((d->running == false) || // Apply turntilt only when RUNNING
		(d->abs_roll_angle < d->balance_conf.roll_turntilt_start_angle) || // Need to be above certain angle to apply turntilt
		(d->abs_erpm < d->balance_conf.roll_turntilt_start_erpm) || // Need to be above certain erpm to apply turntilt
		(fabsf(d->pitch_angle - d->noseangling_interpolated) > 4)) // No setpoint changes during heavy acceleration or braking
	{
		d->roll_turntilt_target = 0.9 * d->roll_turntilt_target;
	}
	else {
		// Calculate desired angle
		d->roll_turntilt_target = d->abs_roll_angle_sin * d->balance_conf.roll_turntilt_strength;

		// Apply speed scaling
		if (d->abs_erpm < d->balance_conf.roll_turntilt_erpm_boost_end) {
			d->roll_turntilt_target *= 1 + ((d->balance_conf.roll_turntilt_erpm_boost / 100.0f) *
					(d->abs_erpm / d->balance_conf.roll_turntilt_erpm_boost_end));
		} else {
			d->roll_turntilt_target *= 1 + (d->balance_conf.roll_turntilt_erpm_boost / 100.0f);
		}

		// Limit angle to max angle
		d->roll_turntilt_target = fminf(d->roll_turntilt_target, d->balance_conf.roll_turntilt_angle_limit);

		// Add directionality
		d->roll_turntilt_target *= SIGN(d->erpm);

		// Torque Tilt interference: Reduce turntilt_target during moments of high torque response
		float torquetilt_min = 2;
		float torquetilt_max = 5;
		if (SIGN(d->torquetilt_target) != SIGN(d->roll_turntilt_target)) {
			// further reduced turntilt during moderate to steep downhills
			torquetilt_min = 1;
			torquetilt_max = 4;
		}
		if (fabsf(d->torquetilt_target) > torquetilt_min) {
			// Start scaling turntilt when torquetilt>2, down to 0 turntilt for torquetilt > 5 degrees
			float tt_scaling = (torquetilt_max - fabsf(d->torquetilt_target)) / (torquetilt_max-torquetilt_min);
			if (tt_scaling < 0) {
				tt_scaling = 0;
			}
			d->roll_turntilt_target *= tt_scaling;
		}
	}

	// Move towards target limited by max speed
	if (fabsf(d->roll_turntilt_target - d->roll_turntilt_interpolated) < d->roll_turntilt_step_size) {
		d->roll_turntilt_interpolated = d->roll_turntilt_target;
	} else if (d->roll_turntilt_target - d->roll_turntilt_interpolated > 0) {
		d->roll_turntilt_interpolated += d->roll_turntilt_step_size;
	} else {
		d->roll_turntilt_interpolated -= d->roll_turntilt_step_size;
	}
}

// Yaw turntilt calculation
static void calc_yaw_turntilt_interpolation(data *d) {
	if (d->balance_conf.yaw_turntilt_strength == 0) {
		d->yaw_turntilt_interpolated = 0;
		return;
	}

	// Apply cutzone
	if ((d->running == false) || // Apply turntilt only when RUNNING
		(d->abs_yaw_change * 100 < d->balance_conf.yaw_turntilt_start_angle) || // Need to be above certain angle to apply turntilt
		(d->abs_erpm < d->balance_conf.yaw_turntilt_start_erpm) || // Need to be above certain erpm to apply turntilt
		(fabsf(d->pitch_angle - d->noseangling_interpolated) > 4)) // No setpoint changes during heavy acceleration or braking
	{
		d->yaw_turntilt_target = 0.9 * d->yaw_turntilt_target;
		
		if (fabsf(d->pitch_angle - d->noseangling_interpolated) > 4) {
			// Reset yaw_aggregate during heavy acceleration or braking
			d->yaw_aggregate = 0;
		}
	}
	else {
		// Calculate desired angle
		d->yaw_turntilt_target = d->abs_yaw_change * d->balance_conf.yaw_turntilt_strength;

		// Apply speed scaling
		if (d->abs_erpm < d->balance_conf.yaw_turntilt_erpm_boost_end) {
			d->yaw_turntilt_target *= 1 + ((d->balance_conf.yaw_turntilt_erpm_boost / 100.0f) *
					(d->abs_erpm / d->balance_conf.yaw_turntilt_erpm_boost_end));
		} else {
			d->yaw_turntilt_target *= 1 + (d->balance_conf.yaw_turntilt_erpm_boost / 100.0f);
		}

		// Increase turntilt based on aggregate yaw change (at most: double it)
		float aggregate_damper = 1.0;
		if (d->abs_erpm < 2000) {
			aggregate_damper = 0.5;
		}
		
		float boost = 1 + aggregate_damper * d->yaw_aggregate / d->yaw_aggregate_target;
		boost = fminf(boost, 2);
		d->yaw_turntilt_target *= boost;

		// Limit angle to max angle
		d->yaw_turntilt_target = fminf(d->yaw_turntilt_target, d->balance_conf.yaw_turntilt_angle_limit);

		// Add directionality
		d->yaw_turntilt_target *= SIGN(d->erpm);

		// Torque Tilt interference: Reduce turntilt_target during moments of high torque response
		float torquetilt_min = 2;
		float torquetilt_max = 5;
		if (SIGN(d->torquetilt_target) != SIGN(d->yaw_turntilt_target)) {
			// further reduced turntilt during moderate to steep downhills
			torquetilt_min = 1;
			torquetilt_max = 4;
		}
		if (fabsf(d->torquetilt_target) > torquetilt_min) {
			// Start scaling turntilt when torquetilt>2, down to 0 turntilt for torquetilt > 5 degrees
			float torquetilt_scaling = (torquetilt_max - fabsf(d->torquetilt_target)) / (torquetilt_max-torquetilt_min);
			if (torquetilt_scaling < 0) {
				torquetilt_scaling = 0;
				// during heavy torque response clear the yaw aggregate too
				d->yaw_aggregate = 0;
			}
			d->yaw_turntilt_target *= torquetilt_scaling;
		}
	}

	// Move towards target limited by max speed
	if (fabsf(d->yaw_turntilt_target - d->yaw_turntilt_interpolated) < d->yaw_turntilt_step_size) {
		d->yaw_turntilt_interpolated = d->yaw_turntilt_target;
	} else if (d->yaw_turntilt_target - d->yaw_turntilt_interpolated > 0) {
		d->yaw_turntilt_interpolated += d->yaw_turntilt_step_size;
	} else {
		d->yaw_turntilt_interpolated -= d->yaw_turntilt_step_size;
	}
}

// Calculate the total turntilt interpolation
static void calc_total_turntilt_interpolation(data *d)
{
	float roll_turntilt = 0;
	if (d->balance_conf.roll_turntilt_weight > 0.0) {
		calc_roll_turntilt_interpolation(d);
		roll_turntilt = d->roll_turntilt_interpolated * d->balance_conf.roll_turntilt_weight;
	}

	float yaw_turntilt = 0;
	if (d->balance_conf.yaw_turntilt_weight > 0.0){
		calc_yaw_turntilt_interpolation(d);
		yaw_turntilt = d->yaw_turntilt_interpolated * d->balance_conf.yaw_turntilt_weight;
	}

	if (d->balance_conf.turntilt_mixing_mode == ADDITION)
		d->total_turntilt_interpolated = roll_turntilt + yaw_turntilt;
	else if (d->balance_conf.turntilt_mixing_mode == MEAN)
		d->total_turntilt_interpolated = (roll_turntilt + yaw_turntilt) / 2.0;
	else if (d->balance_conf.turntilt_mixing_mode == MAX) {
		if (SIGN(yaw_turntilt) == SIGN(roll_turntilt)) {
			float max = fmaxf(fabsf(yaw_turntilt), fabsf(roll_turntilt)) * SIGN(yaw_turntilt);
			d->total_turntilt_interpolated = max;
		}
		else {
			d->total_turntilt_interpolated = roll_turntilt + yaw_turntilt;
		}
	}
}

// Calculate final setpoint with all modifiers
static void calc_final_setpoint(data* d){
	d->setpoint = d->setpoint_target_interpolated;
	d->setpoint += d->noseangling_interpolated;
	d->setpoint += d->torquetilt_interpolated;
	d->setpoint += d->total_turntilt_interpolated;
}

// Are we detected wheelslip?
static bool is_wheelslip(data *d) {
	if (d->state == RUNNING_WHEELSLIP || 
	    (d->current_time - d->wheelslip_end_timer) * 1000 < 100 || // allow the wheel decelerate for 100ms
		(fabsf(d->acceleration) > 10 && d->abs_erpm > 1000))
	{
		if (d->state == RUNNING_WHEELSLIP && 
			(d->current_time - d->wheelslip_end_timer) * 1000 >= 100 && 
			(fabsf(d->acceleration) <= 10 || d->abs_erpm <= 1000))
		{
			d->wheelslip_end_timer = d->current_time;
		}
		return true;
	}
	return false;
}

static void brake(data *d) {
	// Brake timeout logic /*Hard-Coded to 1s Brake Timeout*/
	if ((d->abs_erpm > 1 || d->brake_timeout == 0)) {
		d->brake_timeout = d->current_time + 1;
	}

	if (d->brake_timeout != 0 && d->current_time > d->brake_timeout) {
		return;
	}

	// Reset the timeout
	VESC_IF->timeout_reset();

	// Set current
	VESC_IF->mc_set_brake_current(d->balance_conf.brake_current);
}

static void set_current(data *d, float current){
	// Limit current output to configured max output (does not account for yaw_current)
	if (current > 0 && current > VESC_IF->get_cfg_float(CFG_PARAM_l_current_max)) {
		current = VESC_IF->get_cfg_float(CFG_PARAM_l_current_max);
	} else if(current < 0 && current < VESC_IF->get_cfg_float(CFG_PARAM_l_current_min)) {
		current = VESC_IF->get_cfg_float(CFG_PARAM_l_current_min);
	}

	// Reset the timeout
	VESC_IF->timeout_reset();
	// Set the current delay
	VESC_IF->mc_set_current_off_delay(d->motor_timeout_seconds);
	// Set Current
	VESC_IF->mc_set_current(current);
}

static void do_rc_move(data *d)
{
	if (d->rc_steps > 0) {
		d->rc_current = d->rc_current * 0.95 + d->rc_current_target * 0.05;
		if (d->abs_erpm > 800)
			d->rc_current = 0;
		set_current(d, d->rc_current);
		d->rc_steps--;
		d->rc_counter++;
		if ((d->rc_counter == 500) && (d->rc_current_target > 2)) {
			d->rc_current_target /= 2;
		}
	}
	else {
		d->rc_counter = 0;
		d->rc_current = 0;
		// Disable output
		brake(d);
	}
}

static float calc_booster(data *d, float min_pitch, float max_pitch, float current_limit)
{
	float booster_current = 0;
	float true_proportional = d->setpoint - d->true_pitch_angle;
	float abs_proportional = fabsf(true_proportional);
	if (abs_proportional >= min_pitch) {
		booster_current = current_limit;
		if (abs_proportional < max_pitch && max_pitch > min_pitch) {
			booster_current *= ((abs_proportional - min_pitch) / (max_pitch - min_pitch));
		}

		if (booster_current > current_limit) {
			booster_current = current_limit;
		}
		else if (booster_current < 0) {
			booster_current = 0;
		}

		booster_current *= SIGN(d->proportional);
	}

	return booster_current;
}

static void get_tuneA_current(data *d)
{
	// Apply integral and limiter
	if (d->abs_erpm > 150) {
		d->integral += d->proportional * d->balance_conf.pitch_thi;
		if (d->balance_conf.pitch_thi_limit > 0 && fabsf(d->integral) > d->balance_conf.pitch_thi_limit) {
			d->integral = d->balance_conf.pitch_thi_limit * SIGN(d->integral);
		}
	}
	else {
		d->integral = d->integral * 0.999;
	}

	// Lowering the integral while reverse stop
	if (d->setpointAdjustmentType == REVERSESTOP) {
		d->integral = d->integral * 0.9;
	}

	// Calculate new current
	float output_current = d->balance_conf.pitch_th * d->proportional + d->integral;
			
	// Start Rate PID and Booster portion a few cycles later, after the start clicks have been emitted
	// this keeps the start smooth and predictable
	if (d->start_counter_clicks == 0) {
		if (d->balance_conf.gyro_th > 0) {
			output_current += d->balance_conf.gyro_th * d->proportional2;
		}

		// Add booster
		if (d->balance_conf.booster_current_limit > 0) {
			float booster_current = calc_booster(d, d->balance_conf.booster_min_pitch, d->balance_conf.booster_max_pitch, 
												 d->balance_conf.booster_current_limit);
			d->tuneA_booster_current = 0.01 * booster_current + 0.99 * d->tuneA_booster_current;
			output_current += d->tuneA_booster_current;
		}
	}
	output_current = d->tuneA_current * (1.0 - d->balance_conf.current_out_filter) + output_current * d->balance_conf.current_out_filter;

	// Brake Amp Rate Limiting
	if (d->braking && (fabsf(output_current - d->current_request) > d->balance_conf.brake_max_amp_change)) {
		if (output_current > d->current_request) {
			output_current = d->current_request + d->balance_conf.brake_max_amp_change;
		}
		else {
			output_current = d->current_request - d->balance_conf.brake_max_amp_change;
		}
	}
	d->tuneA_current = output_current;
}

static void get_tuneB_current(data *d)
{
	// Apply integral_b and limiter
	if (d->abs_erpm > 150) {
		d->integral_b += d->proportional * d->balance_conf.pitch_thi_b;
		if (d->balance_conf.pitch_thi_limit_b > 0 && fabsf(d->integral_b) > d->balance_conf.pitch_thi_limit_b) {
			d->integral_b = d->balance_conf.pitch_thi_limit_b * SIGN(d->integral_b);
		}
	}
	else {
		d->integral_b = d->integral_b * 0.999;
	}

	// Lowering the integral_b while reverse stop
	if (d->setpointAdjustmentType == REVERSESTOP) {
		d->integral_b = d->integral_b * 0.9;
	}

	// Calculate new current
	float output_current = d->balance_conf.pitch_th_b * d->proportional + d->integral_b;
			
	// Start Rate PID and Booster portion a few cycles later, after the start clicks have been emitted
	// this keeps the start smooth and predictable
	if (d->start_counter_clicks == 0) {
		if (d->balance_conf.gyro_th_b > 0) {
			output_current += d->balance_conf.gyro_th_b * d->proportional2;
		}

		// Add booster
		if (d->balance_conf.booster_current_limit_b > 0) {
			float booster_current = calc_booster(d, d->balance_conf.booster_min_pitch_b, d->balance_conf.booster_max_pitch_b, 
												 d->balance_conf.booster_current_limit_b);
			d->tuneB_booster_current = 0.01 * booster_current + 0.99 * d->tuneB_booster_current;
			output_current += d->tuneB_booster_current;
		}
	}
	output_current = d->tuneB_current * (1.0 - d->balance_conf.current_out_filter_b) + output_current * d->balance_conf.current_out_filter_b;

	// Brake Amp Rate Limiting
	if (d->braking && (fabsf(output_current - d->current_request) > d->balance_conf.brake_max_amp_change_b)) {
		if (output_current > d->current_request) {
			output_current = d->current_request + d->balance_conf.brake_max_amp_change_b;
		}
		else {
			output_current = d->current_request - d->balance_conf.brake_max_amp_change_b;
		}
	}

	d->tuneB_current = output_current;
}

static void get_tuneC_current(data *d)
{
	// Apply integral_c and limiter
	if (d->abs_erpm > 150) {
		d->integral_c += d->proportional * d->balance_conf.pitch_thi_c;
		if (d->balance_conf.pitch_thi_limit_c > 0 && fabsf(d->integral_c) > d->balance_conf.pitch_thi_limit_c) {
			d->integral_c = d->balance_conf.pitch_thi_limit_c * SIGN(d->integral_c);
		}
	}
	else {
		d->integral_c = d->integral_c * 0.999;
	}

	// Lowering the integral_c while reverse stop
	if (d->setpointAdjustmentType == REVERSESTOP) {
		d->integral_c = d->integral_c * 0.9;
	}

	// Calculate new current
	float output_current = d->balance_conf.pitch_th_c * d->proportional + d->integral_c;
			
	// Start Rate PID and Booster portion a few cycles later, after the start clicks have been emitted
	// this keeps the start smooth and predictable
	if (d->start_counter_clicks == 0) {
		if (d->balance_conf.gyro_th_c > 0) {
			output_current += d->balance_conf.gyro_th_c * d->proportional2;
		}

		// Add booster
		if (d->balance_conf.booster_current_limit_c > 0) {
			float booster_current = calc_booster(d, d->balance_conf.booster_min_pitch_c, d->balance_conf.booster_max_pitch_c, 
												 d->balance_conf.booster_current_limit_c);
			d->tuneC_booster_current = 0.01 * booster_current + 0.99 * d->tuneC_booster_current;
			output_current += d->tuneC_booster_current;
		}
	}
	output_current = d->tuneC_current * (1.0 - d->balance_conf.current_out_filter_c) + output_current * d->balance_conf.current_out_filter_c;

	// Brake Amp Rate Limiting
	if (d->braking && (fabsf(output_current - d->current_request) > d->balance_conf.brake_max_amp_change_c)) {
		if (output_current > d->current_request) {
			output_current = d->current_request + d->balance_conf.brake_max_amp_change_c;
		}
		else {
			output_current = d->current_request - d->balance_conf.brake_max_amp_change_c;
		}
	}

	d->tuneC_current = output_current;
}

static void imu_ref_callback(float *acc, float *gyro, float *mag, float dt) {
	data *d = (data*)ARG;
    balance_filter_update(&d->balance_filter, gyro, acc, dt);
}

static void balance_thd(void *arg) {
	data *d = (data*)arg;

	while (!VESC_IF->should_terminate()) {
		// Update times
		d->current_time = VESC_IF->system_time();
		if (d->last_time == 0) {
			d->last_time = d->current_time;
		}
		d->diff_time = d->current_time - d->last_time;
		d->last_time = d->current_time;
		d->filtered_diff_time = 0.03 * d->diff_time + 0.97 * d->filtered_diff_time; // Purely a metric
		d->loop_overshoot = d->diff_time - (d->loop_time_seconds - roundf(d->filtered_loop_overshoot));
		d->filtered_loop_overshoot = d->loop_overshoot_alpha * d->loop_overshoot + (1.0 - d->loop_overshoot_alpha) * d->filtered_loop_overshoot;

		// Get motor values
		d->motor_current = VESC_IF->mc_get_tot_current_directional_filtered();
		d->motor_position = VESC_IF->mc_get_pid_pos_now();
		if (d->balance_conf.torquetilt_filter > 0) {
			// Filter current (Biquad)
			d->torquetilt_filtered_current = biquad_process(&d->torquetilt_current_biquad, d->motor_current);
		} else {
			d->torquetilt_filtered_current = d->motor_current;
		}

		// Get pitch & true pitch (true pitch is derived from the secondary IMU filter running with kp=0.2)
		d->last_pitch_angle = d->pitch_angle;
		d->true_pitch_angle = RAD2DEG_f(VESC_IF->imu_get_pitch());
		d->pitch_angle = RAD2DEG_f(balance_filter_get_pitch(&d->balance_filter));

		// Get roll
		d->roll_angle = RAD2DEG_f(VESC_IF->imu_get_roll());
		d->abs_roll_angle = fabsf(d->roll_angle);
		d->abs_roll_angle_sin = sinf(DEG2RAD_f(d->abs_roll_angle));

		// Get gyro
		d->last_gyro_y = d->gyro[1];
		VESC_IF->imu_get_gyro(d->gyro);

		// Get duty cycle
		d->duty_cycle = VESC_IF->mc_get_duty_cycle_now();
		d->abs_duty_cycle = fabsf(d->duty_cycle);

		// Get erpm
		d->last_erpm = d->smooth_erpm;
		d->erpm = VESC_IF->mc_get_rpm();
		d->abs_erpm = fabsf(d->erpm);
		d->smooth_erpm = biquad_process(&d->smooth_erpm_biquad, d->erpm);

		// Calculate erpm acceleration
		float erpm_acceleration_raw = d->smooth_erpm - d->last_erpm;
		d->acceleration += (erpm_acceleration_raw - d->accelhist[d->accelidx]) / ACCEL_ARRAY_SIZE;
		float abs_accel = fabsf(d->acceleration);
		d->accelhist[d->accelidx] = erpm_acceleration_raw;
		d->accelidx++;
		if (d->accelidx == ACCEL_ARRAY_SIZE)
			d->accelidx = 0;

		// Get ADC1 & ADC2
		d->adc1 = VESC_IF->io_read_analog(VESC_PIN_ADC1);
		d->adc2 = VESC_IF->io_read_analog(VESC_PIN_ADC2);
		if (d->adc2 < 0.0) {
			// Returns -1.0 if the pin is missing on the hardware
			d->adc2 = d->adc1;
		}

		// Calculate switch state from ADC values
		d->switch_state = check_adcs(d);

		// Yaw Turn Tilt:
		d->yaw_angle = VESC_IF->imu_get_yaw() * 180.0f / M_PI;
		float new_change = d->yaw_angle - d->last_yaw_angle;
		bool unchanged = false;
		if ((new_change == 0) // Exact 0's only happen when the IMU is not updating between loops
			|| (fabsf(new_change) > 100)) // yaw flips signs at 180, ignore those changes
		{
			new_change = d->last_yaw_change;
			unchanged = true;
		}
		d->last_yaw_change = new_change;
		d->last_yaw_angle = d->yaw_angle;

		// To avoid overreactions at low speed, limit change here:
		new_change = fminf(new_change, 0.10);
		new_change = fmaxf(new_change, -0.10);
		d->yaw_change = d->yaw_change * 0.8 + 0.2 * (new_change);
		// Clear the aggregate yaw whenever we change direction
		if (SIGN(d->yaw_change) != SIGN(d->yaw_aggregate))
			d->yaw_aggregate = 0;
		d->abs_yaw_change = fabsf(d->yaw_change);
		if ((d->abs_yaw_change > 0.04) && !unchanged)	// don't count tiny yaw changes towards aggregate
			d->yaw_aggregate += d->abs_yaw_change;

		// Control Loop State Logic
		switch(d->state) {
		case (STARTUP):
				// Disable output
				brake(d);
				if (VESC_IF->imu_startup_done()) {
					reset_vars(d);
					d->state = FAULT_STARTUP; // Trigger a fault so we need to meet start conditions to start
				}
				break;

		case (RUNNING):
		case (RUNNING_TILTBACK_DUTY):
		case (RUNNING_TILTBACK_HIGH_VOLTAGE):
		case (RUNNING_TILTBACK_LOW_VOLTAGE):
		case (RUNNING_TILTBACK_TEMP):
		case (RUNNING_WHEELSLIP):
			// Check for faults
			if (check_faults(d, false)) {
				break;
			}
			d->running = true;
			d->odometer_dirty = 1;

			//Initialize variables
			if ((d->abs_erpm > 250) && (SIGN(d->torquetilt_filtered_current) != SIGN(d->erpm))) {
				// current is negative, so we are braking or going downhill
				// high currents downhill are less likely
				d->braking = true;
			}
			else {
				d->braking = false;
			}

			// Calculate setpoint and interpolation
			calculate_state_and_initial_setpoint(d);
			bool is_wheelslip_var = is_wheelslip(d);
			if (!is_wheelslip_var) {
				calc_noseangling_interpolation(d);
				calc_torquetilt_interpolation(d);
				calc_total_turntilt_interpolation(d);
			}
			else{
				// Are we dealing with a free-spinning wheel?
				// If yes, don't change the tiltback till we got traction again
				// instead slightly decrease it each cycle
				d->noseangling_interpolated *= 0.995;
				d->torquetilt_interpolated *= 0.995;
				d->yaw_turntilt_interpolated *= 0.995;
				d->roll_turntilt_interpolated *= 0.995;
				d->total_turntilt_interpolated *= 0.995;
				d->torquetilt_target *= 0.99;
				d->yaw_turntilt_target *= 0.99;
				d->roll_turntilt_target *= 0.99;
			}
			calc_final_setpoint(d);
			
			// Calculate current weight target for Tune B (blend between Tune (A) & Tune (B))
			float tuneB_weight_target = 0.0; // 0 = Tune (A), 1 = Tune (B)
			bool enable_second_tune = d->balance_conf.tune_b_only_for_brakes ? d->braking : true;
			if (enable_second_tune) 
			{
				// calculate how much weight to give to Tune (B) based on acceleration
				if (d->balance_conf.tunes_mixing_b == ACCELERATION_BASED) {
					if (abs_accel - d->balance_conf.asym_min_accel_b > 0) 
					{
						tuneB_weight_target = fminf(abs_accel - d->balance_conf.asym_min_accel_b, d->asym_max_accel_b) / d->asym_max_accel_b;
					}
				}
				// calculate how much weight to give to Tune (B) based on ERPM
				else if (d->balance_conf.tunes_mixing_b == ERPM_BASED) {
					if (d->abs_erpm - d->balance_conf.asym_min_erpm_b > 0) 
					{
						tuneB_weight_target = fminf(d->abs_erpm - d->balance_conf.asym_min_erpm_b, d->asym_max_erpm_b) / d->asym_max_erpm_b;
					}
				}
			}

			// Calculate tuneB_weight and step size for interpolation (cow_ss = current out weight step size)
			float cow_ss = tuneB_weight_target > d->last_tuneB_weight_target ? d->tuneb_transition_step_size : d->tunea_transition_step_size;
			if (fabsf(tuneB_weight_target - d->tuneB_weight) <= cow_ss) {
				d->tuneB_weight = tuneB_weight_target;
			} else if (tuneB_weight_target - d->tuneB_weight > 0) {
				d->tuneB_weight += cow_ss;
			} else {
				d->tuneB_weight -= cow_ss;
			}

			// Set d->last_tuneB_weight_target to the new value
			d->last_tuneB_weight_target = tuneB_weight_target;

			// Calculate current weight target for Tune C (blend between Tune (A) & Tune (C))
			float tuneC_weight_target = 0.0; // 0 = Tune (A), 1 = Tune (C)
			bool enable_third_tune = d->balance_conf.tune_c_only_for_brakes ? d->braking : true;
			if (enable_third_tune) 
			{
				// calculate how much weight to give to Tune (C) based on acceleration
				if (d->balance_conf.tunes_mixing_c == ACCELERATION_BASED) {
					if (abs_accel - d->balance_conf.asym_min_accel_c > 0) 
					{
						tuneC_weight_target = fminf(abs_accel - d->balance_conf.asym_min_accel_c, d->asym_max_accel_c) / d->asym_max_accel_c;
					}
				}
				// calculate how much weight to give to Tune (C) based on ERPM
				else if (d->balance_conf.tunes_mixing_c == ERPM_BASED) {
					if (d->abs_erpm - d->balance_conf.asym_min_erpm_c > 0) 
					{
						tuneC_weight_target = fminf(d->abs_erpm - d->balance_conf.asym_min_erpm_c, d->asym_max_erpm_c) / d->asym_max_erpm_c;
					}
				}
			}

			// Calculate tuneC_weight and step size for interpolation (cow_ss = current out weight step size)
			cow_ss = tuneC_weight_target > d->last_tuneC_weight_target ? d->tunec_transition_step_size : d->tunea_transition_step_size;
			if (fabsf(tuneC_weight_target - d->tuneC_weight) <= cow_ss) {
				d->tuneC_weight = tuneC_weight_target;
			} else if (tuneC_weight_target - d->tuneC_weight > 0) {
				d->tuneC_weight += cow_ss;
			} else {
				d->tuneC_weight -= cow_ss;
			}

			// Set d->last_tuneC_weight_target to the new value
			d->last_tuneC_weight_target = tuneC_weight_target;

			// Calculate proportional
			d->proportional = d->setpoint - d->pitch_angle;
			d->proportional2 = -d->gyro[1];

			// Tune A Calculate output current
			get_tuneA_current(d);
			// Tune B Calculate output current
			get_tuneB_current(d);
			if (tuneB_weight_target == 0.0 && d->tuneB_weight == 0) {
				d->tuneB_current = d->tuneA_current;
				d->tuneB_booster_current = d->tuneA_booster_current;
				if (d->balance_conf.reset_pitch_thi_on_entering_b)
					d->integral_b = 0;
			}
			// Tune C Calculate output current
			get_tuneC_current(d);
			if (tuneC_weight_target == 0.0 && d->tuneC_weight == 0) {
				d->tuneC_current = d->tuneA_current;
				d->tuneC_booster_current = d->tuneA_booster_current;
				if (d->balance_conf.reset_pitch_thi_on_entering_c)
					d->integral_c = 0;
			}

			// Blend between Tune A & Tune B & Tune C
			float new_output_current = d->tuneA_current;
			if (d->balance_conf.transitions_order == ALL_TOGETHER) {
				float tuneA_weight = 1.0;
				float tuneB_weight = 0;
				float tuneC_weight = 0;
				if (d->tuneB_weight > 0 || d->tuneC_weight) {
					float BC_weights = d->tuneB_weight + d->tuneC_weight;
					float tuneB_scaleBy = d->tuneB_weight / BC_weights;
					float tuneC_scaleBy = d->tuneC_weight / BC_weights;
					tuneB_weight = d->tuneB_weight * tuneB_scaleBy;
					tuneC_weight = d->tuneC_weight * tuneC_scaleBy;
					tuneA_weight = 1.0 - (tuneB_weight + tuneC_weight);
				}
				new_output_current = d->tuneA_current * tuneA_weight + d->tuneB_current * tuneB_weight + d->tuneC_current * tuneC_weight;
			}
			else if (d->balance_conf.transitions_order == B_THEN_C) {
				new_output_current = d->tuneA_current * (1.0 - d->tuneB_weight) + d->tuneB_current * d->tuneB_weight;
				new_output_current = new_output_current * (1.0 - d->tuneC_weight) + d->tuneC_current * d->tuneC_weight;
			}
			else if (d->balance_conf.transitions_order == C_THEN_B) {
				new_output_current = d->tuneA_current * (1.0 - d->tuneC_weight) + d->tuneC_current * d->tuneC_weight;
				new_output_current = new_output_current * (1.0 - d->tuneB_weight) + d->tuneB_current * d->tuneB_weight;
			}

			// Filter the current
			new_output_current = d->current_request * 0.65 + new_output_current * 0.35;
			
			// Current Limiting!
			float current_limit;
			if (d->braking) {
				current_limit = d->mc_brake_max_current;
			}
			else {
				current_limit = d->mc_max_current;
			}
			if (fabsf(new_output_current) > current_limit) {
				new_output_current = SIGN(new_output_current) * current_limit;
			}

			// Freewheel while traction loss is detected
			if (d->traction_control && d->balance_conf.enable_traction_control) {
				d->current_request = d->current_request * d->balance_conf.traction_control_mul_by;
			}
			// Everything ok, set new_output_current as the final current
			else {
				d->current_request = new_output_current;
			}

			// Soft start
			if (d->softstart_pid_limit < d->mc_max_current) {
				d->current_request = fminf(d->current_request, d->softstart_pid_limit);
				d->softstart_pid_limit += d->softstart_ramp_step_size;
			}

			// Output to motor
			if (d->start_counter_clicks && d->abs_erpm < 200) {
				// Generate alternate pulses to produce distinct "click"
				d->start_counter_clicks--;
				if ((d->start_counter_clicks & 0x1) == 0)
					set_current(d, d->current_request - d->start_click_current);
				else
					set_current(d, d->current_request + d->start_click_current);
			}
			else {
				set_current(d, d->current_request);
			}
			break;

		case (FAULT_ANGLE_PITCH):
		case (FAULT_ANGLE_ROLL):
		case (FAULT_REVERSE):
		case (FAULT_QUICKSTOP):
		case (FAULT_SWITCH_HALF):
		case (FAULT_SWITCH_FULL):
		case (FAULT_STARTUP):
			d->running = false;
			check_odometer(d);

			// Check for valid startup position and switch state
			if (fabsf(d->pitch_angle) < d->balance_conf.startup_pitch_tolerance &&
					fabsf(d->roll_angle) < d->balance_conf.startup_roll_tolerance && d->switch_state == ON) {
				reset_vars(d);
				break;
			}

			// Set RC current or maintain brake current (and keep WDT happy!)
			do_rc_move(d);
			break;
		}

		// Debug outputs
//		app_balance_sample_debug();
//		app_balance_experiment();

		// Delay between loops
		VESC_IF->sleep_us((uint32_t)((d->loop_time_seconds - roundf(d->filtered_loop_overshoot)) * 1000000.0));
	}
}

static float app_balance_get_debug(int index) {
	data *d = (data*)ARG;

	switch(index){
		case(1):
			return d->motor_position;
		case(2):
			return d->setpoint;
		case(3):
			return d->torquetilt_filtered_current;
		case(4):
			return d->proportional;
		case(5):
			return d->last_pitch_angle - d->pitch_angle;
		case(6):
			return d->motor_current;
		case(7):
			return d->erpm;
		case(8):
			return d->abs_erpm;
		case(9):
			return d->loop_time_seconds;
		case(10):
			return d->diff_time;
		case(11):
			return d->loop_overshoot;
		case(12):
			return d->filtered_loop_overshoot;
		case(13):
			return d->filtered_diff_time;
		case(14):
			return d->integral;
		case(15):
			return d->integral * d->balance_conf.pitch_thi;
		default:
			return 0;
	}
}

static void send_realtime_data(data *d){
	int32_t ind = 0;
	uint8_t send_buffer[140];
	send_buffer[ind++] = 101;
	buffer_append_uint16(send_buffer, d->running, &ind);
	buffer_append_float32_auto(send_buffer, d->diff_time, &ind);
	buffer_append_uint16(send_buffer, d->state, &ind);
	buffer_append_float32_auto(send_buffer, d->abs_duty_cycle, &ind);
	buffer_append_float32_auto(send_buffer, d->motor_current, &ind);
	buffer_append_float32_auto(send_buffer, d->torquetilt_filtered_current, &ind);
	buffer_append_float32_auto(send_buffer, d->erpm, &ind);
	buffer_append_float32_auto(send_buffer, d->acceleration, &ind);
	buffer_append_uint16(send_buffer, d->braking, &ind);
	buffer_append_float32_auto(send_buffer, d->current_request, &ind);
	buffer_append_float32_auto(send_buffer, d->pitch_angle, &ind);
	buffer_append_float32_auto(send_buffer, d->roll_angle, &ind);
	buffer_append_uint16(send_buffer, d->switch_state, &ind);
	buffer_append_float32_auto(send_buffer, d->adc1, &ind);
	buffer_append_float32_auto(send_buffer, d->adc2, &ind);
	buffer_append_float32_auto(send_buffer, d->setpoint_target_interpolated, &ind);
	buffer_append_float32_auto(send_buffer, d->noseangling_interpolated, &ind);
	buffer_append_float32_auto(send_buffer, d->torquetilt_interpolated, &ind);
	buffer_append_float32_auto(send_buffer, d->yaw_turntilt_interpolated * d->balance_conf.yaw_turntilt_weight, &ind);
	buffer_append_float32_auto(send_buffer, d->roll_turntilt_interpolated * d->balance_conf.roll_turntilt_weight, &ind);
	buffer_append_float32_auto(send_buffer, d->total_turntilt_interpolated, &ind);
	buffer_append_float32_auto(send_buffer, d->tuneA_current, &ind);
	buffer_append_float32_auto(send_buffer, d->tuneB_current, &ind);
	buffer_append_float32_auto(send_buffer, d->tuneC_current, &ind);
	buffer_append_float32_auto(send_buffer, d->tuneB_weight, &ind);
	buffer_append_float32_auto(send_buffer, d->tuneC_weight, &ind);
	buffer_append_float32_auto(send_buffer, d->tuneA_booster_current, &ind);
	buffer_append_float32_auto(send_buffer, d->tuneB_booster_current, &ind);
	buffer_append_float32_auto(send_buffer, d->tuneC_booster_current, &ind);
	VESC_IF->send_app_data(send_buffer, ind);
}

void cmd_rc_move(data *d, unsigned char *cfg)//int amps, int time)
{
	int ind = 0;
	int direction = cfg[ind++];
	int current = cfg[ind++];
	int time = cfg[ind++];
	int sum = cfg[ind++];
	if (sum != time+current) {
		current = 0;
	}
	else if (direction == 0) {
		current = -current;
	}

	if (!d->running) {
		d->rc_counter = 0;
		if (current == 0) {
			d->rc_steps = 1;
			d->rc_current_target = 0;
			d->rc_current = 0;
		}
		else {
			d->rc_steps = time * 100;
			d->rc_current_target = current / 10.0;
			if (d->rc_current_target > 8) {
				d->rc_current_target = 2;
			}
		}
	}
}

// Handler for incoming app commands
static void on_command_recieved(unsigned char *buffer, unsigned int len) {
	data *d = (data*)ARG;
	if(len < 2){
		VESC_IF->printf("Balance OW: Missing Args\n");
		return;
	}

	uint8_t magicnr = buffer[0];
	if (magicnr != 101) {
		VESC_IF->printf("Balance OW: Wrong magic number %d\n", magicnr);
		return;
	}

	uint8_t command = buffer[1];
	if(command == 0x01){ // RT Data
		send_realtime_data(d);
	}
	else if(command == 7){ // RC Move
		if (len == 6) {
			cmd_rc_move(d, &buffer[2]);
		}
		else {
			VESC_IF->printf("Balance OW: Command length incorrect (%d)\n", len);
		}
	}else{
		VESC_IF->printf("Unknown command received %d", command);
	}
}

// Register get_debug as a lisp extension
static lbm_value ext_bal_dbg(lbm_value *args, lbm_uint argn) {
	if (argn != 1 || !VESC_IF->lbm_is_number(args[0])) {
		return VESC_IF->lbm_enc_sym_eerror;
	}

	return VESC_IF->lbm_enc_float(app_balance_get_debug(VESC_IF->lbm_dec_as_i32(args[0])));
}

// These functions are used to send the config page to VESC Tool
// and to make persistent read and write work
static int get_cfg(uint8_t *buffer, bool is_default) {
	data *d = (data*)ARG;
	balance_config *cfg = VESC_IF->malloc(sizeof(balance_config));

	*cfg = d->balance_conf;

	if (is_default) {
		confparser_set_defaults_balance_config(cfg);
	}

	int res = confparser_serialize_balance_config(buffer, cfg);
	VESC_IF->free(cfg);

	return res;
}

static bool set_cfg(uint8_t *buffer) {
	data *d = (data*)ARG;
	if (d->running && d->abs_erpm > 500)
		return false;

	bool res = confparser_deserialize_balance_config(buffer, &(d->balance_conf));

	// Store to EEPROM
	if (res) {
		uint32_t ints = sizeof(balance_config) / 4 + 1;
		uint32_t *buffer = VESC_IF->malloc(ints * sizeof(uint32_t));
		bool write_ok = true;
		memcpy(buffer, &(d->balance_conf), sizeof(balance_config));
		for (uint32_t i = 0;i < ints;i++) {
			eeprom_var v;
			v.as_u32 = buffer[i];
			if (!VESC_IF->store_eeprom_var(&v, i + 1)) {
				write_ok = false;
				break;
			}
		}

		VESC_IF->free(buffer);

		if (write_ok) {
			eeprom_var v;
			v.as_u32 = BALANCE_CONFIG_SIGNATURE;
			VESC_IF->store_eeprom_var(&v, 0);
		}

		configure(d);
	}

	return res;
}

static int get_cfg_xml(uint8_t **buffer) {
	// Note: As the address of data_balance_config_ is not known
	// at compile time it will be relative to where it is in the
	// linked binary. Therefore we add PROG_ADDR to it so that it
	// points to where it ends up on the STM32.
	*buffer = data_balance_config_ + PROG_ADDR;
	return DATA_BALANCE_CONFIG__SIZE;
}

// Called when code is stopped
static void stop(void *arg) {
	data *d = (data*)arg;
	VESC_IF->set_app_data_handler(NULL);
	VESC_IF->conf_custom_clear_configs();
	VESC_IF->request_terminate(d->thread);
	VESC_IF->printf("Balance App Terminated");
	VESC_IF->free(d);
}

INIT_FUN(lib_info *info) {
	INIT_START

	data *d = VESC_IF->malloc(sizeof(data));
	memset(d, 0, sizeof(data));
	
	if (!d) {
		VESC_IF->printf("Out of memory!");
		return false;
	}

	// Read config from EEPROM if signature is correct
	eeprom_var v;
	uint32_t ints = sizeof(balance_config) / 4 + 1;
	uint32_t *buffer = VESC_IF->malloc(ints * sizeof(uint32_t));
	bool read_ok = VESC_IF->read_eeprom_var(&v, 0);
	if (read_ok && v.as_u32 == BALANCE_CONFIG_SIGNATURE) {
		for (uint32_t i = 0;i < ints;i++) {
			if (!VESC_IF->read_eeprom_var(&v, i + 1)) {
				read_ok = false;
				break;
			}
			buffer[i] = v.as_u32;
		}
	} else {
		read_ok = false;
	}
	
	if (read_ok) {
		memcpy(&(d->balance_conf), buffer, sizeof(balance_config));
	} else {
		confparser_set_defaults_balance_config(&(d->balance_conf));
	}
	
	VESC_IF->free(buffer);

	info->stop_fun = stop;	
	info->arg = d;
	
	VESC_IF->conf_custom_add_config(get_cfg, set_cfg, get_cfg_xml);

	configure(d);

    balance_filter_init(&d->balance_filter);
	VESC_IF->imu_set_read_callback(imu_ref_callback);

	d->thread = VESC_IF->spawn(balance_thd, 2048, "Balance Main", d);

	VESC_IF->set_app_data_handler(on_command_recieved);
	VESC_IF->lbm_add_extension("ext-balance-dbg", ext_bal_dbg);

	return true;
}

