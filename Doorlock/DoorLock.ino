#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
/**************************************/
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
/**********************************/
#ifndef APSSID
#define APSSID "ESPap"
#define APPSK  "esp8266ap"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

/****************************************/
const char* www_username = "admin";
const char* www_password = "esp8266";
/****************************************/
bool    Open_status       = LOW;
bool    Logout_status     = LOW;
int     ActivateSignalPin = 4;

void handleOnConnected();
void handle_OpenDoor();
void handle_CloseDoor();
void handle_NotFound();
void handle_LoggedOut();
String SendHTML(uint8_t Open_status, uint8_t Logout_status);

ESP8266WebServer server(80);

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", handleOnConnected);  
  server.on("/open", handle_OpenDoor);
  server.on("/close", handle_CloseDoor);
  server.on("/logout", handle_LoggedOut);
  server.onNotFound(handle_NotFound);
    
  ArduinoOTA.begin();
  server.begin();
  Serial.println("HTTP server started");
  pinMode(ActivateSignalPin,OUTPUT);
    
}


void loop() {
 // digitalWrite(ActivateSignalPin,HIGH);

// if(Open_status == 1){
//  digitalWrite(ActivateSignalPin,HIGH);
// }
// else if(Open_status == 0){
//  digitalWrite(ActivateSignalPin,LOW);
// }
 ArduinoOTA.handle();
 server.handleClient(); 
}

void handleOnConnected() {
    Open_status = LOW;
    Logout_status = LOW;
    
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }
    server.send(200, "text/html", SendHTML(false,true));
}
void handle_OpenDoor() {
  Open_status = HIGH;
  Serial.println("Door opened");
  digitalWrite(ActivateSignalPin,HIGH);
  server.send(200, "text/html", SendHTML(true, Logout_status));
}

void handle_CloseDoor() {
  Open_status = LOW;
  Serial.println("Door closed");
  digitalWrite(ActivateSignalPin,LOW);
  server.send(200, "text/html", SendHTML(false, Logout_status));
}
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
void handle_LoggedOut() {
  Logout_status = HIGH;
  server.send(401, "text/html", "<h1>Logged Out</h1>");
  delay(1000);
  server.requestAuthentication();
  }

String SendHTML(uint8_t Open_status, uint8_t Logout_status) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Door Lock Controller</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 100px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Lock Control Page</h1>\n";
  ptr += "<h3>Developed by dev : Hamdy</h3>\n";

  if (Open_status)
  {
    ptr += "<p>Door Status: Opened</p><a class=\"button button-off\" href=\"/close\">Close</a>\n";
  }
  else
  {
    ptr += "<p>Door Status: Closed</p><a class=\"button button-on\" href=\"/open\">Open</a>\n";
  }

  if (Logout_status)
  {
    ptr += "<p>Press to logout</p><a class=\"button button-off\" href=\"/logout\">LogOut</a>\n";
  }
  else
  {
    ptr += "<p>Press to logout</p><a class=\"button button-on\" href=\"/logout\">LogOut</a>\n";
  }
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
