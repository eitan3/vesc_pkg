// This file is autogenerated by VESC Tool

#ifndef CONF_DEFAULT_H_
#define CONF_DEFAULT_H_

// Pitch Throttle
#ifndef APPCONF_BALANCE_PITCH_TH
#define APPCONF_BALANCE_PITCH_TH 20
#endif

// Pitch Throttle (B)
#ifndef APPCONF_BALANCE_PITCH_TH_B
#define APPCONF_BALANCE_PITCH_TH_B 20
#endif

// Pitch Throttle Integral
#ifndef APPCONF_BALANCE_PITCH_THI
#define APPCONF_BALANCE_PITCH_THI 0.005
#endif

// Pitch Throttle Integral (B)
#ifndef APPCONF_BALANCE_PITCH_THI_B
#define APPCONF_BALANCE_PITCH_THI_B 0.005
#endif

// Gyro Throttle
#ifndef APPCONF_BALANCE_GYRO_TH
#define APPCONF_BALANCE_GYRO_TH 0.6
#endif

// Gyro Throttle (B)
#ifndef APPCONF_BALANCE_GYRO_TH_B
#define APPCONF_BALANCE_GYRO_TH_B 0.6
#endif

// Current Out Filter
#ifndef APPCONF_BALANCE_CURRENT_OUT_FILTER
#define APPCONF_BALANCE_CURRENT_OUT_FILTER 0.4
#endif

// Current Out Filter (B)
#ifndef APPCONF_BALANCE_CURRENT_OUT_FILTER_B
#define APPCONF_BALANCE_CURRENT_OUT_FILTER_B 0.4
#endif

// Tune B Only for Braking?
#ifndef APPCONF_BALANCE_TUNE_B_ONLY_FOR_BRAKES
#define APPCONF_BALANCE_TUNE_B_ONLY_FOR_BRAKES 1
#endif

// Tune (A) To Tune (B) Speed
#ifndef APPCONF_BALANCE_NORMAL_TO_BRAKE_SPEED
#define APPCONF_BALANCE_NORMAL_TO_BRAKE_SPEED 4
#endif

// Tune (B) to Tune (A) Speed
#ifndef APPCONF_BALANCE_BRAKE_TO_NORMAL_SPEED
#define APPCONF_BALANCE_BRAKE_TO_NORMAL_SPEED 1.5
#endif

// Tunes Miximg
#ifndef APPCONF_BALANCE_TUNES_MIXING
#define APPCONF_BALANCE_TUNES_MIXING 0
#endif

// Asym Start ERPM
#ifndef APPCONF_BALANCE_ASYM_ERPM_START
#define APPCONF_BALANCE_ASYM_ERPM_START 750
#endif

// Asym Min Acceleration
#ifndef APPCONF_BALANCE_ASYM_MIN_ACCEL
#define APPCONF_BALANCE_ASYM_MIN_ACCEL 0.75
#endif

// Asym Max Acceleration
#ifndef APPCONF_BALANCE_ASYM_MAX_ACCEL
#define APPCONF_BALANCE_ASYM_MAX_ACCEL 2
#endif

// Asym Min Duty Cycle
#ifndef APPCONF_BALANCE_ASYM_MIN_DUTY
#define APPCONF_BALANCE_ASYM_MIN_DUTY 65
#endif

// Asym Max Duty Cycle
#ifndef APPCONF_BALANCE_ASYM_MAX_DUTY
#define APPCONF_BALANCE_ASYM_MAX_DUTY 90
#endif

// Loop Hertz
#ifndef APPCONF_BALANCE_HERTZ
#define APPCONF_BALANCE_HERTZ 800
#endif

// Pitch Axis Fault Cutoff
#ifndef APPCONF_BALANCE_FAULT_PITCH
#define APPCONF_BALANCE_FAULT_PITCH 90
#endif

// Roll Axis Fault Cutoff
#ifndef APPCONF_BALANCE_FAULT_ROLL
#define APPCONF_BALANCE_FAULT_ROLL 90
#endif

// ADC1 Switch Voltage
#ifndef APPCONF_BALANCE_FAULT_ADC1
#define APPCONF_BALANCE_FAULT_ADC1 2.5
#endif

