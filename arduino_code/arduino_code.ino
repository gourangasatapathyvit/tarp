#if defined(ESP32)
#include <WiFi.h>
#include <time.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

/* 1. Define the WiFi credentials */
#define WIFI_SSID "polopolo"
#define WIFI_PASSWORD "********"
#include "DHT.h"

/* 2. Define the API Key */
#define API_KEY "AIzaSyB25fJSi_15-W7mLPiJiXgyKorDjYwIHUI"

/* 3. Define the RTDB URL */
#define DATABASE_URL "elated-capsule-249919.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "pulusatapathy@gmail.com"
#define USER_PASSWORD "********"

//Define Firebase Data object
FirebaseData fbdo;
DHT dht;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

int timezone = 5 * 3600;
int dst = 0;
int count = 0;
int dlt = 0;

boolean countStatus;
int beat, bpm;
unsigned long millisBefore;


void setup()
{

  Serial.begin(115200);
  dht.setup(2);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the user sign in credentials */
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

    configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }
  Serial.println("\nTime response....OK"); 
}

void loop(){
  if(dlt==0){
    Firebase.deleteNode(fbdo, "/test/push");
    dlt=1;
  }
  
  int sensorValue = analogRead(A0);
  sendDataPrevMillis = millis();
  
  if (countStatus == 0) {
    if (sensorValue > 550) {
      countStatus = 1;
      beat++;
      Serial.println((millis()*0.001));
    }
  }else{
    if(sensorValue <= 540){
      countStatus = 0;
    }
  }
  
  if (millis()-millisBefore>15000){
    float humidity = dht.getHumidity();     /* Get humidity value */
    float temperature = dht.getTemperature(); /* Get temperature value */
    bpm=beat*4;
    Serial.println(beat);
    beat=0;
    Serial.println(bpm);
    Serial.println(millis()-millisBefore);
    FirebaseJson json;
    json.add("time", "om");
    json.add("pulsevalue", "om");
    json.add("temperature", "om");
    json.add("humidity", "om");
    Serial.printf("Push json... %s\n", Firebase.pushJSON(fbdo, "/test/push", json) ? "ok" : fbdo.errorReason().c_str());
    json.set("time",int(millis()*0.001));
    json.set("pulsevalue",bpm);
    json.set("temperature",temperature);
    json.set("humidity",humidity);
    Serial.printf("Update json... %s\n\n", Firebase.updateNode(fbdo, String("/test/push/" + fbdo.pushName()), json) ? "ok" : fbdo.errorReason().c_str());
    millisBefore=millis();
    
  }
    delay(10);
}
