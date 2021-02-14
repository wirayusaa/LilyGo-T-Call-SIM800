#include <SPI.h>
#include <LoRa.h>

// LORA XL1278 pin definitions
#define SCK     25    // GPIO25  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISnO
#define MOSI    12   // GPIO12 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     15   // GPIO12 -- SX1278's IRQ(Interrupt Request)
#define BAND  915E6

float  tem;
char tem_1[8]={"\0"};   
char *node_id = "<5679>";  //From LG01 via web Local Channel settings on MQTT.Please refer <> dataformat in here. 
uint8_t datasend[36];
unsigned int count = 1; 
unsigned long new_time,old_time=0;

void setup()
{
      Serial.begin(115200);
      while (!Serial);
      Serial.println(F("Start MQTT Example of the ThingSpeak"));
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
//
      LoRa.onReceive(onReceive);   
      LoRa.receive();     
}

void ir()
{
  tem=digitalRead(3);
  Serial.println(tem);
}
void irWrite()
{
    char data[50] = "\0";
    for(int i = 0; i < 50; i++)
    {
       data[i] = node_id[i];
    }

    dtostrf(tem,0,1,tem_1);
  

    // Serial.println(tem_1);
     strcat(data,"field1=");
     strcat(data,tem_1);
     strcpy((char *)datasend,data);
     
  
      
}



void SendData()
{
     LoRa.beginPacket();
     LoRa.print((char *)datasend);
     LoRa.endPacket();
     Serial.println("Packet Sent");
}    
    


void loop()
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
      ir();
      irWrite();
      SendData();
      LoRa.receive();
      delay(3000);
    }
}

void onReceive(int packetSize) {
 
  // received a packet
  Serial.print("Received packet : ");

  // read packet
  for (int i = 0; i < packetSize; i++) {
      Serial.print((char)LoRa.read());
  }
  Serial.print("\n\r");  
}
