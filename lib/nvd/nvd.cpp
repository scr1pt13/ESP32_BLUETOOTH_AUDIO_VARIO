#include <Arduino.h>
#include <Preferences.h>
#include <config.h>
#include "nvd.h"


#define MODE_READ_WRITE  false
#define MODE_READ_ONLY   true

#define CLAMP(x,mn,mx)       {if (x <= (mn)) x = (mn); else if (x >= (mx)) x = (mx);}

// saves and retrieve non-volatile data (NVD) in flash memory
Preferences Prefs;
CALIB_PARAMS_t Calib;
CONFIG_PARAMS_t Config;

void nvd_calib_load(CALIB_PARAMS_t &calib) {
	if (Prefs.begin("calib", MODE_READ_ONLY) == false) {
		dbg_println(("Preferences 'calib' namespace not found, creating with defaults"));
		Prefs.end();
		nvd_calib_reset(calib);
		}
	else {
		calib.axBias = Prefs.getInt("axb", 0); // set to 0 if not found 
		calib.ayBias = Prefs.getInt("ayb", 0);
		calib.azBias = Prefs.getInt("azb", 0);
		calib.gxBias = Prefs.getInt("gxb", 0);
		calib.gyBias = Prefs.getInt("gyb", 0);
		calib.gzBias = Prefs.getInt("gzb", 0);
		Prefs.end();
#ifdef NVD_DEBUG	
		dbg_println(("Calibration Values"));
		dbg_printf(("axBias = %d\n", calib.axBias));
		dbg_printf(("ayBias = %d\n", calib.ayBias));
		dbg_printf(("azBias = %d\n", calib.azBias));
		dbg_printf(("gxBias = %d\n", calib.gxBias));
		dbg_printf(("gyBias = %d\n", calib.gyBias));
		dbg_printf(("gzBias = %d\n", calib.gzBias));
#endif		
		}
	}


void nvd_calib_reset(CALIB_PARAMS_t &calib) {
	calib.axBias = 0;
	calib.ayBias = 0;
	calib.azBias = 0;
	calib.gxBias = 0;
	calib.gyBias = 0;
	calib.gzBias = 0;
	nvd_calib_store(calib);
	}	


void nvd_calib_store(CALIB_PARAMS_t &calib) {
	Prefs.begin("calib", MODE_READ_WRITE);
	Prefs.clear();
	Prefs.putInt("axb", calib.axBias);
	Prefs.putInt("ayb", calib.ayBias);
	Prefs.putInt("azb", calib.azBias);
	Prefs.putInt("gxb", calib.gxBias);
	Prefs.putInt("gyb", calib.gyBias);
	Prefs.putInt("gzb", calib.gzBias);
	Prefs.end();
	}


