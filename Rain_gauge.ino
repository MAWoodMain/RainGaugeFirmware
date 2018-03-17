#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <stdlib.h>  // for itoa() call
#include "pressure.hpp"
#include "tx_counter.hpp"
#include "blink.hpp"
#include "persistence.hpp"

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
        char buf[5];
        // convert 123 to string [buf]
        const char base[] = "#";
        sprintf(buf, "%s%.3i", base, mydatanum);
        uint8_t buff[5];
        for(int i = 0; i < 5; i++) buff[i] = buf[i];
        LMIC_setTxData2(1, buff, sizeof(buff)-1, 1);
        increment_counter();
        mydatanum++;
        Serial.println(F("Packet queued"));
    }
}

void setup() {
    Serial1.begin(9600);
    blinkInit();
    backupRegInit();
    initPressure();
    
    doubleBlink();
    delay(10000);
    shortBlink();
    Serial1.println(F("Starting"));
    int readings[16];
    int i = 0;
    getReadings(readings);
    for(i = 0; i<16; i++)
    {
      Serial1.print(i);
    }
    
/*
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
    do_send(&sendjob);*/
}


void loop() {    
    Serial1.println("PRES");
    Serial1.println(adjustPressure(getAverageRawPressure()));
    //os_runloop_once();
    delay(500);
}
