//Colour recorded values constants
#define NUM_CHANNELS 4
#define NUM_MAP_COLOURS 5

// Colour sensor pins
#define S0Pin 2
#define S1Pin 3
#define S2Pin 12
#define S3Pin 11
#define leftSensorPin 8
#define rightSensorPin 7

//Drive pins
#define leftDrive1Pin 10
#define leftDrive2Pin 9
#define rightDrive1Pin 5
#define rightDrive2Pin 6

//Drive speeds
// #define straightSpeed 35
// #define turnSpeedSlow -30
// #define turnSpeedFast 45

#define straightSpeed 47
#define turnSpeedSlow -18
#define turnSpeedFast 63
#define iterSlow 0
#define iterFast 0.0001

enum colours {
  white = 0,
  black,
  red,
  blue,
  green
};

enum turns {
  left = 0,
  right
};

//Recorded colour values
int rightRecordedValues[NUM_MAP_COLOURS][NUM_CHANNELS] = {
  //Red, Blue, Clear, Green
  {430,300,140,450}, // white
  {6000,4500,1700,6300}, // black
  {740,1800,450,2700}, // red
  {4000,1780,900,3700}, // blue
  {3300,1600,690,1700} // green
};

int leftRecordedValues[NUM_MAP_COLOURS][NUM_CHANNELS] = {
  //Red, Blue, Clear, Green
  {530,380,160,530}, // white
  {2600,2050,750,2700}, // black
  {730,1600,430,2300}, // red
  {2800,1250,615,2350}, // blue
  {1800,1200,455,1420} // green
};

//Colour sensor values
int leftMeasuredValues[NUM_CHANNELS]; //Red, Blue, Clear, Green
int rightMeasuredValues[NUM_CHANNELS]; //Red, Blue, Clear, Green 

int iterCount = 0;
int lastTurn = left;