// ADC2 Switch Voltage
#ifndef APPCONF_BALANCE_FAULT_ADC2
#define APPCONF_BALANCE_FAULT_ADC2 2.5
#endif

// Pitch Fault Delay
#ifndef APPCONF_BALANCE_FAULT_DELAY_PITCH
#define APPCONF_BALANCE_FAULT_DELAY_PITCH 1000
#endif

// Roll Fault Delay
#ifndef APPCONF_BALANCE_FAULT_DELAY_ROLL
#define APPCONF_BALANCE_FAULT_DELAY_ROLL 1000
#endif

// Half Switch Fault Delay
#ifndef APPCONF_BALANCE_FAULT_DELAY_SWITCH_HALF
#define APPCONF_BALANCE_FAULT_DELAY_SWITCH_HALF 250
#endif

// Full Switch Fault Delay
#ifndef APPCONF_BALANCE_FAULT_DELAY_SWITCH_FULL
#define APPCONF_BALANCE_FAULT_DELAY_SWITCH_FULL 1000
#endif

// ADC Half State Fault ERPM
#ifndef APPCONF_BALANCE_FAULT_ADC_HALF_ERPM
#define APPCONF_BALANCE_FAULT_ADC_HALF_ERPM 300
#endif

// Treat both sensors as one
#ifndef APPCONF_BALANCE_FAULT_IS_SINGLE_SWITCH
#define APPCONF_BALANCE_FAULT_IS_SINGLE_SWITCH 0
#endif

// ADC To Copy
#ifndef APPCONF_BALANCE_FAULT_ADC_TO_COPY
#define APPCONF_BALANCE_FAULT_ADC_TO_COPY 0
#endif

// Angle
#ifndef APPCONF_BALANCE_TILTBACK_DUTY_ANGLE
#define APPCONF_BALANCE_TILTBACK_DUTY_ANGLE 8
#endif

// Speed
#ifndef APPCONF_BALANCE_TILTBACK_DUTY_SPEED
#define APPCONF_BALANCE_TILTBACK_DUTY_SPEED 3
#endif

// Duty Cycle
#ifndef APPCONF_BALANCE_TILTBACK_DUTY
#define APPCONF_BALANCE_TILTBACK_DUTY 0.8
#endif

// Angle
#ifndef APPCONF_BALANCE_TILTBACK_HV_ANGLE
#define APPCONF_BALANCE_TILTBACK_HV_ANGLE 5
#endif

// Speed
#ifndef APPCONF_BALANCE_TILTBACK_HV_SPEED
#define APPCONF_BALANCE_TILTBACK_HV_SPEED 1
#endif

// High Voltage
#ifndef APPCONF_BALANCE_TILTBACK_HV
#define APPCONF_BALANCE_TILTBACK_HV 81
#endif

// Angle
#ifndef APPCONF_BALANCE_TILTBACK_LV_ANGLE
#define APPCONF_BALANCE_TILTBACK_LV_ANGLE 10
#endif

// Speed
#ifndef APPCONF_BALANCE_TILTBACK_LV_SPEED
#define APPCONF_BALANCE_TILTBACK_LV_SPEED 1
#endif

// Low Voltage
#ifndef APPCONF_BALANCE_TILTBACK_LV
#define APPCONF_BALANCE_TILTBACK_LV 58.9
#endif

// Return To Level Speed
#ifndef APPCONF_BALANCE_TILTBACK_RETURN_SPEED
#define APPCONF_BALANCE_TILTBACK_RETURN_SPEED 2.5
#endif

// Constant Tiltback
#ifndef APPCONF_BALANCE_TILTBACK_CONSTANT
#define APPCONF_BALANCE_TILTBACK_CONSTANT 0
#endif

// Constant Tiltback ERPM
#ifndef APPCONF_BALANCE_TILTBACK_CONSTANT_ERPM
#define APPCONF_BALANCE_TILTBACK_CONSTANT_ERPM 500
#endif

// Variable Tiltback
#ifndef APPCONF_BALANCE_TILTBACK_VARIABLE
#define APPCONF_BALANCE_TILTBACK_VARIABLE 0.1
#endif

