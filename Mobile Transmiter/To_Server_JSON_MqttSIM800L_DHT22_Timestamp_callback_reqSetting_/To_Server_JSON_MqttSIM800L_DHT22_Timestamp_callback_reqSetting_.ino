#include <TinyGPS++.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "DHT.h"

//========================================================================================

//loncat lora sim800 
int skipp = 300;
int delta = 60;
float tmaxH = 80;
float tmaxT = -2;
float tminH = 40;
float tminT = -8 ;

//========================================================================================
//Variabel Sensor
//Levelsensor1===================
//#define PinLevel1 34
//float ValueLevel1=0;
//int DigitalLevel1=0;
//int Resistor1=330;
//int MaxSensorLevel1 = 5;
//Temperatur and Humidity Sensor1 DHT22======================
#define DHTPIN 13    // what pin we're connected to
#define DHTTYPE DHT22   // DHT22
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino
float ValueTemperatur1=0;
float ValueHumidity1=0;
//========================================================================================
////OTa WIFI Variable
//
//const char* host = "esp32";
//const char* ssid = "Elefante_Network";
//const char* password = "Elefante.123";

//=========================================================
//OTa WIFI Variable

//WebServer server(80);
//
///* Style */
//String style =
//"<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
//"input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
//"#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
//"#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
//"form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
//".btn{background:#3498db;color:#fff;cursor:pointer}</style>";
//
///* Login page */
//String loginIndex = 
//"<form name=loginForm>"
//"<h1>ELEFANTE</h1>"
//"<h1>Device Login</h1>"
//"<input name=userid placeholder='User ID'> "
//"<input name=pwd placeholder=Password type=Password> "
//"<input type=submit onclick=check(this.form) class=btn value=Login></form>"
//"<script>"
//"function check(form) {"
//"if(form.userid.value=='elefante' && form.pwd.value=='Elefante.123')"
//"{window.open('/serverIndex')}"
//"else"
//"{alert('Error Password or Username')}"
//"}"
//"</script>" + style;
//
///* Server Index Page */
//String serverIndex = 
//"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
//"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
//"<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
//"<label id='file-input' for='file'>   Choose file...</label>"
//"<input type='submit' class=btn value='Update'>"
//"<br><br>"
//"<div id='prg'></div>"
//"<br><div id='prgbar'><div id='bar'></div></div><br></form>"
//"<script>"
//"function sub(obj){"
//"var fileName = obj.value.split('\\\\');"
//"document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
//"};"
//"$('form').submit(function(e){"
//"e.preventDefault();"
//"var form = $('#upload_form')[0];"
//"var data = new FormData(form);"
//"$.ajax({"
//"url: '/update',"
//"type: 'POST',"
//"data: data,"
//"contentType: false,"
//"processData:false,"
//"xhr: function() {"
//"var xhr = new window.XMLHttpRequest();"
//"xhr.upload.addEventListener('progress', function(evt) {"
//"if (evt.lengthComputable) {"
//"var per = evt.loaded / evt.total;"
//"$('#prg').html('progress: ' + Math.round(per*100) + '%');"
//"$('#bar').css('width',Math.round(per*100) + '%');"
//"}"
//"}, false);"
//"return xhr;"
//"},"
//"success:function(d, s) {"
//"console.log('success!') "
//"},"
//"error: function (a, b, c) {"
//"}"
//"});"
//"});"
//"</script>" + style;


//========================================================================================
// The serial connection to the GPS device
//#define ss Serial2
//
////GPS Variable
//static const int RXPin = 2, TXPin = 0;
//static const uint32_t GPSBaud = 9600;
//
//// The TinyGPS++ object
//TinyGPSPlus gps;

//========================================================================================
// LORA XL1278 pin definitions
//#define SCK     25    // GPIO25  -- SX1278's SCK
//#define MISO    19   // GPIO19 -- SX1278's MISnO
//#define MOSI    12   // GPIO12 -- SX1278's MOSI
//#define SS      18   // GPIO18 -- SX1278's CS
//#define RST     14   // GPIO14 -- SX1278's RESET
//#define DI0     15   // GPIO12 -- SX1278's IRQ(Interrupt Request)
//#define BAND  915E6
//int rssi = 0;
//char *node_id = "<5679>";  //From LG01 via web Local Channel settings on MQTT.Please refer <> dataformat in here. 
//char count_1 [8];
//unsigned long new_time,old_time=0;

//========================================================================================
//Json Variabel
char jsonnodeLoraString[850];
unsigned int count = 1; 



