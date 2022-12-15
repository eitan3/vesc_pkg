/*
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

#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	float kp;
	float ki;
	float kp2;
	float ki2;
	float pid_filtering_weight;
	uint16_t hertz;
	float fault_pitch;
	float fault_roll;
	float fault_adc1;
	float fault_adc2;
	uint16_t fault_delay_pitch;
	uint16_t fault_delay_roll;
	uint16_t fault_delay_switch_half;
	uint16_t fault_delay_switch_full;
	uint16_t fault_adc_half_erpm;
	bool fault_is_single_switch;
	float tiltback_duty_angle;
	float tiltback_duty_speed;
	float tiltback_duty;
	float tiltback_hv_angle;
	float tiltback_hv_speed;
	float tiltback_hv;
	float tiltback_lv_angle;
	float tiltback_lv_speed;
	float tiltback_lv;
	float tiltback_return_speed;
	float tiltback_constant;
	uint16_t tiltback_constant_erpm;
	float tiltback_variable;
	float tiltback_variable_max;
	float noseangling_speed;
	float startup_pitch_tolerance;
	float startup_roll_tolerance;
	float startup_speed;
	float brake_current;
	float pid_brake_max_amp_change;
	float ki_limit;
	float booster_angle;
	float booster_ramp;
	float booster_current;
	float torquetilt_start_current;
	float torquetilt_angle_limit;
	float torquetilt_on_speed;
	float torquetilt_off_speed;
	float torquetilt_strength;
	float torquetilt_filter;
	float roll_turntilt_weight;
	float roll_turntilt_strength;
	float roll_turntilt_angle_limit;
	float roll_turntilt_start_angle;
	uint16_t roll_turntilt_start_erpm;
	float roll_turntilt_speed;
	uint16_t roll_turntilt_erpm_boost;
	uint16_t roll_turntilt_erpm_boost_end;
	float yaw_turntilt_weight;
	float yaw_turntilt_strength;
	float yaw_turntilt_angle_limit;
	float yaw_turntilt_start_angle;
	uint16_t yaw_turntilt_start_erpm;
	float yaw_turntilt_speed;
	uint16_t yaw_turntilt_erpm_boost;
	uint16_t yaw_turntilt_erpm_boost_end;
	uint16_t yaw_turntilt_aggregate;
	float temp_tiltback_start_offset;
	float temp_tiltback_speed;
	float temp_tiltback_angle;
	bool enable_reverse_stop;
	bool enable_quickstop;
	uint16_t quickstop_erpm;
	float quickstop_angle;
	uint16_t startup_click_current;
} balance_config;

// DATATYPES_H_
#endif
