// Pump Timer with Sensor Control and Morse Readout
// Repeating Pump Cycle
// Paul VandenBosch, 20190306

#define HEATSET 325     // Sensor value to turn on heater below this temperature setting
#define AIRTEMPSET 300  // Sensor value to turn on exhaust fan above this temperature setting
#define AIRTEMPOFFSET 5 // Circulation fan will start x number below AIRTEMPSET for exhaust fan
#define DRYSOILSET 460  // Moisture sensor Value that triggers watering, soil is dry at this value
#define WETSOILSET 400  // Moisture sensor Value that shuts off water pump
#define EXHAUSTFANCYCLETIME 600000 // ms between exhaust fan recurring runs
#define EXHAUSTFANDELAY 6000 // ms to run exhaust fan on recurring runs

#define WATERPUMPDELAY 90   // Seconds to run water pump for each watering event
#define WATERPUMPCYCLE 4   //  Hours between watering cycles

/*
#define FIRSTWATERING 5    // Day after start for the first watering event
#define SECONDWATERING 40  // Day after start for the first watering event
#define THIRDWATERING 40   // Day after start for the first watering event
#define OVERFLOWDAYS 40    // Days after which the crop will be completed
*/

#define HEATRELAYPIN 4  // Pin used for heat relay  
#define CIRCRELAYPIN 5  // Pin used for circulation fan relay
#define FANRELAYPIN 6   //  Pin used for fan relay
#define PUMPRELAYPIN 7  // Pin used for pump relay

#define TRAYSENSORPIN 0 // Pin used for heat sensor on tray at seed level
#define BOXSENSORPIN 2  // Pin used for heat sensor in box for air temperature
#define MOISTUREPIN 1   // Analog pin used for moisture sensor
#define BUTTONPIN 12    // Pin used to start manual pump by grounding it
#define PUMPBUTTONPIN 11  // Pin used to turn on pump


int moistureSensor = analogRead(MOISTUREPIN);
int heatSensor = analogRead(TRAYSENSORPIN);
int airTempSensor = analogRead(BOXSENSORPIN);
int fan1Toggle = 0;
int waterToggle = 1;

unsigned long dayMs = 86400000;
unsigned long hourMs = 3600000;
unsigned long minuteMs = 60000;

unsigned long waterPumpDelay = WATERPUMPDELAY * 1000.;
unsigned long waterPumpCycle = WATERPUMPCYCLE * hourMs; 
unsigned long waterStartTime = 0;
/*
unsigned long firstWatering = FIRSTWATERING * dayMs;
unsigned long secondWatering = SECONDWATERING * dayMs; 
unsigned long thirdWatering = THIRDWATERING *dayMs;
*/

unsigned long currentMillis;
unsigned long recurringFanStart = EXHAUSTFANCYCLETIME;


// Simple Arduino Morse Beacon
// ****** Morse Beacon Begins ******

#define N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))
#define SPEED  (12)
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



void setup() {
  Serial.begin(9600);
  
  pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(PUMPBUTTONPIN, INPUT_PULLUP);
  pinMode(MOISTUREPIN, INPUT);
  pinMode(TRAYSENSORPIN, INPUT);
  pinMode(BOXSENSORPIN, INPUT);
  pinMode(PUMPRELAYPIN, OUTPUT);
  pinMode(HEATRELAYPIN, OUTPUT);
  pinMode(FANRELAYPIN, OUTPUT);
  pinMode(CIRCRELAYPIN, OUTPUT);

  
  // ****** Morse Beacon Begins ******
  pinMode(morseLEDpin, OUTPUT) ;
  // ****** Morse Beacon Ends ******
}

