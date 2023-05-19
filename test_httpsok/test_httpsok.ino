#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

String device_id = "army000";

const char* ssid = "JOLEENFIRST";
const char* password = "11111111";

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
}

void update_api(String device_id,float temp,float humid,float hic,String flag){
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;

  if(https.begin(*client, "https://ht-army-api-default-rtdb.asia-southeast1.firebasedatabase.app/post.json")) {  // HTTPS    https://www.arduinoall.net/arduino-tutor/code/client.php
      String p = "{\"device_id\":\"" + device_id + "\",\"temperature\":\"" + String(temp) + "\",\"humidity\":\"" + String(humid) + "\",\"heat_index\":\"" + String(hic) + "\"}";    
      int httpResponseCode = https.POST(p);
      String content = https.getString();
      Serial.print("\nhttpResponseCode"); Serial.print(httpResponseCode);
      Serial.print("content"); Serial.println(content);
      https.end();
    }else{
      Serial.printf("[HTTPS] Unable to connect\n");
    }
    

}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    float temp = 11.11;
    float humid = 22.22;
    float hic = 33.33;    
    // String device_id = "army001";
    String flag = "red";   
    update_api(device_id, temp, humid, hic, flag);

    delay(10000);
  }
}

