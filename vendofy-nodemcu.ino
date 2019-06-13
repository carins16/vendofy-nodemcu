#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

SoftwareSerial ArduinoMega(D2,D3);

const char* ssid = "VENDOFY";
const char* password = "helloworld";

IPAddress ip(192, 168, 137, 100); //set static ip
IPAddress gateway(192, 168, 137, 1); //set getteway
IPAddress subnet(255, 255, 255, 0);//set subnet

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);

uint8_t clientID;

void setup() {
    Serial.begin(9600);
    ArduinoMega.begin(9600);

    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
    Serial.println("");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

   //  server.on("/", [](){
   //  server.send(200, "text/html", WebPage);
   //  });

    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    pinMode(D4, OUTPUT);
}

void loop() {

    if (WiFi.status() != WL_CONNECTED) {
        digitalWrite(D4, HIGH);
    } else {
        digitalWrite(D4, LOW);
    }

    webSocket.loop();
    server.handleClient();

    if (ArduinoMega.available() > 0) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, ArduinoMega);

        String jsonStr;
        serializeJsonPretty(doc, Serial);
        serializeJson(doc, jsonStr);

        webSocket.sendTXT(clientID, jsonStr);
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    if (type == WStype_DISCONNECTED) {

        Serial.println("Disconnected: [" + (String)num + "]");

    } else if (type == WStype_CONNECTED) {

        IPAddress clientIP = webSocket.remoteIP(num);
        // format ip address
        String clientIPAddress = (String)clientIP[0] + "." + (String)clientIP[1] + "." + (String)clientIP[2] + "." + (String)clientIP[3];
        // send response to connected client
        // webSocket.sendTXT(num, "message for you");

        clientID = num;
        Serial.println("Connected: [" + (String)num + "][" + clientIPAddress + "]");

    } else if (type == WStype_TEXT) {

        char data[length];

        for(uint8_t i = 0; i < length; i++) data[i] = payload[i];

        data[length] = '\0';

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data);

        serializeJsonPretty(doc, Serial);
        serializeJson(doc, ArduinoMega);
        // send data to all connected clients
        // webSocket.broadcastTXT("message for all");
    }
}
