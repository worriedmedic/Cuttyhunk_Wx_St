#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <RH_RF95.h>
#include <LowPower.h>

//#define SEALEVELPRESSURE_HPA (1013.25)

#define RFM95_CS 10
#define RFM95_RST A1
#define RFM95_INT 2
#define BATTERY_SENSE A0

#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

String ident = "01"; //Define Address of Sensor

double tempnow = 0;
double presnow = 0;
double altinow = 0;
double humdnow = 0;
float pressconv = 0.000295299830714;  //Conversion from hPa to inHg

int sleeptimer = 12;

Adafruit_BME280 bme; // I2C

void setup() {
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  analogReference(INTERNAL);
  
  Serial.begin(9600);

  while (!Serial);

  while (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    delay(5000);
  }

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  
  while (!rf95.init()) {
    Serial.println("RFM95 LoRa Radio Init Failed");
  }

  while (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("RMF95 Set Frequency Failed");
  }

  Serial.println("Set Frequency to: ");
  Serial.println(RF95_FREQ);
  rf95.setTxPower(5, false);

  Serial.println("TX Interval: ");Serial.println(sleeptimer * 8);
  
}

void loop() {
    
    tempnow = ((bme.readTemperature() * 1.8) + 32);
    presnow = (bme.readPressure() * pressconv);
    //altinow = (bme.readAltitude(SEALEVELPRESSURE_HPA));
    humdnow = (bme.readHumidity());

    // 1M, 470K divider across battery and using internal ADC ref of 1.1V
    // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
    // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
    // 3.44/1023 = Volts per bit = 0.003363075
    
    float sensorValue = analogRead(BATTERY_SENSE);
    float batteryV  = sensorValue * 0.003363075;
    float batteryPcnt = sensorValue / 10;
  
    String buffer;
    char txbuffer [31];
    String tbuffer = "T";
    String pbuffer = "P";
    String hbuffer = "H";
    String vbuffer = "V";

    tbuffer = tbuffer + tempnow;
    pbuffer = pbuffer + presnow;
    hbuffer = hbuffer + humdnow;
    vbuffer = vbuffer + batteryPcnt;

    buffer = ident + "," + tbuffer + "," + pbuffer + "," + hbuffer + "," + vbuffer;

    buffer.toCharArray(txbuffer,31);
    
    Serial.println(txbuffer);  //DEBUG Check out of output

    rf95.setModeIdle(); //Wakeup RFM_95 from sleep mode
    delay(10);
    
    rf95.send((uint8_t *)txbuffer, 31);  //Send Packet

    delay(10);
    rf95.waitPacketSent();
    delay(10);
    rf95.sleep();  //Return RFM_95 to sleep mode

    int i = 0; 
    
    while (i < sleeptimer) {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      i++;
    }
    
}

