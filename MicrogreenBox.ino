#define HEATSETPOINT 300
#define HYSTERESIS 2

#define FANONDURATION 5          // SECONDS
#define FANCYCLEDURATION 10      // MINUTES
#define DARKPERIOD 6             // DAYS
#define LIGHTONDURATION 16       // HOURS
#define LIGHTCYCLEDURATION 24    // HOURS
#define PUMPONDURATION 60        // SECONDS
#define PUMPCYCLEDURATION 6      // HOURS
#define GERMPUMPCYCLEDURATION 6  // HOURS
#define GROWPUMPCYCLEDURATION 8  // HOURS
#define HVSTPUMPCYCLEDURATION 12 // HOURS

#define GROWPHASE 6              // DAYS
#define HARVESTPHASE 9           // DAYS

#define HEATRELAYPIN 4           // Heat relay pin number
#define FANRELAYPIN 5            // Fan relay pin number
#define LIGHTRELAYPIN 6          // Light relay pin number
#define PUMPRELAYPIN 7           // Pump relay pin number
#define LIGHTBUTTONPIN 11         // Button grounds out this pin, NOTE: pin mode is INPUT_PULLUP
#define PUMPBUTTONPIN 12         // Button grounds out this pin, NOTE: pin mode is INPUT_PULLUP
#define THERMISTORPIN 0          // Thermistor pin number, put thermistor at seed level in tray over heatpad

const unsigned long msDay = 86400000;  // Number of milliseconds in a day: 86400000
const unsigned long msHour = 3600000;  // Number of milliseconds in an hour: 3600000
const unsigned long msMinute = 60000;  // Number of milliseconds in a minute: 60000
const unsigned long msSecond = 1000;   // Number of milliseconds in a second: 1000

unsigned long pumpCycleStart = 0;
unsigned long pumpCycleTime = 0;
unsigned long pumpCycleDuration = PUMPCYCLEDURATION * msHour;
unsigned long pumpOnDuration = PUMPONDURATION * msSecond;
unsigned long germPumpCycleDuration = GERMPUMPCYCLEDURATION * msHour;
unsigned long growPumpCycleDuration = GROWPUMPCYCLEDURATION * msHour;
unsigned long hvstPumpCycleDuration = HVSTPUMPCYCLEDURATION * msHour;

unsigned long darkPeriod = DARKPERIOD * msDay;
unsigned long lightCycleStart = 0;
unsigned long lightCycleTime = 0;
unsigned long lightCycleDuration = LIGHTCYCLEDURATION * msHour;
unsigned long lightOnDuration = LIGHTONDURATION * msHour;

unsigned long fanCycleStart = 0;
unsigned long fanCycleTime = 0;
unsigned long fanCycleDuration = FANCYCLEDURATION * msMinute;
unsigned long fanOnDuration = FANONDURATION * msSecond;

unsigned long growPhase = GROWPHASE * msDay;
unsigned long hvstPhase = HARVESTPHASE * msDay;

int hysteresis = HYSTERESIS;
int heatSetPoint = HEATSETPOINT;
int lighttoggle = 0;

void setup() {
  Serial.begin(9600);

  pinMode(HEATRELAYPIN, OUTPUT);
  pinMode(FANRELAYPIN, OUTPUT);
  pinMode(LIGHTRELAYPIN, OUTPUT);
  pinMode(PUMPRELAYPIN, OUTPUT);
  pinMode(LIGHTBUTTONPIN, INPUT_PULLUP);
  pinMode(PUMPBUTTONPIN, INPUT_PULLUP);
  pinMode(THERMISTORPIN, INPUT);
}

