#include <WaziDev.h>

// new WaziDev with node address = 8 
WaziDev *wazidev = new WaziDev("MyDevice", 8);

void setup()
{
  wazidev->setup();
}

void loop(void)
{

  //Send some data on sensor TC1
  char *val = wazidev->getSensorValue(A0);
  wazidev->sendSensorValue("TC1", val);
  delay(1000);


  //Receive actuation
  char* res = wazidev->receiveActuatorValue("TC1");

  if(res != NULL) {
    wazidev->writeSerial("Actuator value: %s\n", res);
    int val; 
    sscanf(res, "%d", &val);
    wazidev->putActuatorValue(LED_BUILTIN, val);
  }

  Serial.flush();         
  delay(5000);
}
