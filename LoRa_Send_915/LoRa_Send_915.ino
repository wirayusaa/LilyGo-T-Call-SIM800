#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
//#include "SSD1306.h" 
//#include "images.h"
#include <TinyGPS++.h>                       

//#define SCK     14    // GPIO5  -- SX1278's SCK
//#define MISO    12   // GPIO19 -- SX1278's MISO
//#define MOSI    13   // GPIO27 -- SX1278's MOSI
//#define SS      15   // GPIO18 -- SX1278's CS
//#define RST     2   // GPIO14 -- SX1278's RESET
//#define DI0     19   // GPIO26 -- SX1278's IRQ(Interrupt Request)
//#define BAND    915E6

#define SCK     25    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISnO
#define MOSI    12   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     15   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND  915E6


TinyGPSPlus gps;                            
HardwareSerial Seri(2);  
unsigned int counter = 0;

//SSD1306 display(0x3c, 21, 22);
String rssi = "RSSI --";
String packSize = "--";
String packet ;

 

void setup() {
  pinMode(16,OUTPUT);
  pinMode(2,OUTPUT);
  
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  Serial.begin(115200);
  Seri.begin(9600, SERIAL_8N1, 0, 2);   //17-TX 18-RX
   while (!Serial);
  Serial.println();
  Serial.println("LoRa Sender Test");
  
  
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.onReceive(cbk);
//  LoRa.receive();
  Serial.println("init ok");
//  display.init();
//  display.flipScreenVertically();  
//  display.setFont(ArialMT_Plain_10);
   
  delay(1500);
}

void loop() {
//  display.clear();
  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.setFont(ArialMT_Plain_10);
  
 // display.drawString(0, 0, "Sending packet: ");
 // display.drawString(90, 0, String(counter));
  Serial.println(String(counter));
 // display.display();

  // send packet
  LoRa.beginPacket();
  //LoRa.print("hello ");
  LoRa.println(counter);
  LoRa.print("Latitude  : ");
  LoRa.println(gps.location.lat(), 8);
  LoRa.print("Longitude : ");
  LoRa.println(gps.location.lng(), 7);
  LoRa.print("Satellites: ");
  LoRa.println(gps.satellites.value());
  LoRa.print("Altitude  : ");
  LoRa.print(gps.altitude.feet() / 3.2808);
  LoRa.println("M");
  LoRa.print("Time      : ");
  LoRa.print(gps.time.hour());
  LoRa.print(":");
  LoRa.print(gps.time.minute());
  LoRa.print(":");
  LoRa.println(gps.time.second());
  LoRa.println("T-Cell");
  LoRa.println("**********************");

  LoRa.endPacket();
  smartDelay(1000);                                      

  if (millis() > 5000 && gps.charsProcessed() < 10)
  Serial.println(F("No GPS data received: check wiring"));
  
  counter++;
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(5000);                       // wait for a second
}

static void smartDelay(unsigned long ms)                
{
  unsigned long start = millis();
  do
  {
    while (Seri.available())
      gps.encode(Seri.read());
  } while (millis() - start < ms);
}
