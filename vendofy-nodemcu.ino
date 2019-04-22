#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char* ssid = "Mancao";
const char* password = "28DWIFI3F8";

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);


void setup() {
    Serial.begin(115200);
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

}

void loop() {
    webSocket.loop();
    server.handleClient();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    if (type == WStype_DISCONNECTED) {

        IPAddress ip = webSocket.remoteIP(num);
        // format ip address
        String clientIPAddress = (String)ip[0] + "." + (String)ip[1] + "." + (String)ip[2] + "." + (String)ip[3];
        Serial.println("Disconnected: [" + (String)num + "][" + clientIPAddress + "]");

    } else if (type == WStype_CONNECTED) {

        IPAddress ip = webSocket.remoteIP(num);
        // format ip address
        String clientIPAddress = (String)ip[0] + "." + (String)ip[1] + "." + (String)ip[2] + "." + (String)ip[3];
        // send response to connected client
        webSocket.sendTXT(num, clientIPAddress);

        Serial.println("Connected: [" + (String)num + "][" + clientIPAddress + "]");

    } else if (type == WStype_TEXT) {

        char data[length];
        
        for(uint8_t i = 0; i < length; i++) data[i] = payload[i];

        data[length] = '\0';

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data);

        serializeJsonPretty(doc, Serial);

        // send data to all connected clients
        // webSocket.broadcastTXT("message for all");
    }
}