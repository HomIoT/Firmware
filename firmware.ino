
// Import libraries
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// Init WiFi and HTTO
WiFiClient wifiClient;
HTTPClient httpClient;

// WiFi Cerditionals
String ssid = "Apple";
String password = "";

String baseUrl = "http://192.168.50.4:5000/api";

// Start Web Server on port 80
ESP8266WebServer server(80);

// Sample controller
void sayHi() {
  if (server.method() != HTTP_GET) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  DynamicJsonDocument doc(1024);

  doc["message"] = "Hi";

  String json;
  serializeJson(doc, json);

  server.send(200, "application/json", json);
}

void predict() {
  if (server.method() != HTTP_GET) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  httpClient.setURL(baseUrl + "/predict");

  DynamicJsonDocument doc(1024);

  doc["question"] = "deep";

  String json;
  serializeJson(doc, json);

  int httpCode = httpClient.POST(json);

  String response = httpClient.getString();

  deserializeJson(doc, response);

  server.send(httpCode, "application/json", response);

  httpClient.end();
}

// Handle not found controller
void handleNotFound() {
  DynamicJsonDocument doc(1024);

  doc["message"] = "Not found";

  String json;
  serializeJson(doc, json);

  server.send(404, "application/json", json);
}

void act() {
  if (server.method() != HTTP_GET) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String digital_arg = server.arg("digital");
  String state_arg = server.arg("state");

  int digital_pin = digital_arg.toInt();
  bool state = state_arg.toInt() == 1;

  pinMode(digital_pin, OUTPUT);
  digitalWrite(digital_pin, state);

  Serial.println(String(digital_pin));
  Serial.println(String(state));

  DynamicJsonDocument doc(1024);

  doc["message"] = message;

  String json;
  serializeJson(doc, json);

  server.send(200, "application/json", json);
}

void setup() {
  // Start Serial
  Serial.begin(9600);

  // Setup WiFi connection
  WiFi.begin(ssid, password);

  // Check WiFi is connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("IP: " + WiFi.localIP().toString());

  // Return that Wifi started
  Serial.println("Connected to WiFi");

  // API Instance
  httpClient.begin(wifiClient, baseUrl);

  // API routes
  server.on("/api", sayHi);
  server.on("/api/predict", predict);
  server.on("/api/act", act);
  server.onNotFound(handleNotFound);

  // Setup server
  server.begin();

  // Return that server started
  Serial.println("Server started");
}

void loop() {
  // Start to handle
  server.handleClient();
}