//========================================================================================
//SIM800L Variable

//#define MODEM_RST            5
//#define MODEM_PWKEY          4
//#define MODEM_POWER_ON       23
//#define MODEM_TX             26
//#define MODEM_RX             27

// Please select the corresponding model
// #define SIM800L_IP5306_VERSION_20190610
// #define SIM800L_AXP192_VERSION_20200327
// #define SIM800C_AXP192_VERSION_20200609
 #define SIM800L_IP5306_VERSION_20200811
 #include "utilities.h"
 // Select your modem:
#define TINY_GSM_MODEM_SIM800

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1


#include <TinyGsmClient.h>
#include <PubSubClient.h>
// See all AT commands, if wanted
#define DUMP_AT_COMMANDS
// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Add a reception delay - may be needed for a fast processor at a slow baud rate
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define TINY_GSM_TEST_GPRS true
#define TINY_GSM_TEST_BATTERY true

// set GSM PIN, if any
#define GSM_PIN ""

long lastMsg = 0;
char jsonnodeGSM800String[850];
String deviceInfo;
String alldata;
char deviceInfoString[850];
char alldatastring[850];

// Your GPRS credentials, if any
const char apn[] = "AXIS";
const char gprsUser[] = "axis";
const char gprsPass[] = "123456";
//const char apn[] = "internet";
//const char gprsUser[] = "";
//const char gprsPass[] = "";


// MQTT details
//const char deviceId = "el0001";
const char *broker = "iot.elefante.co.id";

const char *topicLed = "GsmClientTest/led";
const char *topicInit = "GsmClientTest/init";
const char *topicLedStatus = "GsmClientTest/ledStatus";
const char *dataPub111 = "GsmClientTest/led";
//const char *dataPub = String("data/")+deviceId;
//const char *dataPub3= "GsmClientTest/led";
//const char *dataPub3= "GsmClientTest/led";
//mqtt.publish("data/el0001", alldatastring);
//     mqtt.subscribe("setting/el0001/#");
//    mqtt.subscribe("reqCurrentData/el0001/#");
//
//"currentData/el0001"
//        "confirm/setting/el0001/reqData"
//        "reqCurrentData/el0001"
//        "confirm/setting/el0001/sampling"
//        "setting/el0001/sampling"
//        "confirm/setting/el0001/tmaxH"
//        "setting/el0001/tmaxH"


#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient mqtt(client);

int ledStatus = LOW;
uint32_t lastReconnectAttempt = 0;
//========================================================================================
//
//void LoraSetup()
//{
//  SPI.begin(SCK,MISO,MOSI,SS);
//  LoRa.setPins(SS,RST,DI0);
//  if (!LoRa.begin(915E6))   //868000000 is frequency
//      { 
//          Serial.println("Starting LoRa failed!");
//          while (1);
//      }
//    // Setup Spreading Factor (6 ~ 12)
//   LoRa.setSpreadingFactor(7);
//   // Setup BandWidth, option: 7800,10400,15600,20800,31250,41700,62500,125000,250000,500000
//   //Lower BandWidth for longer distance.
//   LoRa.setSignalBandwidth(125000);  
//   // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
//   LoRa.setCodingRate4(5);
//   LoRa.setSyncWord(0x34); 
//   Serial.println("LoRa init succeeded.");
//   //Serial.println(frequency);
//   LoRa.onReceive(onReceive);   
//   LoRa.receive();     
//}

//========================================================================================
//void GpsSetup()
//{
//  
//  ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
//
//  Serial.println(F("Full Setup GPS DATA"));
//  Serial.println(F("An extensive example of many interesting TinyGPS++ features"));
//  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
//  Serial.println(F("by Mikal Hart"));
//  Serial.println();
//  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
//  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
//  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
//}

