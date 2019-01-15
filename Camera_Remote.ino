

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

IPAddress    apIP(42, 42, 42, 42);  // Defining a static IP address: local & gateway
// Default IP in AP mode is 192.168.4.1

/* This are the WiFi access point settings. Update them to your likin */
const char *ssid = "CANON A-1";
const char *password = "myCanonTest";

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

const int ledPin = D1; // an LED is connected to NodeMCU pin D1 (ESP8266 GPIO5) via a 1K Ohm resistor

bool ledState = false;
boolean firstCall = true;

void handleRoot() {
  digitalWrite (LED_BUILTIN, 0); //turn the built in LED on pin DO of NodeMCU on
  if (!firstCall) {
    digitalWrite (ledPin, 1);
  }


  /* Dynamically generate the LED toggle link, based on its current state (on or off)*/
  char ledText[80];
  delay(100);

  digitalWrite(ledPin, 0);
  ledState = digitalRead(ledPin);
  Serial.println(ledState);
  if (ledState) {
    strcpy(ledText, "<a href=\"/?led=0\">Auslösen</a>");

  }

  else {
    strcpy(ledText, "<a href=\"/?led=1\">Auslösen</a>");
  }

  ledState = digitalRead(ledPin);

  char html[1000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  int brightness = analogRead(A0);
  brightness = (int)(brightness + 5) / 10; //converting the 0-1024 value to a (approximately) percentage value

  // Build an HTML page to display on the web-server root address
  snprintf ( html, 1000,

             "<html>\
  <head>\
    <title>Canon A-1 Remote</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; font-size: 80%;}\
      h1 { Color: #AA0000; }\
      main { position: relative; height: 100%; width: 100%; margin-left: auto; margin-right: auto; }\
    </style>\
  </head>\
  <body>\
  <main>\
  <div>\
    <form action='http://42.42.42.42/?led=0\' method='get'>\
    <a href='http://42.42.42.42/?led=0'>release</a>\
</form>\
  </div>\
</main>\
  </body>\
</html>",
             ledText
           );
  server.send ( 200, "text/html", html );
  digitalWrite ( LED_BUILTIN, 1 );
  //delay(3);
  //digitalWrite ( LED_BUILTIN, 0 );
  ledState = digitalRead(ledPin);
  Serial.println(ledState);
  firstCall = false;
}

void handleNotFound() {
  digitalWrite ( LED_BUILTIN, 0 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( LED_BUILTIN, 1 ); //turn the built in LED on pin DO of NodeMCU off

}

void setup() {
  pinMode ( ledPin, OUTPUT );
  digitalWrite ( ledPin, 0 );

  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on ( "/", handleRoot );
  server.on ( "/led=1", handleRoot);
  server.on ( "/led=0", handleRoot);
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
