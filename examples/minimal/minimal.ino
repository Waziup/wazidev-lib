#include <WaziDev.h>

// new WaziDev with node address = 8 
WaziDev *wazidev = new WaziDev("MyDevice", 8);

void setup()
{
  wazidev->setup();
}

void loop(void)
{
  //Get a sensor value
  char *val = wazidev->getSensorValue(A0);
  //Send it as snesor "TC1"
  wazidev->sendSensorValue("TC1", val);
  //Sleep for a while
  wazidev->powerDown(5);
  delay(5000);
}