// Variable Tiltback Maximum
#ifndef APPCONF_BALANCE_TILTBACK_VARIABLE_MAX
#define APPCONF_BALANCE_TILTBACK_VARIABLE_MAX 1.5
#endif

// Nose Angling Speed
#ifndef APPCONF_BALANCE_NOSEANGLING_SPEED
#define APPCONF_BALANCE_NOSEANGLING_SPEED 5
#endif

// Startup Pitch Axis Angle Tolerance
#ifndef APPCONF_BALANCE_STARTUP_PITCH_TOLERANCE
#define APPCONF_BALANCE_STARTUP_PITCH_TOLERANCE 5
#endif

// Startup Roll Axis Angle Tolerance
#ifndef APPCONF_BALANCE_STARTUP_ROLL_TOLERANCE
#define APPCONF_BALANCE_STARTUP_ROLL_TOLERANCE 45
#endif

// Startup Centering Speed
#ifndef APPCONF_BALANCE_STARTUP_SPEED
#define APPCONF_BALANCE_STARTUP_SPEED 60
#endif

// Brake Current
#ifndef APPCONF_BALANCE_BRAKE_CURRENT
#define APPCONF_BALANCE_BRAKE_CURRENT 6
#endif

// Brake max amp change
#ifndef APPCONF_BALANCE_BRAKE_MAX_AMPS
#define APPCONF_BALANCE_BRAKE_MAX_AMPS 20
#endif

// Brake max amp change (B)
#ifndef APPCONF_BALANCE_BRAKE_MAX_AMPS_B
#define APPCONF_BALANCE_BRAKE_MAX_AMPS_B 20
#endif

// Pitch Throttle Integral Limit
#ifndef APPCONF_BALANCE_PITCH_THI_LIMIT
#define APPCONF_BALANCE_PITCH_THI_LIMIT 30
#endif

// Pitch Throttle Integral Limit (B)
#ifndef APPCONF_BALANCE_PITCH_THI_LIMIT_B
#define APPCONF_BALANCE_PITCH_THI_LIMIT_B 30
#endif

// Integral Decay on Wheelslip
#ifndef APPCONF_BALANCE_PITCH_THI_DECAY_ON_WHEELSLIP
#define APPCONF_BALANCE_PITCH_THI_DECAY_ON_WHEELSLIP -1
#endif

// Integral Decay on Wheelslip (B)
#ifndef APPCONF_BALANCE_PITCH_THI_DECAY_ON_WHEELSLIP_B
#define APPCONF_BALANCE_PITCH_THI_DECAY_ON_WHEELSLIP_B -1
#endif

// Start Current Threshold (A)
#ifndef APPCONF_BALANCE_TORQUETILT_START_CURRENT
#define APPCONF_BALANCE_TORQUETILT_START_CURRENT 15
#endif

// Start Current Threshold (B)
#ifndef APPCONF_BALANCE_TORQUETILT_START_CURRENT_B
#define APPCONF_BALANCE_TORQUETILT_START_CURRENT_B 15
#endif

// Tilitback Angle Limit
#ifndef APPCONF_BALANCE_TORQUETILT_ANGLE_LIMIT
#define APPCONF_BALANCE_TORQUETILT_ANGLE_LIMIT 8
#endif

// Max Tiltback Speed
#ifndef APPCONF_BALANCE_TORQUETILT_ON_SPEED
#define APPCONF_BALANCE_TORQUETILT_ON_SPEED 5
#endif

// Max Tiltback Release Speed
#ifndef APPCONF_BALANCE_TORQUETILT_OFF_SPEED
#define APPCONF_BALANCE_TORQUETILT_OFF_SPEED 3
#endif

// Strength
#ifndef APPCONF_BALANCE_TORQUETILT_STRENGTH
#define APPCONF_BALANCE_TORQUETILT_STRENGTH 0.15
#endif

// Strength (Regen)
#ifndef APPCONF_BALANCE_TORQUETILT_STRENGTH_REGEN
#define APPCONF_BALANCE_TORQUETILT_STRENGTH_REGEN 0.07
#endif

// Current Filter
#ifndef APPCONF_BALANCE_TORQUETILT_FILTER
#define APPCONF_BALANCE_TORQUETILT_FILTER 10
#endif

