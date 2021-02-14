#include <TinyGPS++.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <LoRa.h>


//========================================================================================
// The serial connection to the GPS device
#define ss Serial2

//GPS Variable
static const int RXPin = 2, TXPin = 0;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

//========================================================================================
// LORA XL1278 pin definitions
#define SCK     25    // GPIO25  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISnO
#define MOSI    12   // GPIO12 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     15   // GPIO12 -- SX1278's IRQ(Interrupt Request)
#define BAND  915E6
int rssi = 0;
char *node_id = "<5679>";  //From LG01 via web Local Channel settings on MQTT.Please refer <> dataformat in here. 
char count_1 [8];
unsigned long new_time,old_time=0;

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

// set GSM PIN, if any
#define GSM_PIN ""

char jsonnodeGSM800String[850];

// Your GPRS credentials, if any
const char apn[] = "AXIS";
const char gprsUser[] = "axis";
const char gprsPass[] = "123456";
//const char apn[] = "internet";
//const char gprsUser[] = "";
//const char gprsPass[] = "";


// MQTT details
const char *broker = "trace.elefante.co.id";

const char *topicLed = "GsmClientTest/led";
const char *topicInit = "GsmClientTest/init";
const char *topicLedStatus = "GsmClientTest/ledStatus";

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

void LoraSetup()
{
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(915E6))   //868000000 is frequency
      { 
          Serial.println("Starting LoRa failed!");
          while (1);
      }
    // Setup Spreading Factor (6 ~ 12)
   LoRa.setSpreadingFactor(7);
   // Setup BandWidth, option: 7800,10400,15600,20800,31250,41700,62500,125000,250000,500000
   //Lower BandWidth for longer distance.
   LoRa.setSignalBandwidth(125000);  
   // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
   LoRa.setCodingRate4(5);
   LoRa.setSyncWord(0x34); 
   Serial.println("LoRa init succeeded.");
   //Serial.println(frequency);
   LoRa.onReceive(onReceive);   
   LoRa.receive();     
}

//========================================================================================
void GpsSetup()
{
  
  ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);

  Serial.println(F("Full Setup GPS DATA"));
//  Serial.println(F("An extensive example of many interesting TinyGPS++ features"));
//  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
//  Serial.println(F("by Mikal Hart"));
//  Serial.println();
//  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
//  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
//  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
}






//MqttSIM800setup
//========================================================================================
void mqttCallback(char *topic, byte *payload, unsigned int len)
{
    SerialMon.print("Message arrived [");
    SerialMon.print(topic);
    SerialMon.print("]: ");
    SerialMon.write(payload, len);
    SerialMon.println();

    // Only proceed if incoming message's topic matches
    if (String(topic) == topicLed) {
        ledStatus = !ledStatus;
        digitalWrite(LED_GPIO, ledStatus);
        mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
    }
}
//=========================================
boolean mqttConnect()
{
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
    SerialMon.println(" success");
    //mqtt.publish(topicInit, "GsmClientTest started");
     mqtt.publish("tracking/33", jsonnodeGSM800String);
    mqtt.subscribe("tracking/33");
    return mqtt.connected();
}
//=========================================
void MqttSIM800setup()
{   
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

    // MQTT Broker setup
    mqtt.setServer(broker, 185);
    mqtt.setCallback(mqttCallback);
}

//========================================================================================
void setup()
{
  Serial.begin(115200);
  GpsSetup();
  LoraSetup();
  MqttSIM800setup();
}

//========================================================================================
void GpsLoop()
{
  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

  unsigned long distanceKmToLondon =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT, 
      LONDON_LON) / 1000;
  printInt(distanceKmToLondon, gps.location.isValid(), 9);

  double courseToLondon =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT, 
      LONDON_LON);

  printFloat(courseToLondon, gps.location.isValid(), 7, 2);

  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  Serial.println();
  Serial.println();
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
    
}