void setup() {
  //Colour sensor pins
  pinMode(S0Pin, OUTPUT);
  pinMode(S1Pin, OUTPUT);
  pinMode(S2Pin, OUTPUT);
  pinMode(S3Pin, OUTPUT);
  pinMode(leftSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);

  //Drive pins
  pinMode(leftDrive1Pin, OUTPUT);
  pinMode(leftDrive2Pin, OUTPUT);
  pinMode(rightDrive1Pin, OUTPUT);
  pinMode(rightDrive2Pin, OUTPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(S0Pin,HIGH);
  digitalWrite(S1Pin,LOW);
  Serial.begin(9600);
}
void loop() {
  
  readColourSensors();

  //Printing colour estimates
  // Serial.print("Left: ");
  // Serial.println(getNearestColour(leftMeasuredValues, leftRecordedValues, false));
  // Serial.print("Right: ");
  // Serial.println(getNearestColour(rightMeasuredValues, rightRecordedValues, true));
  // Serial.println();
  // delay(200);

  Tier2();

  //Printing colour values
  // printLeft();
  // printRight();
  // delay(500);
}

int colourDistanceFunction(int* col1, int* col2) {
  int diff = 0;

  for (int i = 0; i<NUM_CHANNELS; i++)
  {
    int thisDiff = abs(col1[i] - col2[i]);
    if (i == 1) {
      diff += 1 * thisDiff;
    }
    else {
      diff += thisDiff;
    }
  }

  return diff;
}

int getNearestColour(int measuredColour[NUM_CHANNELS], int recordedColour[NUM_MAP_COLOURS][NUM_CHANNELS], bool side) {
  // if (side) { //true for right
  //   if (measuredColour[2] > 720) {
  //     return(1);
  //   }
  //   else if (measuredColour[2] < 250) {
  //     return(0);
  //   }
  // }
  // else { //false for left
  //   if (measuredColour[2] > 380) {
  //     return(1);
  //   }
  //   else if (measuredColour[2] < 120) {
  //     return(0);
  //   }
  // }

  int minDiff = colourDistanceFunction(measuredColour, recordedColour[0]);
  int minDiffIndex = 0;
  for (int i = 1; i<NUM_MAP_COLOURS; i++) {
    int diff = colourDistanceFunction(measuredColour, recordedColour[i]);
    if (diff < minDiff)
    {
      minDiff = diff;
      minDiffIndex = i;
    }
  }
  return minDiffIndex;
}

void driveTurn(int lSpeed, int rSpeed) {
  if (lSpeed >= 0) {
    lSpeed = constrain(lSpeed,0,160);
    analogWrite(leftDrive1Pin, lSpeed);
    analogWrite(leftDrive2Pin, 0);
  }
  else {
    lSpeed = abs(lSpeed);
    lSpeed = constrain(lSpeed,0,160);
    analogWrite(leftDrive1Pin, 0);
    analogWrite(leftDrive2Pin, lSpeed);
  }

  if (rSpeed >= 0) {
    rSpeed = constrain(rSpeed,0,160);
    analogWrite(rightDrive1Pin, rSpeed);
    analogWrite(rightDrive2Pin, 0);
  }
  else {
    rSpeed = abs(rSpeed);
    rSpeed = constrain(rSpeed,0,160);
    analogWrite(rightDrive1Pin, 0);
    analogWrite(rightDrive2Pin, rSpeed);
  }
}

void driveStraight(int speed) {
  if (speed >= 0) {
    speed = constrain(speed,0,160);
    analogWrite(leftDrive1Pin, speed);
    analogWrite(rightDrive1Pin, speed);
    analogWrite(leftDrive2Pin, 0);
    analogWrite(rightDrive2Pin, 0);
  }
  else {
    speed = abs(speed);
    speed = constrain(speed,0,160);
    analogWrite(leftDrive1Pin, 0);
    analogWrite(rightDrive1Pin, 0);
    analogWrite(leftDrive2Pin, speed);
    analogWrite(rightDrive2Pin, speed);
  }

}

void Tier1() {
  int left = getNearestColour(leftMeasuredValues, leftRecordedValues, false);
  int right = getNearestColour(rightMeasuredValues, rightRecordedValues, true);
  if (left != white && right == white) {
    turnLeft(0);
    // lastTurn = left;
    // iterCount = 0;
  }
  else if (left == white && right != white) {
    turnRight(0);
    // lastTurn = right;
    // iterCount = 0;
  }
  else {
    // if (lastTurn == left) {
    //   turnLeft(iterCount);
    // }
    // else {
    //   turnRight(iterCount);
    // }
    // iterCount ++;
    straight();
  }
}

void Tier2() {
  int left = getNearestColour(leftMeasuredValues, leftRecordedValues, false);
  int right = getNearestColour(rightMeasuredValues, rightRecordedValues, true);
  if (left == white && right == white) {
    straight();
  }
  else if (left == white) {
    turnRight(0);
  }
  else if (right == white) {
    turnLeft(0);
  }
  else if (right != red && left == red) {
    turnLeft(0);
    lastTurn = left;
  }
  else if (left != red && right == red) {
    turnRight(0);
    lastTurn = right;
  }
  else {
    creepForward();
  }
}

void Tier3() {
  int left = getNearestColour(leftMeasuredValues, leftRecordedValues, false);
  int right = getNearestColour(rightMeasuredValues, rightRecordedValues, true);
  if (left == white && right == white) {
    straight();
  }
  else if (left == white) {
    turnRight(0);
  }
  else if (right == white) {
    turnLeft(0);
  }
  else if (right != black && isColour(left)) {
    turnLeft(0);
    lastTurn = left;
  }
  else if (left != black && isColour(right)) {
    turnRight(0);
    lastTurn = right;
  }
  else {
    creepForward();
  }
}

bool isColour(int colourIn) {
  if (colourIn == red || colourIn == green || colourIn == blue) {
    return(true);
  }
  else {
    return(false);
  }
}

void creepForward() {
  driveTurn(0,45);
}

void turnRight(int iterCount) {
  driveTurn(turnSpeedFast + iterCount * iterFast, turnSpeedSlow + iterCount * iterSlow);
}

void turnLeft(int iterCount) {
  driveTurn(turnSpeedSlow + iterCount * iterSlow, turnSpeedFast + iterCount * iterFast);
}

void straight() {
  driveStraight(straightSpeed);
}

void readColourSensors() {
  //left sensor
  leftMeasuredValues[0] = readRed(leftSensorPin);
  leftMeasuredValues[1] = readBlue(leftSensorPin);
  leftMeasuredValues[2] = readClear(leftSensorPin);
  leftMeasuredValues[3] = readGreen(leftSensorPin);
  //right sensor
  rightMeasuredValues[0] = readRed(rightSensorPin);
  rightMeasuredValues[1] = readBlue(rightSensorPin);
  rightMeasuredValues[2] = readClear(rightSensorPin);
  rightMeasuredValues[3] = readGreen(rightSensorPin);
}

int readRed(int sensorInputPin) {
    // Setting red filtered photodiodes to be read
    digitalWrite(S2Pin,LOW);
    digitalWrite(S3Pin,LOW);

    // Reading the output frequency
    int frequency = pulseIn(sensorInputPin, LOW);

    //Returning value
    return(frequency);
}

int readBlue(int sensorInputPin) {
  // Setting blue filtered photodiodes to be read
  digitalWrite(S2Pin,LOW);
  digitalWrite(S3Pin,HIGH);

  // Reading the output frequency
  int frequency = pulseIn(sensorInputPin, LOW);

  //Returning value
  return(frequency);
}

int readGreen(int sensorInputPin) {
  // Setting green filtered photodiodes to be read
  digitalWrite(S2Pin,HIGH);
  digitalWrite(S3Pin,HIGH);

  // Reading the output frequency
  int frequency = pulseIn(sensorInputPin, LOW);

  //Returning value
  return(frequency);
}

int readClear(int sensorInputPin) {
  // Setting readClear filtered photodiodes to be read
  digitalWrite(S2Pin,HIGH);
  digitalWrite(S3Pin,LOW);

  // Reading the output frequency
  int frequency = pulseIn(sensorInputPin, LOW);

  //Returning value
  return(frequency);
}

void printLeft() {
  Serial.println("Left");
  Serial.print("R: ");
  Serial.println(leftMeasuredValues[0]);
  Serial.print("B: ");
  Serial.println(leftMeasuredValues[1]);
  Serial.print("C: ");
  Serial.println(leftMeasuredValues[2]);
  Serial.print("G: ");
  Serial.println(leftMeasuredValues[3]);
  Serial.println();
}

void printRight() {
  Serial.println("right");
  Serial.print("R: ");
  Serial.println(rightMeasuredValues[0]);
  Serial.print("B: ");
  Serial.println(rightMeasuredValues[1]);
  Serial.print("C: ");
  Serial.println(rightMeasuredValues[2]);
  Serial.print("G: ");
  Serial.println(rightMeasuredValues[3]);
  Serial.println();
}
