#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "BayMax";
const char* password = "11012001";
String serverAddress = "http://192.168.43.59:3000"; 
String endpoint = "/api/iot/distance"; 

const int trigPin = D1; 
const int echoPin = D2; 
const int relayPin = D3; 
long duration;
int distance;
bool pumpOn = false; 
void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  
  connectToWiFi();
}

void loop() {
  measureDistance();
  sendDistanceToServer();
  controlPump();
  delay(1000); 
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}

void measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
 
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}
void controlPump() {
  if (distance <8) {
    if (!pumpOn) {
      digitalWrite(relayPin, HIGH);
      pumpOn = true;
      Serial.println("Pump turned on");
    }
  } else {
    if (pumpOn) {
      digitalWrite(relayPin, LOW);
      pumpOn = false;
      Serial.println("Pump turned off");
    }
  }
}

void sendDistanceToServer() {
  WiFiClient client;
  HTTPClient http;
  String jsonData = "{\"distance\": " + String(distance) + "}";
  
   String url = serverAddress + endpoint;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.println("Failed to connect to server");
  }
  
  http.end();
}