/** Traffic Lights
* Made by Shaurya (Rye)
**/

/** Pins- North Control **/
const int northR = 7;
const int northY = 6;
const int northG = 5;

/** Pins- East-West Control **/
const int eastWR = 4;
const int eastWY = 3;
const int eastWG = 2;

/** Pins- South Control **/
const int southR = 10;
const int southY = 9;
const int southG = 8;                    

/** Delays Between Different Switches of Lights **/
const int delayRGY = 2000;
const int delayYR = 1100;

/** Pin- Photoresistor **/
const int lightPin = A0;

/** Checks for Car at Northbound Left Turn Lane; 0 = No Car Present, 1 = Car Detected**/
int carCheckNS = 0;

/** Pins- Pedestrian **/
const int pedBtn = 12;
const int dontWalkPin = 13;
const int walkPin = 11;
const int buzzerPin = A1;

/** Current Pedestrian Button State; 0 = LOW/NOT PRESSED, 1 = HIGH/PRESSED **/
int curPedState = 0;

/** Pins- Checks for Car Crossing at Red Lights (Infrared Sensor) **/
const int ir1 = A2; // dedicated to checking in the north and south bound traffic lights
const int ir2 = A3; // dedicated to checking in the east-west bound traffic lights

/** Determines if there is a Red or Green/Yellow Light at Specific Bounds; 0 = Red light, 1 = Green/Yellow **/
int runN = 0;
int runS = 0;
int runEW = 0;

/** Stores Previous Time for millis() Delay **/
unsigned long previousDelay = 0;

