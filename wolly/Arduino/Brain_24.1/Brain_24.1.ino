// Master
#include <Wire.h>

// neo pixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN            2
#define NUMPIXELS      5
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#include "TimerOne.h"
int leds[5][3] = {
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0,
  0, 0, 0
};

const int InternalLed = 13;
const bool IsDebug = 0;

int channelA = 5;
int channelB = 4;
int channelC = 6;


int mini = 10;
int maxi = 180;

int segs[3][5] = {
  mini, mini, mini, mini, mini,
  mini, mini, mini, mini, mini,
  mini, mini, mini, mini, mini
};

int angle1;
int angle2;
int angle3;
const float pi = 3.1415926;


const int sensorPin0 = A1;
const int sensorPin1 = A2;
int sensorValue0 = 0;
int sensorValue1 = 1;
int sensorOffset0 = 0;
int sensorOffset1 = 0;
int sensorMax0 = 0;
int sensorMax1 = 0;
int p1 = 0;
int p2 = 0;


int state = 1;
int sensor = 0;
bool toggle = 0;
int sensorBefore = sensor;
unsigned long time = 0;
unsigned long timeDelta = 0;
unsigned long timeDisabled = millis();
int pubSpeed = 30; // opt = 30 : max = 50
int myDelay = 500; // opt = 500 : min = 300

void setup() {

  if (IsDebug) {
    for (int i = 0; i < 14; i++)
    {
      digitalWrite(InternalLed, HIGH);
      delay(500);
      digitalWrite(InternalLed, LOW);
      delay(200);
    }
  }
  delay(1000);
  Wire.begin();
  delay(200);

  pixels.begin(); // This initializes the NeoPixel library.
  time = millis();

  InitialPosition();
  Publish(pubSpeed);
  delay(2000);


  SetLed(0, 22, 0, 0);
  SetLed(1, 22, 0, 0);
  SetLed(2, 22, 0, 0);
  SetLed(3, 22, 0, 0);
  SetLed(4, 22, 0, 0);
  ShowLeds();
  delay (1000);

  SetLed(0, 0, 11, 0);
  SetLed(1, 0, 11, 0);
  SetLed(2, 0, 11, 0);
  SetLed(3, 0, 11, 0);
  SetLed(4, 0, 11, 0);
  ShowLeds();
  delay (1000);

  ResetLeds();

  // calibrate:
  // read the value from the sensor:
  CalibrateSensors();

  Timer1.initialize(50000);
  Timer1.attachInterrupt(ReadSensors);

  InitialShowOff();

}

// here we go ...
void loop() {

  showState();
  delay(100);

}

void showState()
{
  switch (state)
  {
    case 1:
      showIdle();
      DoIdle();
      break;
    case 3:
      showRunningForward();
      DoRunForward();
      break;
    case 5:
      showRunningBackward();
      DoRunBackward();
      break;
    case 7:
      showLookLeft();
      FullBodyLeft();
      // LookLeft();
      break;
    case 9:
      showLookRight();
      FullBodyRight();
      // LookRight();
      break;
    case 11:
      LookUp();
      CalibrateSensors();
      break;

    case 12:
      showMoveLeft();
      MoveLeft();
      state = 3;
      break;
    case 13:
      showMoveRight();
      MoveRight();
      state = 3;
      break;
  }
}




void SetSegment(int segNumber, int s1, int s2, int s3)
{
  segs[segNumber - 1][0] = s1;
  segs[segNumber - 1][1] = s2;
  segs[segNumber - 1][2] = s3;
}

void SetRing(int ring, int value)
{
  if (ring < 4)
  {
    segs[ring - 1][3] = value;
  }
  else
  {
    segs[ring - 2][4] = value;
  }
}


