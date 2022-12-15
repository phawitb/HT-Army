# HT-Army
## ref:  
https://randomnerdtutorials.com/esp8266-data-logging-firebase-realtime-database/#Set-up-Realtime-Database  
https://randomnerdtutorials.com/esp32-esp8266-firebase-gauges-charts/  
 

## Web App  
#### 1.firebase console  
#### 2.Project overview >> Add app >> html >> register app >> copy firebaseConfig  
#### 3.Authentication  
Authentication >> Add User  
Realtime Database >> Rules  
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
#### 4.add path to firebase
config > sent_data:60             //sent data to firebase every 60 secound. 

## Terminal  
#### 1.cd HT-Army  
#### 2.firebase login  
-Allow firebase to collect CLI usage and error reporting information >> n  

#### 3.firebase init  
-Are you ready to proceed >> yes  
-select >> Realtime Database : Configure.....  
        >> Hosting : Configure.....  
-Use an existing project  
-What do you want to use as your public directory? Hit Enter to select public.  
-Configure as a single-page app (rewrite urls to /index.html)? No  
-Set up automatic builds and deploys with GitHub? No  

#### 4.firebase deploy  

#### 5.copy HT-Army App from GitHub to project floder  
#### 6.firebase deploy  

https://ht-army.web.app/  

## Arduino  
DHT22 <---> D1  
VCC <---> 3V3  
GND <---> GND  
LED2.4Display  
CS <---> D2  
RST <---> D3  
D/C <---> D4  
MOSI <---> D7  
SCK <---> D5  
VCC <---> 3V3  
GND <---> GND  


