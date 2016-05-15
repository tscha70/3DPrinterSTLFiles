#include <Wire.h>
#define I2CAddressESPWifi 6
#define ledPin 13
#define numberOfBytes 15 // number of bytes expected by ESP8266


// buffer for return-message:
char charBuffer[] = "012345678901234"; // initilize buffer with 15 bytes!

// counter to have something going and see changing values.. for testing purpose
unsigned long counter = 0;

void setup()   {
  Wire.begin(I2CAddressESPWifi);    // join I2Cbus as slave with identified address
  Wire.onReceive(receiveEvent);     // register a function to be called when slave receive a transmission from master//
  Wire.onRequest(requestEvent);     // register a function when master request data from this slave device//
  Serial.begin(9600);               //set serial baud rate

  // say hello to internal LED
  pinMode(ledPin, OUTPUT);
  flashLEDMany();
  
  Serial.println("Hello, ArduinoSlave Version 11.7 here!");
}



void loop() {
  // nothing really happens in main-loop:
  // just increment the couter
  delay(100);
  counter++;
  if (counter > 4294967294)
  {
    counter = 0;
  }
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
  
  // show message arrived by flashing the Onboard-LED
  flashLEDOnce();

  // just for testing ...
  char bytebuffer[] = "xABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz..........";
  int bufferIndex = 0;
  bufferIndex = getIntParamValue(message);
  Serial.println(message);
  Serial.println("byte: " + String(bufferIndex) + " value: " + bytebuffer[bufferIndex]);
  // .. testing

  // preparing the messageBuffer for the answer to ESP8266 when requested by requestEvent()
  pushMessageToCharBuffer("OK" + getStringFixLengthFromUnsignedLong(numberOfBytes-2, counter), charBuffer);

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

// function to read the value of a parameter when sending "/getByte?a=1234"
int getIntParamValue(String s)
{
  if (s.startsWith("/getByte", 0))
  {
    int indexStart = s.indexOf('<');
    int indexEnd = s.indexOf('>');
    if (indexStart > 0 and indexEnd > indexStart)
    {
      int i = s.substring(indexStart + 1 , indexEnd).toInt();
      return i;
    }
    return 0;

  }
  return 0;
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




