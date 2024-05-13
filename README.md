# STEP_Controller_v3
The `TestMultiButton` folder contains the Arduino sketch needed to program the STEP controller's onboard ESP32.
The `libraries` folder contains additional libraries needed to implement BLE, the encoder wheel, and button debouncing.

To flash the controller, connect the TX, RX, and GND pins to an FTDI adapter. PLug the FTDI adapter into your computer, select it from the `Tools` > `Port` menu, and put the ESP32 into boot mode (hold the boot \[soldered to the PCB next to the ESP\] button down while powering the system on).

NOTE: when flashing, you may need to increase the board's available program storage space.
This can be done by changing the board's `Partition Scheme` config parameter to `Minimal SPIFFS (1.9MB APP...)` under the Arduino IDE's `Tools` menu bar.
