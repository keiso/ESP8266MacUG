#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "****";
const char* password = "****";

ESP8266WebServer server(80);

const int ledRed    = 12;
const int ledGreen  = 13;
const int ledBlue   = 15;


String colorForm(String rgb){
  String html = "<form><input type=\"color\" name=\"rgb\" value=\"" + rgb + "\"><input type=\"submit\" value=\"Go\"></form>";
  return html;
}

void handleRoot() {
  String rgb = server.arg("rgb");
  String strRed = rgb.substring(1,3);
  int red = ::strtol(strRed.c_str(),NULL,16);
  analogWrite(ledRed, red);

  String strGreen = rgb.substring(3,5);
  int green = ::strtol(strGreen.c_str(),NULL,16);
  analogWrite(ledGreen, green * 0.4f);

  String strBlue = rgb.substring(5,7);
  int blue = ::strtol(strBlue.c_str(),NULL,16);
  analogWrite(ledBlue, blue * 0.25f);


  String html = "<h1>hello from esp8266!</h1>";
  html += "<p>red:0x"+ strRed + "(" + String(red) + ")</p>";
  html += "<p>green:0x"+ strGreen + "(" + String(green) + ")</p>";
  html += "<p>blue:0x"+ strBlue + "(" + String(blue) + ")</p>";
  html += colorForm(rgb);
  server.send(200, "text/html", html);
}

void handleNotFound(){
  server.send(404, "text/plain", "File Not Found");
}

void setup(void){
  //初期化
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);

  analogWriteRange(255);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/led", []() {
    int red = server.arg("r").toInt();
    if(red > -1){
      analogWrite(ledRed, red);
    }
    int green = server.arg("g").toInt();
    if(green > -1){
      analogWrite(ledGreen, green);
    }

    int blue = server.arg("b").toInt();
    if(blue > -1){
      analogWrite(ledBlue, blue);
    }

    server.send(200, "text/plain", "Led is now Red:" + String(red) + "/Green:"+ String(green) + "/Blue:"+ String(blue));
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
