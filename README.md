This is fork [ESP32C3_BLUETOOTH_AUDIO_VARIO](https://github.com/har-in-air/ESP32C3_BLUETOOTH_AUDIO_VARIO) project, for experiments with separate modules MPU9250, MS5611 and ESP32_WROOM_DevKit_v1 development board.

 Accurate, zero-lag audio variometer using Kalman filter fusion of accelerometer and pressure sensor data. 
 This project uses the [KF4D kalman filter algorithm from the ESP32_IMU_GPS_BARO_VARIO project](https://github.com/har-in-air/ESP32_IMU_BARO_GPS_VARIO/blob/master/offline/kf/compare_kf2_kf3_kf4.ipynb).

Hardware
* ESP32_WROOM_DevKit_v1 (doit esp32 devkit v1) module (4MByte flash, Wi-Fi and Bluetooth LE)
* MPU9250 and MS5611 barometric pressure sensor