//MqttSIM800setup
//========================================================================================
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    // In order to republish this payload, a copy must be made
    // as the orignal payload buffer will be overwritten whilst
    // constructing the PUBLISH packet.
    SerialMon.println();
    SerialMon.println("=====================Mqtt Callback======================");
    SerialMon.print("Message arrived [");
    SerialMon.print(topic);
    SerialMon.print("]: ");
    SerialMon.write(payload, length);
    SerialMon.println();
    
    String messageTemp;
    for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
    }
    Serial.println();
     

    // Only proceed if incoming message's topic matches
    if (String(topic) == "/setting/el0001/tminT") {
    // Allocate the correct amount of memory for the payload copy
    tminT = messageTemp.toFloat();
    Serial.print("Minimum Temepratur Threshold Berubah = ");
    SerialMon.print(tminT);
    SerialMon.println(" C");
    char tminTString[length+1];
    messageTemp.toCharArray(tminTString,length+1);
    mqtt.publish("/confirm/setting/el0001/tminT", tminTString);
    
//    byte* tminT = (byte*)malloc(length);
//    Serial.print("Minimum Temperatur Threshold Berubah = ");
//    SerialMon.write(payload, length);
//    Serial.println();
//    // Copy the payload to the new buffer
//    memcpy(tminT,payload,length);
//    mqtt.publish("/confirm/setting/el0001/tminT", tminT, length);
//    // Free the memory
//    free(tminT);
    }
    
    if (String(topic) == "/setting/el0001/tminH") {
    // Allocate the correct amount of memory for the payload copy
    tminH = messageTemp.toFloat();
    Serial.print("Minimum Humidity Threshold Berubah = ");
    SerialMon.print(tminH);
    SerialMon.println(" %");
    char tminHString[length+1];
    messageTemp.toCharArray(tminHString,length+1);
    mqtt.publish("/confirm/setting/el0001/tminH", tminHString);
    
//    byte* tmaxT = (byte*)malloc(length);
//    Serial.print("Maximum Temperatur Threshold Berubah = ");
//    SerialMon.write(payload, length);
//    Serial.println();
//    // Copy the payload to the new buffer
//    memcpy(tmaxT,payload,length);
//    mqtt.publish("/confirm/setting/el0001/tmaxT", tmaxT, length);
//    // Free the memory
//    free(tmaxT);
    }

    if (String(topic) == "/setting/el0001/tmaxT") {
    // Allocate the correct amount of memory for the payload copy
    tmaxT = messageTemp.toFloat();
    Serial.print("Maximum Temperatur Threshold Berubah = ");
    SerialMon.print(tmaxT);
    SerialMon.println(" C");
    char tmaxTString[length+1];
    messageTemp.toCharArray(tmaxTString,length+1);
    mqtt.publish("/confirm/setting/el0001/tmaxT", tmaxTString);
//    byte* tminH = (byte*)malloc(length);
//    Serial.print("Minimum Humidity Threshold Berubah = ");
//    SerialMon.write(payload, length);
//    Serial.println();
//    // Copy the payload to the new buffer
//    memcpy(tminH,payload,length);
//    mqtt.publish("/confirm/setting/el0001/tminH", tminH, length);
//    // Free the memory
//    free(tminH);
    }

    if (String(topic) == "/setting/el0001/tmaxH") {
    // Allocate the correct amount of memory for the payload copy
    tmaxH = messageTemp.toFloat();
    Serial.print("Maximum Humidity Threshold Berubah = ");
    SerialMon.print(tmaxH);
    SerialMon.println(" %");
    char tmaxHString[length+1];
    messageTemp.toCharArray(tmaxHString,length+1);
    mqtt.publish("/confirm/setting/el0001/tmaxH", tmaxHString);
    
//    byte* tmaxH = (byte*)malloc(length);
//    Serial.print("Maximum Humidity Threshold Berubah = ");
//    SerialMon.write(payload, length);
//    Serial.println();
//    // Copy the payload to the new buffer
//    memcpy(tmaxH,payload,length);
//    mqtt.publish("/confirm/setting/el0001/tmaxH", tmaxH, length);
//    // Free the memory
//    free(tmaxH);
    }
    
    if (String(topic) == "/setting/el0001/sampling") {
    // Allocate the correct amount of memory for the payload copy
    delta = messageTemp.toInt();
    Serial.print("waktu sampling  Berubah = ");
    SerialMon.print(delta);
    SerialMon.println(" second");
    char deltaString[length+1];
    messageTemp.toCharArray(deltaString,length+1);
    mqtt.publish("/confirm/setting/el0001/sampling", deltaString);
//    byte* sampling = (byte*)malloc(length);
////    delta = sampling
//    Serial.print("waktu sampling  Berubah = ");
//    SerialMon.write(payload, length);
//    Serial.println();
//    // Copy the payload to the new buffer
//    memcpy(sampling,payload,length);
    
    // Free the memory
//    free(sampling);
    }
    if (String(topic) == "/reqCurrentData/el0001") {
    // Allocate the correct amount of memory for the payload copy
    byte* reqData = (byte*)malloc(length);
    Serial.print("Server Request curent Data Status = ");
    SerialMon.write(payload, length);
    Serial.println();
    // Copy the payload to the new buffer
    memcpy(reqData,payload,length);
    mqtt.publish("/confirm/setting/el0001/reqData", reqData, length);
    mqtt.publish("/currentData/el0001",alldatastring);
    // Free the memory
    free(reqData);
    }
