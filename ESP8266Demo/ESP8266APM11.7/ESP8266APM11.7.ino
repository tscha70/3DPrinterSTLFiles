#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>    //declare I2C library function
#define I2CAddressESPWifi 6

ESP8266WebServer server(80);                          // configure server port
const char WiFiSSID[] = "ESP8266A";                   // WIFI-SSID (Name shown on your network)
const char WiFiPassword[] = "12345678";               // password to join the network


void setup()
{
  Serial.begin(9600);                                     // Initialize serial communication for Arduino or Serial Monitor
  Serial.println("");                                     // write space line
  Serial.println("Starting WLAN-Hotspot" + String(WiFiSSID) + "...");
  WiFi.mode(WIFI_AP);                                     // configure as access point
  WiFi.softAP(WiFiSSID, WiFiPassword);                    // Name of WIFI and Password
  delay(500);                                             // wait till connected
  Serial.println("WiFi connected");
  Serial.print("IP:");                                     // print current IP address
  Serial.println(WiFi.softAPIP());


  // Attach onNotFound to server
  server.onNotFound(handleNotFound);
  server.begin();                                        // Start Web Server

  Wire.begin(0, 2);                                      // join the I2C bus as master
  Serial.println("HTTP Server READY!");

}

void loop()
{
  server.handleClient();
}

void handleNotFound() {
  String message = "<!doctype html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n<title>" + String(WiFiSSID) + "-SWV-11.7</title>\n</head>\n<body>\n";
  String forwardingMessage = server.uri();
  message += "<h2>URI: ";
  message += server.uri() + "</h2>";
  message += "\n<h3>Method: ";
  message += ( server.method() == HTTP_GET ) ? "GET</h3>" : "POST</h3>";
  message += "\n<h3>Arguments: ";
  message += server.args();
  message += "</h3>\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += "<p>" + server.argName ( i ) + ": " + server.arg ( i ) + "</p>\n";
    forwardingMessage += "|" + server.argName ( i ) + "<" + server.arg ( i ) + ">";
  }

  String answer;
  if (forwardingMessage != "/favicon.ico")
  {
    answer = sendMessage(forwardingMessage);
    // Serial.println(forwardingMessage);
  }

  message += "\n<h3>Slave-Answer: ";
  message += answer;
  message += "</h3>\n";
  message += "</body>\n</html>";
  server.send ( 200, "text/html", message );
}

String sendMessage(String msg)
{
  if (msg.length() > 32)
  {
    msg = msg.substring(0,32);
  }
  String answer = "";                                 // initialize answer;
  char buf[33];                                       // actually we should not send more than 32-characters
  unsigned int len = msg.length() + 1;                // one extra for null-terminator
  msg.toCharArray(buf, len);                          // convert string to buf
  Wire.beginTransmission(I2CAddressESPWifi);          // begin a transmission to slave device 6
  Wire.write(buf, len);                               // send character string
  Wire.endTransmission();                             // end a transmission to slave
  char c;
  delay(1000);                                         // delay 1000 Milliseconds = 1 Second
  Wire.requestFrom(I2CAddressESPWifi, 15);             // request 15 bytes from slave 6
  while (Wire.available())                             // when data is received from slave to master
  {
    c = Wire.read();                                   // receive one byte from slave and assign to variable c    
    delayMicroseconds(500);                            // delay, otherwise won't work
    answer += c;
  }
  
  return answer;
}


