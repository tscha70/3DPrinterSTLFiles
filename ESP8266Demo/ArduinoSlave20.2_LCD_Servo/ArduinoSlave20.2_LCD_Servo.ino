#include <Servo.h>
#include <Wire.h>
#include "U8glib.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define backlight_pin 9

U8GLIB_PCD8544 u8g(8, 4, 7, 5, 6);  // CLK=8, DIN=4, CE=7, DC=5, RST=6

#define I2CAddressESPWifi 6
#define ledPin 13
#define numberOfBytes 15 // number of bytes expected by ESP8266


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            2
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      1

byte r = 0;
byte g = 0;
byte b = 0;

byte angle = 0;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// buffer for return-message:
char charBuffer[] = "012345678901234"; // initilize buffer with 15 bytes!
char currentText[] = ""; // text to display on LCD

// counter to have something going and see changing values.. for testing purpose
unsigned long counter = 0;

Servo myservo;  // create servo object to control a servo

void setup()   {
  Wire.begin(I2CAddressESPWifi);    // join I2Cbus as slave with identified address
  Wire.onReceive(receiveEvent);     // register a function to be called when slave receive a transmission from master//
  Wire.onRequest(requestEvent);     // register a function when master request data from this slave device//
  Serial.begin(9600);               //set serial baud rate

  // say hello to internal LED
  pinMode(ledPin, OUTPUT);
  flashLEDMany();

  pixels.begin(); // This initializes the NeoPixel library.
  r = 10;
  g = 0;
  b = 0;
  updateNeoPixel();

  // Nokia-LCD 5110
  analogWrite(backlight_pin, 50);  /* Set the Backlight intensity */

  displayText("ready!");

  myservo.attach(11);  // attaches the servo on pin 11 to the servo object

  angle = 100;
  myservo.write(angle);

  Serial.println("Hello, ArduinoSlave Version 11.9 with RGB-LED here!");

}



void loop() {
  // nothing really happens in main-loop:
  // just increment the couter
  delay(250);
  counter++;
  if (counter > 4294967294)
  {
    counter = 0;
  }

  updateNeoPixel();
  updateServo();

}


// when slave receives string from master, trigger this event.
void receiveEvent(int howMany)
{
  String message = "";
  while ( Wire.available() ) // execute repeatedly and read all bytes in the data packet from master//
  {
    char c = Wire.read();    // receive data from master and assign it to char c
    message += c;
  }

  displayText(message);
  updatePixelValues(message);
  updateServoValues(message);

  if (isGetServo(message))
  {
     pushMessageToCharBuffer("Servo-angle:" + getStringFixLengthFromUnsignedLong(numberOfBytes - 12, angle), charBuffer);
  }
  else
  {
    // preparing the messageBuffer for the answer to ESP8266 when requested by requestEvent()
    pushMessageToCharBuffer("OK" + getStringFixLengthFromUnsignedLong(numberOfBytes - 2, counter), charBuffer);
  }
}

// trigger this event when master requests data from slave,
void requestEvent()
{
  // IMPORTANT! this code needs to be executed quickly, so do not write any other code in here!
  // buffer containing message (answer) must have a fixed length of 15 because ESP expects so...
  Wire.write(charBuffer);
}

// flash the Onboard-LED to show a message is received
void flashLEDOnce()
{
  digitalWrite(ledPin, HIGH);
  delay(25);
  digitalWrite(ledPin, LOW);
}

// flash the Onboard-LED to show Arduino is ready!
void flashLEDMany()
{
  for (int i = 0; i < 15; i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(10);
    digitalWrite(ledPin, LOW);
    delay(20);
  }
}


void updateServo()
{
  myservo.write(angle);
}

void updateNeoPixel()
{
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void updatePixelValues(String s)
{
  if (s.startsWith("/pixel", 0))
  {
    int indexStart = s.indexOf("r<");
    if (indexStart > 0)
    {
      r = s.substring(indexStart + 2 , indexStart + 5).toInt();
    }
    indexStart = s.indexOf("g<");
    if (indexStart > 0)
    {
      g = s.substring(indexStart + 2 , indexStart + 5).toInt();
    }
    indexStart = s.indexOf("b<");
    if (indexStart > 0)
    {
      b = s.substring(indexStart + 2 , indexStart + 5).toInt();
    }
  }
}

bool isGetServo(String s)
{
  if (s.startsWith("/getServo", 0))
  {
    return 1;
  }
  return 0;
}

void updateServoValues(String s)
{
  if (s.startsWith("/servo", 0))
  {
    int indexStart = s.indexOf("v<");
    if (indexStart > 0)
    {
      angle = s.substring(indexStart + 2 , indexStart + 5).toInt();
      Serial.println("new angle: " + String(angle));
    }
  }
}


// convert message to char-buffer
void pushMessageToCharBuffer(String s, char *charArray)
{
  unsigned int len = s.length() + 1; // one extra for null-terminator
  s.toCharArray(charArray, len);
}

// fill up string with hashes in front of value
String getStringFixLengthFromUnsignedLong(int stringSize, unsigned long value)
{
  String s = String(value);
  String f = "";
  int spacers = stringSize - s.length();
  for (int i = 0; i < spacers; i++)
  {
    f += "#";
  }
  return (f + s).substring(0, stringSize);
}


void draw(String s) {
  const int charactersPerLine = 14;
  const int maxLines = 5;
  // initalize with 14 spaces
  char line[maxLines][charactersPerLine + 1] = {"              ", "              ", "              ", "              ", "              "};

  int i = 0;
  while (s.length() > charactersPerLine && i < (maxLines - 1))
  {
    if (s.length() >= charactersPerLine)
    {
      pushMessageToCharBuffer(s.substring(0, charactersPerLine), line[i]);

      // cut off 14 characters
      s = s.substring(charactersPerLine, s.length());
    }
    i++;
  }
  if (s.length() >= charactersPerLine)
  {
    pushMessageToCharBuffer(s.substring(0, charactersPerLine), line[i]);
  }
  else if (s.length() > 0)
  {
    pushMessageToCharBuffer(s, line[i]);
  }

  u8g.setFont(u8g_font_profont11);  // select font
  const int offset = 8;
  for (int i = 0; i < (maxLines); i++)
  {
    u8g.drawStr(0, offset + i * 10, line[i]);  // text-line n
  }
}

void displayText(String text)
{
  // Nokia-LCD 5110 show on display
  u8g.firstPage();
  do {
    draw(text);
  } while ( u8g.nextPage() );
}




