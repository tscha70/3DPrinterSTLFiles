#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <Servo.h>
#include "U8glib.h"
#define NeoPixelPin 1
#define NUMPIXELS 3
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NeoPixelPin, NEO_GRB + NEO_KHZ800);
#define charactersPerLine 14
#define backlight_pin 2 // LCD-Backlight
#define switchS2 9 // calibrate
#define switchS1 8 // start/stop
#define led1 13 // start-stop-LED
#define led2 12 // calibrate-LED
#define servo1 10 // left
#define servo2 11 // right
#define SERVONULL 90
#define FASTFWD 55
#define FASTBWD 135
#define SLOWFWD 85
#define SLOWBWD 95

// free pins 0,1 (
// used pins 2,3,4,5,6,7,8,9,10,11,12,13

// Infrared-Sensors
#define sensorM1 0 // middle
#define sensorM2 1 // right
#define sensorM3 2 // left


#define MOVEFWD 8     // move forward
#define MOVELFT 4     // move left
#define MOVERGT 6     // move right
#define MOVEBCK -8    // move backward
#define MOVELFTB -4   // move left backward
#define MOVERGTB -6   // move right backward
#define MOVELFTS 2    // move sharp left
#define MOVERGTS 3    // move sharp right
#define MOVESTOP 0    // stop
#define MOVESRCH 10   // signal low move searching
#define ABORT 11 // abort, cause no line was found in x turns

#define t3 200 // threshould t3 // 8
#define t2 50 // threshould t2 // 2
#define t1 20 // threshould t1 

int currentDirection = MOVESTOP;
int previousDirection = currentDirection;
bool isStarted = 0;
bool isCalibrated = 0;
int turnCounter = 0;
int stopCounter = 0;




// NOKIA 5110
U8GLIB_PCD8544 u8g(3, 4, 6, 5, 7);  // CLK=3, DIN=4, CE=6, DC=5, RST=7

// variables
bool bottonS2Pressed = 0;
bool buttonS1Pressed = 0;
int m1mes = 0;
int m1min = 1000;
float m1max = 0;
int m1 = 0;

int m2mes = 0;
float m2min = 1000;
int m2max = 0;
int m2 = 0;

int m3mes = 0;
int m3min = 1000;
float m3max = 0;
int m3 = 0;

Servo servoLeft;
Servo servoRight;

// the setup function runs once when you press reset or power the board
void setup() {
  pixels.begin(); // This initializes the NeoPixel library.

  testNeoPixel();

  servoLeft.attach(servo1);
  servoRight.attach(servo2);
  servoLeft.write(SERVONULL);
  servoRight.write(SERVONULL);
  // findServoZero();

  // Nokia-LCD 5110
  analogWrite(backlight_pin, 35);  /* Set the Backlight intensity */

  // initialize digital pin 13 as an output.
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(switchS2, INPUT);
  pinMode(switchS1, INPUT);
  digitalWrite(switchS2, HIGH);
  digitalWrite(switchS1, HIGH);
  displayLines("PLEASE", "CALIBRATE", "FIRST!", "<PUSH BUTTON!>");
}

// the loop function runs over and over again forever
void loop() {

  //start-stop-button
  readButtonS1();

  // calibration-button
  readButtonS2();

  // read and display the infrared-sensors (m1, m2, m3)
  readAndDisplayPIRS();

  getCurrentDirection();
  showDirection();
  followTheLine();


  /* end of loop */
}

void followTheLine()
{
  if (isStarted)
  {
    // do something
    if (turnCounter > 1500)
    {
      currentDirection = ABORT;
      isStarted = 0;
      digitalWrite(led1, LOW);
      servoLeft.write(SERVONULL);
      servoRight.write(SERVONULL);
      displayLines("ABORTED", "", ".. TO START", "<PUSH BUTTON!>");
    }

    switch (currentDirection)
    {
      case MOVEFWD:
        turnCounter = 0;
        stopCounter = 0;
        servoLeft.write(FASTFWD);
        servoRight.write(FASTFWD);
        break;

      case MOVESTOP:
        turnCounter = 0;
        stopCounter = 0;
        servoLeft.write(SERVONULL);
        servoRight.write(SERVONULL);
        break;

      case MOVELFT:
        if (turnCounter > t3)
        {
          servoLeft.write(FASTBWD);
          servoRight.write(FASTFWD);
        }
        else if (turnCounter > t2)
        {
          servoLeft.write(SLOWBWD);
          servoRight.write(FASTFWD);
        }
        else if (turnCounter > t1)
        {
          servoLeft.write(SERVONULL);
          servoRight.write(FASTFWD);
        }
        else
        {
          servoLeft.write(SLOWFWD);
          servoRight.write(FASTFWD);
        }


        turnCounter ++;
        break;
      case MOVERGT:
        if (turnCounter > t3)
        {
          servoLeft.write(FASTFWD);
          servoRight.write(FASTBWD);
        }
        else if (turnCounter > t2)
        {
          servoLeft.write(FASTFWD);
          servoRight.write(SLOWBWD);
        }
        else if (turnCounter > t1)
        {
          servoLeft.write(FASTFWD);
          servoRight.write(SERVONULL);
        }
        else
        {
          servoLeft.write(FASTFWD);
          servoRight.write(SLOWFWD);
        }
        turnCounter ++;
        break;
      case MOVESRCH:
        stopCounter = 0;
        turnCounter = 0;
        // keep going!
        servoLeft.write(FASTFWD);
        servoRight.write(FASTFWD);
        //servoLeft.write(SLOWFWD);
        //servoRight.write(SLOWFWD);
        break;
      default:
        stopCounter = 0;
        turnCounter = 0;
        servoLeft.write(SERVONULL);
        servoRight.write(SERVONULL);
        break;
    }
  }
  else
  {
    servoLeft.write(SERVONULL);
    servoRight.write(SERVONULL);
  }
}

