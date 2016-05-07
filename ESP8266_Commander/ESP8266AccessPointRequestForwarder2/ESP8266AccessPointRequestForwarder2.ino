#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);                          // configure server port
int ledPin = 2;

const char WiFiSSID[] = "ESP8266A";
const char WiFiPassword[] = "12345678";

void setup()
{
  pinMode(ledPin, OUTPUT);                              // GPIO2 attach LED for Output
  digitalWrite(ledPin, LOW);                            // ... and write LOW

  Serial.begin(9600);                                   // Initialize serial communication for Arduino or Serial Monitor
  Serial.println("");                                   // write space line
  Serial.println("Starting WLAN-Hotspot" + String(WiFiSSID) + "...");
  WiFi.mode(WIFI_AP);                                     // configure as access point
  WiFi.softAP(WiFiSSID, WiFiPassword);                    // Name of WIFI and Password
  delay(500);                                             // wait till connected
  Serial.println("WiFi connected");
  Serial.print("IP:");                          // print current IP address
  Serial.println(WiFi.softAPIP());


  // Attach onNotFound to server
  server.onNotFound(handleNotFound);


  server.begin();                                        // Start Web Server
  flashLED();                                           // flash LED to indicate ready!
  Serial.println("HTTP Server READY!");

}

void loop()
{
  server.handleClient();
}


void flashLED()
{
  for (int i = 0; i < 10; i++)
  {
    flashLEDOnce();
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}

void flashLEDOnce()
{
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
}


void handleNotFound() {
  String message = "<!doctype html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n<title>ESP8266-Forwarder</title>\n</head>\n<body>\n";
  String forwardingMessage = server.uri();
  message += "<h1>URI: ";
  message += server.uri() + "</h1>";
  message += "\n<h2>Method: ";
  message += ( server.method() == HTTP_GET ) ? "GET</h2>" : "POST</h2>";
  message += "\n<h3>Arguments: ";
  message += server.args();
  message += "</h3>\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += "<p>" + server.argName ( i ) + ": " + server.arg ( i ) + "</p>\n";
    forwardingMessage += "|" + server.argName ( i ) + "<" + server.arg ( i ) + ">";
  }
  message += "</body>\n</html>";

  server.send ( 200, "text/html", message );
  flashLEDOnce();
  if (forwardingMessage != "/favicon.ico")
  {
    Serial.println(forwardingMessage);
  }
}