//        
//        // Only proceed if incoming message's topic matches
//    if (String(topic) == "setting/el0001/tminT") { 
//        unsigned char* tminT = payload;
//        String payloadString = reinterpret_cast<const char*>(payload);
//        char payloadChar[8];
//       Serial.print("Minimum Temperatur Threshold Berubah = ");
//       SerialMon.write(payload, len);
//       payloadString.toCharArray(payloadChar,8);
//       mqtt.publish("confirm/setting/el0001/tminT", payloadChar);
//    }
}
//=========================================
void reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect("el001Client")) {
      SerialMon.println("connected");
      // Subscribe
      mqtt.subscribe("/setting/el0001/#");
      delay(100);
      mqtt.subscribe("/reqCurrentData/el0001");
      delay(100);
    } else {
      SerialMon.print("failed, rc=");
      SerialMon.print(mqtt.state());
      SerialMon.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


//=========================================

boolean mqttConnect()
{
    SerialMon.println();
    SerialMon.println("==================================Mqtt Connect=================================");
    SerialMon.print("Connecting to ");
    SerialMon.print(broker);

    // Connect to MQTT Broker
    boolean status = mqtt.connect("GsmClientTest");

    // Or, if you want to authenticate MQTT:
    //boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

    if (status == false) {
        SerialMon.println(" fail");
        return false;
    }
    count++;
    SerialMon.println(" success");
    mqtt.publish("/data/el0001", "mqttConnect()");
    SerialMon.println("mqttConnect()");
//     mqtt.publish("data/el0001", jsonnodeGSM800String);
//     mqtt.publish("data/el0001", alldatastring);
//    mqtt.subscribe("data/el0001");
//    mqtt.subscribe("setting/el0001/#");
//    mqtt.subscribe("reqCurrentData/el0001/#");
    
    return mqtt.connected();
}
//=========================================
void MqttSIM800setup()
{   
    SerialMon.println();
    SerialMon.println("==================================Mqtt Sim800 Setup=================================");
    setupModem();

    SerialMon.println("Wait...");

    // Set GSM module baud rate and UART pins
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

    delay(6000);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.println("Initializing modem...");
    modem.restart();
    // modem.init();

    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

    String name = modem.getModemName();
    DBG("Modem Name:", name);
    String ccid = modem.getSimCCID();
    DBG("CCID:", ccid);
    String imei = modem.getIMEI();
    DBG("IMEI:", imei);
  
#if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if ( GSM_PIN && modem.getSimStatus() != 3 ) {
        modem.simUnlock(GSM_PIN);
    }
#endif

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected()) {
        SerialMon.println("Network connected");
    }

    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected()) {
        SerialMon.println("GPRS connected");
    }

    String cop = modem.getOperator();
    DBG("Operator:", cop);
    IPAddress local = modem.localIP();
    DBG("Local IP:", local);
    int csq = modem.getSignalQuality();
    DBG("Signal quality:", csq);


    // MQTT Broker setup
    mqtt.setServer(broker, 184);
    mqtt.setCallback(mqttCallback);

    String gsmdate = modem.getGSMDateTime(DATE_DATE);
    String gsmtime = modem.getGSMDateTime(DATE_TIME);
    DBG("DATETIME   :", gsmdate);
    String SignalString = String(csq);
    String TimeString = gsmdate+","+gsmtime;

    deviceInfo = String(TimeString + "?" + modemInfo + "?" + name + "?" + ccid + "?" + imei + "?" + cop + "?" + local + "?" + SignalString + "?" + 44 );
    deviceInfo.toCharArray(deviceInfoString,850);
    SerialMon.println(deviceInfoString);

    SerialMon.println();
    SerialMon.println("================Loop MqttSIM800=================");
    if (!mqtt.connected()) {
        SerialMon.println("=== MQTT NOT CONNECTED ===");
        reconnect();
        }
    delay(100);
    mqtt.loop();
    mqtt.publish("/reqSetting/all", deviceInfoString);

}

//========================================================================================
//WIFISetup
//void Wifi()
//{
//  // Connect to WiFi network
//  WiFi.begin(ssid, password);
//  Serial.println("");
//
//  // Wait for connection
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//  Serial.println("");
//  Serial.print("Connected to ");
//  Serial.println(ssid);
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());
//}
//========================================================================================
//Ota Setup

