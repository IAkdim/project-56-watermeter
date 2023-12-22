#include <Arduino.h>
#include "LoRaWan_APP.h"
#define WATERPIN 6

unsigned long previousMillisSendlora = 0;
const long intervalLoraSend = 1 * 60 * 1000;  // 15 minutes in milliseconds
volatile bool newPulse = false;

const int arraySize = 24;
uint16_t waterDataArray[arraySize];
unsigned long previousMillisSaveData = 0;
const long intervalSaveData = 1 * 30 * 1000;

int currentIndex = 0;

uint16_t waterverbruik = 0;

/* OTAA para*/
uint8_t devEui[] = { 0x00, 0x59, 0xAC, 0x00, 0x00, 0x1B, 0x26, 0x3F };
uint8_t appEui[] = { 0x00, 0x59, 0xAC, 0x00, 0x00, 0x01, 0x0A, 0x8C };
uint8_t appKey[] = { 0x90, 0xfa, 0x0c, 0x61, 0x75, 0x88, 0xa9, 0x4d, 0x56, 0xc0, 0x83, 0x21, 0x8a, 0xbc, 0x53, 0xae };

/* ABP para*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda, 0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef, 0x67 };
uint32_t devAddr = (uint32_t)0x007e6ae1;

/*LoraWan channelsmask*/
uint16_t userChannelsMask[6] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t loraWanClass = CLASS_B;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15000;

/*OTAA or ABP*/
bool overTheAirActivation = true;

/*ADR enable*/
bool loraWanAdr = true;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 2;
/*!
 * Number of trials to transmit the frame, if the LoRaMAC layer did not
 * receive an acknowledgment. The MAC performs a datarate adaptation,
 * according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
 * to the following table:
 *
 * Transmission nb | Data Rate
 * ----------------|-----------
 * 1 (first)       | DR
 * 2               | DR
 * 3               | max(DR-1,0)
 * 4               | max(DR-1,0)
 * 5               | max(DR-2,0)
 * 6               | max(DR-2,0)
 * 7               | max(DR-3,0)
 * 8               | max(DR-3,0)
 *
 * Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
 * the datarate, in case the LoRaMAC layer did not receive an acknowledgment
 */
uint8_t confirmedNbTrials = 4;

/* Prepares the payload of the frame */
static void prepareTxFrame(uint8_t port)
{
    /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
     *appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
     *if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
     *if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
     *for example, if use REGION_CN470,
     *the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and
     *BandwidthsCN470 in "RegionCN470.h".
     */
    //uint8_t appData[arraySize * sizeof(uint16_t)];

    appDataSize = arraySize * sizeof(uint16_t);

    for (int i = 0; i < arraySize; ++i) {
      uint16_t value = waterDataArray[i];
      appData[i * sizeof(uint16_t)] = lowByte(value);
      appData[i * sizeof(uint16_t) + 1] = highByte(value);
    }
    for (int i = 0; i < arraySize * sizeof(uint16_t); ++i) {
      Serial.print(appData[i],HEX);
      Serial.print(" ");
    }
    Serial.println();
}

// if true, next uplink will add MOTE_MAC_DEVICE_TIME_REQ
void pulseCounter() {
    waterverbruik++;
    newPulse = true;
}
void saveDataToArray() {
  // Save waterverbruik to the array
  Serial.println(waterverbruik);
  waterDataArray[currentIndex] = waterverbruik;
  waterverbruik = random(0,255); 
  Serial.println(waterverbruik);
} 
void setup()
{
    Serial.begin(115200);
    Serial.println("setup started");
    //attachInterrupt(digitalPinToInterrupt(WATERPIN), pulseCounter, RISING);    
    Mcu.begin();
    deviceState = DEVICE_STATE_INIT;
    Serial.print("Water Data Array: ");
    for (int i = 0; i < arraySize; ++i) {
      Serial.print(waterDataArray[i],HEX);
      Serial.print(" ");
    }
    Serial.println();
    prepareTxFrame(appPort);

}

void loop()
{   
    unsigned long currentMillis = millis();
    if(newPulse){
        newPulse = false;
        //Serial.println(waterverbruik);
    }
    if (currentMillis - previousMillisSaveData >= intervalSaveData) {
        // Save data to the array every hour
        saveDataToArray();

        // Update the timestamp and rotate the index
        previousMillisSaveData = currentMillis;
        currentIndex = (currentIndex + 1) % arraySize;
      }
    if (currentMillis - previousMillisSendlora >= intervalLoraSend) {
        Serial.println("data send try?");
        deviceState = DEVICE_STATE_SEND;
        // waterverbruik++;
        // if(waterverbruik == 200){
        //     multiplierwater++;
        //     waterverbruik=0;
        // }
        prepareTxFrame(appPort);
        LoRaWAN.send();

        // Reset the timer
        previousMillisSendlora = currentMillis;
    }

    switch (deviceState) {
    case DEVICE_STATE_INIT: {
#if (LORAWAN_DEVEUI_AUTO)
        LoRaWAN.generateDeveuiByChipID();
#endif
        LoRaWAN.init(loraWanClass, loraWanRegion);
        break;
    }
    case DEVICE_STATE_JOIN: {
        LoRaWAN.join();
        break;
    }
    case DEVICE_STATE_SEND: {
        
        //prepareTxFrame(appPort);
        //LoRaWAN.send();
        Serial.print("DEVICESEND");

        deviceState = DEVICE_STATE_CYCLE;
        break;
    }
    case DEVICE_STATE_CYCLE: {
        // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
    }
    case DEVICE_STATE_SLEEP: {
        LoRaWAN.sleep(loraWanClass);
        break;
    }
    default: {
        deviceState = DEVICE_STATE_INIT;
        break;
    }
    }
}