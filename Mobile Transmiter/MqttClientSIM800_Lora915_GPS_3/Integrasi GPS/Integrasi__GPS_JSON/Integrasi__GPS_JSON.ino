#include <TinyGPS++.h>
#include <ArduinoJson.h>

// The serial connection to the GPS device
#define ss Serial2

//GPS Variable
static const int RXPin = 2, TXPin = 0;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;



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
void setup()
{
  Serial.begin(115200);
  GpsSetup();
}

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

//Json Integrated
  DynamicJsonDocument jsonnode(700);
  jsonnode["nodeid"] = 33;
  jsonnode["signal"] = 12;
  jsonnode["battery"] = 0.4;
  jsonnode["context"] = "";
  jsonnode["location"] = location;
  jsonnode["sensor"] = sensor;

// {"nodeid":33,"signal":12,"battery":0.4,"context":"","location":{"lat":-7.293284,"long":112.8111,"altitude":17},"sensor":{"temperature":[{"sensorid":"33-1-temperature","value":25,"context":""},{"sensorid":"33-2-temperature","value":45,"context":""}],"level":[{"sensorid":"33-1-level","value":10,"context":""},{"sensorid":"33-2-level","value":6,"context":""}],"gas":[{"sensorid":"33-1-CO","value":23,"context":""},{"sensorid":"33-1-NH3"}]}}

//  Json Integrated To String
 char jsonstring[850];
 serializeJson(jsonnode, jsonstring);
 Serial.println(jsonstring);
 Serial.println();
 delay(100);
  
}

void loop()
{
 GpsLoop();
 JsonLoop();
 delay(1000);
}


// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
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

static void printInt(unsigned long val, bool valid, int len)
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

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
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

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}
