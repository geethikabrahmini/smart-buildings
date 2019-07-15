#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

const char* ssid = "vivo Y21L";
const char* password = "sweety26";
#include "DHT.h"
#define DHTPIN D3   // what pin we're connected to
#define DHTTYPE DHT11   // define type of sensor DHT 11
DHT dht (DHTPIN, DHTTYPE);

#define trigPin D5
 #define echoPin D2
 

#define ORG "lngaje"
#define DEVICE_TYPE "smart"
#define DEVICE_ID "1234"
#define TOKEN "12345678"
String command;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char subtopic[] = "iot-2/cmd/home/fmt/String";
char pubtopic[] = "iot-2/evt/Data/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
//Serial.println(clientID);

WiFiClient wifiClient;
void callback(char* subtopic, byte* payload, unsigned int payloadLength);
PubSubClient client(server, 1883, callback, wifiClient);
//PubSubClient client(server, 1883,wifiClient);

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.print("Ultrasonic Sensor Starting!!!");
  Serial.println("");
  delay(1000);
  
  myservo.attach(D4);  // attaches the servo on GIO2 to the servo object
  
  Serial.begin(115200);
  Serial.println();
  dht.begin();
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());

  pinMode(D1,OUTPUT);
  wifiConnect();
  mqttConnect();
  
}

void loop() {
 int duration, distance;
  digitalWrite(trigPin, HIGH);
  delay(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance =(duration/2) / 29.1;

  Serial.print(distance);
  Serial.println(" cm");
  if (distance <= 10)
  {
    myservo.write(90);                       
  Serial.print("Gate open");   
  }
  else
  {
  Serial.println("Gate close");
  myservo.write(0);
  }
   delay(5000);
  int sensorvalue=analogRead(A0);
 Serial.println(sensorvalue);
float h = dht.readHumidity();
float t = dht.readTemperature();

/*if (isnan(h) || isnan(t))
{
Serial.println("Failed to read from DHT sensor!");
delay(1000);
return;
}*/
PublishData(t,h,sensorvalue);
 if (!client.loop()) {
    mqttConnect();
  }
delay(100);

  if (!client.loop()) {
    mqttConnect();
  }
delay(100);
}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() {
  if (client.subscribe(subtopic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* subtopic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for topic: "); Serial.println(subtopic);

  for (int i = 0; i < payloadLength; i++) {
    //Serial.println((char)payload[i]);
    command += (char)payload[i];
  }
Serial.println(command);
if(command == "lighton"){
  digitalWrite(D1,HIGH);
  Serial.println("Light is Switched ON");
}
else if(command == "lightoff"){
  digitalWrite(D1,LOW);
  Serial.println("Light is Switched OFF");
}

command ="";
}
void PublishData(float temp, float humid,int light){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"temperature\":";
  payload += temp;
  payload+="," "\"humidity\":";
  payload += humid;
  payload+="," "\"light\":";
  payload += light;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
 if (client.publish(pubtopic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
