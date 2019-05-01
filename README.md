Arduino Controller for a Microgreen Grow Box, with heater, exhaust vent, lighting, internal circulation fan and flood and drain system.

This system provides a ventilated, temperature controlled box which is secure from insects and rodents, and automates watering of a flood and drain watering system.

This code is under development and will likely change soon.

Features include:

Flood and Drain Pump can be run at a desired frequency (for example, every six hours), with a specified run duration (for example, 60 seconds).

Button switch allows manual running of the pump through one cycle.

Heat Mats are thermostaticaly controlled with a thermistor. The temperature set point can be programmed.  There is an hysteresis setting to avoid frequent on/off relay activity due to sensor swings.

Exhaust fan and circulation fan are controled by the thermistor, and turn on when the heat mats have reached their maximum setpoint.  The system runs in heating mode, then switches to fan mode, then back to heating mode as the internal temperature rises and falls.  This provides a good amount of ventilation air while maintaining a seed germination and growth temperature ideal for the plant.

The LED strip lighting is controlled to run after a germination phase (for example, five days with no light), and then it will switch to a daily light regimen (for example, 8 hours off, 16 hours on).

This project is still in testing and development.  If successful I intend to provide how-to-build instructions as an open source project.

Build Notes:

Sensors:

Tray thermistor (nominal 10k ohms) to 3.3V and analog pin 0 with 10k pulldown resistor from pin 0 to ground. 

Display:
Pin 13 is used to flash the thermistor sensor readout in morse code.

Relays and Devices:

120V pump relay is on Pin 4
12V fan relay is Pin 5
12V LED strip light relay is Pin 6
120V heat mat relay is Pin 7


Paul VandenBosch, 20190421

Suggested Product List (January, 2019):

Arduino Uno Clone: https://www.aliexpress.com/item/1pcs-Smart-Electronics-high-quality-UNO-R3-MEGA328P-CH340G-for-arduino-Compatible-with-USB-CABLE/32803501325.html

Four Relay Shield for Arduino: https://www.aliexpress.com/item/4-Channel-5V-Relay-Swtich-Expansion-Drive-Board-LED-indicator-Relay-Shield-V2-0-Development-Board/32886386019.html

Thermistor on 2m cable: https://www.aliexpress.com/item/2m-200cm-Two-2-Meter-Waterproof-NTC-Thermistor-Accuracy-Temperature-Sensor-10K-1-3950-Wire-Cable/32904302533.html


