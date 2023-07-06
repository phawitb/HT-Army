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
*/

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h>
#include "DHT.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#define TFT_CS         D2   //14
#define TFT_RST        D3   //15
#define TFT_DC         D4   //32
#define DHTPIN D1   
#define DHTTYPE DHT22 
#define ST77XX_B 0X04FF
#define ST77XX_M 0x07E0  //0XF81F
#define ST77XX_BLACK 0xFFFF//0x0000
#define ST77XX_RED 0xFFE0//0x001F
#define ST77XX_GREEN 0xF81F //0x07E0
#define ST77XX_WHITE 0x0000//0xFFFF
#define ST77XX_BLUE 0xF800
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F

String device_id = "202306050";  //-------------------------------
bool firsttime = true;
float humid,temp,hic,water;
int train,rest,timestamp;
String flag,status;
unsigned long sendDataPrevMillis = 0;
// unsigned long timerDelay = TIME_SENDFIREBASE;
int timerDelay = 10;

// String url = "https://raw.githubusercontent.com/phawitb/HT-Army/main/adjusterror.txt";
String url = "https://raw.githubusercontent.com/phawitb/adjustHT4/main/adjust_error.txt";
String strs[20];
int StringCount = 0;
float adjust_temp = 0;
float adjust_humid = 0;

WiFiUDP ntpUDP;
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);
WiFiManager wm;

String splitString(String str,String id) {
  char charArray[str.length() + 1];  
  str.toCharArray(charArray, sizeof(charArray));  
  char* token = strtok(charArray, "\n");
  while (token != NULL) {
    String d = String(token[0])+String(token[1])+String(token[2])+String(token[3])+String(token[4])+String(token[5])+String(token[6])+String(token[7])+String(token[8]);
    if(d==id){
      return token;
    }
    token = strtok(NULL, "\n");
  }
}

void splitString2(String str) {
  Serial.print("xxxx");
  Serial.print(str);

  String strs[10];
  int StringCount = 0;

  while (str.length() > 0)
  {
    int index = str.indexOf(',');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index+1);
    }
  }
  adjust_temp = strs[1].toFloat();
  adjust_humid = strs[2].toFloat();
}

