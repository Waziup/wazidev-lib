#include <WaziDev.h>

// new WaziDev with node address = 8 
WaziDev *wazidev;

void setup()
{
  wazidev = new WaziDev("MyDevice", 8);
  wazidev->setup();
}

void loop(void)
{

  //Send some data on sensor TC1
  //float senVal = wazidev->getSensorValue(A0);
  //wazidev->sendSensorValue("TC1", senVal);
  //delay(1000);


  //Receive actuation
  String act;
  int res = wazidev->receiveActuatorValue(String("TC1"), 10000, act);

  if(res == 0) {
    wazidev->writeSerial("Actuator value: " + act + "\n");
    //wazidev->putActuatorValue(LED_BUILTIN, 1);
  }

  Serial.flush();         
  delay(5000);
}
