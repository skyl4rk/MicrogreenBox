// MicrogreenBox
// Controller for a microgreen grow chamber using a flood and drain pump, seedling mat heaters, 
// pc fan ventilation and LED strip lights to automate microgreen production.

#define PUMPONTIME 60000         // Run pump for X milliseconds - 90 seconds is 90000
#define PUMPOFFTIME 21600000     // Pause pump for X milliseconds - 6 hours is 21600000
#define LIGHTONTIME 57600000     // Run light for X milliseconds - 16 hours is 57600000
#define LIGHTOFFTIME 28800000    // Pause light for X milliseconds - 8 hours is 28800000
#define LIGHTSTARTTIME 432000000 // Wait X milliseconds before starting light - 5 days is 432000000
#define HEATSETPOINT 330         // Thermistor setting below which heater is turned on
#define HYSTERESIS 10            // HEATSETPOINT + HYSTERESIS is when the heat turns off and the fan turns on

#define HEATRELAYPIN 4           // Heat relay pin number
#define FANRELAYPIN 5            // Fan relay pin number
#define LIGHTRELAYPIN 6          // Light relay pin number
#define PUMPRELAYPIN 7           // Pump relay pin number

#define BUTTONPIN 12             // Button grounds out this pin, NOTE: pin mode is INPUT_PULLUP
#define THERMISTORPIN 0          // Thermistor pin number, put thermistor at seed level in sunflower tray
                                 // Note: thermistor is connected to 3.3V and with 10k ohm resistor to ground

// Declare the class RelayTimer

class RelayTimer
{
  // Class Member Variables
  int relayPin;    // the number of the relay pin
  unsigned long OnTime;     // milliseconds of on-time
  unsigned long OffTime;    // milliseconds of off-time

  // These maintain the current state
  int relayState;                 // relayState used to set the relay 
  unsigned long relayPreviousMillis;   // will store last time relay was updated

  // Constructor - creates a RelayTimer 
  // and initializes the member variables and state
  public:
  RelayTimer(int pin, unsigned long on, unsigned long off)
  {
  relayPin = pin;
  pinMode(relayPin, OUTPUT);     
    
  OnTime = on;
  OffTime = off;
  
  relayState = LOW; 
  relayPreviousMillis = 0;
  }

  // Member Function:
  void Update()
  {
    // check to see if it's time to change the state of the relay
    unsigned long currentMillis = millis();
     
    if((relayState == HIGH) && (currentMillis - relayPreviousMillis >= OnTime))
    {
      relayState = LOW;  // Turn it off
      relayPreviousMillis = currentMillis;  // Remember the time
      digitalWrite(relayPin, relayState);  // Update the actual relay pin
    }
    else if ((relayState == LOW) && (currentMillis - relayPreviousMillis >= OffTime))
    {
      relayState = HIGH;  // turn it on
      relayPreviousMillis = currentMillis;   // Remember the time
      digitalWrite(relayPin, relayState);   // Update the actual relay pin
      Serial.print("Pin On: ");Serial.println(relayPin);
    }
  }
};

// Create instances of the class RelayTimer:
RelayTimer pump(PUMPRELAYPIN, PUMPONTIME, PUMPOFFTIME);
RelayTimer light(LIGHTRELAYPIN, LIGHTONTIME, LIGHTOFFTIME);

void setup()
{
  Serial.begin(9600);
  pinMode(THERMISTORPIN, INPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(HEATRELAYPIN, OUTPUT);
  pinMode(FANRELAYPIN, OUTPUT);

  // INITIAL PUMP RUN ON RESET
  digitalWrite(PUMPRELAYPIN, HIGH);
  delay(PUMPONTIME);
  digitalWrite(PUMPRELAYPIN, LOW);
}

void loop()
{

  // Call Update for each instance of the class RelayTimer:

  pump.Update();

  // Start light timer after start time delay (stay dark during germination)
  if(millis() > LIGHTSTARTTIME){
    light.Update();
  }

  // Read thermistor 100 times and take the average to reduce sensor readout variation and reduce relay chatter
  long average = 0;
  for (int i=0; i < 100; i++) {
  average = average + analogRead(THERMISTORPIN);
  delay(10);
  }
  average = average / 100;

  // Check thermistor and start heat if below set point
  if(average < HEATSETPOINT && digitalRead(HEATRELAYPIN) == LOW){
    digitalWrite(HEATRELAYPIN, HIGH);
    Serial.print(average); Serial.println(" Heat On ++++++++++++++");
    }
  if(average > HEATSETPOINT + HYSTERESIS && digitalRead(HEATRELAYPIN) == HIGH){
    digitalWrite(HEATRELAYPIN, LOW);
    Serial.print(average); Serial.println(" Heat Off -------------");
    }
  
  // Check thermistor and shut down fan if below set point, start fan if above set point + hysterisis 
  if(average > HEATSETPOINT + HYSTERESIS && digitalRead(FANRELAYPIN) == LOW){
    digitalWrite(FANRELAYPIN, HIGH);
    Serial.print(average); Serial.println(" Fan On ----------------");
  }
  if(average < HEATSETPOINT + HYSTERESIS - HYSTERESIS && digitalRead(FANRELAYPIN) == HIGH){
    digitalWrite(FANRELAYPIN, LOW);
    Serial.print(average); Serial.println(" Fan Off");
  }

  // DEBUG
//  Serial.print("Pin 0:"); Serial.print(analogRead(0)); Serial.print(" Pin 2: "); Serial.println(analogRead(2));
    
  // Serial.print("Ave: "); 
  Serial.print(average);
  
  if(digitalRead(HEATRELAYPIN)){
    Serial.println(" HEAT ON");
  }
  if(digitalRead(FANRELAYPIN)){
    Serial.println(" FAN ON");
  }

  // Check if button has been pressed, if so, run pump
  if(digitalRead(BUTTONPIN) == LOW){
    digitalWrite(HEATRELAYPIN, LOW);  // Turn off heat during pump activation
    digitalWrite(FANRELAYPIN, LOW);   // Turn off fan during pump activation
    digitalWrite(PUMPRELAYPIN, HIGH); // Run pump for period defined in PUMPONTIME
    delay(PUMPONTIME);
    digitalWrite(PUMPRELAYPIN, LOW);
  }
  
//  delay(1000);                         // Loop delay

}
