#include <WaziDev.h>
#include <xlpp.h>

unsigned char LoRaWANKey[16] = {0x23, 0x15, 0x8D, 0x3B, 0xBC, 0x31, 0xE6, 0xAF, 0x67, 0x0D, 0x19, 0x5B, 0x5A, 0xED, 0x55, 0x25};
unsigned char DevAddr[4] = {0x26, 0x01, 0x1D, 0x87};

WaziDev wazidev;

void setup()
{
    Serial.begin(38400);
    wazidev.setupLoRaWAN(DevAddr, LoRaWANKey);
}

XLPP xlpp(120);

void loop(void)
{
  // 1
  // Create xlpp payload.
  xlpp.reset();
  xlpp.addTemperature(1, 20.3); // °C

  // 2.
  // Send paload with LoRaWAN.
  serialPrintf("LoRaWAN send ... ");
  uint8_t e = wazidev.sendLoRaWAN(xlpp.buf, xlpp.len);
  if (e != 0)
  {
    serialPrintf("Err %d\n", e);
    delay(60000);
    return;
  }
  serialPrintf("OK\n");
  
  // 3.
  // Receive LoRaWAN message (waiting for 6 seconds only).
  serialPrintf("LoRa receive ... ");
  uint8_t offs = 0;
  long startSend = millis();
  e = wazidev.receiveLoRaWAN(xlpp.buf, &xlpp.offset, &xlpp.len, 6000);
  long endSend = millis();
  if (e != 0)
  {
    if (e == ERR_LORA_TIMEOUT){
      serialPrintf("nothing received\n");
    }
    else
    {
      serialPrintf("Err %d\n", e);
    }
    delay(60000);
    return;
  }
  serialPrintf("OK\n");
  
  serialPrintf("Time On Air: %d ms\n", endSend-startSend);
  serialPrintf("LoRa SNR: %d\n", wazidev.loRaSNR);
  serialPrintf("LoRa RSSI: %d\n", wazidev.loRaRSSI);

  // 4.
  // Read xlpp message.
  // You must use the following pattern to properly parse xlpp payload.

  while (xlpp.offset < xlpp.len)
  {
    // Always read the channel first ...
    uint8_t chan = xlpp.getChannel();
    serialPrintf("Chan %2d: ", chan);

    // ... then the type ...
    uint8_t type = xlpp.getType();

    // ... then the value!
    switch (type) {
      case LPP_DIGITAL_OUTPUT:
      {
        uint8_t v = xlpp.getDigitalInput();
        printf("Digital Output: %hu (0x%02x)\n", v, v);
        break;
      }
      case LPP_ANALOG_OUTPUT:
      {
        float f = xlpp.getAnalogOutput();
        printf("Analog Output: %.2f\n", f);
        break;
      }
      default:
        printf("Other unknown type.\n");
        delay(60000);
        return;
    }
  }
  
  delay(60000);
}
