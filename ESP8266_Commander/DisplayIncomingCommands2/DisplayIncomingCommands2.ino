#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// for communication with ESP8266-WIFI
#include <SoftwareSerial.h>


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

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

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const byte ledPin = 13;
unsigned int counter = 0;

SoftwareSerial esp8266(2, 3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
// This means that you need to connect the TX line from the esp to the Arduino's pin 2
// and the RX line from the esp to the Arduino's pin 3

void setup()   {
  // Serial.begin(9600);
  esp8266.begin(9600); // your esp's baud rate might be different

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  // init done

  display.display();
  delay(500);

  outDisplay("ready!" );

  // say hello to internal LED
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < 15; i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(20);
    digitalWrite(ledPin, LOW);
    delay(20);
  }
}



void loop() {


  if (esp8266.available()) // check if the esp is sending a message
  {
    String messageFromESP8266 = "";
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      if (int(c) > 0) {
        messageFromESP8266 += c;
      }
    }

    outDisplay(messageFromESP8266);
  }

}



void outDisplay(String a)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(a);
  display.display();
}

//void outDisplay(String a, String b, String c, String d)
//{
//  display.clearDisplay();
//  display.setCursor(0, 0);
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.println(a);
//  display.println(b);
//  display.println(c);
//  display.println(d);
//
//  display.display();
//
//}