//========================================================================================
void JsonLoop()
{
//Variabel Sensor
  StaticJsonDocument<100> temperature1;
  temperature1["sensorid"] = "33-1-temperature";
  temperature1["value"] = 25;
  temperature1["context"] = "";
   StaticJsonDocument<100> temperature2;
  temperature2["sensorid"] = "33-2-temperature";
  temperature2["value"] = 45;
  temperature2["context"] = "";
  StaticJsonDocument<100> level1;
  level1["sensorid"] = "33-1-level";
  level1["value"] = 10;
  level1["context"] = "";
  StaticJsonDocument<100> level2;
  level2["sensorid"] = "33-2-level";
  level2["value"] = 6;
  level2["context"] = "";
  StaticJsonDocument<100> CO1;
  CO1["sensorid"] = "33-1-CO";
  CO1["value"] = 23;
  CO1["context"] = "";
  StaticJsonDocument<100> NH31;
  NH31["sensorid"] = "33-1-NH3";
  NH31["value"] = 23;
  NH31["context"] = "";
  StaticJsonDocument<100> NO21;
  NO21["sensorid"] = "33-1-NO2";
  NO21["value"] = 23;
  NO21["context"] = "";

//Nested Group Sensor
  StaticJsonDocument<400> sensor;
  JsonArray temperature = sensor.createNestedArray("temperature");
  temperature.add(temperature1);
  temperature.add(temperature2);
  JsonArray level = sensor.createNestedArray("level");
  level.add(level1);
  level.add(level2);
  JsonArray gas = sensor.createNestedArray("gas");
  gas.add(CO1);
  gas.add(NH31);
  gas.add(NO21);

//Variabel GPS
 StaticJsonDocument<200> location;
  location["lat"] = gps.location.lat();
  location["long"] = gps.location.lng();
  location["altitude"] = gps.altitude.meters();

//JsonLora Integrated
  DynamicJsonDocument jsonnodeLora(700);
//  jsonnodeLora["counter"] = count;
   jsonnodeLora["nodeid"] = 0;
  jsonnodeLora["signal"] = 0;
  jsonnodeLora["battery"] = 0.4;
  jsonnodeLora["context"] = "";
  jsonnodeLora["location"] = location;
//jsonnodeLora["sensor"] = sensor;
jsonnodeLora["time"] = count;



//JsonLora Integrated
  DynamicJsonDocument jsonnodeGSM800(700);
  jsonnodeGSM800["counter"] = count;
  jsonnodeGSM800["nodeid"] = 1;
  jsonnodeGSM800["signal"] = rssi;
  jsonnodeGSM800["battery"] = 0.4;
  jsonnodeGSM800["context"] = "";
  jsonnodeGSM800["location"] = location;
  jsonnodeGSM800["sensor"] = sensor;
  jsonnodeGSM800["time"] = count;

// {"nodeid":33,"signal":12,"battery":0.4,"context":"","location":{"lat":-7.293284,"long":112.8111,"altitude":17},"sensor":{"temperature":[{"sensorid":"33-1-temperature","value":25,"context":""},{"sensorid":"33-2-temperature","value":45,"context":""}],"level":[{"sensorid":"33-1-level","value":10,"context":""},{"sensorid":"33-2-level","value":6,"context":""}],"gas":[{"sensorid":"33-1-CO","value":23,"context":""},{"sensorid":"33-1-NH3"}]}}

//  Json Integrated To String
// char jsonnodeLoraString[850];
 serializeJson(jsonnodeLora, jsonnodeLoraString);
 serializeJson(jsonnodeGSM800, jsonnodeGSM800String);
 Serial.println();
 Serial.println(jsonnodeLoraString);
 Serial.println();
 delay(100);
  
}

//========================================================================================
void LoraLoop()
{
  new_time=millis();
    if (new_time - old_time >= 6000 || old_time == 0)
    {
      old_time = new_time;
      Serial.print("###########    ");
      Serial.print("COUNT=");
      Serial.print(count);
      Serial.println("    ###########");
      count++;
      // Gest Sinyal Strenght Lora
      rssi = LoRa.packetRssi();
//integrated data Json, node ID To Dragino format 
     
//  Send Data
     LoRa.beginPacket();
//     LoRa.print((char *)datasend);
     LoRa.print(node_id); //Header Dragino nodeid
//     LoRa.print(count); 
     LoRa.print(jsonnodeLoraString); 
//     LoRa.print("{\"nodeid\":50}");
//     LoRa.print("{\"nodeid\":50,\"signal\":12,\"battery\":0.4,\"context\":\"\",\"location\":{\"lat\":-7.285511,\"long\":112.7895,\"altitude\":4},\"sensor\":{\"temperature\":[{\"sensorid\":\"50-1-temperature\",\"value\":25,\"context\":\"\"},{\"sensorid\":\"50-2-temperature\",\"value\":45,\"context\":\"\"}],\"level\":[{\"sensorid\":\"50-1-level\",\"value\":5,\"context\":\"\"},{\"sensorid\":\"50-2-level\",\"value\":6,\"context\":\"\"}]}}");
//     LoRa.print("{\"nodeid\":50,\"signal\":12,\"battery\":0.4,\"context\":\"\",\"location\":{\"lat\":-7.285511,\"long\":112.7895,\"altitude\":4}");
//     LoRa.print(",\"sensor\":{\"temperature\":[{\"sensorid\":\"50-1-temperature\",\"value\":25,\"context\":\"\"},{\"sensorid\":\"50-2-temperature");  
//     LoRa.print(",\"value\":45,\"context\":\"\"}],\"level\":[{\"sensorid\":\"50-1-level\",\"value\":5,\"context\":\"\"},{\"sensorid\":\"50-2-level\",\"value\":6,");
   
     LoRa.endPacket();
     Serial.println("Packet Sent");
     
//     Lora Receive
     LoRa.receive();
     delay(100);
    }
}   

//MqttSIM800Loop
//========================================================================================
void MqttSIM800loop()
{

    if (!mqtt.connected()) {
        SerialMon.println("=== MQTT NOT CONNECTED ===");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - lastReconnectAttempt > 10000L) {
            lastReconnectAttempt = t;
            if (mqttConnect()) {
                lastReconnectAttempt = 0;
            }
        }
        delay(100);
        return;
    }
    delay(1000);
    mqtt.publish("tracking/33", jsonnodeGSM800String);
    delay(1000);
    mqtt.loop();
    
    delay(100);
}
//========================================================================================

void loop()
{
 GpsLoop();
 JsonLoop();
 LoraLoop();
 MqttSIM800loop();
 delay(300000);
}

//========================================================================================

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) //GPS
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

//========================================================================================
static void printFloat(float val, bool valid, int len, int prec)  //GPS
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

//========================================================================================
static void printInt(unsigned long val, bool valid, int len)// GPS
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

//========================================================================================
static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)  //GPS
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

//========================================================================================
static void printStr(const char *str, int len) //GPS
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}

//========================================================================================
void onReceive(int packetSize) //Lora
{
  // received a packet
  Serial.print("Received packet : ");

  // read packet
  for (int i = 0; i < packetSize; i++) {
      Serial.print((char)LoRa.read());
  }
  Serial.print("\n\r");  
}
