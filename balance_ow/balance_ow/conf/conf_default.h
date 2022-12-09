// This file is autogenerated by VESC Tool

#ifndef CONF_DEFAULT_H_
#define CONF_DEFAULT_H_

// Angle P
#ifndef APPCONF_BALANCE_KP
#define APPCONF_BALANCE_KP 25
#endif

// Angle I
#ifndef APPCONF_BALANCE_KI
#define APPCONF_BALANCE_KI 0.005
#endif

// Rate P
#ifndef APPCONF_BALANCE_KP2
#define APPCONF_BALANCE_KP2 0.6
#endif

// Rate I
#ifndef APPCONF_BALANCE_KI2
#define APPCONF_BALANCE_KI2 0
#endif

// PID Filtering Weight
#ifndef APPCONF_BALANCE_PID_FILTERING_WEIGHT
#define APPCONF_BALANCE_PID_FILTERING_WEIGHT 0.3
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
#define APPCONF_BALANCE_FAULT_ADC1 3
#endif

// ADC2 Switch Voltage
#ifndef APPCONF_BALANCE_FAULT_ADC2
#define APPCONF_BALANCE_FAULT_ADC2 3
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

// Angle
#ifndef APPCONF_BALANCE_TILTBACK_DUTY_ANGLE
#define APPCONF_BALANCE_TILTBACK_DUTY_ANGLE 5
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
#define APPCONF_BALANCE_TILTBACK_HV_ANGLE 8
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
#define APPCONF_BALANCE_TILTBACK_LV 57
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
#ifndef APPCONF_BALANCE_PID_BRAKE_MAX_AMPS
#define APPCONF_BALANCE_PID_BRAKE_MAX_AMPS 5
#endif

// I term limit
#ifndef APPCONF_BALANCE_KI_LIMIT
#define APPCONF_BALANCE_KI_LIMIT 30
#endif

// Start Angle
#ifndef APPCONF_BALANCE_BOOSTER_ANGLE
#define APPCONF_BALANCE_BOOSTER_ANGLE 0
#endif

// Ramp Up
#ifndef APPCONF_BALANCE_BOOSTER_RAMP
#define APPCONF_BALANCE_BOOSTER_RAMP 2
#endif

// Current Boost
#ifndef APPCONF_BALANCE_BOOSTER_CURRENT
#define APPCONF_BALANCE_BOOSTER_CURRENT 15
#endif

// Start Current Threshold
#ifndef APPCONF_BALANCE_TORQUETILT_START_CURRENT
#define APPCONF_BALANCE_TORQUETILT_START_CURRENT 15
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

// Current Filter
#ifndef APPCONF_BALANCE_TORQUETILT_FILTER
#define APPCONF_BALANCE_TORQUETILT_FILTER 10
#endif

// Roll Turntilt Weight
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_WEIGHT
#define APPCONF_BALANCE_ROLL_TURNTILT_WEIGHT 0.5
#endif

// Strength
#ifndef APPCONF_BALANCE_ROLL_TURNTILT_STRENGTH
#define APPCONF_BALANCE_ROLL_TURNTILT_STRENGTH 0
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
#define APPCONF_BALANCE_YAW_TURNTILT_STRENGTH 0
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
#define APPCONF_BALANCE_ENABLE_QUICKSTOP 0
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
#define APPCONF_BALANCE_STARTUP_CLICK_CURRENT 5
#endif

// Number of clicks on startup
#ifndef APPCONF_BALANCE_STARTUP_NUM_OF_CLICKS
#define APPCONF_BALANCE_STARTUP_NUM_OF_CLICKS 3
#endif

// CONF_DEFAULT_H_
#endif

