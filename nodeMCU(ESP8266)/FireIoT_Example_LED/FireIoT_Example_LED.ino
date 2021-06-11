#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Ticker.h>
#define WIFI_SSID "praveensWiFi"
#define WIFI_PASSWORD "zxccxzzxc"
#define FIREBASE_HOST "fireiot-app-default-rtdb.asia-southeast1.firebasedatabase.app"
#define API_KEY "AIzaSyBMbMzH-djvb9E8ywJDF2T9V3f_UeF9G-Q"
#define USER_EMAIL "pk@praveensmedia.com"
#define USER_PASSWORD "123456"
FirebaseData fbdoDn;
FirebaseData fbdoUp;
FirebaseAuth auth;
FirebaseConfig config;
String path = "";
bool switchState[8];//This boll Array is to Store incoming SwithStates(Switch_0 to Switch_7)
signed int seekBarInt[3];// This usigned in is to Store Inconing SeekBar Values(SeekBar_0 to SeekBar_2)
String dispString[] = {"myDisp1","Disp2","Connected"};//This String Array stores Outgoing Strings.(Disp_0 to Disp_2)
Ticker flipper;
Ticker updateDB;
bool ledState=false;
bool diplayUpdate=false;
int count =0;
int updateDelay=1000;

unsigned long sendDataPrevMillis = 0;

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
  fbdoDn.setResponseSize(1024);
  fbdoDn.setBSSLBufferSize(1024, 1024);
  //Firebase.setReadTimeout(fbdo, 3000);
  String base_path = "/FireIoT/Users/";
  
  path = base_path + auth.token.uid.c_str()+"/Data";
  if (!Firebase.beginStream(fbdoDn, path+"/StreamDn")){
    Serial.println("Error Reason: " + fbdoDn.errorReason());
  }
  pinMode(LED_BUILTIN,OUTPUT);
  //updateDB.attach(2, UpdateDispBool);
}

void loop()
{

//if (millis() - sendDataPrevMillis > updateDelay)
//  {
//    sendDataPrevMillis = millis();
//    count++;
//    dispString[1] =(String)count;
//    dispString[0] =(String)(count+5);
//    FirebaseJson json;
//    json.set("inTxt0", dispString[0]);
//    json.set("inTxt1", dispString[1]);
//    json.set("inTxt2", dispString[2]);
//    Firebase.setJSON(fbdoUp, path + "/StreamUp", json);
//  }
  
  if (!Firebase.readStream(fbdoDn)){
    Serial.println("Error Reason: " + fbdoDn.errorReason());    
      }

  if (fbdoDn.streamTimeout())
  {
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
      }

  if (fbdoDn.streamAvailable())
  {/*
    This below string look like 00000000|0|0|0; This is the incoming data from FireIoT App. 
    First 8 char Ripresents 8 switches(Switch_0 to Switch 7)
    Next Strings between | | pipes Represents SeekBars(SeekBar_0 to SeekBar_2)
    */
    String dd =fbdoDn.stringData();
    for(int i=0; i<8;i++){
      if(dd.substring(i,i+1) == "0"){//Converting 0 or 1 char to bool.
        switchState[i]=false;
      }else{
        switchState[i]=true;
        }
    }
    String SeekStr=dd.substring(9);
    int seekCount=0;
    int seekIndex=0;
    for (int i = 0; i < SeekStr.length(); i++) {
        if (SeekStr.substring(i, i+1) == "|") {
          seekBarInt[seekCount] =SeekStr.substring(seekIndex, i).toInt();
          seekCount++;
          seekIndex=i+1;
          }
        }
    Serial.println(dd);//just for Debugging
    //Serial.println(seekBarInt[0]);
    //Serial.println(seekBarInt[1]);
    //Serial.println(seekBarInt[2]);
    digitalWrite(LED_BUILTIN,!switchState[0]);//switchState[Switch number (0 to 7)]
    
    float delayInMills=(float)seekBarInt[0]/1000.0;
    dispString[2]="LED State:"+(String)(switchState[0])+"-"+(String)delayInMills;
    if(switchState[0]){
      flipper.attach(delayInMills, flip);
    }else{
      flipper.detach();
      digitalWrite(LED_BUILTIN,HIGH);
      }
  }
  //if(diplayUpdate)updateDisp();
  
}

void flip() {
  digitalWrite(LED_BUILTIN, ledState);     // set pin to the opposite state
  ledState =!ledState;
}
//void updateDisp() {
//  FirebaseJson json;
//  diplayUpdate=false;
//  count++;
//  dispString[0]= (String)count+".00";
//  
//    json.set("inTxt0", dispString[0]);
//    json.set("inTxt1", dispString[1]);
//    json.set("inTxt2", dispString[2]);
//     //upStrings.add("inTxt"+i, dispString[i]);
//     //Firebase.setString(fbdo, path + "/StreamUp/inTxt"+i, dispString[i]);
//  
//  if (Firebase.setJSON(fbdo, path + "/StreamUp", json))
//    {
//      Serial.println("PASSED");
//      Serial.println();
//    }
//    else
//    {
//      Serial.println("FAILED");
//      Serial.println("REASON: " + fbdo.errorReason());
//      Serial.println("------------------------------------");
//      Serial.println();
//    }
//    
//  
//}
//void UpdateDispBool(){
//  diplayUpdate =true;
//  }
