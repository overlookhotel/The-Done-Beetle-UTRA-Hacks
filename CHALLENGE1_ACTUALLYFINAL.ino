// Define color sensor pins
#include <Servo.h>
#define S0 8
#define S1 9
#define S2 10
#define S3 11
#define sensorOut 12


//Servo for claw
Servo myServo;

int servoPin = 13;
int currentAngle = 90;  // Start at the midpoint
int stepSize = 90;      // Degrees to move per function call


//declare motor variables

//Motor A pins
const int enA = 3; 
const int in1 = 0;
const int in2 = 1;

// Motor B pins
const int enB = 5; 
const int in3 = 2;
const int in4 = 4;

bool moveFwd = true; 

//hansol sprial code
int j = 0;

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


// Variables for final Color values
int redValue;
int greenValue;
int blueValue;

char *colorArr[6]; // outside (black) and the 6 rings
int curColorLoc = -1; // index of colorArr
int colorTolerance = 40;
bool simColor;
char *curColor;
int moveDirection = -1; // 0 is out, 1 is in 

unsigned long startTime;
unsigned long endTime;

int adjustTime = 800;

int step = 0;

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

bool sameColor(int *referenceColor, int *curColor)
{
  // check each rgb within tolerance
  for (int i = 0; i < 3; i++)
  {
    if (abs(referenceColor[i] - curColor[i]) >= 40) // if not within tolerance
    {
      
      return false;
    }
  }
  return true;
}

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

void closeClaw() {
    currentAngle = constrain(currentAngle - stepSize, 0, 180);
    myServo.write(currentAngle);
    delay(500); // Short delay to allow movement
}

void openClaw() {
    currentAngle = constrain(currentAngle + stepSize, 0, 180);
    myServo.write(currentAngle);
    delay(500); // Short delay to allow movement
}

void setup() {
  // Set S0 - S3 as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

//set up servo
  myServo.attach(servoPin);
  myServo.write(currentAngle);
  closeClaw();

  // Set Sensor output as input
  pinMode(sensorOut, INPUT);

  // Set Pulse Width scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  // Setup Serial Monitor
  Serial.begin(9600);

  // Get original outside color
  colorArr[0] = "black";
  curColor = 0;

  // Set motor speed
  analogWrite(enA, 100);
  analogWrite(enB, 100);

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

  // Print output to Serial Monitor
  Serial.print("Red PW = ");
  Serial.print(redValue);
  Serial.print(" - Green PW = ");
  Serial.print(greenValue);
  Serial.print(" - Blue PW = ");
  Serial.println(blueValue);
  Serial.print("\n");

  // checks current color is same as before
  curColor = getColor(redValue, greenValue, blueValue);
  simColor = (colorArr[curColorLoc] == curColor);
  Serial.print(curColor);
  Serial.println(simColor);

  if (moveFwd){
    startForward();
  }
  else{
    startBackward();
  }

  // if color changes 
  if (simColor == false)
  {
    if (curColorLoc == 0) // if outside circle → color change → add new color to array
    {
        colorArr[0] = curColor;
        Serial.print("Color added");
        startTime = millis();
    }
    else // if in circle → color change → determine if moving inwards or outwards
    {
      // check if same as previous 3 (check 2 and 3 ago, not most recent b/c change)
      bool sameColorPrev3;
      int i = 1;
      int index = curColorLoc;
      while (i <= 3 && index >0) // check if same as any of prev 3 colors
      {
        index = curColorLoc - i;
        sameColorPrev3 = (sameColorPrev3 || (colorArr[index] == curColor));
      }
      if (sameColorPrev3)// if yes → (moving outwards)
      {
        curColor--;
        moveDirection = 0;
        Serial.print("color change & moving");
        if (curColorLoc == 0 && step == 0)
        {
          endTime = millis();
          while (millis() - endTime < ((endTime - startTime)/2))
          {
            startBackward();
          }
          turnLeft();
          step++; 
        }
        else if (curColorLoc == 0 && step == 1)
        {
          moveFwd = false;
        }
        else if (curColorLoc == 5 && step == 2)
        {
          endTime = millis();
          while (millis() - endTime < ((endTime - startTime)/2))
          {
            startBackward();
          }
          // adjust for a little more considering arm != sensor
          stopAll();
          startTime = millis();
          while (millis() - startTime < adjustTime)
          {
            startBackward();
          }
          stopAll();

          // open servo
          openClaw();

        }
        else if (step == 2) // lolol missed and is searching for sensor
        {
          startBackward();
        }
      }
      else // if no: update color array & curColor ++ 
      {
        curColorLoc++;
        moveDirection = 1;
        colorArr[curColorLoc - 1] = curColor;
        Serial.print("color change & color added & moving inwards");

        if (curColorLoc == 6)
        {
          startTime = millis();
          step++; // step 2 is at 6
        }
        else if (step == 2){
          startForward();
        }
      }
    }
  }
}