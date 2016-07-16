// Slave Wire

#include <Wire.h>
#include <Servo.h>

const int InternalLed = 13;
const int ChannelSetterA = 7;
const int ChannelSetterB = 8;
const bool IsDebug = 0;
const int speedMinDelay = 1;
const int speedMaxDelay = 100;

// output PWM
int servoPin1 = 3; // 1 (stretch)
int servoPin2 = 5; // 2 (stretch)
int servoPin3 = 6; // 3  (stretch)
int servoPin4 = 9; //  ring
int servoPin5 = 11; // extra ring
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

int speedDelay = 35;

int mini = 20;
int maxi = 180;

int values[] = {mini, mini, mini, mini, mini};
int speedPercentage = 10;

int ts1 = values[0];
int ts2 = values[1];
int ts3 = values[2];
int ts4 = values[3];
int ts5 = values[4];

void setup() {
  Serial.begin(9600);
  // initialize internal LED
  pinMode(InternalLed, OUTPUT);
  digitalWrite(InternalLed, LOW);

  int ch = ReadChannel();
  delay(((ch - 4) * 50) + 20);
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);
  servo5.attach(servoPin5);
  digitalWrite(InternalLed, HIGH);
  delay(200);
  digitalWrite(InternalLed, LOW);

  // read receiver channel from jumper position
  Initialize();
}

void loop() {
  delay(speedDelay);
  MoveToTarget();
  //  Dispatcher();
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {

  int i = 0;
  while (1 < Wire.available()) { // loop through all but the last
    // read angles for all servos
    values[i] = Wire.read();  // receive byte as an integer
    i++;
  }
  // finally read speed
  speedPercentage = Wire.read();    // receive byte as an integer
  if (speedPercentage > 0 && speedPercentage <= 100)
  {
    speedDelay = speedMinDelay / (speedPercentage / 100.0);
    if (speedDelay > speedMaxDelay)
    {
      speedDelay = speedMaxDelay;
    }
  }
  // show event on internal LED
  SignalReceive();

}


int ReadChannel()
{
  pinMode(ChannelSetterA, INPUT);
  pinMode(ChannelSetterB, INPUT);
  delay(500);
  int b = digitalRead(ChannelSetterB);
  int a = digitalRead(ChannelSetterA);
  int myChannel = 4 + (a * 2 + b * 1);
  if (IsDebug) {
    Serial.println("myChannel = " + String(myChannel) + " a: " + String(a) + " - b: " + String(b) );
  }
  return (myChannel);
}

void ShowChannel(int ch)
{

  for (int i = 0; i < 10; i++)
  {
    digitalWrite(InternalLed, HIGH);
    delay(20);
    digitalWrite(InternalLed, LOW);
    delay(50);
  }
  delay(1000);

  for (int i = 0; i < ch; i++)
  {
    digitalWrite(InternalLed, HIGH);
    delay(300);
    digitalWrite(InternalLed, LOW);
    delay(400);
  }
}

void SignalReceive()
{
  digitalWrite(InternalLed, HIGH);
  delay(30);
  digitalWrite(InternalLed, LOW);
}

void MoveToTarget()
{
  if (ts1 < values[0])
  {
    ts1++;
    servo1.write(ts1);
  }
  if (ts1 > values[0])
  {
    ts1--;
    servo1.write(ts1);
  }

  if (ts2 < values[1])
  {
    ts2++;
    servo2.write(ts2);
  }
  if (ts2 > values[1])
  {
    ts2--;
    servo2.write(ts2);
  }

  if (ts3 < values[2])
  {
    ts3++;
    servo3.write(ts3);
  }
  if (ts3 > values[2])
  {
    ts3--;
    servo3.write(ts3);
  }

  if (ts4 < values[3])
  {
    ts4++;
    servo4.write(ts4);
  }
  if (ts4 > values[3])
  {
    ts4--;
    servo4.write(ts4);
  }

  if (ts5 < values[4])
  {
    ts5++;
    servo5.write(ts5);
  }
  if (ts5 > values[4])
  {
    ts5--;
    servo5.write(ts5);
  }
}

void Initialize()
{
  // read receiver channel from jumper position
  int channel = ReadChannel();
  if (IsDebug)
  {
    ShowChannel(channel);
  }

  // setup receiver channel
  Wire.begin(channel);
  Wire.onReceive(receiveEvent);
}


