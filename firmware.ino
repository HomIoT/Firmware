#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

WiFiClient wifiClient;
HTTPClient httpClient;

String ssid = "Apple";
String password = "";

int roofLED = 1;

String baseUrl = "http://192.168.50.4:5000/api";

ESP8266WebServer server(80);

void sayHi()
{
  DynamicJsonDocument doc(1024);

  doc["message"] = "Hi";

  String json;
  serializeJson(doc, json);

  server.send(200, "application/json", json);
}

void predict()
{
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

void handleNotFound()
{
  DynamicJsonDocument doc(1024);

  doc["message"] = "Not found";

  String json;
  serializeJson(doc, json);

  server.send(404, "application/json", json);
}

void system()
{
}

void setup()
{
  // Start Serial
  Serial.begin(9600);

  // Setup WiFi connection
  WiFi.begin(ssid, password);

  // Check WiFi is connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Return that Wifi started
  Serial.println("Connected to WiFi");

  // API Instance
  httpClient.begin(wifiClient, baseUrl);

  // API routes
  server.on("/api", sayHi);
  server.on("/api/predict", predict);
  server.on("/api/system", system);
  server.onNotFound(handleNotFound);

  // Setup server
  server.begin();

  // Return that server started
  Serial.println("Server started");
}

void loop()
{
  server.handleClient();
}
