/*
   MicrogreenBox

   An automated microgreen growth chamber control system.

*/

#define HEATSETPOINT 300         // General thermostat setting
#define BOOSTHEATSETPOINT 370    // Initial thermostat setting (80F-85F)
#define GERMHEATSETPOINT 340     // Germination phase thermostat setting (70F-75F)
#define GROWHEATSETPOINT 320     // Grow phase thermostat setting (68F-72F)
#define HARVESTHEATSETPOINT 200  // Harvest phase thermostat setting (62F-65F)
#define HYSTERESIS 2             // Hysteresis between cutoff and restart of heat

#define FANONDURATION 10         // SECONDS - Fan run time
#define FANCYCLEDURATION 20      // MINUTES - Cycle between fan runs
#define DARKPERIOD 6             // DAYS - Days before lights turn on
#define LIGHTONDURATION 16       // HOURS - Hours of light per day
#define LIGHTCYCLEDURATION 24    // HOURS - One day of light
#define PUMPONDURATION 60        // SECONDS - Pump run time
#define PUMPCYCLEDURATION 6      // HOURS - Cycle between pump runs
#define GERMPUMPCYCLEDURATION 3  // HOURS - Germination Phase cycle between pump runs
#define GROWPUMPCYCLEDURATION 5  // HOURS - Grow Phase cycle between pump runs
#define HVSTPUMPCYCLEDURATION 10 // HOURS - Harvest Phase cycle between pump runs

#define BOOSTPHASE 72            // HOURS - Initial phase for starting germination
#define GROWPHASE 5              // DAYS - Start day of Grow Phase
#define HARVESTPHASE 9           // DAYS - Start day of Harvest Phase

#define HEATRELAYPIN 4           // Heat relay pin number
#define FANRELAYPIN 5            // Fan relay pin number
#define LIGHTRELAYPIN 6          // Light relay pin number
#define PUMPRELAYPIN 7           // Pump relay pin number
#define LIGHTBUTTONPIN 11        // Button grounds out this pin, NOTE: pin mode is INPUT_PULLUP
#define PUMPBUTTONPIN 12         // Button grounds out this pin, NOTE: pin mode is INPUT_PULLUP
#define THERMISTORPIN 0          // Thermistor pin number, put thermistor at seed level in tray over heatpad

// Variable type setting, avoid changing variables below this line

const unsigned long msDay = 86400000;  // Number of milliseconds in a day: 86400000 (constant)
const unsigned long msHour = 3600000;  // Number of milliseconds in an hour: 3600000 (constant)
const unsigned long msMinute = 60000;  // Number of milliseconds in a minute: 60000 (constant)
const unsigned long msSecond = 1000;   // Number of milliseconds in a second: 1000 (constant)

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

unsigned long boostPhase = BOOSTPHASE * msHour;
unsigned long growPhase = GROWPHASE * msDay;
unsigned long hvstPhase = HARVESTPHASE * msDay;

int hysteresis = HYSTERESIS;
int heatSetPoint = HEATSETPOINT;
int boostHeatSetPoint = BOOSTHEATSETPOINT;
int germHeatSetPoint = GERMHEATSETPOINT;
int growHeatSetPoint = GROWHEATSETPOINT;
int harvestHeatSetPoint = HARVESTHEATSETPOINT;
int lighttoggle = 0;

// end variable declaration and typesetting

// Simple Arduino Morse Beacon
// ****** Morse Beacon Begins ******

#define N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))
#define SPEED  (14)
#define DOTLEN  (1200/SPEED)
#define DASHLEN  (4*(1200/SPEED))

int morseLEDpin = 13 ;

struct t_mtab {
  char c, pat;
} ;

struct t_mtab morsetab[] = {
  {'.', 106},
  {',', 115},
  {'?', 76},
  {'/', 41},
  {'A', 6},
  {'B', 17},
  {'C', 21},
  {'D', 9},
  {'E', 2},
  {'F', 20},
  {'G', 11},
  {'H', 16},
  {'I', 4},
  {'J', 30},
  {'K', 13},
  {'L', 18},
  {'M', 7},
  {'N', 5},
  {'O', 15},
  {'P', 22},
  {'Q', 27},
  {'R', 10},
  {'S', 8},
  {'T', 3},
  {'U', 12},
  {'V', 24},
  {'W', 14},
  {'X', 25},
  {'Y', 29},
  {'Z', 19},
  {'1', 62},
  {'2', 60},
  {'3', 56},
  {'4', 48},
  {'5', 32},
  {'6', 33},
  {'7', 35},
  {'8', 39},
  {'9', 47},
  {'0', 63}
} ;

