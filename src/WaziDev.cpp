#include "WaziDev.h"
#include <SPI.h> 
#include <EEPROM.h>
#include <LowPower.h>

//setup WaziDev
WaziDev::WaziDev(char *deviceId, int nodeAddr) {

  this->deviceId = deviceId;
  this->nodeAddr = nodeAddr;
}
    
WaziDev::WaziDev(char *deviceId, int nodeAddr, int destAddr, int loraMode, int channel, int maxDBm) {

  this->deviceId = deviceId;
  this->nodeAddr = nodeAddr;
  this->destAddr = destAddr;
  this->loraMode = loraMode;
  this->channel = channel;
  this->maxDBm = maxDBm;

};

void WaziDev::setup()
{
  Serial.begin(38400);  

  // Print a start message
  writeSerial("WaziDev starting...\n");

  // Power ON the module
  sx1272.ON();

  // get config from EEPROM
  EEPROM.get(0, config);

  // found a valid config?
  if (config.flag1==0x12 && config.flag2==0x34) {
    writeSerial("Getting back previous sx1272 config...\n");

    // set sequence number for SX1272 library
    sx1272._packetNumber=config.seq;
    writeSerial("Using packet sequence number of %d\n", sx1272._packetNumber);
  }
  else {
    // otherwise, write config and start over
    config.flag1=0x12;
    config.flag2=0x34;
    config.seq=sx1272._packetNumber;
  }
  
  // Set transmission mode and print the result
  int modeRes = sx1272.setMode(loraMode);
  writeSerial("Setting Mode: state %d\n", modeRes);
    
  int chanRes = sx1272.setChannel(channel);  
  writeSerial("Setting Channel: state %d\n", chanRes);

  // enable carrier sense
  sx1272._enableCarrierSense=true;
  // TODO: with low power, when setting the radio module in sleep mode
  // there seem to be some issue with RSSI reading
  sx1272._RSSIonSend=false;

  sx1272._needPABOOST=true; 

  int dbmRes = sx1272.setPowerDBM(maxDBm);
  writeSerial("Setting Power: state %d\n", dbmRes);
  
  // Set the node address and print the result
  int nodeAddrRes = sx1272.setNodeAddress(nodeAddr);
  writeSerial("Setting node addr: state %d\n", nodeAddrRes);
  
  // Print a success message
  writeSerial("WaziDev successfully configured\n");

}

void WaziDev::sendSensorValue(char sensorId[], float val)
{

  //Preparing payload
  char devPayload[50] = "";
  if(deviceId != NULL) {
    sprintf(devPayload,"UID/%s/", deviceId);
  }
  char senPayload[50];
  sprintf(senPayload,"%s/%s", sensorId, String(val).c_str());

  uint8_t message[103];
  uint8_t r_size = sprintf(message,"\\!%s%s", devPayload, senPayload);

  writeSerial("Sending %s\n", message);
  writeSerial("Real payload size is %d\n", r_size);
      
  sx1272.CarrierSense();
 
  // just a simple data packet
  sx1272.setPacketType(PKT_TYPE_DATA);
  
  long startSend = millis();

  int sendRes = sx1272.sendPacketTimeout(destAddr, message, r_size);

  long endSend = millis();
    
  // save packet number for next packet in case of reboot
  config.seq=sx1272._packetNumber;     
  EEPROM.put(0, config);

  writeSerial("LoRa pkt size %d\nLoRa pkt seq %d\nLoRa Sent in %ld\nLoRa Sent w/CAD in %ld\nPacket sent, state %d\nRemaining ToA is %d\n",
              r_size,
              sx1272.packet_sent.packnum,
              endSend-startSend,
              endSend-sx1272._startDoCad,
              sendRes, sx1272.getRemainingToA());

  writeSerial("Switch to power saving mode\n");
  int resSleep = sx1272.setSleepMode();

  if (!resSleep)
    writeSerial("Successfully switch LoRa module in sleep mode\n");
  else  
    writeSerial("Could not switch LoRa module in sleep mode\n");
    
  Serial.flush();
             
}

float WaziDev::receiveActuatorValue(char *actuatorId) {

  char uidVal[30]="";
  char actId[20]="";
  static float actVal;
  
  char *res = WaziDev::receive();
   
  sscanf(res, "\\!UID/%[^/]/%[^/]/%f", uidVal, actId, actVal);
  
  writeSerial("\nReceived: uid = %s, actuator Id = %s, value = %f\n", uidVal, actId, actVal);
  
  if(strcmp(uidVal, deviceId) == 0 &&
     strcmp(actId, actuatorId) == 0) {
    
    //Return the actuator value.
    return actVal;

  } else {
    //No actuator value found in that payload.
    return NULL;
  }
  
}

char* WaziDev::receive() {

  int resRec = sx1272.receiveAll(MAX_TIMEOUT);

  if (resRec != 0 && resRec != 3) {
     writeSerial("Receive error %d\n", resRec);

     if (resRec == 2) {
         // Power OFF the module
         sx1272.OFF();
         writeSerial("Resetting radio module\n");
         int resON = sx1272.ON();
         writeSerial("Setting power ON: state %d\n", resON);
     }
     Serial.flush();         
  }
      
  sx1272.getSNR();
  sx1272.getRSSIpacket();
  char* data = malloc(sx1272._payloadlength * sizeof(char));;
  memcpy(data, sx1272.packet_received.data, sx1272._payloadlength);

  writeSerial("Received from LoRa:\n  data = %s\n", data);
  writeSerial("  dst = %d, type = 0x%02X, src = %d, seq = %d\n",
              sx1272.packet_received.dst,
              sx1272.packet_received.type,
              sx1272.packet_received.src,
              sx1272.packet_received.packnum);
  writeSerial("  len = %d, SNR = %d, RSSIpkt = %d, BW = %d, CR = 4/%d, SF = %d\n",
              sx1272._payloadlength,
              sx1272._SNR,
              sx1272._RSSIpacket,
              (sx1272._bandwidth==BW_125) ? 125 : ((sx1272._bandwidth==BW_250) ? 250 : 500),
              sx1272._codingRate+4,
              sx1272._spreadingFactor);

  Serial.flush();

  return data;

}

float WaziDev::getSensorValue(int pin) {

  //read the raw sensor value
  float value = analogRead(pin);

  writeSerial("Reading %f\n", value);

  return value;
}

void WaziDev::putActuatorValue(int pin, float val) {

  writeSerial("Writing on pin %d with value %d\n", pin, val);
  analogWrite(pin, val);

}

// Power down the WaziDev for "duration" seconds
void WaziDev::powerDown(const int duration) {

  for (uint8_t i=0; i<duration; i++) {  
      // ATmega2560, ATmega328P, ATmega168, ATmega32U4
      LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
                              
      writeSerial(".");
      delay(1);                        
  }    
  writeSerial("\n");
  Serial.flush();

}

void WaziDev::writeSerial(const char* format, ...)
{
    char       msg[100];
    va_list    args;

    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    Serial.print(msg);
}
