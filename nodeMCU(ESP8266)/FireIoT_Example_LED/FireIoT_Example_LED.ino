#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Ticker.h>
#define FIREBASE_HOST "fireiot-app-default-rtdb.asia-southeast1.firebasedatabase.app" //Do not change this
#define API_KEY "AIzaSyBMbMzH-djvb9E8ywJDF2T9V3f_UeF9G-Q" //Do not change this
//Wifi Credentials:
#define WIFI_SSID "Wifi_name" //Your Home WiFi SSID (name)
#define WIFI_PASSWORD "pass" //WiFi Password
//User Credentials:
#define USER_EMAIL "Email"//Rigistered Email on FireioT App
#define USER_PASSWORD "pass"//Your passoword
FirebaseData fbdoDn;//Firebase Database Instance for Down Stream(getting Commands from FireioT App)
FirebaseData fbdoUp;//Firebase Database instance for Up Stream(Sending Data from This Device to FireioT App)
FirebaseAuth auth;  //Instance of Firebase Authentication
FirebaseConfig config;//Instance of Firebase Configuration
String path = ""; //To store the full Path of the Database
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
  String base_path = "/FireIoT/Users/";//Do not Change this
  
  path = base_path + auth.token.uid.c_str()+"/Data";////Do not Change this
  if (!Firebase.beginStream(fbdoDn, path+"/StreamDn")){//Do not Change this
    Serial.println("Error Reason: " + fbdoDn.errorReason());//Print for Debugging
  }
  pinMode(LED_BUILTIN,OUTPUT);// Declaring  LED_BUILTIN as OUTPUT
}

void loop()
{
  if (!Firebase.readStream(fbdoDn)){
    Serial.println("Error Reason: " + fbdoDn.errorReason());  //Print for Debugging  
      }

  if (fbdoDn.streamTimeout()){
    Serial.println("Stream timeout, resume streaming...");//Print for Debugging
      }

  if (fbdoDn.streamAvailable())//If Down Stream Available(Any Input from The FireioT App)
  {/*
    This below string look like 00000000|0|0|0; This is the incoming data from FireIoT App. 
    First 8 char Ripresents 8 switches(Switch_0 to Switch 7)
    Next Strings between | | pipes Represents SeekBars(SeekBar_0 to SeekBar_2)
    */
    String dd =fbdoDn.stringData();//Do not change this
    Serial.println(dd);//just for Debugging,, prints incoming data string from FireioT App
    for(int i=0; i<8;i++){//Do not change this
      if(dd.substring(i,i+1) == "0"){//Converting 0 or 1 char to bool.
        switchState[i]=false;
      }else{
        switchState[i]=true;
        }
    }
    String SeekStr=dd.substring(9);//Do not change this-- Extracts the Data for SeekBars
    int seekCount=0;//Do not change this
    int seekIndex=0;//Do not change this
    for (int i = 0; i < SeekStr.length(); i++) {//Do not change this
        if (SeekStr.substring(i, i+1) == "|") {//Do not change this
          seekBarInt[seekCount] =SeekStr.substring(seekIndex, i).toInt();//Do not change this
          seekCount++;//Do not change this
          seekIndex=i+1;//Do not change this
          }
        }
    
    digitalWrite(LED_BUILTIN,!switchState[0]);//switchState[Switch number (0 to 7)]

    float delayInMills=(float)seekBarInt[0]/1000.0; //seekBarInt[SeekBar Number (0 to 2)]-- we are using seekBar 0 for Blynk Delay;
    //dispString[2]="LED State:"+(String)(switchState[0])+"-"+(String)delayInMills;
    if(switchState[0]){
      flipper.attach(delayInMills, flip);
    }else{
      flipper.detach();
      digitalWrite(LED_BUILTIN,HIGH);
      }
  }
}

void flip() {
  digitalWrite(LED_BUILTIN, ledState);     // set pin to the opposite state
  ledState =!ledState;
}