//void OtaSetup()
//{
///*use mdns for host name resolution*/
//  if (!MDNS.begin(host)) { //http://esp32.local
//    Serial.println("Error setting up MDNS responder!");
//    while (1) {
//      delay(1000);
//    }
//  }
//  Serial.println("mDNS responder started");
//  /*return index page which is stored in serverIndex */
//  server.on("/", HTTP_GET, []() {
//    server.sendHeader("Connection", "close");
//    server.send(200, "text/html", loginIndex);
//  });
//  server.on("/serverIndex", HTTP_GET, []() {
//    server.sendHeader("Connection", "close");
//    server.send(200, "text/html", serverIndex);
//  });
//  /*handling uploading firmware file */
//  server.on("/update", HTTP_POST, []() {
//    server.sendHeader("Connection", "close");
//    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
//    ESP.restart();
//  }, []() {
//    HTTPUpload& upload = server.upload();
//    if (upload.status == UPLOAD_FILE_START) {
//      Serial.printf("Update: %s\n", upload.filename.c_str());
//      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
//        Update.printError(Serial);
//      }
//    } else if (upload.status == UPLOAD_FILE_WRITE) {
//      /* flashing firmware to ESP*/
//      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//        Update.printError(Serial);
//      }
//    } else if (upload.status == UPLOAD_FILE_END) {
//      if (Update.end(true)) { //true to set the size to the current progress
//        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//      } else {
//        Update.printError(Serial);
//      }
//    }
//  });
//  server.begin();
//}
//========================================================================================
//Sensor Setup
void SensorSetup()
{
  SerialMon.println();
  SerialMon.println("==================================Sensor Setup=================================");
  Serial.println("DHT22 sensor run");
  //Initialize the DHT sensor
  dht.begin();  //sensor suhu dan kelembapan 1 dht22
}
//========================================================================================
void setup()
{
  Serial.begin(115200);
//  Wifi();
  SensorSetup();
//  OtaSetup();
//  GpsSetup();
//  LoraSetup();
  MqttSIM800setup();
}
//========================================================================================
//void OtaLoop()
//{
//server.handleClient();
// delay(1);
//}
//========================================================================================
//SEnsor Loop
void SensorRead(){
  SerialMon.println();
  SerialMon.println("================sensor Read=================");
  
//Sensor Level1=========================
//DigitalLevel1=analogRead(PinLevel1);
//ValueLevel1= (((DigitalLevel1*3.3/4096)/Resistor1)-0.004)*MaxSensorLevel1/16;
//Senosr temperatur1 humiditi1 dht22_1==================
//2000mS delay between reads
    delay(2200);
float converted = 0.00;
    //Read data and store it to variables hum and temp
    ValueHumidity1 = dht.readHumidity();
    ValueTemperatur1= dht.readTemperature();

    Serial.print("Celsius = ");
    Serial.print(ValueTemperatur1);
    //Print degree symbol
    Serial.write(176); 
    Serial.println("C");

//    //Fahrenheit
//    //T(°F) = T(°C) × 9/5 + 32
//    converted = ( ValueTemperatur1 * 1.8 ) + 32;
//    Serial.print("Fahrenheit = ");
//    Serial.print(converted);
//    //Print degree symbol
//    Serial.write(176); 
//    Serial.println("F");

//    //Kelvin
//    //T(K) = T(°C) + 273.15          
//    converted = ValueTemperatur1 + 273.15;
//    Serial.print("Kelvin = ");
//    Serial.print(converted);
//    Serial.println(" K");

//    //Rankine
//    //T(°R) = (T(°C) + 273.15) × 9/5          
//    converted = ValueTemperatur1 + 273.15;
//    converted = (converted * 1.8);
//    Serial.print("Rankin = ");
//    Serial.print(converted);
//    //Print degree symbol
//    Serial.write(176);    
//    Serial.println("R");

    Serial.print("Humidity =");
    Serial.println(ValueHumidity1);

}
//========================================================================================