void MoveLeft()
{
  int currentPubSpeed = pubSpeed;
  int currentDelay = myDelay;

  myDelay = 300;
  pubSpeed = 50;

  int dir = 170;
  SetRing(1, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(1, maxi,  maxi, maxi - dir);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(1, maxi);

  SetRing(4, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(3, maxi , maxi - dir, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(4, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }


  for (int i = 0; i < 10; i++)
  {

    SetRing(2, mini);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
    pubSpeed = 90;
    SetSegment(2, maxi, maxi, maxi - dir);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
    SetRing(2, maxi);
    SetRing(3, mini);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
    pubSpeed = 30;
    SetSegment(2, mini, mini, mini);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
    pubSpeed = 50;
    SetRing(3, maxi);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
  }

  pubSpeed = currentPubSpeed;
  myDelay = currentDelay;

}


void MoveRight()
{
  int currentPubSpeed = pubSpeed;
  int currentDelay = myDelay;

  myDelay = 300;
  pubSpeed = 50;

  int dir = 170;
  SetRing(1, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(1, maxi, maxi, maxi - dir);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(1, maxi);

  SetRing(4, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(3, maxi , maxi - dir, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(4, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }


  for (int i = 0; i < 10; i++)
  {

    SetRing(2, mini);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
    pubSpeed = 60;
    SetSegment(2, maxi , maxi - dir, maxi);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
    SetRing(2, maxi);
    SetRing(3, mini);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
    pubSpeed = 30;
    SetSegment(2, mini, mini, mini);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
    pubSpeed = 50;
    SetRing(3, maxi);
    Publish(pubSpeed);
    if (state == 1) {
      return;
    }
  }

  pubSpeed = currentPubSpeed;
  myDelay = currentDelay;

}



void MoveForward()
{

  SetRing(1, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(1, maxi, maxi, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(1, maxi);
  SetRing(2, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(1, mini, mini, mini);
  SetSegment(2, maxi , maxi, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(2, maxi);
  SetRing(3, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(3, maxi, maxi, maxi);
  SetSegment(2, mini, mini, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(3, maxi);
  SetRing(4, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(3, mini, mini, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(4, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
}

void MoveBackward()
{
  SetRing(4, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(3, maxi, maxi, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(4, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(3, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(3, mini, mini, mini);
  SetSegment(2, maxi , maxi, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(3, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(2, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(1, maxi, maxi, maxi);
  SetSegment(2, mini, mini, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(2, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(1, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(1, mini, mini, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(1, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
}

void Publish(int speedPercentage)
{
  sendToChannel(channelA, speedPercentage, segs[0][0], segs[0][1], segs[0][2], segs[0][3], segs[0][4]);
  sendToChannel(channelB, speedPercentage, segs[1][0], segs[1][1], segs[1][2], segs[1][3], segs[1][4]);
  sendToChannel(channelC, speedPercentage, segs[2][0], segs[2][1], segs[2][2], segs[2][3], segs[2][4]);
  delay(myDelay);
}

void sendToChannel(int channel, int speedPercentage, byte s1, byte s2, byte s3, byte s4, byte s5)
{
  Wire.beginTransmission(channel);
  Wire.write(s1);
  Wire.write(s2);
  Wire.write(s3);
  Wire.write(s4);
  Wire.write(s5);
  Wire.write(speedPercentage);

  Wire.endTransmission();
}



void FullBodyUp()
{
  SetSegment(1, mini, maxi, maxi);
  SetSegment(2, mini, maxi, maxi);
  SetSegment(3, mini, maxi, maxi);
  Publish(pubSpeed);
  delay(myDelay);
}

void FullBodyLeft()
{
  SetSegment(1, maxi, mini, maxi);
  SetSegment(2, maxi, mini, maxi);
  SetSegment(3, maxi, mini, maxi);
  Publish(pubSpeed);
  delay(myDelay);
}

void FullBodyRight()
{
  SetSegment(1, maxi, maxi, mini);
  SetSegment(2, maxi, maxi, mini);
  SetSegment(3, maxi, maxi, mini);
  Publish(pubSpeed);
  delay(myDelay);
}


void LookUp()
{
  SetRing(1, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetSegment(1, mini, maxi, maxi);
  Publish(20);
  delay(myDelay);
}

void LookLeft()
{
  SetRing(1, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetSegment(1, maxi, mini, maxi);
  Publish(20);
  delay(myDelay);
}

void LookRight()
{
  SetRing(1, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetSegment(1, maxi, maxi, mini);
  Publish(20);
  delay(myDelay);
}




void ContractFromBehind()
{

  // prepare
  SetRing(1, mini);
  SetSegment(1, mini, mini, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }


  SetRing(4, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(3, mini, mini, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(4, maxi);
  SetRing(3, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(2, mini, mini, mini);
  SetSegment(3, maxi, maxi, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(3, maxi);
  SetRing(2, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(2, maxi, maxi, maxi);
  SetSegment(3, mini, mini, mini);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(1, mini);
  SetRing(2, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetSegment(1, maxi, maxi, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }
  SetRing(1, maxi);
  Publish(pubSpeed);
  if (state == 1) {
    return;
  }

}

void Nutation(int angle, int offSetA, int offSetB, int offSetC)
{
  float rad1 = (angle + offSetA) * pi / 180.0;
  float rad2 = (angle + offSetB) * pi / 180.0;
  float rad3 = (angle + offSetC) * pi / 180.0;
  angle1 = (int)( mini + (maxi - mini) / 2.0 ) - (sin(rad1) * (maxi - mini) / 2);
  angle2 = (int)( mini + (maxi - mini) / 2.0 ) - (sin(rad2) * (maxi - mini) / 2);
  angle3 = (int)( mini + (maxi - mini) / 2.0 ) - (sin(rad3) * (maxi - mini) / 2);
}


void DoIdle()
{
  InitialPosition();
  Publish(pubSpeed);
}

void DoRunForward()
{
  MoveForward();

}

void DoRunBackward()
{
  MoveBackward();
}

void DoTurnLeft()
{
  FullBodyLeft();
  ContractFromBehind();
}

// -----------------------------
void showIdle()
{
  toggle = !toggle;
  if (toggle)
  {
    SetLed(0, 0, 0, 0);
    SetLed(1, 22, 0, 0);
    SetLed(2, 0, 0, 0);
  }
  else
  {
    SetLed(0, 22, 0, 0);
    SetLed(1, 0, 0, 0);
    SetLed(2, 22, 0, 0);
  }
}

void showMoveLeft()
{
  SetLed(0, 11, 11, 0);
  SetLed(1, 11, 11, 0);
  SetLed(2, 11, 11, 0);
}

void showMoveRight()
{
  SetLed(0, 11, 0, 11);
  SetLed(1, 11, 0, 11);
  SetLed(2, 11, 0, 11);
}

void showRunningForward()
{
  toggle = !toggle;
  if (toggle)
  {
    SetLed(0, 0, 11, 0);
    SetLed(1, 0, 0, 0);
    SetLed(2, 0, 11, 0);
  }
  else
  {
    SetLed(0, 0, 0, 0);
    SetLed(1, 0, 11, 0);
    SetLed(2, 0, 0, 0);
  }
}

void showRunningBackward()
{
  toggle = !toggle;
  if (toggle)
  {
    SetLed(0, 0, 0, 15);
    SetLed(1, 0, 0, 0);
    SetLed(2, 0, 0, 15);
  }
  else
  {
    SetLed(0, 0, 0, 0);
    SetLed(1, 0, 0, 15);
    SetLed(2, 0, 0, 0);
  }
}

void showLookLeft()
{

  SetLed(0, 0, 0, 0);
  SetLed(1, 0, 0, 0);
  SetLed(2, 0, 15, 15);
}

void showLookRight()
{

  SetLed(0, 0, 0, 0);
  SetLed(1, 0, 0, 0);
  SetLed(2, 15, 15, 0);
}








void InitialShowOff()
{
  DoIdle();
  Wave2();
  DoIdle();
  Wave();
  DoIdle();
  ShrinkExpand();
  DoIdle();
}

// ---------------------------------------------
void Wave()
{
  SetSegment(1, mini, mini, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetSegment(2, mini, mini, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetSegment(3, mini, mini, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetSegment(3, maxi, maxi, maxi);
  Publish(pubSpeed);
  delay(myDelay);
  SetSegment(2, maxi, maxi, maxi);
  Publish(pubSpeed);
  delay(myDelay);
  SetSegment(1, maxi, maxi, maxi);
  Publish(pubSpeed);
  delay(myDelay);
}

void Wave2()
{
  SetRing(1, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetRing(2, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetRing(3, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetRing(4, mini);
  Publish(pubSpeed);
  delay(myDelay);
  SetRing(4, maxi);
  Publish(pubSpeed);
  delay(myDelay);
  SetRing(3, maxi);
  Publish(pubSpeed);
  delay(myDelay);
  SetRing(2, maxi);
  Publish(pubSpeed);
  delay(myDelay);
  SetRing(1, maxi);
  Publish(pubSpeed);
  delay(myDelay);

}

void ShrinkExpand()
{
  SetSegment(1, mini, mini, mini);
  SetSegment(2, mini, mini, mini);
  SetSegment(3, mini, mini, mini);
  SetRing(1, mini);
  SetRing(2, mini);
  SetRing(3, mini);
  SetRing(4, mini);
  Publish(pubSpeed);
  delay(myDelay);

  SetSegment(1, maxi, maxi, maxi);
  SetSegment(2, maxi, maxi, maxi);
  SetSegment(3, maxi, maxi, maxi);
  SetRing(1, maxi);
  SetRing(2, maxi);
  SetRing(3, maxi);
  SetRing(4, maxi);
  Publish(pubSpeed);
  delay(myDelay);
}

void ZigZack()
{
  SetRing(1, 100);
  SetRing(2, 100);
  SetRing(3, 100);
  SetRing(4, 100);
  Publish(pubSpeed);
  delay(myDelay);

  int speedo = 30;
  for (int i = 0; i < 5; i++)
  {
    SetSegment(1, mini, maxi, mini);
    SetSegment(2, mini, mini, maxi);
    SetSegment(3, mini, maxi, mini);
    Publish(speedo);
    delay(myDelay);
    SetSegment(1, mini, mini, maxi);
    SetSegment(2, mini, maxi, mini);
    SetSegment(3, mini, mini, maxi);
    Publish(speedo);
    delay(myDelay);
    speedo += 10;
  }

  speedo = 30;
  for (int i = 0; i < 5; i++)
  {
    SetSegment(1, maxi, maxi, mini);
    SetSegment(2, mini, maxi, maxi);
    SetSegment(3, maxi, maxi, mini);
    Publish(speedo);
    delay(myDelay);
    SetSegment(1, mini, mini, maxi);
    SetSegment(2, maxi, maxi, mini);
    SetSegment(3, mini, mini, maxi);
    Publish(speedo);
    delay(myDelay);
    speedo += 10;
  }

}

void CrazyNutation()
{
  for (int i = 0; i < 360; i += 10)
  {
    Nutation(i, 0, 90, 180);
    SetSegment(1, angle1, angle2, angle3);
    SetSegment(2, angle1, angle2, angle3);
    SetSegment(3, angle1, angle2, angle3);
    Publish(100);
    delay(1);


  }
}

// sealed !!!
void ContractAll()
{
  // set all segments to mini;
  for (int i = 0; i < 3; i++)
  {
    for (int k = 0; k < 5; k++)
    {
      segs[i][k] = mini;
    }
  }
}

// sealed !!!
void InitialPosition()
{
  ContractAll();
  // expand all rings
  // ring 1
  int i = 0;
  segs[i][3] = maxi;
  // ring 2
  i = 1;
  segs[i][3] = maxi;
  // ring 3
  i = 2;
  segs[i][3] = maxi;
  // ring 4
  segs[i][4] = maxi;
}

// sealed !!!
void CalibrateSensors()
{
  for (int i = 0; i < 20; i++)
  {
    SetLed(3, 0, 0, 11);
    SetLed(4, 0, 0, 11);
    ShowLeds();
    delay(50);
    SetLed(3, 0, 0, 0);
    SetLed(4, 0, 0, 0);
    ShowLeds();
    delay(50);
  }

  sensorMax0 = 0;
  sensorMax1 = 0;
  for (int i = 0; i < 10; i++)
  {
    sensorValue0 = analogRead(sensorPin0);
    if (sensorValue0 > sensorMax0)
    {
      sensorMax0 = sensorValue0;
    }
    if (sensorValue1 > sensorMax1)
    {
      sensorMax1 = sensorValue1;
    }
    sensorValue1 = analogRead(sensorPin1);
    delay(50);
  }
}

void ShowLeds()
{
  for (int i = 0; i < NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color((int)leds[i][0], (int)leds[i][1], (int)leds[i][2])); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}

void SetLed(int i, int c1, int c2, int c3)
{
  leds[i][0] = c1;
  leds[i][1] = c2;
  leds[i][2] = c3;
}

void ResetLeds()
{
  SetLed(0, 0, 0, 0);
  SetLed(1, 0, 0, 0);
  SetLed(2, 0, 0, 0);
  SetLed(3, 0, 0, 0);
  SetLed(4, 0, 0, 0);
  ShowLeds();
}

// ---- read all sensors and evaluate and set state accordingly
void ReadSensors()
{
  // +1 = right, -1 = left , 0 = straight
  int directionIndicator = 0;

  // read the value from the sensor:
  sensorValue0 = analogRead(sensorPin0);
  sensorValue1 = analogRead(sensorPin1);
  p1 = (sensorMax0 - sensorValue0) / (sensorMax0 * 1.0) * 100.0;
  p2 = (sensorMax1 - sensorValue1) / (sensorMax1 * 1.0) * 100.0;

  if (p1 > 0 && p1 < 4)
  {
    SetLed(3, 22, 0, 0);
  }
  if (p2 > 0 && p2 < 4)
  {
    SetLed(4, 22, 0, 0);
  }

  if (p1 >= 4 && p1 < 10)
  {
    SetLed(3, 33, 0, 0);
  }
  if (p2 >= 4 && p2 < 10)
  {
    SetLed(4, 33, 0, 0);
  }

  if (p1 >= 10 && p1 < 90)
  {
    SetLed(3, 0, p1, 0);
  }
  if (p2 >= 10 && p2 < 90)
  {
    SetLed(4, 0, p2, 0);
  }

  if (p1 >= 90)
  {
    SetLed(3, 0, 0, 100);
  }
  if (p2 >= 90)
  {
    SetLed(4, 0, 0, 100);
  }


  if (p1 <= 0)
  {
    SetLed(3, 0, 0, 0);
  }
  if (p2 <= 0)
  {
    SetLed(4, 0, 0, 0);
  }

  if (p1 < 1 && p2 < 1)
  {
    sensor = 0;
  }

  ShowLeds();

  if (millis() < timeDisabled)
  {
    return;
  }

  // run forward
  if (p1 >= 10 && p2 >= 10  && ( p1 < 90 || p2 < 90))
  {
    timeDelta = millis() - time;
    if (timeDelta > 500)
    {
      sensorBefore = sensor;
      sensor = 3;
      time = millis();
      if (state == 1)
      {
        if (sensorBefore == sensor)
        {
          // go to state running
          state = 3;
          sensorBefore = -1;
          timeDisabled = millis() + 2000;
        }
      }
      else
      {
        if (sensorBefore == sensor)
        {
          // back to idle
          state = 1;
          sensorBefore = -1;
          timeDisabled = millis() + 2000;
        }
      }
    }
  }

  // move right
  if (state == 3 && p1 >= 10 && p2 <= 5)
  {
    timeDelta = millis() - time;
    if (timeDelta > 500)
    {
      sensorBefore = sensor;
      sensor = 13;
      time = millis();
      if (sensorBefore == sensor)
      {
        // Move left
        state = 13;
        sensorBefore = -1;
        timeDisabled = millis() + 2000;
      }
    }
  }


  // move left
  if (state == 3 && p1 <= 5 && p2 >= 10)
  {
    timeDelta = millis() - time;
    if (timeDelta > 500)
    {
      sensorBefore = sensor;
      sensor = 12;
      time = millis();
      if (sensorBefore == sensor)
      {
        // Move right
        state = 12;
        sensorBefore = -1;
        timeDisabled = millis() + 2000;
      }
    }
  }


  // run backward
  if (p1 >= 90 && p2 >= 90 && state == 1)
  {
    timeDelta = millis() - time;
    if (timeDelta > 500)
    {
      sensorBefore = sensor;
      sensor = 5;
      time = millis();
      if (state == 1)
      {
        if (sensorBefore == sensor)
        {
          // go to state running
          state = 5;
          timeDisabled = millis() + 2000;
        }
      }
    }
  }

  // re-calibrate
  if (p1 >= 15 && p2 >= 15 && (state == 7 || state == 9))
  {
    timeDelta = millis() - time;
    if (timeDelta > 200)
    {
      sensorBefore = sensor;
      sensor = 11;
      time = millis();
      if (sensorBefore == sensor)
      {
        // go to state running
        state = 11;
        timeDisabled = millis() + 2000;
      }
    }
  }

  // look left
  if (state != 3 && p1 >= 15 && p2 <= 5)
  {
    timeDelta = millis() - time;
    if (timeDelta > 200)
    {
      sensorBefore = sensor;
      sensor = 7;
      time = millis();
      if (state == 1)
      {
        if (sensorBefore == sensor)
        {
          // go to state running
          state = 7;
          timeDisabled = millis() + 2000;
        }
      }
    }
  }

  // look right
  if (state != 3 && p1 <= 5 && p2 >= 15)
  {
    timeDelta = millis() - time;
    if (timeDelta > 500)
    {
      sensorBefore = sensor;
      sensor = 9;
      time = millis();
      if (state == 1)
      {
        if (sensorBefore == sensor)
        {
          // go to state running
          state = 9;
          timeDisabled = millis() + 2000;
        }
      }
    }
  }

  // back to idle
  if (p1 <= 10 && p2 <= 10 && (state == 7 || state == 9 || state == 11))
  {
    timeDelta = millis() - time;
    if (timeDelta > 500)
    {
      sensorBefore = sensor;
      sensor = 0;
      time = millis();

      state = 1;
      timeDisabled = millis() + 500;

    }
  }
}
