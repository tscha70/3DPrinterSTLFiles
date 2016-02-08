/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32 size display using SPI to communicate
4 or 5 pins are required to interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

bool alternator = false;
// int ledPin = 13; // choose the pin for the LED
int buttonPin = 8;   // choose the input pin (for a pushbutton)
int buttonVal = 0;     // variable for reading the pin status


// stepper 1 Pins
int pinDir_1 = 30; // 2;
int pinStep_1 = 28; //3;
int pinSleep_1 = 26; //4;
int pinMS3_1 = 24; //5;
int pinMS2_1 = 22; // 6;
int pinMS1_1 = 20; // 7;

int pinDir_2 = 42; // 2;
int pinStep_2 = 40; //3;
int pinSleep_2 = 38; //4;
int pinMS3_2 = 36; //5;
int pinMS2_2 = 34; // 6;
int pinMS1_2 = 32; // 7;

int absoluteX = 0;
int absoluteY = 0;

long randNumberX;
long randNumberY;


#define ARRAYSIZE 2
String stepperDirections[ARRAYSIZE] = { "backward", "forward" };

int maxDistance = 200;
int minSleepMillis = 70;
int enableDebug = 0;

void setup()   {
  Serial.begin(9600);

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  pinMode(buttonPin, INPUT);    // declare pushbutton as input

  pinMode(pinMS1_1, OUTPUT);
  pinMode(pinMS2_1, OUTPUT);
  pinMode(pinMS3_1, OUTPUT);
  pinMode(pinStep_1, OUTPUT);
  pinMode(pinDir_1, OUTPUT);
  pinMode(pinSleep_1, OUTPUT);

  pinMode(pinMS1_2, OUTPUT);
  pinMode(pinMS2_2, OUTPUT);
  pinMode(pinMS3_2, OUTPUT);
  pinMode(pinSleep_2, OUTPUT);
  pinMode(pinStep_2, OUTPUT);
  pinMode(pinDir_2, OUTPUT);

  // initialize stepper 1
  // MS1, MS2, MS3 low = full speed -- 111 == 1/16-steps
  digitalWrite(pinMS1_1, HIGH);
  digitalWrite(pinMS2_1, HIGH);
  digitalWrite(pinMS3_1, HIGH);
  digitalWrite(pinSleep_1, LOW);
  digitalWrite(pinDir_1, LOW);
  digitalWrite(pinStep_1, LOW);

  // initialize stepper 2
  digitalWrite(pinMS1_2, HIGH);
  digitalWrite(pinMS2_2, HIGH);
  digitalWrite(pinMS3_2, HIGH);
  digitalWrite(pinSleep_2, LOW);
  digitalWrite(pinDir_2, LOW);
  digitalWrite(pinStep_2, LOW);

  alternator = false;

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(250);

  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
  // display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  //display.display();
  //delay(2000);
  // display.clearDisplay();

  testdrawchar();
  delay(250);
  initalizeTextDisplay();

}


void loop() {
  alternator = !alternator;
  buttonVal = digitalRead(buttonPin);
  if (buttonVal == HIGH)
  {
    if (alternator)
    {
      showOnDisplay( "press button..");
    }
    else
    {
      showOnDisplay( "press button...");
    }
    // send stepper to sleep - no current flow!
    SleepBoth();
  }
  else
  {
for (int i = 0; i < 2; i++)
{
    WriteArduinoDue();
    delay(1000);
    }
    
    OpenAndCloseCircle();
    
    for (int i = 0; i < 10; i++)
    {
      PrintStar(50, 0, 0, 0, 0);
    }

    for (int i = 0; i < 10; i++)
    {
      PrintStar(15, 0, 0, 0, 0);
    }
    
    showOnDisplay( "RunStarInBox");
    RunStarInBox(15, 500);

    //    OpenAndCloseCircle();
    //    MoveRight();
    //    MoveUp();
    //    MoveLeft();
    //    MoveDown();
    //    MoveCenter();
    //    MoveTopLeft();
    //
    //    RunInBox();

    Run(-100, 100);

    showOnDisplay("X: " + String(absoluteX), "Y: " + String(absoluteY));
    delay(150);
    SleepBoth();
    delay(1000);

  }

  delay(150);
  SleepBoth();

}
// loop end

