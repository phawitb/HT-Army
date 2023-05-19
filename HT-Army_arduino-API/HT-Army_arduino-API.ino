/*Connect
HT22
HT22 <---> D1
VCC <---> 3V3,
GND <---> GND
LED2.4Display
LED <---> 3V3
CS <---> D2 
RST <---> D3 
D/C <---> D4 
MOSI <---> D7 
SCK <---> D5 
VCC <---> 3V3,l
GND <---> GND

{
  "rules": {
    "UsersData": {
      "$uid": {
        ".read": "$uid === auth.uid",
        ".write": "$uid === auth.uid"
      }
    },
    "config" : {
      ".read": true,
    	".write": true
    }
  }
}

*/
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <NTPClient.h> //firebase
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "DHT.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Arduino.h>

#define TFT_CS         D2   //14
#define TFT_RST        D3   //15
#define TFT_DC         D4   //32
#define DHTPIN D1   
#define DHTTYPE DHT22 
#define USER_EMAIL "army009@ht.com"        //---------------------
#define DEVICE_ID "army009"  //----------------------------------------
#define ST77XX_B 0X04FF
#define ST77XX_M 0XF81F
#define ST77XX_CYAN 0X07FF

String url = "https://ht-army-api-default-rtdb.asia-southeast1.firebasedatabase.app/post.json";  //----------------------------------------
int timerDelay = 60;  //----------------------------------------

bool firsttime = true;
float humid,temp,hic,water;
int train,rest,timestamp;
String flag,status;
unsigned long sendDataPrevMillis = 0;
float adjust_temp = 0;
float adjust_humid = 0;

// FirebaseJson json,json2;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);
WiFiManager wm;

unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

