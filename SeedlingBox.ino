#define HEATSETPOINT 300
#define HYSTERESIS 2

#define FANONDURATION 5          // SECONDS
#define FANCYCLEDURATION 10      // MINUTES
#define LIGHTONDURATION 16       // HOURS
#define LIGHTCYCLEDURATION 24    // HOURS
#define PUMPONDURATION 60        // SECONDS
#define PUMPCYCLEDURATION 96     // HOURS

#define HEATRELAYPIN 4           // Heat relay pin number
#define FANRELAYPIN 5            // Fan relay pin number
#define LIGHTRELAYPIN 6          // Light relay pin number
#define PUMPRELAYPIN 7           // Pump relay pin number
#define BUTTONPIN 12             // Button grounds out this pin, NOTE: pin mode is INPUT_PULLUP
#define THERMISTORPIN 0          // Thermistor pin number, put thermistor at seed level in sunflower tray

const unsigned long msDay = 86400000;  // Number of milliseconds in a day: 86400000
const unsigned long msHour = 3600000;  // Number of milliseconds in an hour: 3600000
const unsigned long msMinute = 60000;  // Number of milliseconds in a minute: 60000
const unsigned long msSecond = 1000;   // Number of milliseconds in a second: 1000

unsigned long pumpCycleStart = 0;
unsigned long pumpCycleTime = 0;
unsigned long pumpCycleDuration = PUMPCYCLEDURATION * msHour;
unsigned long pumpOnDuration = PUMPONDURATION * msSecond;

unsigned long lightCycleStart = 0;
unsigned long lightCycleTime = 0;
unsigned long lightCycleDuration = LIGHTCYCLEDURATION * msHour;
unsigned long lightOnDuration = LIGHTONDURATION * msHour;

unsigned long fanCycleStart = 0;
unsigned long fanCycleTime = 0;
unsigned long fanCycleDuration = FANCYCLEDURATION * msMinute;
unsigned long fanOnDuration = FANONDURATION * msSecond;

int hysteresis = HYSTERESIS;
int heatSetPoint = HEATSETPOINT;

void setup() {
  Serial.begin(9600);

  pinMode(HEATRELAYPIN, OUTPUT);
  pinMode(FANRELAYPIN, OUTPUT);
  pinMode(LIGHTRELAYPIN, OUTPUT);
  pinMode(PUMPRELAYPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);
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

  pumpCycleTime = currentTime - pumpCycleStart;

  if (pumpCycleTime < pumpCycleDuration - pumpOnDuration) {
    digitalWrite(PUMPRELAYPIN, LOW);
    Serial.print("PUMP OFF, "); Serial.print((pumpCycleDuration - pumpOnDuration - pumpCycleTime) / msHour); Serial.println(" Hours to Pump Run");
  }

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

  // LIGHT CYCLE

  lightCycleTime = currentTime - lightCycleStart;

  if (lightCycleTime < lightOnDuration) {
    digitalWrite(LIGHTRELAYPIN, HIGH);
    Serial.println("LIGHT ++++++++++++++++++++++++++++++++++++++++++++");
    Serial.print("LIGHT ON, "); Serial.print((lightOnDuration - lightCycleTime) / msHour); Serial.println(" hours remaining");
  }

  if (lightCycleTime > lightOnDuration) {
    digitalWrite(LIGHTRELAYPIN, LOW);
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

  // FAN CYCLE

  fanCycleTime = currentTime - fanCycleStart;

  if (fanCycleTime < fanOnDuration) {
    digitalWrite(FANRELAYPIN, HIGH);
    Serial.println("FAN ++++++++++++++++++++++++++++++++++++++++++++");
    Serial.print("FAN ON, "); Serial.print((fanOnDuration - fanCycleTime) / msSecond); Serial.println(" seconds remaining");
  }

  if (fanCycleTime > fanOnDuration) {
    digitalWrite(FANRELAYPIN, LOW);
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

  if (digitalRead(BUTTONPIN) == LOW) {
    digitalWrite(PUMPRELAYPIN, HIGH); // Run pump for period defined in PUMPONTIME
    Serial.println("Manual Pump Activiation, Pausing Loop...");
    delay(pumpOnDuration);
    digitalWrite(PUMPRELAYPIN, LOW);  // Shut pump off and return to loop
  }
  
  delay(2000);
  Serial.println();
}