void PrintStar(int fullSize, double transX, double transY, double rotX, double rotY)
{
  double o = 0.0;
  double u = (double)fullSize;
  
  Run(o + transX, u + transY);
  Run((0.8 * u) + transX, (-0.8 * u) + transY);
  Run(-u + transX, (0.2 * u) + transY);
  Run(u + transX, (0.2 * u) + transY);
  Run((-0.8 * u) + transX, (-0.8 * u) + transY);


}

void WriteArduinoDue()
{
  for (int i = 0; i < 1; i++)
  {
    PrintA();
  }
  for (int i = 0; i < 1; i++)
  {
    PrintR();
  }
  for (int i = 0; i < 1; i++)
  {
    PrintD();
  }

  for (int i = 0; i < 1; i++)
  {
    PrintU();
  }
  for (int i = 0; i < 1; i++)
  {
    PrintI();
  }

  for (int i = 0; i < 1; i++)
  {
    PrintN();
  }
  for (int i = 0; i < 1; i++)
  {
    PrintO();
  }
  for (int i = 0; i < 1; i++)
  {
    PrintDot();
  }

  for (int i = 0; i < 1; i++)
  {
    PrintD();
  }

  for (int i = 0; i < 1; i++)
  {
    PrintU();
  }

  for (int i = 0; i < 1; i++)
  {
    PrintE();
  }
}

void PrintA()
{
  for (int i = 0; i < 3; i++)
  {
    Run(-20, 0);
    Run(0, 40);
    Run(20, 0);
    Run(10, 15);
    Run(-10, 15);
    Run(-20, 0);
  }
}

void PrintR()
{
  for (int i = 0; i < 2; i++)
  {
    Run(-15, 0);
    Run(-15, 40);
    Run(15, 40);
    Run(15, 20);
    Run(0, 20);
    Run(15, 0);
    Run(0, 20);
    Run(15, 20);
    Run(15, 40);
    Run(-15, 40);
  }
}

void PrintD()
{
  for (int i = 0; i < 3; i++)
  {
    Run(-15, 0);
    Run(-15, 40);
    Run(10, 40);
    Run(15, 30);
    Run(15, 10);
    Run(10, 0);
  }
}

void PrintU()
{
  for (int i = 0; i < 3; i++)
  {
    Run(-15, 0);
    Run(-15, 40);
    Run(-15, 0);
    Run(15, 0);
    Run(15, 40);
    Run(15, 0);
    Run(-15, 0);
  }
}

void PrintI()
{
  for (int i = 0; i < 3; i++)
  {
    Run(-5, 0);
    Run(5, 0);
    Run(0, 0);
    Run(0, 40);
    Run(-5, 40);
    Run(5, 40);
    Run(0, 40);
    Run(0, 0);
  }
}

void PrintN()
{
  for (int i = 0; i < 3; i++)
  {
    Run(-15, 0);
    Run(-15, 40);
    Run(15, 0);
    Run(15, 40);
    Run(15, 0);
    Run(-15, 40);
  }
}

void PrintO()
{
  for (int i = 0; i < 2; i++)
  {
    Run(-10, 0);
    Run(-15, 10);
    Run(-15, 30);
    Run(-10, 40);
    Run(10, 40);
    Run(15, 30);
    Run(15, 10);
    Run(10, 0);
  }
}

void PrintE()
{
  for (int i = 0; i < 2; i++)
  {
    Run(20, 0);
    Run(-20, 0);
    Run(-20, 40);
    Run(20, 40);
    Run(-20, 40);
    Run(-20, 20);
    Run(15, 20);
    Run(-20, 20);
    Run(-20, 0);
  }
}

void PrintDot()
{
  for (int i = 0; i < 3; i++)
  {
    Run(-10, 0);
    Run(-10, 10);
    Run(10, 10);
    Run(10, 0);
  }
}