// Turntilt Miximg Mode
#ifndef APPCONF_BALANCE_TURNTILT_MIXING_MODE
#define APPCONF_BALANCE_TURNTILT_MIXING_MODE 2
#endif

// Roll Turntilt Weight
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_WEIGHT
#define APPCONF_BALANCE_ROLL_TURNTILT_WEIGHT 0.5
#endif

// Strength
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_STRENGTH
#define APPCONF_BALANCE_ROLL_TURNTILT_STRENGTH 3
#endif

// Tilitback Angle Limit
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_ANGLE_LIMIT
#define APPCONF_BALANCE_ROLL_TURNTILT_ANGLE_LIMIT 3
#endif

// Roll Angle Threshold
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_START_ANGLE
#define APPCONF_BALANCE_ROLL_TURNTILT_START_ANGLE 2
#endif

// ERPM Threshold
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_START_ERPM
#define APPCONF_BALANCE_ROLL_TURNTILT_START_ERPM 1000
#endif

// Max Tiltback Speed
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_SPEED
#define APPCONF_BALANCE_ROLL_TURNTILT_SPEED 3
#endif

// Speed Boost %
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_ERPM_BOOST
#define APPCONF_BALANCE_ROLL_TURNTILT_ERPM_BOOST 200
#endif

// Speed Boost Max ERPM
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_ERPM_BOOST_END
#define APPCONF_BALANCE_ROLL_TURNTILT_ERPM_BOOST_END 5000
#endif

// Yaw Turntilt Weight
#ifndef APPCONF_BALANCE_YAW_TURNTILT_WEIGHT
#define APPCONF_BALANCE_YAW_TURNTILT_WEIGHT 0.5
#endif

// Strength
#ifndef APPCONF_BALANCE_YAW_TURNTILT_STRENGTH
#define APPCONF_BALANCE_YAW_TURNTILT_STRENGTH 6
#endif

// Tilitback Angle Limit
#ifndef APPCONF_BALANCE_YAW_TURNTILT_ANGLE_LIMIT
#define APPCONF_BALANCE_YAW_TURNTILT_ANGLE_LIMIT 3
#endif

// Yaw Angle Threshold
#ifndef APPCONF_BALANCE_YAW_TURNTILT_START_ANGLE
#define APPCONF_BALANCE_YAW_TURNTILT_START_ANGLE 2
#endif

// ERPM Threshold
#ifndef APPCONF_BALANCE_YAW_TURNTILT_START_ERPM
#define APPCONF_BALANCE_YAW_TURNTILT_START_ERPM 1000
#endif

// Max Tiltback Speed
#ifndef APPCONF_BALANCE_YAW_TURNTILT_SPEED
#define APPCONF_BALANCE_YAW_TURNTILT_SPEED 5
#endif

// Speed Boost %
#ifndef APPCONF_BALANCE_YAW_TURNTILT_ERPM_BOOST
#define APPCONF_BALANCE_YAW_TURNTILT_ERPM_BOOST 200
#endif

// Speed Boost Max ERPM
#ifndef APPCONF_BALANCE_YAW_TURNTILT_ERPM_BOOST_END
#define APPCONF_BALANCE_YAW_TURNTILT_ERPM_BOOST_END 5000
#endif

// Yaw Aggregate Target
#ifndef APPCONF_BALANCE_YAW_TURNTILT_AGGREGATE
#define APPCONF_BALANCE_YAW_TURNTILT_AGGREGATE 90
#endif

// Temp tiltback start offset
#ifndef APPCONF_BALANCE_TEMP_TILTBACK_START
#define APPCONF_BALANCE_TEMP_TILTBACK_START 3
#endif

// Temp tiltback speed
#ifndef APPCONF_BALANCE_TEMP_TILTBACK_SPEED
#define APPCONF_BALANCE_TEMP_TILTBACK_SPEED 3
#endif

// Temp tiltback angle
#ifndef APPCONF_BALANCE_TEMP_TILTBACK_ANGLE
#define APPCONF_BALANCE_TEMP_TILTBACK_ANGLE 2
#endif

