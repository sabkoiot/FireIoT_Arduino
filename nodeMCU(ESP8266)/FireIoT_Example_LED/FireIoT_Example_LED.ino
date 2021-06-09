#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#define WIFI_SSID "praveensWiFi"
#define WIFI_PASSWORD "zxccxzzxc"
#define FIREBASE_HOST "fireiot-app-default-rtdb.asia-southeast1.firebasedatabase.app"
#define API_KEY "AIzaSyBMbMzH-djvb9E8ywJDF2T9V3f_UeF9G-Q"
#define USER_EMAIL "pk@praveensmedia.com"
#define USER_PASSWORD "123456"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String path = "/FireIoT/Users";
uint16_t count = 0;

void setup()
{
  Serial.begin(115200);

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

  /* Assign the project host and api key (required) */
  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(1024);
  fbdo.setBSSLBufferSize(1024, 1024);
  String base_path = "/FireIoT/Users/";
  
  path = base_path + auth.token.uid.c_str()+"/Data";
  if (!Firebase.beginStream(fbdo, path+"/StreamDn")){
    Serial.println("Error Reason: " + fbdo.errorReason());
  }
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop()
{
  if (!Firebase.readStream(fbdo)){
    Serial.println("Error Reason: " + fbdo.errorReason());    
      }

  if (fbdo.streamTimeout())
  {
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
      }

  if (fbdo.streamAvailable())
  {
    Serial.print("VALUE: ");
    String dd =fbdo.stringData();
    Serial.println(dd);
    if(dd.substring(0,1) == "0"){
      digitalWrite(LED_BUILTIN,HIGH);
    }else{
      digitalWrite(LED_BUILTIN,LOW);
      }
  }
}
