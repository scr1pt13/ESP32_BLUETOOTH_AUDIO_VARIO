#ifndef CONFIG_H_
#define CONFIG_H_


#define pinPCCA		4	// program/configure/calibrate/audio button
#define pinAudio	15	// pwm beeper audio output

#define pinCSB		21	// CSB (ms5611)
#define pinMISO		18	// SDO ms5611 & AD0 mpu9250
#define pinNCS		5 	// NCS (mpu9250)
#define pinMOSI		22	// SDA
#define pinSCK		23	// SCL
#define pinDRDYInt	19  // INT
#define pinLED		2	// power-on and bluetooth active indication



#define BTN_PCCA()  (digitalRead(pinPCCA) == HIGH ? 1 : 0)

#define LED_ON() 	{digitalWrite(pinLED, 0); LEDState = 1;}
#define LED_OFF() 	{digitalWrite(pinLED, 1); LEDState = 0;}

#define PWR_ON_DELAY_MS		1000UL
#define PWR_OFF_DELAY_MS	2000UL

////////////////////////////////////////////////////////////////////
// WEB CONFIGURATION PARAMETER DEFAULTS AND LIMITS

// vario thresholds in cm/sec for generating different
// audio tones. Between the sink threshold and the zero threshold,
// the vario is quiet

#define VARIO_CLIMB_THRESHOLD_CPS_DEFAULT  	30
#define VARIO_CLIMB_THRESHOLD_CPS_MIN   	20
#define VARIO_CLIMB_THRESHOLD_CPS_MAX   	100

#define VARIO_ZERO_THRESHOLD_CPS_DEFAULT  	5
#define VARIO_ZERO_THRESHOLD_CPS_MIN    	-20
#define VARIO_ZERO_THRESHOLD_CPS_MAX    	20

#define VARIO_SINK_THRESHOLD_CPS_DEFAULT  	-200
#define VARIO_SINK_THRESHOLD_CPS_MIN    	-400
#define VARIO_SINK_THRESHOLD_CPS_MAX    	-100

// When generating climbtones, the vario allocates most of the speaker 
// frequency bandwidth to climbrates below this crossover threshold 
// so you have more frequency discrimination. So set the crossover threshold 
// to the average thermal core climbrate you expect for the site and conditions.
#define VARIO_CROSSOVER_CPS_DEFAULT     600
#define VARIO_CROSSOVER_CPS_MIN         300
#define VARIO_CROSSOVER_CPS_MAX         600

// Kalman filter configuration
#define KF_ACCEL_VARIANCE_DEFAULT     100
#define KF_ACCEL_VARIANCE_MIN         50
#define KF_ACCEL_VARIANCE_MAX         150

#define KF_ADAPT_DEFAULT	100
#define KF_ADAPT_MIN		50
#define KF_ADAPT_MAX		150

// Power-off timeout. The vario will power down
// if it does not detect climb or sink rates more than
// PWR_OFF_THRESHOLD_CPS, for the specified minutes.
#define PWR_OFF_TIMEOUT_MINUTES_DEFAULT   10
#define PWR_OFF_TIMEOUT_MINUTES_MIN       5
#define PWR_OFF_TIMEOUT_MINUTES_MAX       20

// audio feedback tones
#define BATTERY_TONE_HZ			400
#define CALIBRATING_TONE_HZ		800
#define UNCALIBRATED_TONE_HZ	2000
#define MPU9250_ERROR_TONE_HZ	200 
#define MS5611_ERROR_TONE_HZ	2500

// BLE LK8EX1 transmission is enabled as default
#define BLE_DEFAULT  true

////////////////////////////////////////////////////////////////////////////////
// COMPILED CONFIGURATION PARAMETERS ( cannot be changed with web configuration )

#define BLE_TASK_PRIORITY		2
#define WIFI_CFG_TASK_PRIORITY	2
#define VARIO_TASK_PRIORITY		(configMAX_PRIORITIES-1)

// change these parameters based on the frequency bandwidth of the speaker
#define VARIO_SPKR_MIN_FREQHZ      	200
#define VARIO_SPKR_MAX_FREQHZ       3200

// three octaves (2:1) of frequency for climbrates below crossoverCps,
// and one octave of frequency for climbrates above crossoverCps.
// This gives you more perceived frequency discrimination for climbrates 
// below crossoverCps
#define VARIO_CROSSOVER_FREQHZ    	1600

#define KF_ADAPT         1.0f

// Acceleration bias uncertainty is set low as the residual acceleration bias 
// (post-calibration) is expected to have low variation/drift. It is further reduced
// depending on the acceleration magnitude, as we want the acceleration bias estimate 
// to evolve ideally in a zero acceleration environment.
#define KF_ACCELBIAS_VARIANCE   	0.005f

// KF4 Acceleration Measurement Noise variance
#define KF_A_MEAS_VARIANCE   		10.0f

// KF4 Altitude Measurement Noise Variance
#define KF_Z_MEAS_VARIANCE			200.0f

// If climbrate or sinkrate stays below this threshold for the configured
// time interval, vario goes to sleep to conserve power
#define PWR_OFF_THRESHOLD_CPS    	50

// if you find that gyro calibration fails even when you leave
// the unit undisturbed, increase this offset limit
// until you find that gyro calibration works consistently.
#define GYRO_OFFSET_LIMIT_1000DPS   500

// print debug information to the serial port for different code modules

#define TOP_DEBUG
#ifdef TOP_DEBUG
	#define dbg_println(x) {Serial.println x;}
	#define dbg_printf(x)  {Serial.printf x;}
	#define dbg_flush()  Serial.flush()
#else
	#define dbg_println(x)
	#define dbg_printf(x)
	#define dbg_flush()
#endif
// these #defines can be left uncommented after debugging, as the enclosed
// debug prints do not appear in the critical run-time loop
#define KF_DEBUG
#define VARIO_DEBUG
#define NVD_DEBUG
#define MPU9250_DEBUG
#define MS5611_DEBUG
#define WEBCFG_DEBUG

// !! ensure these #defines are commented out after debugging, as the 
// enclosed debug prints are in the critical run-time loop.
//#define IMU_DEBUG
//#define PERF_DEBUG
//#define BLE_DEBUG

#endif
