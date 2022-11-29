/*Connect
HT22
HT22 <---> D1
VCC <---> 3V3,
GND <---> GND
LED2.4Display
CS <---> D2 
RST <---> D3 
D/C <---> D4 
MOSI <---> D7 
SCK <---> D5 
VCC <---> 3V3,
GND <---> GND
*/
#include <ESP8266WiFi.h>          
#include <ESP8266WebServer.h>     
#include <AutoConnect.h>
#include <NTPClient.h> //firebase
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "DHT.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_CS         D2   //14
#define TFT_RST        D3   //15
#define TFT_DC         D4   //32
#define DHTPIN D1   
#define DHTTYPE DHT22 
#define API_KEY "AIzaSyAS18pOu8eEeyTO6iZM80MZQAtsgLVilW0"
#define DATABASE_URL "https://ht-army-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "ht-army01@htarmy.com"       //---------------------
#define USER_PASSWORD "12345678"        //---------------------
#define TIME_SENDFIREBASE 30000        //---------------------

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid;
String databasePath;
String databasePath2;
String tempPath = "/temperature";
String humPath = "/humidity";
String presPath = "/pressure";
String timePath = "/timestamp";
String parentPath,parentPath2;
FirebaseJson json,json2;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer Server;          
AutoConnect      Portal(Server);

float humid,temp,hic,water;
int train,rest,timestamp;
String flag;
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = TIME_SENDFIREBASE;

void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}

unsigned long getTime() {
  timeClient.update();
  delay(500);
  unsigned long now = timeClient.getEpochTime();
  return now;
}

void setup(){
  Serial.begin(115200);

  dht.begin();

  tft.init(240, 320);
  tft.invertDisplay(false);
  tft.setRotation(3);        
  tft.fillScreen(ST77XX_BLACK);

  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }

  timeClient.begin();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  databasePath = "/UsersData/" + uid + "/history";
  databasePath2 = "/UsersData/" + uid + "/last";

}

void update_firebase(float temp,float humid){
  sendDataPrevMillis = millis();
  timestamp = getTime();
  Serial.print ("time: ");
  Serial.println (timestamp);

  parentPath= databasePath + "/" + String(timestamp);
  json.set(tempPath, temp);
  json.set(humPath, humid);

  parentPath2= databasePath2;
  json2.set(tempPath, temp);
  json2.set(humPath, humid);
  json2.set(timePath, String(timestamp));

  Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  Serial.printf("Set2 json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath2.c_str(), &json2) ? "ok" : fbdo.errorReason().c_str());
}

void updateScreen(String flag,float temp,float humid) {
  tft.fillScreen(ST77XX_BLACK);
  
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(4);
  tft.println("Temp: "+String(temp)+"C");
  
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(4);
  tft.println("\nHumid: "+String(humid)+"%");

  if(flag=="WHITE"){
    tft.setTextColor(ST77XX_WHITE);
  }
  else if(flag=="GREEN"){
    tft.setTextColor(ST77XX_GREEN);
  }
  else if(flag=="YELLOW"){
    tft.setTextColor(ST77XX_YELLOW);
  }
  else if(flag=="RED"){
    tft.setTextColor(ST77XX_RED);
  }
  else{
    tft.setTextColor(ST77XX_BLACK);
  }
  
  tft.setTextSize(4);
  tft.println("\nFlag: "+flag);
  
}

void loop(){
  //read DHT
  humid = dht.readHumidity();
  temp = dht.readTemperature();
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
  //update screen
  updateScreen(flag,temp,humid);
  //firebase
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    update_firebase(temp,humid);  
  }
  //autoconnect
  Portal.handleClient();

  delay(5000);

}