/**
* setup() - runs only once
* sets up pins for traffic lights, pedestrian button, lights and buzzer, and infrared sensors
* initializes the red traffic and pedestrian light(s)
* opens the serial port
**/
void setup()
{
  /** Traffic Lights Setup **/
  pinMode(northR, OUTPUT);
  pinMode(northY, OUTPUT);
  pinMode(northG, OUTPUT);
 
  pinMode(eastWR, OUTPUT);
  pinMode(eastWY, OUTPUT);
  pinMode(eastWG, OUTPUT);
 
  pinMode(southR, OUTPUT);
  pinMode(southY, OUTPUT);
  pinMode(southG, OUTPUT);
 
  /** Pedestrian Setup **/
  pinMode(pedBtn, INPUT);
  pinMode(dontWalkPin, OUTPUT);
  pinMode(walkPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
 
  /** Vehicle Allowed to Pass or Not Check- Infrared Sensors Setup **/
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
 
  /** Initial Lights Setup **/
  digitalWrite(northR, HIGH);
  digitalWrite(southR, HIGH);
  digitalWrite(eastWR, HIGH);
  digitalWrite(dontWalkPin, HIGH);

  Serial.begin(9600);
 
  tDelay(delayRGY);
}

/**
* loop() - starts after setup
* repeats infinitely
* controls the main features of the traffic light
**/
void loop()
{
  checkForCarNL();
 
  northSouth();
  eastWest();
}

/**
* northSouth()- controls north-south bound lights
* NOTE: north and sound bound lights are split
**/
void northSouth()
{
  digitalWrite(northR, LOW);
  runN = 1; // cars are allowed to pass in the northbound traffic

  if(carCheckNS == 0) {
  // runs when no car is detected in the northbound left turn lane
  digitalWrite(northG, HIGH);
  }
  else {
  // carCheckNS is 1
  advancedGreen(); // throw an 'advanced green' when a car is detected in the northbound left turn lane
  carCheckNS = 0; // resets the variable, meaning the car has passed
  }
 
  digitalWrite(southR, LOW);
  digitalWrite(southG, HIGH);
  runS = 1; // cars are allowed to pass in the southbound traffic

  if(curPedState == 1) {
  pedLights(); // display the pedestrian lights if the button has been pressed
  }
 
  tDelay(delayRGY);
  digitalWrite(northG, LOW);
  digitalWrite(southG, LOW);

  digitalWrite(northY, HIGH);
  digitalWrite(southY, HIGH);
  tDelay(delayYR);
  digitalWrite(northY, LOW);
  digitalWrite(southY, LOW);
  runN = 0; // cars are NOT allowed to pass in the northbound traffic
  runS = 0; // cars are NOT allowed to pass in the southbound traffic
 
  digitalWrite(northR, HIGH);
  digitalWrite(southR, HIGH);
  tDelay(delayRGY);
}

/**
* eastWest()- controls east-west bound lights
**/
void eastWest()
{
  digitalWrite(eastWR, LOW);

  digitalWrite(eastWG, HIGH);
  runEW = 1; // cars are allowed to pass in the east-west bound traffic
  tDelay(delayRGY);
  digitalWrite(eastWG, LOW);

  digitalWrite(eastWY, HIGH);
  tDelay(delayYR);
  digitalWrite(eastWY, LOW);
  runEW = 0; // cars are NOT allowed to pass in the east-west bound traffic
 
  digitalWrite(eastWR, HIGH);
 
  tDelay(delayRGY);
}

/**
* tDelay()- uses millis() to delay for ms milliseconds
**/            
void tDelay(int ms)
{
  int run = 1; // determines if the delay is to be still active or not
  while (run == 1) {
  unsigned long currentMillis = millis(); // milliseconds passed from the starting time of 0ms
  if (currentMillis - previousDelay > ms) {
    previousDelay = currentMillis;
    run = 0; // the delay is over
    }
    pedBtnState(); // tracks if the pedestrian button has been pressed
    infraredCheck(); // tracks if vehicles are passing on a red light in a particular direction

  }
}

/**
* checkForCarNL()- checks for car at northbound left turn lane
**/
void checkForCarNL()
{
  int lightLevel = analogRead(lightPin);
  lightLevel = map(lightLevel, 0, 900, 0, 255);
  lightLevel = constrain(lightLevel, 0, 255);
 
  if(lightLevel == 255) {
  carCheckNS = 1; // car detected at the northbound left turn lane
  }
}

/**
* advancedGreen()- flashes 'advanced green' at northbound traffic if a car is present in the northbound left turn lane
**/
void advancedGreen()
{
  for(int i = 0; i < 4; i++) {
     digitalWrite(northG,LOW);
     tDelay(200);
     digitalWrite(northG,HIGH);
     tDelay(900);
  }
}

/**
* pedBtnState()- updates pedestrian button state
**/
void pedBtnState()
{
  if(digitalRead(pedBtn) ==  HIGH) {
    curPedState = 1; // pedestrian button has been pressed
  }
}

/**
* pedLights()- displays the pedestrian lights
**/
void pedLights()
{
  digitalWrite(dontWalkPin, LOW);
  digitalWrite(walkPin, HIGH);
  walkSound(); // walking sound that is produced to alert the pedestrian(s) to move; alerts pedestrian(s) when the light is going to turn red
  digitalWrite(walkPin, LOW);
  digitalWrite(dontWalkPin, HIGH);
  curPedState = 0; // resets pedestrian button state- meaning the pedestrian(s) have passed
}

/**
* walkSound()- Alerts pedestrian(s) to walk
* Alerts pedestrian(s) when the light is going to turn red
**/
void walkSound()
{
  for(int i = 0; i<4; i++) {
  tone(buzzerPin, 200);
  tDelay(500);
  tone(buzzerPin, 300);
  tDelay(500);
  } // plays a sound at the normal rate when the pedestrian light is green and not going to red soon
 
  for (int i = 0; i < 3; i++) {
  tone(buzzerPin, 300);
  digitalWrite(walkPin, LOW);
  tDelay(175);
  digitalWrite(walkPin,HIGH);
  tDelay(900);
  } // flashes the green pedestrian light and plays a more rapid sound to alert pedestrian(s) that the light is going to turn red soon

  noTone(buzzerPin); // buzzer stops producing sound once the pedestrian light is red
}

/**
* infraredCheck()- checks if vehicles are passing on a red light in a particular direction
**/
void infraredCheck()
{

  if (digitalRead(ir1) == LOW && runN == 0) {
    Serial.println("N Bound Car Plate Captured!");
  } // car is passing in the northbound when it's not supposed too
   if (digitalRead(ir1) == LOW && runS == 0) {
    Serial.println("S Bound Car Plate Captured!");
  } // car is passing in the southbound when it's not supposed too
  if (digitalRead(ir2) == LOW && runEW == 0) {
    Serial.println("E-W Bound Car Plate Captured!");
  } // car is passing in the east-west bound when it's not supposed too
}

// CODE END
