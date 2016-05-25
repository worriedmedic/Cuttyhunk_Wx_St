// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

#include <SPI.h>
#include <RH_RF95.h>

#if defined(__AVR_ATmega32U4__)
// Pin layout for Adafruit Feather 32u4 Datalogger
#define RFM95_CS 11
#define RFM95_RST A1
#define RFM95_INT 3
#define LED_ERROR 13
#define LED_RX 12
#elif defined(__AVR_ATmega328P__)
// Pin layout for Arduino Pro Mini
#define RFM95_CS 10
#define RFM95_RST A1
#define RFM95_INT 2
#define LED_ERROR 8
#define LED_RX 9
#endif

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {  
  pinMode(LED_ERROR, OUTPUT);
  pinMode(LED_RX, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  //RH_RF95 rf95(RFM95_CS, RFM95_INT);

  Serial.begin(9600);
  while (!Serial);

  Serial.println("Feather LoRa RX Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    digitalWrite(LED_ERROR, HIGH);
    delay(750);
    digitalWrite(LED_ERROR, LOW);
    delay(750);
    }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    digitalWrite(LED_ERROR, HIGH);
    delay(750);
    digitalWrite(LED_ERROR, LOW);
    delay(750);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  digitalWrite(LED_ERROR, HIGH); digitalWrite(LED_RX, HIGH);
  delay(2000);
  digitalWrite(LED_ERROR, LOW); digitalWrite(LED_RX, LOW);
  delay(1000);
  digitalWrite(LED_ERROR, HIGH); digitalWrite(LED_RX, HIGH);
  delay(2000);
  digitalWrite(LED_ERROR, LOW); digitalWrite(LED_RX, LOW);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {

      //RH_RF95::printBuffer("Received: ", buf, len);
      //Serial.print("Got: ");
      Serial.print((char*)buf);
      Serial.print(",");
      Serial.println(rf95.lastRssi(), DEC);

      digitalWrite(LED_RX, HIGH);
      delay(750);
      digitalWrite(LED_RX, LOW);
      delay(500);

    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}
