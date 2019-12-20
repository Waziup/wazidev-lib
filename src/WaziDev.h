
#ifndef wazidev_h
#define wazidev_h

#include <SX1272.h>


class WaziDev
{

  public:

   // * Contructors & setup

   //setup WaziDev with the node address
    WaziDev(String deviceId, int nodeAddr);
    
    //setup WaziDev with all parameters
    WaziDev(String deviceId, int nodeAddr, int destAddr, int loraMode, int channel, int maxDBm);

    //setup the WaziDev. Must be called before any LoRa operations.
    void setup();


    // * Sensors and actuators interactions

    //read a sensor
    float getSensorValue(int pin);

    //write an actuator
    void putActuatorValue(int pin, float val);


    // * LoRa network

    //Send a LoRa message. deviceId is optional.
    void sendSensorValue(String sensorId, float val);

    int receiveActuatorValue(String actuatorId, int wait, String &res);

    //Send a raw payload
    void send(String payload);
   
    //Receive a raw payload
    int receive(String &data, int wait);


    // * Power management

    // Power down the WaziDev for "duration" seconds
    void powerDown(const int duration); 


    // * Serial port interactions

    // Write a message to the serial monitor using printf format.
    // Note: due to arduino limitations, floats (%f) won't work.
    void writeSerial(const char *format, ...);

    // Write a message to the serial monitor
    void writeSerial(String format);


  private:

    String  deviceId;
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
