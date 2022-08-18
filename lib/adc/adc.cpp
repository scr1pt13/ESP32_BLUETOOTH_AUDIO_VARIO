#include <Arduino.h>
#include <driver/adc.h>
#include <config.h>
#include "adc.h"

float BatteryVoltage;

void adc_init() {
    adc1_config_width(ADC_WIDTH_BIT_12); // no 10-bit resolution option !?
	// need to ensure maximum voltage at adc pin is < 800mV for ATTEN_DB_0
    adc1_config_channel_atten(CHAN_VBAT, ADC_ATTEN_DB_0);
    }

int adc_sample_average() {
	int adcSample = 0;
		for (int inx = 0; inx < 4; inx++) {
		adcSample += adc1_get_raw(CHAN_VBAT);
		delay(1);
		}
	adcSample /= 16; // 10-bit resolution
	return adcSample;  
	}


// two-point calibration for slope & intercept using ATTEN_DB_0, 10bit resolution
#define V1		3.252f
#define ADC1	723.0f
#define V2		4.080f
#define ADC2	908.0f

float adc_battery_voltage(int sample) {
	float slope = (V2 - V1)/(ADC2 - ADC1);
	float voltage = slope*(sample - ADC1) + V1;
	return voltage;
	}

float adc_battery_voltage() {
	int	adcSample = adc1_get_raw(CHAN_VBAT) / 4;
	float slope = (V2 - V1)/(ADC2 - ADC1);
	float voltage = slope*((float)adcSample - ADC1) + V1;
	return voltage;
	}