void RunInBox()
{
  double x = 8;
  double o = 0;
  double f = 0.66 * x;
  // from 0 to 1000;
  randNumberX = random(1000);
  randNumberY = random(1000);
  double slopeX = randNumberX / 1000.0;
  double slopeY = randNumberY / 1000.0;
  double offsetX = 0;
  double offsetY = 0;
  long faktor = 5;
  for (long n = 0; n < 1000; n++)
  {

    offsetX += slopeX * faktor;
    offsetY += slopeY * faktor;
    for (int i = 0; i < 1; i++)
    {
      Run(o + offsetX, x + offsetY);
      Run(f + offsetX, f + offsetY);
      Run(x + offsetX, o + offsetY);
      Run(f + offsetX, -f + offsetY);
      Run(o + offsetX, -x + offsetY);
      Run(-f + offsetX, -f + offsetY);
      Run(-x + offsetX, o + offsetY);
      Run(-f + offsetX, f + offsetY);
    }

    // shuffle new
    if (offsetX > 80)
    {
      randNumberX = random(200, 1000);
      slopeX = -randNumberX / 1000.0;
      x = random (8, 16);
      faktor = random(5, 20);
    }

    if (offsetX < -80)
    {
      randNumberX = random(200, 1000);
      slopeX = randNumberX / 1000.0;
      x = random (8, 16);
      faktor = random(5, 20);
    }

    if (offsetY < -80)
    {
      randNumberY = random(200, 1000);
      slopeY = randNumberY / 1000.0;
      x = random (8, 16);
      long faktor = random(5, 20);
    }

    if (offsetY > 80)
    {
      randNumberY = random(200, 1000);
      slopeY = -randNumberY / 1000.0;
      x = random (8, 16);
      faktor = random(5, 20);
    }

  }
}

void RunStarInBox(int starSize, long numberOfLoops)
{
  double x = 8;
  double o = 0;
  double f = 0.66 * x;
  // from 0 to 1000;
  randNumberX = random(1000);
  randNumberY = random(1000);
  double slopeX = randNumberX / 1000.0;
  double slopeY = randNumberY / 1000.0;
  double offsetX = 0;
  double offsetY = 0;
  long faktor = 5;
  for (long n = 0; n < numberOfLoops; n++)
  {

    offsetX += slopeX * faktor;
    offsetY += slopeY * faktor;
    for (int i = 0; i < 1; i++)
    {
      PrintStar(starSize, offsetX, offsetY, 0, 0);
    }

    // shuffle new
    if (offsetX > 80)
    {
      randNumberX = random(200, 1000);
      slopeX = -randNumberX / 1000.0;
      x = random (8, 16);
      faktor = random(5, 20);
    }

    if (offsetX < -80)
    {
      randNumberX = random(200, 1000);
      slopeX = randNumberX / 1000.0;
      x = random (8, 16);
      faktor = random(5, 20);
    }

    if (offsetY < -80)
    {
      randNumberY = random(200, 1000);
      slopeY = randNumberY / 1000.0;
      x = random (8, 16);
      long faktor = random(5, 20);
    }

    if (offsetY > 80)
    {
      randNumberY = random(200, 1000);
      slopeY = -randNumberY / 1000.0;
      x = random (8, 16);
      faktor = random(5, 20);
    }

  }

}

void MoveRight()
{
  double x = 8;
  double o = 0;
  double f = 0.66 * x;
  for (int k = -80; k < 80; k += 5)
  {
    double offsetX = k;
    double offsetY = -80;
    for (int i = 0; i < 1; i++)
    {
      Run(o + offsetX, x + offsetY);
      Run(f + offsetX, f + offsetY);
      Run(x + offsetX, o + offsetY);
      Run(f + offsetX, -f + offsetY);
      Run(o + offsetX, -x + offsetY);
      Run(-f + offsetX, -f + offsetY);
      Run(-x + offsetX, o + offsetY);
      Run(-f + offsetX, f + offsetY);
    }
  }
}

void MoveUp()
{
  double x = 8;
  double o = 0;
  double f = 0.66 * x;
  for (int k = -80; k < 80; k += 5)
  {
    double offsetX = 80;
    double offsetY = k;
    for (int i = 0; i < 1; i++)
    {
      Run(o + offsetX, x + offsetY);
      Run(f + offsetX, f + offsetY);
      Run(x + offsetX, o + offsetY);
      Run(f + offsetX, -f + offsetY);
      Run(o + offsetX, -x + offsetY);
      Run(-f + offsetX, -f + offsetY);
      Run(-x + offsetX, o + offsetY);
      Run(-f + offsetX, f + offsetY);
    }
  }
}
void MoveLeft()
{
  double x = 8;
  double o = 0;
  double f = 0.66 * x;
  for (int k = 80; k > -80; k -= 5)
  {
    double offsetX = k;
    double offsetY = 80;
    for (int i = 0; i < 1; i++)
    {
      Run(o + offsetX, x + offsetY);
      Run(f + offsetX, f + offsetY);
      Run(x + offsetX, o + offsetY);
      Run(f + offsetX, -f + offsetY);
      Run(o + offsetX, -x + offsetY);
      Run(-f + offsetX, -f + offsetY);
      Run(-x + offsetX, o + offsetY);
      Run(-f + offsetX, f + offsetY);
    }
  }
}

