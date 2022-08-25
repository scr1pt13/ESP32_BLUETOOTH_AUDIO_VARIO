#include <Arduino.h>
#include <NimBLEDevice.h>
#include <config.h>
#include "ble_uart.h"


#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

NimBLEServer* pBLEServer                = NULL;
NimBLEService* pService                 = NULL;
NimBLECharacteristic* pTxCharacteristic = NULL;
NimBLECharacteristic* pRxCharacteristic = NULL;

static uint8_t ble_uart_nmea_checksum(const char *szNMEA);

void ble_uart_init() {
	NimBLEDevice::init("BLE-Vario");
	NimBLEDevice::setMTU(46);
	NimBLEDevice::setPower(ESP_PWR_LVL_N9);
	NimBLEDevice::setSecurityAuth(true, true, true);
	NimBLEDevice::setSecurityPasskey(123456);
	NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

	pBLEServer = NimBLEDevice::createServer();

	pService = pBLEServer->createService(SERVICE_UUID);
	pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, NIMBLE_PROPERTY::NOTIFY);
	pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN);
	pService->start();
	pBLEServer->getAdvertising()->start();
	}


static uint8_t ble_uart_nmea_checksum(const char *szNMEA){
	const char* sz = &szNMEA[1]; // skip leading '$'
	uint8_t cksum = 0;
	while ((*sz) != 0 && (*sz != '*')) {
		cksum ^= (uint8_t) *sz;
		sz++;
		}
	return cksum;
	}

   
void ble_uart_transmit_LK8EX1(int32_t altm, int32_t cps, float batVoltage) {
	char szmsg[40];
	sprintf(szmsg, "$LK8EX1,999999,%d,%d,99,%.1f*", altm, cps, batVoltage);
	uint8_t cksum = ble_uart_nmea_checksum(szmsg);
	char szcksum[5];
	sprintf(szcksum,"%02X\r\n", cksum);
	strcat(szmsg, szcksum);
#ifdef BLE_DEBUG	
    dbg_printf(("%s", szmsg)); 
#endif
	pTxCharacteristic->setValue((const uint8_t*)szmsg, strlen(szmsg));
	pTxCharacteristic->notify();   
	}