void update_api(String device_id,float temp,float humid,float hic,String flag){
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;

  if(https.begin(*client, url)) {  // HTTPS    https://www.arduinoall.net/arduino-tutor/code/client.php
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

void updateScreen(String flag,float temp,float humid,String status,int bat_percentage) {

  tft.fillScreen(ST77XX_BLACK);
  if(status=="online"){
    tft.setTextColor(ST77XX_M);
  }
  else{
    tft.setTextColor(ST77XX_RED);
  }
  
  tft.setCursor(0, 30);
  tft.setTextSize(10);
  tft.print(String(int(floor(temp))));
  tft.setTextSize(3);
  tft.print("O");

  tft.setTextColor(ST77XX_B);
  tft.setCursor(167, 30);
  tft.setTextSize(10);
  tft.print(String(int(floor(humid))));
  tft.setTextSize(4);
  tft.print("%");

  tft.setTextSize(4);
  tft.setCursor(110, 72);

  if(status=="online"){
    tft.setTextColor(ST77XX_M);
  }
  else{
    tft.setTextColor(ST77XX_RED);
  }
  tft.print("." + String(int(10*(temp-floor(temp)))));

  tft.setCursor(270, 72);
  tft.setTextColor(ST77XX_B);
  tft.print("." + String(int(10*(humid-floor(humid)))));

  tft.setCursor(0, 160);
  tft.setTextSize(7);
  if(flag=="GREEN"){
    tft.fillRect(0,140,350,140,ST77XX_GREEN);  //bg
    tft.setTextColor(ST77XX_BLACK); //text
    tft.println(" GREEN ");   //7
  }
  else if (flag=="RED") {
    tft.fillRect(0,140,350,140,ST77XX_RED);
    tft.setTextColor(ST77XX_WHITE);
    tft.println("  RED  ");   //7
  } 
  else if (flag=="YELLOW") {
    tft.fillRect(0,140,350,140,ST77XX_YELLOW);
    tft.setTextColor(ST77XX_BLACK);
    tft.println(" YELLOW ");   //7
  }
  else if (flag=="BLACK") {
    tft.fillRect(0,140,350,140,ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.println(" BLACK ");   //7
  }  
  else if (flag=="WHITE") {
    tft.fillRect(0,140,350,140,ST77XX_WHITE);
    tft.setTextColor(ST77XX_BLACK);
    tft.println(" WHITE ");   //7
  }

  if(status == "online"){
    tft.setCursor(240, 220);    
    if(flag=="GREEN"){
      tft.setTextColor(ST77XX_BLUE);
    }
    else{
      tft.setTextColor(ST77XX_GREEN);
    }
  }
  else{
    tft.setCursor(230, 220);
    if(flag=="RED"){
      tft.setTextColor(ST77XX_BLACK);
    }
    else{
      tft.setTextColor(ST77XX_RED);
    }    
  }

  tft.setTextSize(2);
  String s = status;
  s.toUpperCase();
  tft.print(s);
  

}

void setup(){
  Serial.begin(115200);

  dht.begin();

  tft.init(240, 320);
  tft.invertDisplay(false);
  tft.setRotation(3);        
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(0, 40);
  tft.setTextSize(7);
  tft.println(String(USER_EMAIL).substring(0, 7));

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    
  //wm.resetSettings();
  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(60);
  if(wm.autoConnect("AutoConnectAP")){
      Serial.println("connected...yeey :)");
  }
  else {
      Serial.println("Configportal running");
  }
}


void loop(){
  
  int bat_percentage = analogRead(A0);

  Serial.print("Batterry Percen...");
  Serial.println(bat_percentage);
  Serial.print("A0...");
  Serial.println(analogRead(A0));

  if(WiFi.status()==3 && firsttime == true){
    timeClient.begin();

    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(4);
    tft.setCursor(0, 120);
    String s = "";
    if(adjust_temp >0){
      s = "+";
    }
    tft.println("Temp "+s+String(adjust_temp));
    tft.setCursor(0, 170);
    s = "";
    if(adjust_humid >0){
      s = "+";
    }
    tft.println("Humid "+s+String(adjust_humid));
    delay(5000);

    firsttime = false;

  }
  //read DHT
  humid = dht.readHumidity() + adjust_humid;
  temp = dht.readTemperature() + adjust_temp;
  if(isnan(humid) || isnan(temp)) {
    humid = -99;
    temp = -99;
    hic = -99;
  }
  else{
    hic = dht.computeHeatIndex(temp, humid, false);
  }
  if(hic<27){
    flag = "WHITE";
    water = 0.5;
    train = 60;
    rest = 0;
  }
  else if(hic<32){
    flag = "GREEN";
    water = 1.5;
    train = 50;
    rest = 10;
  }
  else if(hic<41){
    flag = "YELLOW";
    water = 1;
    train = 45;
    rest = 15;
  }
  else if(hic<55){
    flag = "RED";
    water = 1;
    train = 30;
    rest = 30;
  }
  else{
    flag = "BLACK";
    water = 1;
    train = 20;
    rest = 40;
  }
  //find status
  if(WiFi.status()==3){
    status = "online";
    if(Firebase.ready()!=1){
      status = "online";
    }    
  }
  else{
    status = "offline";
  }
  //update screen
  updateScreen(flag,temp,humid,status,bat_percentage);
  Serial.print("\ntemp");
  Serial.println(temp);
  Serial.print("humid");
  Serial.println(humid);
  //firebase
  Serial.print("timerDelay");
  Serial.println(timerDelay);
  Serial.print("adjust_temp");
  Serial.println(adjust_temp);
  Serial.print("adjust_humid");
  Serial.println(adjust_humid);
  Serial.print("WiFi.status()"); Serial.println(WiFi.status());
  Serial.print("sendDataPrevMillis"); Serial.println(sendDataPrevMillis);
  Serial.print("timerDelay"); Serial.println(timerDelay);
  // if (WiFi.status()==3 && Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay*1000 || sendDataPrevMillis == 0)){
  //   update_firebase(temp,humid,flag);
  // }
  if (WiFi.status()==3 && (millis() - sendDataPrevMillis > timerDelay*1000 || sendDataPrevMillis == 0)){
    // update_firebase(temp,humid,flag);
    String device_id = DEVICE_ID;
    update_api(device_id, temp, humid, hic, flag);
  }

  wm.process();
  delay(1000);

}

