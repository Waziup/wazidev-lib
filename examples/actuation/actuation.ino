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
  float senVal = wazidev->getSensorValue(A0);
  wazidev->sendSensorValue("TC1", senVal);
  delay(1000);


  //Receive actuation
  float actVal = wazidev->receiveActuatorValue("TC1");

  if(actVal != NULL) {
    wazidev->writeSerial("Actuator value: %f\n", actVal);
    wazidev->putActuatorValue(LED_BUILTIN, actVal);
  }

  Serial.flush();         
  delay(5000);
}
