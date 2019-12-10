
#ifndef wazidev_h
#define wazidev_h

#include <SX1272.h>


class WaziDev
{
  public:
    //setup WaziDev with the node address
    WaziDev(int nodeAddr);
    
    //setup WaziDev with all parameters
    WaziDev(int nodeAddr, int destAddr, int loraMode, int channel, int maxDBm);

    //setup the WaziDev. Must be called before any LoRa operations.
    void setup();
   
    //read a sensor
    int getSensorValue(int pin);

    //Send a LoRa message. deviceId is optional.
    void send(char sensorId[], float val, char deviceId[] = NULL);

    // Power down the WaziDev for "duration" seconds
    void powerDown(const int duration); 
    
    // Write a message to the serial monitor
    void writeSerial(const char* format, ...);

  private:
    //Send a raw payload
    void WaziDev::send(char payload[]);
    
    uint8_t  maxDBm = 14;
    uint32_t channel = CH_10_868;
    int nodeAddr = 8;
    int destAddr = 1;
    int loraMode = 1;
    
    struct Config {
    
      uint8_t flag1;
      uint8_t flag2;
      uint8_t seq;
      // can add other fields such as LoRa mode,...
    };

    Config config;
    

};

#endif
