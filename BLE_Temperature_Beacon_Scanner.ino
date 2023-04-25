/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
   Changed to a beacon scanner to report iBeacon, EddystoneURL and EddystoneTLM beacons by beegee-tokyo
*/

#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEEddystoneURL.h>
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

int scanTime = 5; //In seconds
BLEScan *pBLEScan;
uint16_t thex;
uint8_t temp;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      BLEUUID checkUrlUUID = (uint16_t)0x0602;

      if (advertisedDevice.haveServiceUUID())

      {
        if (advertisedDevice.getServiceUUID().equals(checkUrlUUID))
        {
          BLEUUID devUUID = advertisedDevice.getServiceUUID();
//          Serial.print("Found ServiceUUID: ");
//          Serial.println(devUUID.toString().c_str());
//          Serial.println("");
          uint8_t cServiceData[100];
          uint8_t* payloadPtr = advertisedDevice.getPayload();

          if ( (*(payloadPtr + 5) == 0x2) && (*(payloadPtr + 6) == 0x6) )  // frame with temperature
          {
            for (int i = 0; i < advertisedDevice.getPayloadLength(); i++)
            {
              cServiceData[i] = *(payloadPtr + i);
              //Serial.printf("0x%X ", cServiceData[i]);
            }
            thex = cServiceData[22] | (cServiceData[21] << 8);
            temp = (uint8_t) - 45 + (( thex * 175) / 0xFFFF);
            Serial.printf("Temperature= %d\n", temp);
          }
        }
      }
    }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(500);
  pBLEScan->setWindow(499); // less or equal setInterval value
}

void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //  Serial.print("Devices found: ");
  //  Serial.println(foundDevices.getCount());
  //  Serial.println("Scan done!");
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}