//void GpsLoop()
//{
//  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
//  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
//  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
//  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
//
//  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
//  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
//  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
//  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
//  printInt(gps.location.age(), gps.location.isValid(), 5);
//  printDateTime(gps.date, gps.time);
//  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
//  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
//  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
//  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);
//
//  unsigned long distanceKmToLondon =
//    (unsigned long)TinyGPSPlus::distanceBetween(
//      gps.location.lat(),
//      gps.location.lng(),
//      LONDON_LAT, 
//      LONDON_LON) / 1000;
//  printInt(distanceKmToLondon, gps.location.isValid(), 9);
//
//  double courseToLondon =
//    TinyGPSPlus::courseTo(
//      gps.location.lat(),
//      gps.location.lng(),
//      LONDON_LAT, 
//      LONDON_LON);
//
//  printFloat(courseToLondon, gps.location.isValid(), 7, 2);
//
//  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);
//
//  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);
//
//  printInt(gps.charsProcessed(), true, 6);
//  printInt(gps.sentencesWithFix(), true, 10);
//  printInt(gps.failedChecksum(), true, 9);
//  Serial.println();
//  Serial.println();
//  smartDelay(1000);
//
//  if (millis() > 5000 && gps.charsProcessed() < 10)
//    Serial.println(F("No GPS data received: check wiring"));
//    
//}

//========================================================================================
void JsonLoop()
{
    SerialMon.println();
   SerialMon.println("================Json Loop=================");
  //Variabel Device Parameter
    int csq = modem.getSignalQuality();
    DBG("Signal quality:", csq);
    String gsmdate = modem.getGSMDateTime(DATE_DATE);
    String gsmtime = modem.getGSMDateTime(DATE_TIME);
    DBG("DATETIME   :", gsmdate);
//Variabel Sensor
  StaticJsonDocument<100> temperature1;
  temperature1["sensorid"] = "33-1-temperature";
  temperature1["value"] = ValueTemperatur1;
  temperature1["context"] = "";
//   StaticJsonDocument<100> temperature2;
//  temperature2["sensorid"] = "33-2-temperature";
//  temperature2["value"] = 45;
//  temperature2["context"] = "";
  StaticJsonDocument<100> humidity1;
  humidity1["sensorid"] = "33-1-humidity";
  humidity1["value"] = ValueHumidity1;
  humidity1["context"] = "";
//  StaticJsonDocument<100> humidity2;
//  humidity2["sensorid"] = "33-2-humidity";
//  humidity2["value"] = 56;
//  humidity2["context"] = "";
//  StaticJsonDocument<100> level1;
//  level1["sensorid"] = "33-1-level";
//  level1["value"] = ValueLevel1;
//  level1["context"] = "";
//  StaticJsonDocument<100> level2;
//  level2["sensorid"] = "33-2-level";
//  level2["value"] = 6;
//  level2["context"] = "";
//  StaticJsonDocument<100> CO1;
//  CO1["sensorid"] = "33-1-CO";
//  CO1["value"] = 23;
//  CO1["context"] = "";
//  StaticJsonDocument<100> NH31;
//  NH31["sensorid"] = "33-1-NH3";
//  NH31["value"] = 23;
//  NH31["context"] = "";
//  StaticJsonDocument<100> NO21;
//  NO21["sensorid"] = "33-1-NO2";
//  NO21["value"] = 23;
//  NO21["context"] = "";

//Nested Group Sensor
  StaticJsonDocument<400> sensor;
  JsonArray temperature = sensor.createNestedArray("temperature");
  temperature.add(temperature1);
//  temperature.add(temperature2);
  JsonArray humidity = sensor.createNestedArray("humidity");
  humidity.add(humidity1);
//  humidity.add(humidity2);
//  JsonArray level = sensor.createNestedArray("level");
//  level.add(level1);
//  level.add(level2);
//  JsonArray gas = sensor.createNestedArray("gas");
//  gas.add(CO1);
//  gas.add(NH31);
//  gas.add(NO21);

//Variabel GPS
// StaticJsonDocument<200> location;
//  location["lat"] = gps.location.lat();
//  location["long"] = gps.location.lng();
//  location["altitude"] = gps.altitude.meters();

//JsonLora Integrated
//  DynamicJsonDocument jsonnodeLora(700);
////  jsonnodeLora["counter"] = count;
//   jsonnodeLora["nodeid"] = 0;
//  jsonnodeLora["signal"] = 0;
//  jsonnodeLora["battery"] = 0.4;
//  jsonnodeLora["context"] = "";
//  jsonnodeLora["location"] = location;
//jsonnodeLora["sensor"] = sensor;
//jsonnodeLora["time"] = count;



//JsonLora Integrated
  DynamicJsonDocument jsonnodeGSM800(700);
  jsonnodeGSM800["counter"] = count;
  jsonnodeGSM800["nodeid"] = 1;
  jsonnodeGSM800["signal"] = 99;
  jsonnodeGSM800["battery"] = 0.4;
  jsonnodeGSM800["context"] = "";
//  jsonnodeGSM800["location"] = location;
  jsonnodeGSM800["sensor"] = sensor;
  jsonnodeGSM800["time"] = count;

  String CountString = String(count);
  String SignalString = String(csq);
  String TimeString = gsmdate+","+gsmtime;
  String Temperatur1String = String(ValueTemperatur1);
  String Humidity1String = String(ValueHumidity1);
  

   alldata = String(TimeString + "?" + CountString + "?" +Temperatur1String + "?" + Humidity1String + "?" + SignalString + "?" + 44 );
   Serial.print("alldata  =   ");
   Serial.println(alldata);

// {"nodeid":33,"signal":12,"battery":0.4,"context":"","location":{"lat":-7.293284,"long":112.8111,"altitude":17},"sensor":{"temperature":[{"sensorid":"33-1-temperature","value":25,"context":""},{"sensorid":"33-2-temperature","value":45,"context":""}],"level":[{"sensorid":"33-1-level","value":10,"context":""},{"sensorid":"33-2-level","value":6,"context":""}],"gas":[{"sensorid":"33-1-CO","value":23,"context":""},{"sensorid":"33-1-NH3"}]}}

//  Json Integrated To String
// char jsonnodeLoraString[850];
// serializeJson(jsonnodeLora, jsonnodeLoraString);
// serializeJson(alldata, alldatastring);
alldata.toCharArray(alldatastring,850);
// Serial.println();
// Serial.println(jsonnodeLoraString);
 Serial.println();
 delay(100);
  
}

