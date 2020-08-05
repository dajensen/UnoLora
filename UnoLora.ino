#include <RH_RF95.h>
#include <Wire.h>

#define RF95_FREQ 915.0

#define RFM95_CS      10
#define RFM95_RST     9
#define RFM95_INT     2

RH_RF95 rf95(RFM95_CS, RFM95_INT);


void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(1000);

  // Initialize communication with the humidity sensor
  Wire.begin();
  Serial.println("Wire Library Initialized");

  // reset the radio
  pinMode(RFM95_RST, OUTPUT);

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println("Radio init failed");
    while (1);
  }
  Serial.println("Radio init OK!");
  
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  Serial.println("Setup complete");
}

void loop() {

  static int16_t packetnum = 0;  // packet counter, we increment per xmission

  const int RECV_BUFFER_LEN = 3;
  int bytesReceived = 0;
  uint8_t recvBuffer[RECV_BUFFER_LEN];
  
  
   Wire.requestFrom(0x40, RECV_BUFFER_LEN);
   while(Wire.available() && bytesReceived < RECV_BUFFER_LEN)    // slave may send less than requested
   {
      uint8_t c = Wire.read();    // receive a byte as character
      recvBuffer[bytesReceived] = c;
      bytesReceived++;
      Serial.print(String(c) + " ");         // print the character
   }
   Serial.println("");


  // Send a message!
  rf95.send((uint8_t *)recvBuffer, RECV_BUFFER_LEN);
  rf95.waitPacketSent();

  // wait for a second before transmitting again
  delay(1000);
}
