# STEP_Controller_v3
The `TestMultiButton` folder contains the Arduino sketch needed to program the STEP controller's onboard ESP32.
The `libraries` folder contains additional libraries needed to implement BLE, the encoder wheel, and button debouncing.

NOTE: when flashing, you may need to increase the board's available program storage space.
This can be done by changing the board's `Partition Scheme` config parameter to `Minimal SPIFFS` under the Arduino IDE's `Tools` menu bar.
