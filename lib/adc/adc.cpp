#include <Arduino.h>
#include <config.h>
#include "adc.h"

// https://www.2ram.ru/video/24/

int batteryLevel;

void adc_init()
{
	analogReadResolution(12);
}

int adc_battery_level()
{
	unsigned int val = analogRead(pinADC);
	float v = float(val);
	int p = round(((v - 5446.43f) * -0.000075893f) * (v - 3120.0f));
	if (p < 0)
	{
		p = 0;
	}
	if (p > 100)
	{
		p = 100;
	}
	return p;
}