void getCurrentDirection()
{
  if (isCalibrated)
  {
    previousDirection = currentDirection;
    if (m2 > m1 && m2 > 40)
    {
      currentDirection = MOVELFT;
    }
    if (m1 > m2 && m1 > m3 && m1 > 10)
    {
      currentDirection = MOVEFWD;
    }
    if (m3 > m1 && m3 > 40)
    {
      currentDirection = MOVERGT;
    }

    if (currentDirection == MOVEFWD && m1 < 10)
    {
      // search for better signal
      currentDirection = MOVESRCH;
    }

    if (m1 > 40 && m2 > 40 && m3 > 40)
    {
      if (stopCounter > 100)
      {
        currentDirection = MOVESTOP;
      }
    }
  }
  else
  {
    currentDirection = MOVESTOP;
  }
}

void showDirection()
{
  switch (currentDirection)
  {
    case MOVELFT:
      setPixel(2, 40, 0, 0);
      setPixel(0, 0 , 0 , 0);
      setPixel(1, 0 , 0 , 0);
      break;
    case MOVERGT:
      setPixel(0, 40, 0, 0);
      setPixel(1, 0 , 0 , 0);
      setPixel(2, 0 , 0 , 0);
      break;
    case MOVEFWD:
      setPixel(1, 0, 25, 0);
      setPixel(0, 0 , 0 , 0);
      setPixel(2, 0 , 0 , 0);
      break;
    case MOVESTOP:
      setPixel(0, 40 , 0 , 0);
      setPixel(1, 40 , 0 , 0);
      setPixel(2, 40 , 0 , 0);
      break;
    case MOVESRCH:
      setPixel(0, 10 , 10 , 10);
      setPixel(1, 0 , 0 , 35);
      setPixel(2, 10 , 10 , 10);
      break;
    case ABORT:
      setPixel(0, 15 , 0 , 0);
      setPixel(1, 0 , 0 , 0);
      setPixel(2, 15, 0 , 0);
      break;
    default:
      setPixel(0, 15 , 0 , 0);
      setPixel(1, 0 , 15 , 0);
      setPixel(2, 0 , 0 , 15);
      break;
  }
}

void readButtonS2()
{
  bottonS2Pressed = !digitalRead(switchS2);
  if (bottonS2Pressed)
  {
    digitalWrite(led2, HIGH);
    calibrate();
  }
  else
  {
    digitalWrite(led2, LOW);
  }

}

void readButtonS1()
{
  buttonS1Pressed = !digitalRead(switchS1);

  if (buttonS1Pressed)
  {
    digitalWrite(led1, HIGH);
    delay(100);
    digitalWrite(led1, LOW);
    if (isStarted == 1)
    {
      isStarted = 0;
      digitalWrite(led1, LOW);
      displayLines("READY TO", "RUMBLE!", ".. TO START", "<PUSH BUTTON!>");
    }
    else
    {
      isStarted = 1;
      digitalWrite(led1, HIGH);
      displayLines("RUNNING", "", ".. TO STOP", "<PUSH BUTTON!>");
    }

    delay(500);
  }

}

void readAndDisplayPIRS()
{
  m1mes = analogRead(sensorM1);
  m2mes = analogRead(sensorM2);
  m3mes = analogRead(sensorM3);

  m1  = map(m1mes, m1min, m1max, 0, 100);
  m2  = map(m2mes, m2min, m2max, 0, 100);
  m3  = map(m3mes, m3min, m3max, 0, 100);

  String xtra = getFixLength(String(m3), 3) + "," + getFixLength(String(m1), 3) + "," + getFixLength(String(m2), 3);

  if (!(m1 < 1000 && m1 > -1000))
  {
    xtra = ""; // otherwise overflow of display - and crash
  }

  // displayLines("m3:" +  getFixLength(String(m3mes), 4) + " +" + getFixLength(String(turnCounter), 3), "m1:" + getFixLength(String(m1mes), 4) + " +" + getFixLength(String(stopCounter), 3), "m2:" + getFixLength(String(m2mes), 4), xtra);
}


