#include "WiFi.h"
#include "mcp_can.h"
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string

String dashSoc;
String miles;
String charging;
String locked;

#define CAN0_INT 5                              // Set INT to pin 5
MCP_CAN CAN0(4);                                // Set CS to pin 4

#define CAN1_INT 22                             // Set INT to pin 22
MCP_CAN CAN1(21);                               // Set CS to pin 21


void setup() {
  Serial.begin(115200);

  if(CAN0.begin(MCP_ANY, CAN_50KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("CAN0 Initialized Successfully!");
  else
    Serial.println("Error Initializing CAN0");

  if(CAN1.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("CAN1 Initialized Successfully!");
  else
    Serial.println("Error Initializing CAN1");
  
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT); 

  CAN1.setMode(MCP_NORMAL);
  pinMode(CAN1_INT, INPUT); 
}


void lockCar() {
  INT32U canId = 0xC41401F;
  byte data[8] = {};

  data[0] = 0x6C;
  CAN0.sendMsgBuf(canId, 1, 8, data);
  delay(100);
  CAN0.sendMsgBuf(canId, 1, 8, data);
}

void unlockCar() {
  INT32U canId = 0xC41401F;
  byte data[8] = {};
  
  data[0] = 0x74;

  CAN0.sendMsgBuf(canId, 1, 8, data);
  delay(100);
  CAN0.sendMsgBuf(canId, 1, 8, data);
}

void beep() {
  INT32U canId = 0xC41401F;
  byte data[8] = {};
  
  data[0] = 0x2;

  CAN0.sendMsgBuf(canId, 1, 8, data);
  delay(100);
  CAN0.sendMsgBuf(canId, 1, 8, data);
}

void chargingOff() {
  INT32U canId = 0xC41401F;
  byte data[8] = {};
  
  data[0] = 0x40;
  CAN0.sendMsgBuf(canId, 1, 8, data);
  delay(100);
  CAN0.sendMsgBuf(canId, 1, 8, data);
}

void chargingOn() {
  INT32U canId = 0xC41401F;
  byte data[8] = {};
  
  data[0] = 0x20;
  CAN0.sendMsgBuf(canId, 1, 8, data);
  delay(100);
  CAN0.sendMsgBuf(canId, 1, 8, data); 
}

void climateOff() {
  INT32U canId = 0xE194031;
  byte data[8] = {};
  
  data[0] = 0x40;
  data[1] = 0x64;

  CAN0.sendMsgBuf(canId, 1, 8, data);
  delay(100);
  CAN0.sendMsgBuf(canId, 1, 8, data);
}

void climateOn() {
  INT32U canId = 0xE194031;
  byte data[8] = {};
  
  data[0] = 0x20;
  data[1] = 0x64;

  CAN0.sendMsgBuf(canId, 1, 8, data);
  delay(100);
  CAN0.sendMsgBuf(canId, 1, 8, data);
}

void loop() {
  if(!digitalRead(CAN0_INT)) {
    //CAN-B
    CAN0.readMsgBuf(&rxId, &len, rxBuf);    
    
    if ((rxId & 0x1FFFFFFF) == 0x06414000) {
      if (rxBuf[3] == 0x17) {
        locked = "No";
      } else {
        locked = "Yes"; 
      }
    }
  }

  if(!digitalRead(CAN1_INT)) {
    //CAN-C
    CAN1.readMsgBuf(&rxId, &len, rxBuf);      
    
    if ((rxId & 0x1FFFFFFF) == 0x0C10A040) {
      sprintf(msgString, "0x%.2X", rxBuf[1]);
      dashSoc = msgString;
    }

    if ((rxId & 0x1FFFFFFF) == 0x0C10A040) {
      sprintf(msgString, "0x%.2X", rxBuf[0]);
      miles = msgString;      
    }

    if ((rxId & 0x1FFFFFFF) == 0x0C50A049) {
      if (rxBuf[1] == 0x00) {
        charging = "No";
      } else {
        charging = "Yes";
      }
    }
  }
}
