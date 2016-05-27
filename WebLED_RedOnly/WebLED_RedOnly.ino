#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "****";
const char* password = "****";

ESP8266WebServer server(80);

const int ledRed    = 12;

void handleRoot() {
  String html = "<h1>hello from esp8266!</h1>";
  html += "<a href=\"/led?r=255\">red</a></p>";
  server.send(200, "text/html", html);
}

void handleNotFound(){
  server.send(404, "text/plain", "File Not Found");
}

void setup(void){
  //出力ピンの初期化
  pinMode(ledRed, OUTPUT);
  analogWriteRange(255);

  //シリアル初期化
  Serial.begin(115200);

  //WiFi初期化
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // 接続確認
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //  mDNS開始
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  //ウェブサーバー設定
  server.on("/", handleRoot);

  server.on("/led", []() {
    int red = server.arg("r").toInt();
    if(red >= 0){
      analogWrite(ledRed, red);
    }

    String html = "<h1>hello from esp8266!</h1>";
    html += "<p>Red:<a href=\"/led?r=255\">255</a> / <a href=\"/led?r=0\">0</a> </p>";
    
    server.send(200, "text/html",html);
  });

  server.onNotFound(handleNotFound);

  //ウェブサーバー開始
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