void MoveDown()
{
  double x = 8;
  double o = 0;
  double f = 0.66 * x;
  for (int k = 80; k > -80; k -= 5)
  {
    double offsetX = -80;
    double offsetY = k;
    for (int i = 0; i < 1; i++)
    {
      Run(o + offsetX, x + offsetY);
      Run(f + offsetX, f + offsetY);
      Run(x + offsetX, o + offsetY);
      Run(f + offsetX, -f + offsetY);
      Run(o + offsetX, -x + offsetY);
      Run(-f + offsetX, -f + offsetY);
      Run(-x + offsetX, o + offsetY);
      Run(-f + offsetX, f + offsetY);
    }
  }
}

void MoveCenter()
{
  double x = 8;
  double o = 0;
  double f = 0.66 * x;
  for (int k = -80; k < 0; k += 5)
  {
    double offsetX = k;
    double offsetY = k;
    for (int i = 0; i < 1; i++)
    {
      Run(o + offsetX, x + offsetY);
      Run(f + offsetX, f + offsetY);
      Run(x + offsetX, o + offsetY);
      Run(f + offsetX, -f + offsetY);
      Run(o + offsetX, -x + offsetY);
      Run(-f + offsetX, -f + offsetY);
      Run(-x + offsetX, o + offsetY);
      Run(-f + offsetX, f + offsetY);
    }
  }
}

void MoveTopLeft()
{
  double x = 8;
  double o = 0;
  double f = 0.66 * x;
  for (int k = 0; k < 80; k += 5)
  {
    double offsetX = -k;
    double offsetY = k;
    for (int i = 0; i < 1; i++)
    {
      Run(o + offsetX, x + offsetY);
      Run(f + offsetX, f + offsetY);
      Run(x + offsetX, o + offsetY);
      Run(f + offsetX, -f + offsetY);
      Run(o + offsetX, -x + offsetY);
      Run(-f + offsetX, -f + offsetY);
      Run(-x + offsetX, o + offsetY);
      Run(-f + offsetX, f + offsetY);
    }
  }
}

void OpenAndCloseCircle()
{
  for (int x1 = 100; x1 > 2; x1 = x1 - 4)
  {
    double x = (double)x1;
    double o = 0;
    double f = 0.66 * x;
    for (int i = 0; i < 3; i++)
    {
      Run(o, x);
      Run(f, f);
      Run(x, o);
      Run(f, -f);
      Run(o, -x);
      Run(-f, -f);
      Run(-x, o);
      Run(-f, f);
    }
  }
  for (int x1 = 2; x1 < 100; x1 = x1 + 4)
  {
    double x = (double)x1;
    double o = 0;
    double f = 0.66 * x;
    for (int i = 0; i < 3; i++)
    {
      Run(o, x);
      Run(f, f);
      Run(x, o);
      Run(f, -f);
      Run(o, -x);
      Run(-f, -f);
      Run(-x, o);
      Run(-f, f);
    }
  }

}

void ReturnTopLeft()
{
  RunRelative(- absoluteX, - absoluteY);
}

void SleepBoth()
{
  digitalWrite(pinSleep_1, LOW);
  digitalWrite(pinSleep_2, LOW);
}

void RunRelativeClock()
{
  // goto center
  RunRelativeCenter();
  showOnDisplay("Running clockwise");
  int intervall = 4;
  for (int i = -100; i <= 100; i = i + intervall)
  {
    RunRelativeAndReturn(i, -100);
  }
  for (int i = -100; i <= 100; i = i + intervall)
  {
    RunRelativeAndReturn(100, i);
  }
  for (int i = 100; i >= -100; i = i - intervall)
  {
    RunRelativeAndReturn(i, 100);
  }
  for (int i = 100; i >= -100; i = i - intervall)
  {
    RunRelativeAndReturn(-100, i);
  }

  // go back to top left corne
  RunRelativeTopLeft();
}