void calibrate()
{
  isStarted = 0;
  m1min = 1000;
  m1max = 0;
  m2min = 1000;
  m2max = 0;
  m3min = 1000;
  m3max = 0;
  for (int i = 0; i < 10; i++)
  {
    delay(100);
    digitalWrite(led2, HIGH);
    delay(100);
    digitalWrite(led2, LOW);
  }

  for (int i = 0; i < 200; i++)
  {
    digitalWrite(led2, HIGH);
    m1mes = analogRead(sensorM1);
    m2mes = analogRead(sensorM2);
    m3mes = analogRead(sensorM3);

    displayLines("m3: " + String(m3mes), "m1: " + String(m1mes), "m2: " + String(m2mes), "calibrating...");

    if (m1mes > m1max)
    {
      m1max = m1mes;
    }

    if (m1mes < m1min)
    {
      m1min = m1mes;
    }

    if (m2mes > m2max)
    {
      m2max = m2mes;
    }

    if (m2mes < m2min)
    {
      m2min = m2mes;
    }

    if (m3mes > m3max)
    {
      m3max = m3mes;
    }

    if (m3mes < m3min)
    {
      m3min = m3mes;
    }

    delay (1);
  }
  digitalWrite(led2, LOW);
  isCalibrated = 1;
  displayLines("READY TO", "RUMBLE!", ".. TO START", "<PUSH BUTTON!>");

}




void draw(String s) {

  // initalize with 14 spaces
  char line[4][15] = {"              ", "              ", "              ", "              "};

  int i = 0;
  while (s.length() > charactersPerLine)
  {
    if (s.length() >= charactersPerLine)
    {
      pushMessageToCharBuffer(s.substring(0, charactersPerLine), line[i]);

      // cut off 14 characters
      s = s.substring(charactersPerLine, s.length());
    }
    i++;
  }
  if (s.length() > 0)
  {
    pushMessageToCharBuffer(s, line[i]);
  }

  u8g.setFont(u8g_font_profont11);  // select font
  u8g.drawStr(0, 10, line[0]);  // text-line 1
  u8g.drawStr(0, 20, line[1]);  // text-line 2
  u8g.drawStr(0, 30, line[2]);  // text-line 3
  u8g.drawStr(0, 40, line[3]);  // text-line 4
  u8g.setPrintPos(15, 15);  // set position
}

void displayText(String text)
{
  // Nokia-LCD 5110 show on display
  u8g.firstPage();
  do {
    draw(text);
  } while ( u8g.nextPage() );
}

// convert message to char-buffer
void pushMessageToCharBuffer(String s, char *charArray)
{
  unsigned int len = s.length() + 1; // one extra for null-terminator
  s.toCharArray(charArray, len);
}

void displayLines(String s1, String s2, String s3, String s4)
{
  char line[4][15] = {"              ", "              ", "              ", "              "};
  pushMessageToCharBuffer(s1, line[0]);
  pushMessageToCharBuffer(s2, line[1]);
  pushMessageToCharBuffer(s3, line[2]);
  pushMessageToCharBuffer(s4, line[3]);
  // Nokia-LCD 5110 show on display
  u8g.firstPage();
  do
  {
    u8g.setFont(u8g_font_profont11);  // select font
    u8g.drawStr(0, 10, line[0]);  // text-line 1
    u8g.drawStr(0, 20, line[1]);  // text-line 2
    u8g.drawStr(0, 30, line[2]);  // text-line 3
    u8g.drawStr(0, 40, line[3]);  // text-line 4
    u8g.setPrintPos(15, 15);  // set position
  } while ( u8g.nextPage() );
}

String getFixLength(String s, int digits)
{
  int x = s.length();
  if (x < digits)
  {
    for (int i = 0; i < (digits - x); i++)
    {
      s = "0" + s;
    }
  }
  return s;
}

void setPixel(int i, int r, int g, int b)
{
  pixels.setPixelColor(i, pixels.Color(r, g, b)); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void testNeoPixel()
{
  for (int k = 0; k < 3; k++)
  {
    for (int i = 0; i < NUMPIXELS; i++)
    {
      setPixel(i, 0, 30, 0);
      delay(50);
      setPixel(i, 0, 0, 0);
      delay (50);
    }
  }
}

void findServoZero()
{
  servoLeft.write(FASTFWD);
  servoRight.write(FASTFWD);

  for (int i = 87; i <= 93; i++)
  {
    servoLeft.write(i);
    displayLines("servoLeft: " + String(i), "", "", "");
    delay(2000);
  }

  for (int i = 87; i <= 93; i++)
  {
    servoRight.write(i);
    displayLines("servoRight: " + String(i), "", "", "");
    delay(2000);
  }

}
