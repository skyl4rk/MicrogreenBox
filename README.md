Arduino Controller for a Microgreen Grow Box, with heater, exhaust vent, internal circulation fan and flood and drain system.

This system provides a ventilated, temperature controlled box which is secure from insects and rodents, and automates watering of a flood and drain watering system.

This code is under development and will likely change soon.
Some features not yet tested!

Features include:

Pump can be started up to three times per crop with a timer.
Pump can be controlled or limited by capacitive moisture sensor.
Button switch to manually run pump one time.
Heat Mat Thermostat Control with a thermistor.
Exhaust fan controled by box air temperature.
Air circulation fan runs either continuously or when exhaust fan is off.
Thermistor and moisture sensor values are flashed in morse code.
Morse can be turned off to speed up cycle loop.

This project is still in testing and development.  If successful I intend to provide how-to-build instructions as an open source project.

Build Notes:

Sensors:

Tray thermistor (nominal 10k ohms) to 3.3V and analog pin 0 with 10k pulldown resistor from pin 0 to ground. 
Capacitive Soil Moisture Sensor with AOUT to pin 1, positive to 5V, negative to ground.
Air thermistor (nominal 10k ohms) to 3.3V and analog pin 2 with 10k pulldown resistor from pin 2 to ground. 

Display:

LED on Pin 13 will flash sensor values in morse code
M indicates moisture sensor value, H indicates tray thermistor value, A indicates box air thermistor value.
Morse readout can be disabled by commenting out a section of code.  Disabling the morse readout speeds up the cycle loop time of the code.

Relays and Devices:

120V Pump Relay is on Pin 4
12V Circulation fan relay is Pin 5
12V Exhaust fan relay is Pin 6
120V Heat relay is Pin 7

Pump Options:
Pump will run on timer.
Pump will run on timer, but will only run if soil moisture sensor indicates dry soil.
Pump will run on timer, but will turn off if soil becomes moist during pumping.
Pump will run if sensor indicates dry soil (no timer).
Note: I am currently in testing to determine the best option for watering.

User may change pumpCycleOnDuration to set pump run time (currently 60 seconds)

Ground Pin 12 with a button switch to manually run pump once 

Confirm DRYSOILSET, WETSOILSET, HEATSET values and pump cycle durations prior to use! 

Paul VandenBosch, 20190222

Suggested Product List (January, 2019):

Arduino Uno Clone: https://www.aliexpress.com/item/1pcs-Smart-Electronics-high-quality-UNO-R3-MEGA328P-CH340G-for-arduino-Compatible-with-USB-CABLE/32803501325.html

Four Relay Shield for Arduino: https://www.aliexpress.com/item/4-Channel-5V-Relay-Swtich-Expansion-Drive-Board-LED-indicator-Relay-Shield-V2-0-Development-Board/32886386019.html

Capacitive Soil Moisture Sensor: https://www.aliexpress.com/item/Capacitive-Soil-Moisture-Sensor-Module-Not-Easy-to-Corrode-Wide-Voltage-Wire-3-3-5-5V/32892951369.html

Note: Capacitive Soil Moisture Sensor must be coated with varnish, epoxy or similar waterproof coating.  I used three coats of varnish.

Thermistor on 2m cable: https://www.aliexpress.com/item/2m-200cm-Two-2-Meter-Waterproof-NTC-Thermistor-Accuracy-Temperature-Sensor-10K-1-3950-Wire-Cable/32904302533.html


