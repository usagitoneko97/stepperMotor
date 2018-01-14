#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

#include <SPI.h>
#include<ESP8266TrueRandom.h>
#include "Delay.h"
uint8_t buffer1;
const uint8_t DUMMY = 0xff;
const uint8_t NOP = 0xff;
const uint8_t NEED_WIFI = 0x02;
const uint8_t WIFI_DATA = 0x3;
const uint8_t DONE_SENDING = 0x4;
const uint8_t NEED = 1;
const uint8_t XNEED = 0;
const char* PASSWORD_DEFAULT = "thereisnospoon";
char* SSID_DEFAULT = "hoguoxia";
uint8_t testBuffer[2] = {0x89, 0x23};

#define seq_order_capitalLetter   3
#define seq_order_smallLetter     2
#define seq_order_Digit           1
char ssid[8];
char password[8];
char *tempSsid;
char *tempPassword;
/*Mock wifiSSID*/
uint8_t WifiSSID[8] = {0x89, 0x2d, 0x89, 0x78, 0x98, 0x54, 0x84, 0x28};
uint8_t WifiPW[8] = {0x23, 0x18, 0x1d, 0xad, 0x03, 0x82, 0x92, 0x12};

NonBlockDelay d;
void setup (void)
{
  Serial.begin(9600);
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);
 
  // Put SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.beginTransaction (SPISettings (2000000, MSBFIRST, SPI_MODE0));

  SPI.begin ();
  // Slow down the master a bit
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  Serial.print("begin!!");

}  // end of setup



void loop (void)
{
     SPI.transfer(NEED_WIFI);
     //delay(1);
     buffer1 = SPI.transfer(DUMMY);
    
     if(buffer1 == NEED){
      //random ssid and pw
      //set ssid and pw
      SSIDGenerator();
      PasswordGenerator();
      delay(1);
      
      SpiTransferArray(ssid);
      SpiTransferArray(password);
      tempSsid = &ssid[0];
      //tempPassword = &password[0];
      *(tempSsid+8) = NULL;
      //*(tempPassword+8) = NULL;
        Serial.println(tempSsid);
        Serial.println(password);
    //  Serial.println(tempPassword);
      //Serial.println(SSID_DEFAULT);
     
       Serial.println(WiFi.softAP(tempSsid, password) ? "Ready" : "Failed!");
      }
     else if (buffer1 == XNEED){  //else?
        // Serial.println("NOP");
       }
     //Serial.println(buffer1);
     delay(1);
   
}


void SpiTransferArray(char* data){
    //TODO: code:
    int i;
    for(i = 0;i < 8;i++){
      SPI.transfer(data[i]);
      delay(1);
    }
    delay(1);
  }
  
void SSIDGenerator() {
for(int i=0;i<8;i++)
 {
  int x = ESP8266TrueRandom.random(1,4);
    if(x==seq_order_Digit)
    {
     ssid[i]=ESP8266TrueRandom.random('0',':');
    }
    else if(x==seq_order_smallLetter)
    {
     ssid[i]=ESP8266TrueRandom.random('a','{');
    }
    else
    {
     ssid[i]=ESP8266TrueRandom.random('A','[');
    }  
}
}

void PasswordGenerator() {
  for(int i=0;i<8;i++)
  {
    int x = ESP8266TrueRandom.random(1,4);
      if(x==seq_order_Digit)
      {
      password[i]=ESP8266TrueRandom.random('0',':');
      }
      else if(x==seq_order_smallLetter)
      {
      password[i]=ESP8266TrueRandom.random('a','{');
      }
      else
      {
      password[i]=ESP8266TrueRandom.random('A','[');
      }  
  }
}

