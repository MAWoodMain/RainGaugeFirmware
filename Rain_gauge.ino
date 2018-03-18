#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <stdlib.h>  // for itoa() call
#include "pressure.hpp"
#include "tx_counter.hpp"
#include "blink.hpp"
#include "persistence.hpp"
#include "payloadBuilder.hpp"

// Pin mappings for radio
#define SSPIN   PA4
#define RST     PB0
#define DIO0    PB1
#define DIO1    PB10
#define DIO2    PB11

const unsigned TX_INTERVAL = 60;

// LoRaWAN NwkSKey, network session key
static const PROGMEM u1_t NWKSKEY[16] = { 0xF7, 0x4B, 0x10, 0x96, 0xF0, 0xD3, 0xDf, 0x8B, 0x72, 0xC5, 0xA3, 0x99, 0x19, 0xA4, 0xAF, 0x73 };
// LoRaWAN AppSKey, application session key
static const u1_t PROGMEM APPSKEY[16] = { 0x00, 0xC7, 0xCA, 0x42, 0xC8, 0x46, 0x7B, 0x39, 0xE0, 0xF2, 0x52, 0xFE, 0x72, 0x47, 0x07, 0x4E };
// Device address
static const u4_t DEVADDR = 0x06BF10C8;

// These callbacks are only used in over-the-air activation
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static uint8_t mydata[] = "Test";
static uint8_t mydatanum = 0;
static osjob_t sendjob;
static uint8_t custom_payload[51];
static int payload_length = 0;

RTClock rt (RTCSEL_LSE);

const lmic_pinmap lmic_pins = {
    .nss = SSPIN,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RST,
    .dio = {DIO0, DIO1, DIO2},
};

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
         default:
            Serial.println(F("Unknown event"));
            break;
    }
}
    
void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        //LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 1);
        LMIC_setTxData2(1, custom_payload, payload_length, 0); // last parameters for ACK (0 or 1)
        increment_counter();
        Serial.println(F("Packet queued"));
    }
}

int getBattery()
{
  pinMode(PA0, INPUT_ANALOG);
  int avg_power = 0;
  for (int i = 0; i < 5; i++) {
    avg_power += analogRead(PA0);
  }
  avg_power = avg_power / 5;
  avg_power -= 3000;
  avg_power = constrain(avg_power, 0, 1023);
  return avg_power;
}

void printAllReadings()
{
  int readings[16];
  getReadings(readings);
  for(int i=0; i<16; i++)
  {
    Serial1.println(readings[i]);
  }
}

void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
     char tmp[16];
       for (int i=0; i<length; i++) { 
         sprintf(tmp, "%.2X",data[i]); 
         Serial1.print(tmp);
       }
}
void PrintHex16(int data) // prints 16-bit data in hex with leading zeroes
{
       char tmp[16];
       sprintf(tmp, "0x%.4X",data); 
       Serial1.print(tmp);
}

void setup() {
    Serial1.begin(9600);
    //backupRegInit();
    blinkInit();
    initPressure();
    shortBlink();
    Serial1.println(F("Starting"));
    
    Serial1.print("State: ");
    Serial1.println(getStateNumber());
    storeReading(getAverageAdjustedPressure());
    
    if(isTransmitState())
    {
      payload_length = constructPayload(getBattery(), rt.getTime(), &custom_payload[0]);
      Serial1.print("Payload size: ");
      Serial1.println(payload_length);
      Serial1.println("Payload");
      for(int i = 0; i<payload_length; i++)
      {
        PrintHex8(&custom_payload[i], 1);
      }
      Serial1.println();
      Serial1.println("Registers");
      for(int n=0;n<10; n++)
      {
        PrintHex16(readBackupReg(n));
      }
      Serial1.println();
      //startRadioAndTransmit();
    }
    incrementState();
    
    printAllReadings();
    delay(1000);
    hal_sleep();
}

void startRadioAndTransmit()
{
    os_init();
    LMIC_reset();
    
    #ifdef PROGMEM
    // On AVR, these values are stored in flash and only copied to RAM
    // once. Copy them to a temporary buffer here, LMIC_setSession will
    // copy them into a buffer of its own again.
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
    #else
    // If not running an AVR with PROGMEM, just use the arrays directly
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
    #endif

    #if defined(CFG_eu868)
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
    #endif

    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;

    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(DR_SF7,14);

    // load fcnt from eeprom
    LMIC.seqnoUp = read_counter();

    // Start job
    do_send(&sendjob);
}


void loop() {    
    Serial1.println("PRES");
    Serial1.println(adjustPressure(getAverageRawPressure()));
    //os_runloop_once();
    delay(500);
}
