// Motor A pins

#define S0 8
#define S1 9
#define S2 10
#define S3 11
#define sensorOut 12

const int enA = 3; 
const int in1 = 0;
const int in2 = 1;

// Motor B pins
const int enB = 5; 
const int in3 = 2;
const int in4 = 4;

// Calibration Values
// *Get these from Calibration Sketch
int redMin = 40; // Red minimum value
int redMax = 504; // Red maximum value
int greenMin = 51; // Green minimum value
int greenMax = 606; // Green maximum value
int blueMin = 46; // Blue minimum value
int blueMax = 520; // Blue maximum value

// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

char *curColor;
int colorTolerance = 40;

// Variables for final Color values
int redValue;
int greenValue;
int blueValue;

const int trigPin = 6; // triggers ultrasonic sound pulses
const int echoPin = 7;
const int DISTANCE_THRESHOLD = 15;

int duration;
float distance; 
int robotMode;

char *getColor(int red, int green, int blue)
{
  char *color;
  if (red < 50 && blue < 50 && green < 50)
  {
    color = "black";
  }
  if (red > (green + colorTolerance) && red > (blue + colorTolerance))
  {
    color = "red";
  }
  else if (green > (red + colorTolerance) && green > (blue + colorTolerance))
  {
    color = "green";
  }
  else if (blue > (red + colorTolerance) && blue > (green + colorTolerance))
  {
    color = "blue";
  }
  return color;
}

// Function to read Red Pulse Widths
int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Green Pulse Widths
int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Blue Pulse Widths
int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT); 
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // MAY NOT BE NECESSARY
  // digitalWrite(in1, LOW);
  // digitalWrite(in2, LOW);
  // digitalWrite(in3, LOW);
  // digitalWrite(in4, LOW);

  analogWrite(enA, 150);
  analogWrite(enB, 100);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // Set Sensor output as input
  pinMode(sensorOut, INPUT);

  // Set Pulse Width scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  // Setup Serial Monitor
  Serial.begin(9600);

  startForward();

  robotMode = 0;
  Serial.begin(9600); //begins serial communication with computer
  Serial.println("Enter Robot Mode 0");

}

void loop() {
  // Read Red Pulse Width
  redPW = getRedPW();
  redValue = map(redPW, redMin,redMax,255,0);
  // Delay to stabilize sensor
  delay(200);

  // Read Green Pulse Width
  greenPW = getGreenPW();
  greenValue = map(greenPW, greenMin,greenMax,255,0);
  // Delay to stabilize sensor
  delay(200);

  // Read Blue Pulse Width
  bluePW = getBluePW();
  blueValue = map(bluePW, blueMin,blueMax,255,0);
  // Delay to stabilize sensor
  delay(200);

  curColor = getColor(redValue, greenValue, blueValue);
  if (curColor == "red")
  {
    turnRight();
    turnRight();
  }
  else if (curColor == "green")
  {
    turnRight();
  }
  else if (curColor == "blue")
  {
    turnLeft();
  }
  else
  {
    startForward();
  }
  checkProximity();
  delay(2000); // checks again every 2s
}

// function that checks distance to wall and acts accordingly.
void checkProximity() {
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  // Display distance
  distance = (duration*0.034)/2;
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance <= DISTANCE_THRESHOLD) {
    Serial.println("Obstacle detected. Stopping motor.");
    stopAll();
    Serial.println("Turning left.");
    Serial.println();
    turnLeft();
  }
  
  else {
    Serial.println("No obstacle. Resuming motor.");
    Serial.println();
    startForward();
  }
} 

void stopAll(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(200);
}

void startForward(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void startBackward(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

}

void turnLeft(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(700);
  stopAll();
}

void turnRight(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(1000);
  stopAll();
}