// rund and return to center
void RunRelativeAndReturn(int x, int y)
{
  RunRelative(x, y);
  RunRelative(-x, -y);
}

void RunRelativeCenter()
{
  showOnDisplay("Center");
  RunRelative(100, 100);
}

void RunRelativeTopLeft()
{
  showOnDisplay("Top Left");
  RunRelative(-100, -100);
}

void Run(int xx, int yy)
{
  int rx = xx - absoluteX + 100;
  int ry = 100 - (yy + absoluteY);
  RunRelative(rx, ry);
}

void Run(double xx, double yy)
{
  int rx = (int)xx - absoluteX + 100;
  int ry = 100 - ((int)yy + absoluteY);
  RunRelative(rx, ry);
}

// find me
void RunRelative(int x, int y)
{
  int sleepDelay = minSleepMillis;
  bool invert = false;
  int incX = 1;
  int incY = 1;
  if (x < 0) {
    incX = -1;
  }
  if (y < 0) {
    incY = -1;
  }
  // absoluteX = absoluteX + x;
  // absoluteY = absoluteY + y;
  int maxSteps = x;
  int minSteps = y;
  int maxDirPin = pinDir_1;
  int minDirPin = pinDir_2;
  int maxPin = pinStep_1;
  int minPin = pinStep_2;
  if (abs(x) < abs(y))
  {
    invert = true;
    maxSteps = y;
    minSteps = x;
    maxPin = pinStep_2;
    minPin = pinStep_1;
    maxDirPin = pinDir_2;
    minDirPin = pinDir_1;
  }
  // wake up stepper
  // initialize steppers
  digitalWrite(pinStep_1, LOW);
  digitalWrite(pinSleep_1, HIGH);
  digitalWrite(maxDirPin, maxSteps > 0);
  digitalWrite(pinStep_2, LOW);
  digitalWrite(pinSleep_2, HIGH);
  digitalWrite(minDirPin, minSteps > 0);



  int k = 0;
  minSteps = abs(minSteps);
  maxSteps = abs(maxSteps);
  double rise = (double)minSteps / (double)maxSteps;
  double factor = ((double)maxSteps / (double)maxDistance) + (double)0.05;
  sleepDelay = (int)((double)minSleepMillis / factor) + 20;
  if (sleepDelay < minSleepMillis)
  {
    sleepDelay = minSleepMillis;
  }
  if (enableDebug == 1)
  {
    Serial.print("factor: ");
    Serial.println(String((int)(factor * 1000)));
    Serial.print("maxSteps: ");
    Serial.println(String((int)maxSteps));
    Serial.print("maxDistance: ");
    Serial.println(String((int)maxDistance));
    Serial.print("sleepDelay: ");
    Serial.println(String((int)sleepDelay));
  }

  bool doStep = true;
  for (long i = 0; i < maxSteps; i++)
  {
    if (invert)
    {
      absoluteY += incY;
    }
    else
    {
      absoluteX += incX;
    }
    digitalWrite(maxPin, HIGH);
    if (k < minSteps && doStep)
    {
      if (invert)
      {
        absoluteX += incX;
      }
      else
      {
        absoluteY += incY;
      }
      digitalWrite(minPin, HIGH);

    }
    delayMicroseconds(sleepDelay);
    digitalWrite(maxPin, LOW);
    digitalWrite(minPin, LOW);

    if (i > k / rise)
    {
      k++;
      doStep = true;
    }
    else
    {
      doStep = false;
    }
  }

  // if one step is missing!
  if (k < minSteps)
  {
    if (invert)
    {
      absoluteX += incX;
    }
    else
    {
      absoluteY += incY;
    }
    digitalWrite(minPin, HIGH);
    delayMicroseconds(minSleepMillis);
    digitalWrite(minPin, LOW);
  }
}





void initalizeTextDisplay(void)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("");
  display.display();
}

void showOnDisplay(String outPutText)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(outPutText);
  display.display();
}

void showOnDisplay(String outPutText1, String outPutText2)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(outPutText1);
  display.println(outPutText2);
  display.display();
}

void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
}