void loop() {
  moistureSensor = analogRead(MOISTUREPIN);
  heatSensor = analogRead(TRAYSENSORPIN);
  airTempSensor = analogRead(BOXSENSORPIN);
  
  currentMillis = millis();

// PUMP ACTUATION

if (waterToggle == 1 && digitalRead(PUMPBUTTONPIN)) {
      digitalWrite(PUMPRELAYPIN, HIGH);
      digitalWrite(HEATRELAYPIN, LOW);
      digitalWrite(morseLEDpin, HIGH);
      waterStartTime = currentMillis;
      waterToggle = 0;
      delay(waterPumpDelay);
      digitalWrite(morseLEDpin, LOW);
      digitalWrite(PUMPRELAYPIN, LOW);
}

if (currentMillis - waterStartTime > waterPumpCycle) {
  waterToggle = 1;
}

/*
  if (currentMillis > firstWatering) {
    digitalWrite(PUMPRELAYPIN, HIGH);
  }
  if (currentMillis > firstWatering + (WATERPUMPTIME * 1000.)) {
    firstWatering = OVERFLOWDAYS * dayMs;
    digitalWrite(PUMPRELAYPIN, LOW);
  }

  if (currentMillis > secondWatering) {
    digitalWrite(PUMPRELAYPIN, HIGH);
  }
  if (currentMillis > secondWatering + (WATERPUMPTIME * 1000.)) {
    secondWatering = OVERFLOWDAYS * dayMs;
    digitalWrite(PUMPRELAYPIN, LOW);
  }
  
  if (currentMillis > thirdWatering) {
    digitalWrite(PUMPRELAYPIN, HIGH);
  }
  if (currentMillis > thirdWatering + (WATERPUMPTIME * 1000.)) {
    thirdWatering = OVERFLOWDAYS * dayMs;
    digitalWrite(PUMPRELAYPIN, LOW);
  }
*/

  
// FAN CONTROL

  if (airTempSensor > AIRTEMPSET) {
    fan1Toggle = 1;
    Serial.print("EXHAUST FAN ON: ");
    Serial.println(airTempSensor);
  }
  else {
    fan1Toggle = 0;
    Serial.print("EXHAUST FAN OFF: ");
    Serial.println(airTempSensor);
  }

  if (fan1Toggle == 1){
    digitalWrite(FANRELAYPIN, HIGH);
//    Serial.print("CIRCULATION FAN OFF: ");
//    Serial.println(airTempSensor);
  }
  else {
    digitalWrite(FANRELAYPIN, LOW);
  }

// CIRCULATION FAN CONTROL
  if (digitalRead(FANRELAYPIN)) {
  digitalWrite(CIRCRELAYPIN, LOW);
  Serial.print("CIRCULATION FAN OFF: ");
  Serial.println(airTempSensor);
  }
  else {
    if (airTempSensor > AIRTEMPSET - AIRTEMPOFFSET){
      digitalWrite(CIRCRELAYPIN, HIGH);
      Serial.print("CIRCULATION FAN ON: ");
      Serial.println(airTempSensor);
  }
  else {
    digitalWrite(CIRCRELAYPIN, LOW);
    Serial.print("CIRCULATION FAN OFF: ");
      Serial.println(airTempSensor);
  }
  }

// EXHAUST FAN RECURRING OPERATION

  if (currentMillis > recurringFanStart) {
    digitalWrite(FANRELAYPIN, HIGH);
    Serial.println ("RECURRING EXHAUST FAN ON");
    recurringFanStart = currentMillis + EXHAUSTFANCYCLETIME;
    delay(EXHAUSTFANDELAY);
    digitalWrite(FANRELAYPIN, LOW);
  }

// HEAT MAT CONTROL

  if (heatSensor < HEATSET) {
    digitalWrite(HEATRELAYPIN, HIGH);
    Serial.print("HEAT MATS ON: ");
    Serial.println(heatSensor);
  }
  else {
    digitalWrite(HEATRELAYPIN, LOW);
    Serial.print("HEAT MATS OFF: ");
    Serial.println(heatSensor);
  }

// BUTTON SENSOR

  if (digitalRead(BUTTONPIN) == LOW) {
    digitalWrite(HEATRELAYPIN, LOW);
    digitalWrite(PUMPRELAYPIN, HIGH);
    Serial.println("");
    Serial.print("Manual Engagement of Pump by Button for ");
    Serial.print(WATERPUMPDELAY);
    Serial.println(" Seconds Run Time");
    delay(waterPumpDelay);
    digitalWrite(PUMPRELAYPIN, LOW);
  }

// MOISTURE SENSOR PUMP LOCKOUT

//  if (moistureSensor < WETSOILSET) {     // turn off pump if soil is wet
//    digitalWrite(PUMPRELAYPIN, LOW);
//  }


    // ****** Morse Beacon Begins ******
    //char message[] = "1234568"; // worked
    int morseI = 57; // set integer variable to be read out equal to morseI here
    char moistureMessage[21]; // enough to hold all numbers up to 64-bits
    char heat0Message[21]; // enough to hold all numbers up to 64-bits
    char airTempMessage[21]; // enough to hold all numbers up to 64-bits
    char pumpMessage[21];
    itoa(moistureSensor, moistureMessage, 10);
    itoa(heatSensor, heat0Message, 10);
    itoa(airTempSensor, airTempMessage, 10);
    int hoursToPump = ((waterStartTime + waterPumpCycle - currentMillis) / hourMs);
    itoa (hoursToPump, pumpMessage, 10);
    Serial.println("Sending Morse");
//    sendmsg("M ");
//    sendmsg(moistureMessage) ;
//    delay(1000);
    sendmsg("H ");
    sendmsg(heat0Message);
    delay(1000);
    sendmsg("A ");
    sendmsg(airTempMessage);
    delay(1000);
    sendmsg("P ");
    sendmsg(pumpMessage);
    delay(1000);
    Serial.println("");
    //sendmsg("K6HX/B CM87") ;// original
//    delay(1000) ;
    // ****** Morse Beacon Ends ******


  
  Serial.println("*******************************");
  Serial.println("Arduino Growth Chamber Controller");
  Serial.print("Days since restart: ");
  Serial.println(currentMillis / 86400000);
  /*  
  Serial.print("Days to First Watering: ");
  Serial.println((firstWatering - currentMillis) / dayMs);
  Serial.print("Hours to First Watering: ");
  Serial.println((firstWatering - currentMillis) / hourMs);
  Serial.print("Days to Second Watering: ");
  Serial.println((secondWatering - currentMillis) / dayMs);
  Serial.print("Hours to Second Watering: ");
  Serial.println((secondWatering - currentMillis) / hourMs);
  Serial.print("Moisture Sensor: ");
  Serial.println(moistureSensor);
  Serial.print("Heat Sensor: ");
  Serial.println(heatSensor);
  Serial.print("HEAT SET TEMP: ");
  Serial.println(HEATSET);
  Serial.print("Air Temp Sensor: ");
  Serial.println(airTempSensor);
  Serial.print("AIR SET TEMP: ");
  Serial.println(AIRTEMPSET);
   */
  Serial.println("   *   *   *   *   *   ");
  Serial.print("Tray Temperature in F: ");
  int heatF = heatSensor/4.34;
  Serial.println(heatF);
  Serial.print("Box Temperature in F: ");
  int airF = airTempSensor / 4.25;
  Serial.println(airF);
  Serial.print("Hours to next Pump Cycle: ");
  Serial.println((waterStartTime + waterPumpCycle - currentMillis) / hourMs);
}