//========================================================================================
//void LoraLoop()
//{
//  new_time=millis();
//    if (new_time - old_time >= 6000 || old_time == 0)
//    {
//      old_time = new_time;
//      Serial.print("###########    ");
//      Serial.print("COUNT=");
//      Serial.print(count);
//      Serial.println("    ###########");
//      count++;
//      // Gest Sinyal Strenght Lora
//      rssi = LoRa.packetRssi();
////integrated data Json, node ID To Dragino format 
//     
////  Send Data
//     LoRa.beginPacket();
////     LoRa.print((char *)datasend);
//     LoRa.print(node_id); //Header Dragino nodeid
////     LoRa.print(count); 
//     LoRa.print(jsonnodeLoraString); 
////     LoRa.print("{\"nodeid\":50}");
////     LoRa.print("{\"nodeid\":50,\"signal\":12,\"battery\":0.4,\"context\":\"\",\"location\":{\"lat\":-7.285511,\"long\":112.7895,\"altitude\":4},\"sensor\":{\"temperature\":[{\"sensorid\":\"50-1-temperature\",\"value\":25,\"context\":\"\"},{\"sensorid\":\"50-2-temperature\",\"value\":45,\"context\":\"\"}],\"level\":[{\"sensorid\":\"50-1-level\",\"value\":5,\"context\":\"\"},{\"sensorid\":\"50-2-level\",\"value\":6,\"context\":\"\"}]}}");
////     LoRa.print("{\"nodeid\":50,\"signal\":12,\"battery\":0.4,\"context\":\"\",\"location\":{\"lat\":-7.285511,\"long\":112.7895,\"altitude\":4}");
////     LoRa.print(",\"sensor\":{\"temperature\":[{\"sensorid\":\"50-1-temperature\",\"value\":25,\"context\":\"\"},{\"sensorid\":\"50-2-temperature");  
////     LoRa.print(",\"value\":45,\"context\":\"\"}],\"level\":[{\"sensorid\":\"50-1-level\",\"value\":5,\"context\":\"\"},{\"sensorid\":\"50-2-level\",\"value\":6,");
//   
//     LoRa.endPacket();
//     Serial.println("Packet Sent");
//     
////     Lora Receive
//     LoRa.receive();
//     delay(100);
//    }
//}   

