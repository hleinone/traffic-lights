#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

struct State {
  int value;
  unsigned long updated;
};

static BLEUUID serviceUUID("00001812-0000-1000-8000-00805f9b34fb");
static BLEUUID charUUID("00002a4d-0000-1000-8000-00805f9b34fb");
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLEAdvertisedDevice* myDevice;
static const int RED_PIN =  23;
static const int AMBER_PIN =  22;
static const int GREEN_PIN =  21;
static const int STATE_WARNING_OFF = 1;
static const int STATE_WARNING_ON = 2;
static const int STATE_STOP = 3;
static const int STATE_STOP_NEXT = 4;
static const int STATE_GO = 5;
static const int STATE_GO_NEXT = 6;
static State state = {STATE_WARNING_OFF, 0};

void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  uint8_t data = *pData;
  if (data == 1) {
    Serial.println("Click");
    unsigned long currentMicros = micros();
    if (state.value == STATE_WARNING_OFF || state.value == STATE_WARNING_ON || state.value == STATE_STOP || state.value == STATE_STOP_NEXT) {
      Serial.printf("State go next %lu\n", currentMicros);
      state = {STATE_GO_NEXT, currentMicros};
    } else {
      Serial.printf("State stop next %lu\n", currentMicros);
      state = {STATE_STOP_NEXT, currentMicros};
    }
  }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    doScan = true;
    Serial.println("Disconnected");
    Serial.println("State warning off");
    state = {STATE_WARNING_OFF, micros()};
  }
};

bool connectToServer() {
  Serial.printf("Connecting to %s\n", myDevice->getAddress().toString().c_str());
  
  BLEClient* pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remote BLE Server, retrying 3 times if connection fails.
  int retries = 3;
  while (retries >= 0 && !pClient->isConnected()) {
    pClient->connect(myDevice);
    retries--;
    if (retries >= 0 && !pClient->isConnected()) {
      Serial.println(" - Connection failed, retrying...");
    }
  }
  if (pClient->isConnected()) {
    Serial.println(" - Connected to server");
    Serial.println("State stop");
    state = {STATE_STOP, micros()};
  } else {
    Serial.println(" - Connection failed");
    return false;
  }

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.printf("Failed to find our service UUID: %s\n", serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");
  BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.printf("Failed to find our characteristic UUID: %s\n", charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  if (pRemoteCharacteristic->canNotify()) {
    Serial.println(" - Registering for notifications");
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  return true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.printf("Advertised Device: %s\n", advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = false;    
    }
  }
};

void setup() {
  Serial.begin(9600, SERIAL_8N1);
  Serial.println("Setting up...");

  pinMode(RED_PIN, OUTPUT);
  pinMode(AMBER_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  BLEDevice::init("TrafficLights");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  doScan = true;
}

void loop() {
  if (doConnect) {
    Serial.println("Do connect.");
    if ((connected = connectToServer())) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  if (!connected && doScan) {
    Serial.println("Scanning...");
    BLEDevice::getScan()->start(1);
  }

  unsigned long currentMicros = micros();
  if (state.value == STATE_WARNING_OFF) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(AMBER_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    if (currentMicros > state.updated + 1000000) {
      Serial.println("State warning on");
      state = {STATE_WARNING_ON, currentMicros};
    }
  } else if (state.value == STATE_WARNING_ON) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(AMBER_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    if (currentMicros > state.updated + 1000000) {
      Serial.println("State warning off");
      state = {STATE_WARNING_OFF, currentMicros};
    }
  } else if (state.value == STATE_GO_NEXT) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(AMBER_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    if (currentMicros > state.updated + 1000000) {
      Serial.println("State go");
      state = {STATE_GO, currentMicros};
    }
  } else if (state.value == STATE_GO) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(AMBER_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
  } else if (state.value == STATE_STOP_NEXT) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(AMBER_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    if (currentMicros > state.updated + 1000000) {
      Serial.println("State stop");
      state = {STATE_STOP, currentMicros};
    }
  } else if (state.value == STATE_STOP) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(AMBER_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
  }
//  delay(100);
}