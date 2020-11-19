#include <TinyGPS++.h>

/*****************************************
* ESP32 GPS VKEL 9600 Bds
******************************************/
#include <ArduinoJson.h>
                     

TinyGPSPlus gps;                            
HardwareSerial Seri(2);                 

void setup()
{
  Serial.begin(115200);
  Seri.begin(9600, SERIAL_8N1, 0, 2);   //17-TX 18-RX
  while (!Serial);
  delay(1000);
}

void loop()
{
//Serial.println(gps.location.lat(), 6); // Latitude in degrees (double)
//Serial.println(gps.location.lng(), 6); // Longitude in degrees (double)
//Serial.print(gps.location.rawLat().negative ? "-" : "+");
//Serial.println(gps.location.rawLat().deg); // Raw latitude in whole degrees
//Serial.println(gps.location.rawLat().billionths);// ... and billionths (u16/u32)
//Serial.print(gps.location.rawLng().negative ? "-" : "+");
//Serial.println(gps.location.rawLng().deg); // Raw longitude in whole degrees
//Serial.println(gps.location.rawLng().billionths);// ... and billionths (u16/u32)
//Serial.println(gps.date.value()); // Raw date in DDMMYY format (u32)
//Serial.println(gps.date.year()); // Year (2000+) (u16)
//Serial.println(gps.date.month()); // Month (1-12) (u8)
//Serial.println(gps.date.day()); // Day (1-31) (u8)
//Serial.println(gps.time.value()); // Raw time in HHMMSSCC format (u32)
//Serial.println(gps.time.hour()); // Hour (0-23) (u8)
//Serial.println(gps.time.minute()); // Minute (0-59) (u8)
//Serial.println(gps.time.second()); // Second (0-59) (u8)
//Serial.println(gps.time.centisecond()); // 100ths of a second (0-99) (u8)
//Serial.println(gps.speed.value()); // Raw speed in 100ths of a knot (i32)
//Serial.println(gps.speed.knots()); // Speed in knots (double)
//Serial.println(gps.speed.mph()); // Speed in miles per hour (double)
//Serial.println(gps.speed.mps()); // Speed in meters per second (double)
//Serial.println(gps.speed.kmph()); // Speed in kilometers per hour (double)
//Serial.println(gps.course.value()); // Raw course in 100ths of a degree (i32)
//Serial.println(gps.course.deg()); // Course in degrees (double)
//Serial.println(gps.altitude.value()); // Raw altitude in centimeters (i32)
//Serial.println(gps.altitude.meters()); // Altitude in meters (double)
//Serial.println(gps.altitude.miles()); // Altitude in miles (double)
//Serial.println(gps.altitude.kilometers()); // Altitude in kilometers (double)
//Serial.println(gps.altitude.feet()); // Altitude in feet (double)
//Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
//Serial.println(gps.hdop.value()); // Horizontal Dim. of Precision (100ths-i32)

  Serial.print("Latitude  : ");
  Serial.println(gps.location.lat(), 9);
  Serial.print("Longitude : ");
  Serial.println(gps.location.lng(), 9);
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("Altitude  : ");
  Serial.print(gps.altitude.feet() / 3.2808);
  Serial.println("M");
  Serial.print("Date Time : ");
  printDateTime(gps.date, gps.time);
  Serial.println("**********************");

  smartDelay(1000);                                      

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
//
//  StaticJsonDocument<200> location;
//  location["lat"] = gps.location.lat();
//  location["long"] = gps.location.lng();
//  location["altitude"] = gps.altitude.feet() / 3.2808;
//
//  DynamicJsonDocument jsonnode(700);
//  jsonnode["nodeid"] = 33;
//  jsonnode["signal"] = 12;
//  jsonnode["battery"] = 0.4;
//  jsonnode["context"] = "";
//  jsonnode["location"] = location;
//  jsonnode["Time"] = 1593138620;
//    
//StaticJsonBuffer<200> jsonBuffer;
//  JsonObject& root = jsonBuffer.createObject();
//  JsonArray& data = root.createNestedArray("coordinates");
//  root["sensor"] = "gps";
//  root["timestamps"] = "somerandomnumber";
//  root["depth"] =  gps.altitude.feet() / 3.2808;
//    data.add(gps.location.lat(), 5);
//  data.add(gps.location.lng(), 4);

delay(5000);
}

//static void printInt(unsigned long val, bool valid, int len)
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
    Serial.println(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", 7+t.hour(), t.minute(), t.second());
    Serial.println(sz);
  }

//  printInt(d.age(), d.isValid(), 12);
  smartDelay(0);
}

static void smartDelay(unsigned long ms)                
{
  unsigned long start = millis();
  do
  {
    while (Seri.available())
      gps.encode(Seri.read());
//      Serial.println("Serial GPS");
//      Serial.print(gps.encode(Seri.read()));
  } while (millis() - start < ms);
}
