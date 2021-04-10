#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>


int s = 0;
int analogInPin = A0; 

#define wifi_ssid "Galaxy Note20 Ultra"
#define wifi_password "12131415"
#define mqtt_server "35.240.247.126"

#define s1 "s1"


WiFiClient espClient;
PubSubClient client(espClient);

//int ledPin1 = D5; // ไฟสีเขียวของค่าความเข้มแสง
//int ledPin2 = D6; // ไฟสีแดงของค่าความเข้มแสง
int ledPin3 = D7; // ไฟสีเขียวของค่าความชื้นในดิน
int ledPin4 = D8;  // ไฟสีแดงของค่าความชื้นในดิน
int Relay1 = D1; //รีเลย์ควบคุมปั้มน้ำหยด

void setup() {
  Serial.begin(115200);
//  pinMode(ledPin1, OUTPUT); // sets the pin as output
 // pinMode(ledPin2, OUTPUT); // sets the pin as output
  pinMode(ledPin3, OUTPUT);  // sets the pin as output
  pinMode(ledPin4, OUTPUT);  // sets the pin as output
  pinMode(Relay1, OUTPUT); // กำหนดขาทำหน้าที่ให้ขา 2 เป็น OUTPUT
  digitalWrite(Relay1, HIGH);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
    
      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;  
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);
      clientName += "-";
      clientName += String(micros() & 0xff, 16);
      Serial.print("Connecting to ");
      Serial.print(mqtt_server);
      Serial.print(" as ");
      Serial.println(clientName);


    // Attempt to connect
    // If you do not want to use a username and password, change next line to
  if (client.connect((char*) clientName.c_str())) {
    //if (client.connect((char*) clientName.c_str()), mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  
      if (!client.connected()) {
        reconnect();
      }
      client.loop();

      // Wait a few seconds between measurements.
      delay(5000);

 int sensorValue1 = analogRead(A0); //ค่าความชื้นในดิน
  s = analogRead(analogInPin);

  if (s < 600) { 
    digitalWrite(ledPin3, HIGH); // สั่งให้ LED ที่ Pin3 ดับ *ไฟสีเขียว
    digitalWrite(ledPin4, LOW); // สั่งให้ LED ที่ Pin4 ติดสว่าง *ไฟสีแดง
    digitalWrite(Relay1, HIGH); // สั่งให้ปั้มน้ำหยดติด
  }
  else {
    digitalWrite(ledPin3, LOW); // สั่งให้ LED ที่ Pin2 ติดสว่าง *ไฟสีเขียว
    digitalWrite(ledPin4, HIGH); // สั่งให้ LED ที่ Pin4 ดับ *ไฟสีแดง
    digitalWrite(Relay1, LOW); // สั่งให้ปั้มน้ำหยดดับ
  }
  
 // float f = l;
  float h = s / 8 ;

      Serial.print(" ֍ ❃ ✻ ✼ ");
      Serial.print("ความชื้นในดิน            ⇨ ");
      Serial.print(String(h).c_str());
      Serial.println(" ％ ");
      client.publish(s1, String(h).c_str(), true);

}