void nvd_config_load(CONFIG_PARAMS_t &config) {
	if (Prefs.begin("config", MODE_READ_ONLY) == false) {
		dbg_println(("Preferences 'config' namespace not found, creating with defaults"));
		Prefs.end();
		nvd_config_reset(config);
		}
	else {
		config.cred.ssid = Prefs.getString("ssid", "");
		config.cred.password = Prefs.getString("passwd", "");

		config.vario.climbThresholdCps = Prefs.getInt("climb", VARIO_CLIMB_THRESHOLD_CPS_DEFAULT);
		config.vario.zeroThresholdCps = Prefs.getInt("zero", VARIO_ZERO_THRESHOLD_CPS_DEFAULT);
		config.vario.sinkThresholdCps = Prefs.getInt("sink", VARIO_SINK_THRESHOLD_CPS_DEFAULT);
		config.vario.crossoverCps = Prefs.getInt("xover", VARIO_CROSSOVER_CPS_DEFAULT);

		config.kf.accelVariance = Prefs.getInt("avar", KF_ACCEL_VARIANCE_DEFAULT);
		config.kf.adapt = Prefs.getInt("adapt", KF_ADAPT_DEFAULT);

		config.misc.bleEnable = Prefs.getBool("ble", BLE_DEFAULT);
		config.misc.pwrOffTimeoutMinutes = Prefs.getInt("timeout", PWR_OFF_TIMEOUT_MINUTES_DEFAULT);
		Prefs.end();
		
		CLAMP(config.vario.climbThresholdCps, VARIO_CLIMB_THRESHOLD_CPS_MIN, VARIO_CLIMB_THRESHOLD_CPS_MAX);
		CLAMP(config.vario.zeroThresholdCps, VARIO_ZERO_THRESHOLD_CPS_MIN, VARIO_ZERO_THRESHOLD_CPS_MAX);
		CLAMP(config.vario.sinkThresholdCps, VARIO_SINK_THRESHOLD_CPS_MIN, VARIO_SINK_THRESHOLD_CPS_MAX);
		CLAMP(config.vario.crossoverCps, VARIO_CROSSOVER_CPS_MIN, VARIO_CROSSOVER_CPS_MAX);

		CLAMP(config.kf.accelVariance, KF_ACCEL_VARIANCE_MIN, KF_ACCEL_VARIANCE_MAX);
		CLAMP(config.kf.adapt, KF_ADAPT_MIN, KF_ADAPT_MAX);

		CLAMP(config.misc.pwrOffTimeoutMinutes, PWR_OFF_TIMEOUT_MINUTES_MIN, PWR_OFF_TIMEOUT_MINUTES_MAX);

#ifdef NVD_DEBUG	
		dbg_println(("WiFi AP credentials"));
		dbg_printf(("SSID = %s\n", config.cred.ssid));

		dbg_println(("VARIO"));
		dbg_printf(("climbThresholdCps = %d\n", config.vario.climbThresholdCps));
		dbg_printf(("zeroThresholdCps = %d\n", config.vario.zeroThresholdCps));
		dbg_printf(("sinkThresholdCps = %d\n", config.vario.sinkThresholdCps));
		dbg_printf(("crossoverCps = %d\n", config.vario.crossoverCps));
		
		dbg_println(("KALMAN FILTER"));
		dbg_printf(("accelVariance = %d\n", config.kf.accelVariance));
		dbg_printf(("adapt = %d\n", config.kf.adapt));
			
		dbg_println(("MISCELLANEOUS"));
		dbg_printf(("pwrOffTimeoutMinutes = %d\n", config.misc.pwrOffTimeoutMinutes));
		dbg_printf(("bleEnable = %s\n", config.misc.bleEnable ? "true" : "false"));
#endif
		}
	}


void nvd_config_reset(CONFIG_PARAMS_t &config) {
	config.cred.ssid = "";
	config.cred.password = "";

	config.vario.climbThresholdCps = VARIO_CLIMB_THRESHOLD_CPS_DEFAULT;
	config.vario.zeroThresholdCps = VARIO_ZERO_THRESHOLD_CPS_DEFAULT;
	config.vario.sinkThresholdCps = VARIO_SINK_THRESHOLD_CPS_DEFAULT;
	config.vario.crossoverCps = VARIO_CROSSOVER_CPS_DEFAULT;

	config.kf.accelVariance = KF_ACCEL_VARIANCE_DEFAULT;
	config.kf.adapt = KF_ADAPT_DEFAULT;

	config.misc.bleEnable = BLE_DEFAULT;
	config.misc.pwrOffTimeoutMinutes = PWR_OFF_TIMEOUT_MINUTES_DEFAULT;
	nvd_config_store(config);
	}


void nvd_config_store(CONFIG_PARAMS_t &config) {
	Prefs.begin("config", MODE_READ_WRITE);
	Prefs.clear();
	Prefs.putString("ssid", config.cred.ssid);
	Prefs.putString("passwd", config.cred.password);

	Prefs.putInt("climb", config.vario.climbThresholdCps);
	Prefs.putInt("zero", config.vario.zeroThresholdCps);
	Prefs.putInt("sink", config.vario.sinkThresholdCps);
	Prefs.putInt("xover", config.vario.crossoverCps);

	Prefs.putInt("avar", config.kf.accelVariance);
	Prefs.putInt("adapt", config.kf.adapt);

	Prefs.putBool("ble", config.misc.bleEnable);
	Prefs.putInt("timeout", config.misc.pwrOffTimeoutMinutes);
	Prefs.end();
	}	