void
dash()
{
  digitalWrite(morseLEDpin, HIGH) ;
  delay(DASHLEN);
  digitalWrite(morseLEDpin, LOW) ;
  delay(DOTLEN) ;
}

void
dit()
{
  digitalWrite(morseLEDpin, HIGH) ;
  delay(DOTLEN);
  digitalWrite(morseLEDpin, LOW) ;
  delay(DOTLEN);
}

void
send(char c)
{
  int i ;
  if (c == ' ') {
    Serial.print(c) ;
    delay(7 * DOTLEN) ;
    return ;
  }
  for (i = 0; i < N_MORSE; i++) {
    if (morsetab[i].c == c) {
      unsigned char p = morsetab[i].pat ;
      Serial.print(morsetab[i].c) ;

      while (p != 1) {
        if (p & 1)
          dash() ;
        else
          dit() ;
        p = p / 2 ;
      }
      delay(2 * DOTLEN) ;
      return ;
    }
  }
  /* if we drop off the end, then we send a space */
  Serial.print("?") ;
}

void
sendmsg(char *str)
{
  while (*str)
    send(*str++) ;
  Serial.println("");
}
// ****** Morse Beacon Ends ******



// begin setup

void setup() {
  Serial.begin(9600);

  pinMode(HEATRELAYPIN, OUTPUT);
  pinMode(FANRELAYPIN, OUTPUT);
  pinMode(LIGHTRELAYPIN, OUTPUT);
  pinMode(PUMPRELAYPIN, OUTPUT);
  pinMode(LIGHTBUTTONPIN, INPUT_PULLUP);
  pinMode(PUMPBUTTONPIN, INPUT_PULLUP);
  pinMode(THERMISTORPIN, INPUT);

  // ****** Morse Beacon Begins ******
  pinMode(morseLEDpin, OUTPUT) ;
  // ****** Morse Beacon Ends ******
}

// begin loop

void loop() {

  // Get current clock time
  unsigned long currentTime = millis();

  // Read thermistor 100 times and take the average to reduce sensor readout variation and reduce relay chatter

  long average = 0;
  for (int i = 0; i < 100; i++) {
    average = average + analogRead(THERMISTORPIN);
    delay(10);
  }
  int heatSensor = average / 100;
  Serial.print("heatSensor: "); Serial.print(heatSensor); Serial.print(" heatSetPoint: "); Serial.println(heatSetPoint);


  // Adjust heat set point based on grow cycle phase

  // Boost Phase Heat Setting

  if (currentTime < boostPhase) {
    heatSetPoint = boostHeatSetPoint;
  }

  // Germination Phase Heat Setting

  if (currentTime > boostPhase && currentTime < growPhase) {
    heatSetPoint = germHeatSetPoint;
  }

  // Grow Phase Heat Setting

  if (currentTime > growPhase && currentTime < hvstPhase) {
    heatSetPoint = growHeatSetPoint;
  }

  // Harvest Phase Heat Setting

  if (currentTime > hvstPhase) {
    heatSetPoint = harvestHeatSetPoint;
  }

  // Turn heater on or off based on heatSensor reading


  if (heatSensor < heatSetPoint) {
    digitalWrite(HEATRELAYPIN, HIGH);
    Serial.print(heatSensor); Serial.println(" Heat On +++++++++++++++++++++++");
  }
  if (heatSensor > heatSetPoint + hysteresis) {
    digitalWrite(HEATRELAYPIN, LOW);
    Serial.print(heatSensor); Serial.println(" Heat Off ----------------------");
  }


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
    Serial.print("PUMP OFF, "); Serial.print((pumpCycleDuration - pumpOnDuration - pumpCycleTime) / msHour); Serial.println(" Hours to Next Pump Run");
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
//    if (currentTime < hvstPhase) {
      digitalWrite(FANRELAYPIN, LOW);
//    }
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

  // ****** Morse Beacon Begins ******
  char airTempMessage[21];
  char daysToHarvestMessage[21];
  unsigned long daysToHarvest = 1 + ((hvstPhase - currentTime) / msDay);
  itoa(daysToHarvest, daysToHarvestMessage, 10);
  itoa(heatSensor, airTempMessage, 10);
  Serial.println("Sending Morse...");
  //  sendmsg("A ");
  sendmsg(daysToHarvestMessage);
  delay(500);
  sendmsg(airTempMessage);
  Serial.println("");
  // ****** Morse Beacon Ends ******

  delay(1000);
  Serial.println();
}