int update_api(String device_id,float temp,float humid,float hic,String flag){
  sendDataPrevMillis = millis();

  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  int httpResponseCode;
  if(https.begin(*client, "https://wssignal.whsse.net/transaction")) {  // HTTPS    https://www.arduinoall.net/arduino-tutor/code/client.php
      String p = "{\"serial_number\": \""+String(device_id)+"\", \"heat_index\":" + String(hic) + ", \"humidity\": " + String(humid) + ", \"flag\": \"" + flag + "\", \"temperature\":" + String(temp) + "}";      

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

void updateScreen(String flag,float temp,float humid,String status,int bat_percentage) {

  tft.fillScreen(ST77XX_BLACK);
  if(status=="online"){
    tft.setTextColor(ST77XX_M);
  }
  else{
    tft.setTextColor(ST77XX_RED);
  }
  if(temp == -99){
    tft.setTextColor(ST77XX_BLACK);
  }
  tft.setCursor(0, 30);
  tft.setTextSize(10);
  tft.print(String(int(floor(temp))));
  tft.setTextSize(3);
  tft.print("O");
  tft.setTextColor(ST77XX_B);
  if(temp == -99){
    tft.setTextColor(ST77XX_BLACK);
  }
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
  if(temp == -99){
    tft.setTextColor(ST77XX_BLACK);
  }
  tft.print("." + String(int(10*(temp-floor(temp)))));
  tft.setCursor(270, 72);
  tft.setTextColor(ST77XX_B);
  if(temp == -99){
    tft.setTextColor(ST77XX_BLACK);
  }
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
  tft.setCursor(5, 220);
  tft.setTextSize(2);
  tft.println(String(hic,1));
  tft.setTextSize(1);
  tft.setCursor(55, 217);
  tft.print("o");
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
  if(status=="severfail"){
    tft.setCursor(210, 220);
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
  // tft.invertDisplay(true);
  // tft.setRotation(3);        
  tft.setRotation(1);        
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(0, 40);
  tft.setTextSize(5);
  tft.println(device_id);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(4);
  tft.setCursor(0, 120);
  tft.println("version2");
  // String s = "";
  // if(adjust_temp >0){
  //   s = "+";
  // }
  // tft.println("Temp "+s+String(adjust_temp));
  // tft.setCursor(0, 170);
  // s = "";
  // if(adjust_humid >0){
  //   s = "+";
  // }
  // tft.println("Humid "+s+String(adjust_humid));

  

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

  int bat_percentage = analogRead(A0);

  Serial.print("Batterry Percen...");  Serial.println(bat_percentage);
  Serial.print("A0...");  Serial.println(analogRead(A0));
  Serial.print("WiFi.status() ");  Serial.println(WiFi.status());
  if(WiFi.status()==3 && firsttime == true){
    firsttime = false;
    Serial.print("firsttime------------------------");

    //get adjHT===========================
    std::unique_ptr<BearSSL::WiFiClientSecure>client2(new BearSSL::WiFiClientSecure);
    client2->setInsecure();
    HTTPClient https2;
    String fullUrl = url;
    Serial.println("Requesting " + fullUrl);
    if (https2.begin(*client2, fullUrl)) {
      int httpCode = https2.GET();
      String content2 = https2.getString();
      Serial.println("============== Response code: " + String(httpCode) + content2);

      String b = splitString(content2,device_id);   //"202306002");
      Serial.println(b);
      splitString2(b);
      Serial.print("adjust_temp"); Serial.println(adjust_temp);
      Serial.print("adjust_humid"); Serial.println(adjust_humid);

      
      // if (httpCode > 0) {
      //   Serial.println(https.getString());
      // }
      https2.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
    //end get adjHT===========================
    tft.fillScreen(ST77XX_BLACK);
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

  }
  //read DHT
  humid = dht.readHumidity();
  temp = dht.readTemperature();
  if(isnan(humid) || isnan(temp)) {
    humid = -99;
    temp = -99;
    hic = -99;
  }
  else{
    humid = humid + adjust_humid;
    temp = temp + adjust_temp;
    hic = dht.computeHeatIndex(temp, humid, false);
  }
  if(hic == -99){
    flag = "none";
    water = -1;
    train = -1;
    rest = -1;
  }    
  else if(hic<27){
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
  }
  else{
    status = "offline";
  }
  //update screen
  Serial.print("temp");
  Serial.println(temp);
  Serial.print("humid");
  Serial.println(humid);
  Serial.print("timerDelay");
  Serial.println(timerDelay);
  Serial.print("adjust_temp");
  Serial.println(adjust_temp);
  Serial.print("adjust_humid");
  Serial.println(adjust_humid);
  if (WiFi.status()==3 && (millis() - sendDataPrevMillis > timerDelay*1000 || sendDataPrevMillis == 0)){
    int httpResponseCode = update_api(device_id, temp, humid, hic, flag);
    if(httpResponseCode != 201){
      status = "severfail";
    }
  }

  updateScreen(flag,temp,humid,status,bat_percentage);

  // delay(500);
  // updateScreen("RED",temp,humid,status,bat_percentage);
  // delay(500);
  // updateScreen("BLACK",temp,humid,status,bat_percentage);
  // delay(500);
  // updateScreen("YELLOW",temp,humid,status,bat_percentage);
  // delay(500);
  // updateScreen("GREEN",temp,humid,status,bat_percentage);
  // delay(500);
  // updateScreen("WHITE",temp,humid,status,bat_percentage);
  // delay(500);
  // updateScreen("none",temp,humid,"severfail",bat_percentage);

  wm.process();
  delay(5000);

}