// Reverse Stop
#ifndef APPCONF_BALANCE_REVERSE_STOP
#define APPCONF_BALANCE_REVERSE_STOP 0
#endif

// Enable Quickstop
#ifndef APPCONF_BALANCE_ENABLE_QUICKSTOP
#define APPCONF_BALANCE_ENABLE_QUICKSTOP 1
#endif

// Quickstop max ERPM
#ifndef APPCONF_BALANCE_QUICKSTOP_ERPM
#define APPCONF_BALANCE_QUICKSTOP_ERPM 200
#endif

// Quickstop start angle
#ifndef APPCONF_BALANCE_QUICKSTOP_ANGLE
#define APPCONF_BALANCE_QUICKSTOP_ANGLE 14
#endif

// Startup Click Current
#ifndef APPCONF_BALANCE_STARTUP_CLICK_CURRENT
#define APPCONF_BALANCE_STARTUP_CLICK_CURRENT 20
#endif

// Enable Traction Control
#ifndef APPCONF_BALANCE_ENABLE_TRACTION_CONTROL
#define APPCONF_BALANCE_ENABLE_TRACTION_CONTROL 1
#endif

// Decrease Current While TC
#ifndef APPCONF_BALANCE_TRACTION_CONTROL_MUL_BY
#define APPCONF_BALANCE_TRACTION_CONTROL_MUL_BY 0.75
#endif

// Booster Start Pitch
#ifndef APPCONF_BALANCE_BOOSTER_MIN_PITCH
#define APPCONF_BALANCE_BOOSTER_MIN_PITCH 0
#endif

// Booster Max Pitch
#ifndef APPCONF_BALANCE_BOOSTER_MAX_PITCH
#define APPCONF_BALANCE_BOOSTER_MAX_PITCH 2
#endif

// Booster Pitch Scale
#ifndef APPCONF_BALANCE_BOOSTER_PITCH_SCALE
#define APPCONF_BALANCE_BOOSTER_PITCH_SCALE 1
#endif

// Booster Base Amps
#ifndef APPCONF_BALANCE_BOOSTER_BASE
#define APPCONF_BALANCE_BOOSTER_BASE 15
#endif

// Booster Exponent
#ifndef APPCONF_BALANCE_BOOSTER_EXPONENT
#define APPCONF_BALANCE_BOOSTER_EXPONENT 1
#endif

// Booster Out Scale
#ifndef APPCONF_BALANCE_BOOSTER_OUT_SCALE
#define APPCONF_BALANCE_BOOSTER_OUT_SCALE 1
#endif

// Booster Max Current
#ifndef APPCONF_BALANCE_BOOSTER_LIMIT
#define APPCONF_BALANCE_BOOSTER_LIMIT 15
#endif

// Booster Start Pitch (B)
#ifndef APPCONF_BALANCE_BOOSTER_MIN_PITCH_B
#define APPCONF_BALANCE_BOOSTER_MIN_PITCH_B 0
#endif

// Booster Max Pitch (B)
#ifndef APPCONF_BALANCE_BOOSTER_MAX_PITCH_B
#define APPCONF_BALANCE_BOOSTER_MAX_PITCH_B 2
#endif

// Booster Pitch Scale (B)
#ifndef APPCONF_BALANCE_BOOSTER_PITCH_SCALE_B
#define APPCONF_BALANCE_BOOSTER_PITCH_SCALE_B 1
#endif

// Booster Base Amps (B)
#ifndef APPCONF_BALANCE_BOOSTER_BASE_B
#define APPCONF_BALANCE_BOOSTER_BASE_B 15
#endif

// Booster Exponent (B)
#ifndef APPCONF_BALANCE_BOOSTER_EXPONENT_B
#define APPCONF_BALANCE_BOOSTER_EXPONENT_B 1
#endif

// Booster Out Scale (B)
#ifndef APPCONF_BALANCE_BOOSTER_OUT_SCALE_B
#define APPCONF_BALANCE_BOOSTER_OUT_SCALE_B 1
#endif

// Booster Max Current (B)
#ifndef APPCONF_BALANCE_BOOSTER_LIMIT_B
#define APPCONF_BALANCE_BOOSTER_LIMIT_B 15
#endif

// CONF_DEFAULT_H_
#endif

