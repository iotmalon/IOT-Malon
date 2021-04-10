#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>

#include "DHT.h"
#define DHTPIN D2 

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
#define DHTTYPE DHT21 // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
#define humid "sensor/humid"
#define tempc "sensor/tempc"
#define tempf "sensor/tempf"

int ledPin1 = D5; // ไฟสีเขียวของค่าวัดอุณหภูมิ
int ledPin2 = D6; // ไฟสีแดงของค่าวัดอุณหภูมิ
int ledPin3 = D7; // ไฟสีเขียวของค่าความชื้นในอากาศ
int ledPin4 = D8;  // ไฟสีแดงของค่าความชื้นในอากาศ
int Relay1 = D3; //รีเลย์ควบคุมพัดลม
int Relay2 = D1; //รีเลย์ควบคุมปั้มหมอก

const char* ssid = "Galaxy Note20 Ultra";
const char* password = "12131415";
const char* mqtt_server = "35.240.247.126";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
if (strcmp(topic,"LED1")==0){
    if ((char)payload[0] == '1') {
      digitalWrite(Relay1, HIGH); 
    } else {
      digitalWrite(Relay1, LOW); 
    }}
      if (strcmp(topic,"LED2")==0){
    if ((char)payload[0] == '1') {
      digitalWrite(Relay2, HIGH); 
    } else {
      digitalWrite(Relay2, LOW); 
    }}}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("LED1");
      client.subscribe("LED2");} 
      else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }}}
void setup() {
  pinMode(ledPin1, OUTPUT); // sets the pin as output
  pinMode(ledPin2, OUTPUT); // sets the pin as output
  pinMode(ledPin3, OUTPUT);  // sets the pin as output
  pinMode(ledPin4, OUTPUT);  // sets the pin as output
  pinMode(Relay1, OUTPUT); // กำหนดขาทำหน้าที่ให้ขา 2 เป็น OUTPUT
  pinMode(Relay2, OUTPUT);
  digitalWrite(Relay1, HIGH);
  digitalWrite(Relay2, HIGH);
  dht.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); }
void loop() {
   if (!client.connected()) {
        reconnect();
      }
      client.loop();

      // Wait a few seconds between measurements.
      delay(10000);

      float t = dht.readTemperature(); // วัดค่าวัดอุณหภูมิ
  if (t < 35) { 
    digitalWrite(ledPin1, LOW); // สั่งให้ LED ที่ Pin ดับ *ไฟสีเขียว
    digitalWrite(ledPin2, HIGH); // สั่งให้ LED ที่ Pin2 ติดสว่าง *ไฟสีแดง
    digitalWrite(Relay1, HIGH); // สั่งให้พัดลมติด
    
  }
  else {
    digitalWrite(ledPin1, HIGH); // สั่งให้ LED ที่ Pin ติดสว่าง *ไฟสีเขียว
    digitalWrite(ledPin2, LOW); // สั่งให้ LED ที่ Pin2 ดับ *ไฟสีแดง
    digitalWrite(Relay1, LOW); // สั่งให้พัดลมดับ
  } 

  float h = dht.readHumidity(); // วัดค่าความชื้นในอากาศ
 // int t = dht.readTemperature();
  if (h < 45) { 
    digitalWrite(ledPin3, HIGH); // สั่งให้ LED ที่ Pin3 ดับ *ไฟสีเขียว
    digitalWrite(ledPin4, LOW); // สั่งให้ LED ที่ Pin4 ติดสว่าง *ไฟสีแดง
    digitalWrite(Relay2, LOW); // สั่งให้ปั้มหมอกติด
  }
  else {
    digitalWrite(ledPin3, LOW); // สั่งให้ LED ที่ Pin2 ติดสว่าง *ไฟสีเขียว
    digitalWrite(ledPin4, HIGH); // สั่งให้ LED ที่ Pin4 ดับ *ไฟสีแดง
    digitalWrite(Relay2, HIGH); // สั่งให้ปั้มหมอกดับ
  }
      
     // float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      //float t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.readTemperature(true);
      
      // Check if any reads failed and exit early (to try again).
      if (isnan(h)|| isnan(t) || isnan(f)) {
      Serial.println("☹ ไม่สามารถอ่านจากเซ็นเซอร์ DHT!");
      return;
      }

      Serial.print(" ☀ ☁ ☂ ☼ ");
      Serial.print("อุณหภูมิ         ⇨ "); //อุณหภูมิ
      Serial.print(String(t).c_str());
      Serial.println(" ℃ ");
      client.publish(tempc, String(t).c_str(), true);

      Serial.print(" ❄ ≋≋≋ ❋ ");
      Serial.print("ความชื้นในอากาศ  ⇨ "); //ความชื้นในอากาศ
      Serial.print(String(h).c_str());
      Serial.println("  RH ");
      client.publish(humid, String(h).c_str(), true);
  if (!client.connected()) {
    reconnect(); }
    client.loop();    }