void loop() {

  // Read thermistor 100 times and take the average to reduce sensor readout variation and reduce relay chatter

  long average = 0;
  for (int i = 0; i < 100; i++) {
    average = average + analogRead(THERMISTORPIN);
    delay(10);
  }
  int heatSensor = average / 100;
  Serial.print("heatSensor: "); Serial.print(heatSensor); Serial.print(" heatSetPoint: "); Serial.println(heatSetPoint);

  // Turn heater on or off based on heatSensor reading

  if (heatSensor < heatSetPoint) {
    digitalWrite(HEATRELAYPIN, HIGH);
    Serial.print(heatSensor); Serial.println(" Heat On +++++++++++++++++++++++");
  }
  if (heatSensor > heatSetPoint + hysteresis) {
    digitalWrite(HEATRELAYPIN, LOW);
    Serial.print(heatSensor); Serial.println(" Heat Off ----------------------");
  }

  // Get current clock time
  unsigned long currentTime = millis();

  // PUMP CYCLE

  // Get current cycle time
  pumpCycleTime = currentTime - pumpCycleStart;

  // Adjust pump cycle duration based on phase
  if (currentTime < growPhase) {
    pumpCycleDuration = germPumpCycleDuration;
  }
  if (currentTime > growPhase && currentTime < hvstPhase) {
    pumpCycleDuration = growPumpCycleDuration;
  }
  if (currentTime > hvstPhase) {
    pumpCycleDuration = hvstPumpCycleDuration;
  }

  // Check to see if it is time to start pump at end of cycle, if not, turn off
  if (pumpCycleTime < pumpCycleDuration - pumpOnDuration) {
    digitalWrite(PUMPRELAYPIN, LOW);
    Serial.print("PUMP OFF, "); Serial.print((pumpCycleDuration - pumpOnDuration - pumpCycleTime) / msHour); Serial.println(" Hours to Pump Run");
  }

  // Check to see if it is time to start pump at end of cycle, if so, turn on
  if (pumpCycleTime > pumpCycleDuration - pumpOnDuration) {
    digitalWrite(PUMPRELAYPIN, HIGH);
    Serial.println("PUMP ++++++++++++++++++++++++++++++++++++++++++++");
    Serial.print("PUMP ON, "); Serial.print((pumpCycleDuration - pumpCycleTime) / msSecond); Serial.println(" seconds remaining");
  }

  // Check if current cycle has ended, if so, start new cycle
  if (currentTime > pumpCycleStart + pumpCycleDuration) {
    pumpCycleStart = currentTime;
  }

  // END PUMP CYCLE

  // BEGIN LIGHT CYCLE

  lightCycleTime = currentTime - lightCycleStart;

  if (lightCycleTime < lightOnDuration) {
    if (currentTime > darkPeriod) {
      digitalWrite(LIGHTRELAYPIN, HIGH);
      lighttoggle = 1;
      Serial.println("LIGHT ++++++++++++++++++++++++++++++++++++++++++++");
      Serial.print("LIGHT ON, "); Serial.print((lightOnDuration - lightCycleTime) / msHour); Serial.println(" hours remaining");
    }
  }

  if (lightCycleTime > lightOnDuration) {
    digitalWrite(LIGHTRELAYPIN, LOW);
    lighttoggle = 0;
    Serial.println("LIGHT OFF");

    if (lightCycleDuration > lightCycleTime) {
      Serial.print((lightCycleDuration - lightCycleTime) / msHour);  Serial.println(" Hours remaining");
    }
    else {
      Serial.println("ENDING LIGHT CYCLE");
    }
  }

  // Check if current cycle has ended, if so, start new cycle
  if (currentTime > lightCycleStart + lightCycleDuration) {
    lightCycleStart = currentTime;
  }

  // END LIGHT CYCLE

  // BEGIN FAN CYCLE

  fanCycleTime = currentTime - fanCycleStart;

  if (fanCycleTime < fanOnDuration) {
    digitalWrite(FANRELAYPIN, HIGH);
    Serial.println("FAN ++++++++++++++++++++++++++++++++++++++++++++");
    Serial.print("FAN ON, "); Serial.print((fanOnDuration - fanCycleTime) / msSecond); Serial.println(" seconds remaining");
  }

  if (fanCycleTime > fanOnDuration) {
    if (currentTime < hvstPhase) {
      digitalWrite(FANRELAYPIN, LOW);
    }
    Serial.print("FAN OFF");

    if (fanCycleDuration > fanCycleTime) {
      Serial.print(", "); Serial.print((fanCycleDuration - fanCycleTime) / msMinute);  Serial.println(" minutes remaining");
    }
    else {
      Serial.println();
    }
  }

  // Check if current cycle has ended, if so, start new cycle
  if (currentTime > fanCycleStart + fanCycleDuration) {
    fanCycleStart = currentTime;
  }

  // END FAN CYCLE

  // OVER HEAT FAN RUN

  if (heatSensor > heatSetPoint + 10) {
    digitalWrite(FANRELAYPIN, HIGH);
    Serial.print(heatSensor); Serial.println(" HIGH HEAT - FAN ON");
    delay(5000);
    digitalWrite(FANRELAYPIN, LOW);
  }

  // BUTTON ACTIVATION OF PUMP CYCLE
  // Set pin 12 to INPUT_PULLUP - pinMode(PUMPBUTTONPIN, INPUT_PULLUP) and ground pin 12 with button activate pump cycle

  if (digitalRead(PUMPBUTTONPIN) == LOW) {
    digitalWrite(PUMPRELAYPIN, HIGH); // Run pump for period defined in PUMPONTIME
    Serial.println("Manual Pump Activiation, Pausing Loop...");
    delay(pumpOnDuration);
    digitalWrite(PUMPRELAYPIN, LOW);  // Shut pump off and return to loop
  }

  //BUTTON ACTIVATION OF LIGHT
  if (digitalRead(LIGHTBUTTONPIN) == LOW && lighttoggle == 0) {
    digitalWrite(LIGHTRELAYPIN, HIGH);
  }
  if (digitalRead(LIGHTBUTTONPIN) == HIGH && lighttoggle == 0) {
    digitalWrite(LIGHTRELAYPIN, LOW);
  }

  delay(2000);
  Serial.println();
}
