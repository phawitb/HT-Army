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
// #include <NTPClient.h> //firebase
#include <WiFiUdp.h>
// #include <Firebase_ESP_Client.h>
// #include "addons/TokenHelper.h"
// #include "addons/RTDBHelper.h"
#include "DHT.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// #include <Arduino.h>
// #include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>


#define TFT_CS         D2   //14
#define TFT_RST        D3   //15
#define TFT_DC         D4   //32
#define DHTPIN D1   
#define DHTTYPE DHT22 
// #define API_KEY "AIzaSyAS18pOu8eEeyTO6iZM80MZQAtsgLVilW0"
// #define DATABASE_URL "https://ht-army-default-rtdb.asia-southeast1.firebasedatabase.app"
// #define USER_EMAIL "army061@ht.com"        //"ht-army04@htarmy.com"       //---------------------
// #define USER_PASSWORD "Eh2JNy"          //"12345678"        //---------------------
// #define TIME_SENDFIREBASE 180000     //180000        //---------------------ms
#define ST77XX_B 0X04FF
#define ST77XX_M 0x07E0  //0XF81F
// #define ST77XX_CYAN 0X07FF

#define ST77XX_BLACK 0xFFFF//0x0000
#define ST77XX_RED 0xFFE0//0x001F
#define ST77XX_GREEN 0xF81F //0x07E0
#define ST77XX_WHITE 0x0000//0xFFFF
#define ST77XX_BLUE 0xF800
#define ST77XX_CYAN 0x07FF
// #define ST77XX_YELLOW 0xFFE0
#define ST77XX_MAGENTA 0xF81F

String device_id = "202306001";  //---------------------

// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;
// String uid;
// String databasePath;
// String databasePath2;
// String tempPath = "/temperature";
// String humPath = "/humidity";
// String flagPath = "/flag";  
// String timePath = "/timestamp";
// String parentPath,parentPath2;
bool firsttime = true;
float humid,temp,hic,water;
int train,rest,timestamp;
String flag,status;
unsigned long sendDataPrevMillis = 0;
// unsigned long timerDelay = TIME_SENDFIREBASE;
int timerDelay = 10;
float adjust_temp = 0;
float adjust_humid = 0;

// FirebaseJson json,json2;
WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org");
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);
WiFiManager wm;

// unsigned long getTime() {
//   timeClient.update();
//   unsigned long now = timeClient.getEpochTime();
//   return now;
// }

int update_api(String device_id,float temp,float humid,float hic,String flag){
  sendDataPrevMillis = millis();

  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  int httpResponseCode;
  if(https.begin(*client, "https://wssignal.whsse.net/transaction")) {  // HTTPS    https://www.arduinoall.net/arduino-tutor/code/client.php
  // if(https.begin(*client, "https://ht-army-api-default-rtdb.asia-southeast1.firebasedatabase.app/post.json")) {  // HTTPS    https://www.arduinoall.net/arduino-tutor/code/client.php
      //String p = "{\"device_id\":\"" + device_id + "\",\"temperature\":\"" + String(temp) + "\",\"humidity\":\"" + String(humid) + "\",\"heat_index\":\"" + String(hic) + "\"}";    
      //String p = "{\"device_id\":\"" + device_id + "\",\"temperature\":\"" + String(temp) + "\",\"humidity\":\"" + String(humid) + "\",\"heat_index\":\"" + String(hic) + "\"}";    

      //String p = "{\"data\": \"" + String(123.33) + "\"}";  //ok
      //String p = "{\"serial_number\": \"202306001\", \"heat_index\": 55.5, \"humidity\": 23.4, \"temperature\": 33.4}";  //ok
     
      String p = "{\"serial_number\": \""+String(device_id)+"\", \"heat_index\":" + String(hic) + ", \"humidity\": " + String(humid) + ", \"temperature\":" + String(temp) + "}";      
      https.addHeader("Content-Type", "application/json");
      httpResponseCode = https.POST(p);
      String content = https.getString();
      Serial.print("\nhttpResponseCode"); Serial.print(httpResponseCode);
      Serial.print("content"); Serial.println(content);
      https.end();
    }else{
      Serial.printf("[HTTPS] Unable to connect\n");
    }

    return httpResponseCode;
    

}