//MqttSIM800Loop
//========================================================================================
void MqttSIM800loop()
{
    SerialMon.println();
  SerialMon.println("================Loop MqttSIM800=================");
    if (!mqtt.connected()) {
        SerialMon.println("=== MQTT NOT CONNECTED ===");
        reconnect();
        // Reconnect every 10 seconds
//        uint32_t t = millis();
//        if (t - lastReconnectAttempt > 1000L) {
//            lastReconnectAttempt = t;
//            if (mqttConnect()) {
//            if (mqttConnect()) {
//                lastReconnectAttempt = 0;
//            }
//        }
//        delay(100);
//        return;
    }
    
    delay(100);
    
//    mqtt.publish("data/el0001", jsonnodeGSM800String);
//    mqtt.publish("/data/el0001", alldatastring);
//    delay(100);
    mqtt.loop();

    if ((ValueTemperatur1 <= tminT )||(ValueTemperatur1 >= tmaxT)||(ValueHumidity1 <= tminH)||(ValueHumidity1>=tmaxH)){
    mqtt.publish("/data/el0001", alldatastring);
    }

    long now = millis();
  if (now - lastMsg > delta * 1000) {
    lastMsg = now;
    mqtt.publish("/data/el0001", alldatastring);
    count++;
  }
//    mqtt.subscribe("/setting/el0001/#");
//    delay(100);
//    mqtt.subscribe("/reqCurrentData/el0001");
//    delay(100);
//    if (skipp >= delta){
//    skipp=0;
//    // LoraLoop();
//    // MqttSIM800loop();
//    mqtt.publish("/data/el0001", alldatastring);
//    delay(100);
//   }
    
    delay(100);
}
//========================================================================================
//mqttSim800Loop Subscribe
//void MqttSubSIM800loop()
//{
//
//    if (!mqtt.connected()) {
//        SerialMon.println("=== MQTT NOT CONNECTED ===");
//        // Reconnect every 10 seconds
//        uint32_t t = millis();
//        if (t - lastReconnectAttempt > 10000L) {
//            lastReconnectAttempt = t;
//            if (mqttConnect()) {
//                lastReconnectAttempt = 0;
//            }
//        }
//        delay(100);
//        return;
//    }
//    delay(100);
//    count++;
////    mqtt.publish("data/el0001", jsonnodeGSM800String);
////    mqtt.publish("data/el0001", alldatastring);
//    mqtt.subscribe("/setting/el0001/#");
//    mqtt.subscribe("/reqCurrentData/el0001");
//    delay(100);
//    mqtt.loop();
//    
//    delay(100);
//}

//========================================================================================
void loop()
{
  SerialMon.println();
  SerialMon.println("==================================loop header=================================");
 ++skipp;
// MqttSubSIM800loop();
// OtaLoop();
 SensorRead();
// GpsLoop();
 JsonLoop();
 MqttSIM800loop();
// if (skipp >= delta){
//  skipp=0;
//// LoraLoop();
// MqttSIM800loop();
// }
 delay(100);
}

//========================================================================================

// This custom version of delay() ensures that the gps object
// is being "fed".
//static void smartDelay(unsigned long ms) //GPS
//{
//  unsigned long start = millis();
//  do 
//  {
//    while (ss.available())
//      gps.encode(ss.read());
//  } while (millis() - start < ms);
//}

//========================================================================================
//static void printFloat(float val, bool valid, int len, int prec)  //GPS
//{
//  if (!valid)
//  {
//    while (len-- > 1)
//      Serial.print('*');
//    Serial.print(' ');
//  }
//  else
//  {
//    Serial.print(val, prec);
//    int vi = abs((int)val);
//    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
//    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
//    for (int i=flen; i<len; ++i)
//      Serial.print(' ');
//  }
//  smartDelay(0);
//}

//========================================================================================
//static void printInt(unsigned long val, bool valid, int len)// GPS
//{
//  char sz[32] = "*****************";
//  if (valid)
//    sprintf(sz, "%ld", val);
//  sz[len] = 0;
//  for (int i=strlen(sz); i<len; ++i)
//    sz[i] = ' ';
//  if (len > 0) 
//    sz[len-1] = ' ';
//  Serial.print(sz);
//  smartDelay(0);
//}

//========================================================================================
//static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)  //GPS
//{
//  if (!d.isValid())
//  {
//    Serial.print(F("********** "));
//  }
//  else
//  {
//    char sz[32];
//    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
//    Serial.print(sz);
//  }
//  
//  if (!t.isValid())
//  {
//    Serial.print(F("******** "));
//  }
//  else
//  {
//    char sz[32];
//    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
//    Serial.print(sz);
//  }
//
//  printInt(d.age(), d.isValid(), 5);
//  smartDelay(0);
//}

//========================================================================================
//static void printStr(const char *str, int len) //GPS
//{
//  int slen = strlen(str);
//  for (int i=0; i<len; ++i)
//    Serial.print(i<slen ? str[i] : ' ');
//  smartDelay(0);
//}

//========================================================================================
//void onReceive(int packetSize) //Lora
//{
//  // received a packet
//  Serial.print("Received packet : ");
//
//  // read packet
//  for (int i = 0; i < packetSize; i++) {
//      Serial.print((char)LoRa.read());
//  }
//  Serial.print("\n\r");  
//}