// void update_firebase(float temp,float humid,String flag){
//   sendDataPrevMillis = millis();
//   timestamp = getTime();
//   // Serial.print ("time: ");
//   // Serial.println (timestamp);
//   parentPath= databasePath + "/" + String(timestamp);
//   json.set(tempPath, temp);
//   json.set(humPath, humid);
//   json.set(flagPath, flag);
//   json.set(timePath, String(timestamp));
//   parentPath2= databasePath2;
//   json2.set(tempPath, temp);
//   json2.set(humPath, humid);
//   json2.set(flagPath, flag);
//   json2.set(timePath, String(timestamp));

//   Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
//   Serial.printf("Set2 json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath2.c_str(), &json2) ? "ok" : fbdo.errorReason().c_str());
// }

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

  // tft.setCursor(0, 220);
  // // tft.setTextColor(ST77XX_RED);
  // tft.setTextSize(2);
  // tft.print("BA" + String(bat_percentage) + "%");

  // tft.setCursor(230, 220);
  // tft.setCursor(0, 200);
  // tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  String s = status;
  s.toUpperCase();
  tft.print(s);
  

}

void setup(){
  Serial.begin(115200);

  dht.begin();

  tft.init(240, 320);
  // tft.invertDisplay(true);
  // tft.setRotation(3);        
  tft.setRotation(1);        
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(0, 40);
  tft.setTextSize(3);
  // tft.println(String(USER_EMAIL).substring(0, 7));
  tft.println(device_id);
  

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

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop(){

  // sensorValue = analogRead(A0);
  // float voltage = (((analogRead(A0) * 3.3) / 1024) * 2 + 0); //multiply by two as voltage divider network is 100K & 100K Resistor
  // int bat_percentage = 2*mapfloat(voltage, 2.8, 4.2, 0, 100)-19; //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
  int bat_percentage = analogRead(A0);

  Serial.print("Batterry Percen...");
  Serial.println(bat_percentage);
  Serial.print("A0...");
  Serial.println(analogRead(A0));
  // Serial.print("VOL..");
  // Serial.println(analogRead(voltage));

  if(WiFi.status()==3 && firsttime == true){
    // timeClient.begin();
    // config.api_key = API_KEY;
    // config.database_url = DATABASE_URL;
    // auth.user.email = USER_EMAIL;
    // auth.user.password = USER_PASSWORD;
    // Firebase.reconnectWiFi(true);
    // fbdo.setResponseSize(4096);
    // config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
    // config.max_token_generation_retry = 5;
    // Firebase.begin(&config, &auth);
    // while ((auth.token.uid) == "") {
    //   Serial.print('.');
    //   delay(1000);
    // }
    // uid = auth.token.uid.c_str();
    // Serial.print("User UID: ");
    // Serial.println(uid);
    // databasePath = "/UsersData/" + uid + "/historys";
    // databasePath2 = "/UsersData/" + uid + "/last";

    // //--------
    // Serial.println("xxx--------------===-");
    // if (Firebase.RTDB.getInt(&fbdo, "/config/sent_data")) {
    //   Serial.println("--------------===-");
    //   if (fbdo.dataType() == "int") {
    //     timerDelay = fbdo.intData();
    //     Serial.println(timerDelay);
    //   }
    // }
    // else {
    //   Serial.println(fbdo.errorReason());
    // }

    // if (Firebase.RTDB.getString(&fbdo, "/UsersData/" + uid + "/config/adjusterror/temp")) {
    //   Serial.println("--------------===-");
    //   if (fbdo.dataType() == "string") {
    //     adjust_temp = fbdo.stringData().toFloat();
    //     Serial.println(adjust_temp);
    //   }
    // }
    // else {
    //   Serial.println(fbdo.errorReason());
    // }

    // if (Firebase.RTDB.getString(&fbdo, "/UsersData/" + uid + "/config/adjusterror/humid")) {
    //   Serial.println("--------------===-");
    //   if (fbdo.dataType() == "string") {
    //     adjust_humid = fbdo.stringData().toFloat();
    //     Serial.println(adjust_humid);
    //   }
    // }
    // else {
    //   Serial.println(fbdo.errorReason());
    // }

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
    // if(Firebase.ready()!=1){
    //   status = "notfb";
    // }    
  }
  else{
    status = "offline";
  }
  //update screen
  
  Serial.print("temp");
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
  if (WiFi.status()==3 && (millis() - sendDataPrevMillis > timerDelay*1000 || sendDataPrevMillis == 0)){
    // update_firebase(temp,humid,flag);
    int httpResponseCode = update_api(device_id, temp, humid, hic, flag);
    if(httpResponseCode != 201){
      status = "severfail";
    }
      
    
  }

  updateScreen(flag,temp,humid,status,bat_percentage);

  wm.process();
  delay(5000);

  // updateScreen("WHITE",temp,humid,status,bat_percentage);
  // delay(200);
  // updateScreen("GREEN",temp,humid,status,bat_percentage);
  // delay(200);
  // updateScreen("YELLOW",temp,humid,status,bat_percentage);
  // delay(200);
  // updateScreen("RED",temp,humid,status,bat_percentage);
  // delay(200);
  // updateScreen("BLACK",temp,humid,status,bat_percentage);
  // delay(200);

}


// #include <ESP8266WiFi.h>          
// #include <ESP8266WebServer.h>     
// #include <AutoConnect.h>
// #include <TridentTD_LineNotify.h>

// ESP8266WebServer Server;          
// AutoConnect      Portal(Server);

// timeClient.begin();
// config.api_key = API_KEY;
// config.database_url = DATABASE_URL;
// auth.user.email = USER_EMAIL;
// auth.user.password = USER_PASSWORD;
// Firebase.reconnectWiFi(true);
// fbdo.setResponseSize(4096);
// config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
// config.max_token_generation_retry = 5;
// Firebase.begin(&config, &auth);
// while ((auth.token.uid) == "") {
//   Serial.print('.');
//   delay(1000);
// }
// uid = auth.token.uid.c_str();
// Serial.print("User UID: ");
// Serial.println(uid);
// databasePath = "/UsersData/" + uid + "/history";
// databasePath2 = "/UsersData/" + uid + "/last";

// for (byte i = 0; i <= 3; i = i + 1) {
//   if(Firebase.ready()){
//     if (Firebase.RTDB.getString(&fbdo, "/UsersData/" + uid + "/config/line/" + String(i))) {
//     // Serial.println(fbdo.dataType());
//       if (fbdo.dataType() == "string") {
//         lines[i] = fbdo.stringData();
//         Serial.println(lines[i]);
//       }
//     }
//     else {
//       lines[i] = "";
//       Serial.println(fbdo.errorReason());
//     }
//   }
// }

//check internet
// Serial.println("WiFi.status: ");    Serial.print(WiFi.status());
// Serial.println("Firebase.ready: "); Serial.print(Firebase.ready());



// void rootPage() {
//   char content[] = "Hello, world";
//   Server.send(200, "text/plain", content);
// }

//autoconnect
// Portal.handleClient();

// //linenotify
// Serial.println("line-------vvvvv-----vvv--");
// Serial.println(lines[0]);
// Serial.println(lines[1]);
// Serial.println(lines[2]);
// Serial.println(lines[3]);

// LINE.setToken("m1XN6O5cu7nQSzkMTOOPg7tTxCh4bk4VTBP0XALbRe6"); LINE.notify("xxxxxxx");
// LINE.setToken(lines[1]); LINE.notify("yyyyyyy");




