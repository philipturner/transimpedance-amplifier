#include <SPI.h>

uint8_t CS_DAC = 36;
uint8_t CS_ADC = 37;

/* Input Shift Register Commands for ADS8689 */
#define ADS8689_DEVICE_ID_REG   0x00
#define ADS8689_RST_PWRCTL_REG  0x04
#define ADS8689_SDI_CTL_REG     0x08
#define ADS8689_SDO_CTL_REG     0x0C
#define ADS8689_DATAOUT_CTL_REG 0x10
#define ADS8689_RANGE_SEL_REG   0x14
#define ADS8689_ALARM_REG       0x20
#define ADS8689_ALARM_H_TH_REG  0x24
#define ADS8689_ALARM_L_TH_REG  0x28

// SPI commands
#define ADS8689_NOP         0b0000000
#define ADS8689_CLEAR_HWORD 0b1100000
#define ADS8689_READ_HWORD  0b1100100
#define ADS8689_READ        0b0100100
#define ADS8689_WRITE_FULL  0b1101000 //write 16 bits to register
#define ADS8689_WRITE_MS    0b1101001
#define ADS8689_WRITE_LS    0b1101010

void setup() {
  delay(50);

  pinMode(CS_DAC, OUTPUT);
  pinMode(CS_ADC, OUTPUT);
  digitalWrite(CS_DAC, 1);
  digitalWrite(CS_ADC, 1);

  delay(50);

  SPI.begin();

  delay(50);

  // Set input voltage for ADC to -5 V.
  transferDAC(0x03, 0x00, 0x80);
  transferDAC(0x04, 0x00, 0x00);
  transferDAC(0x09, 0xFF, 0xFE);
  transferDAC(0x0A, 0x00, 0x05);
  transferDAC(0x10, 0x00, 0x00);

  // Issue commands to read from ADC.

  //transferADC(ADS8689_WRITE_FULL << 1, ADS8689_RST_PWRCTL_REG, 0, 0);
  //transferADC(ADS8689_NOP        << 1, 0, 0, 0);
  //transferADC(ADS8689_READ_HWORD << 1, ADS8689_RST_PWRCTL_REG, 0, 0);
  //transferADC(ADS8689_NOP        << 1, 0, 0, 0);

  //transferADC(ADS8689_WRITE_FULL << 1, ADS8689_RANGE_SEL_REG, 0, 1);
  //transferADC(ADS8689_NOP        << 1, 0, 0, 0);
  //transferADC(ADS8689_READ_HWORD << 1, ADS8689_RANGE_SEL_REG, 0, 0);
  //transferADC(ADS8689_NOP        << 1, 0, 0, 0);

  // Select ADC
  // delayMicroseconds(10);
  // digitalWrite(CS_ADC, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(CS_ADC, LOW);
  // delayMicroseconds(10);

  // // Start conversion by toggling CS/CONVST high
  // digitalWrite(CS_ADC, HIGH);
  // delayMicroseconds(10);  // Short pulse for CONVST
  // digitalWrite(CS_ADC, LOW);  // Return to CS active (low)

  // // Wait for ADC to finish conversion and read value
  // delayMicroseconds(10);
  // transferADC(ADS8689_NOP        << 1, ADS8689_DEVICE_ID_REG, 0, 0);

  // // Deselect ADC
  // digitalWrite(CS_ADC, HIGH);
}

void loop() {

}

// MARK: - Bit Banging

void bitBang_SPI_begin() {
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  digitalWrite(MOSI, 0);
  digitalWrite(SCK, 0);
}

uint8_t bitBang_SPI_transaction(uint8_t input) {
  uint8_t outputShiftRegister = 0;
  uint8_t inputShiftRegister = input;
  digitalWrite(SCK, 0); // for good measure

  for (int bitID = 0; bitID < 8; ++bitID) {
    uint8_t inputBit = inputShiftRegister >> 7;
    inputShiftRegister = inputShiftRegister << 1;
    digitalWrite(MOSI, inputBit);

    delayNanoseconds(500);
    digitalWrite(SCK, 1);

    uint8_t outputBit = digitalRead(MISO);
    outputShiftRegister = (outputShiftRegister << 1) | outputBit;

    delayNanoseconds(500);
    digitalWrite(SCK, 0);
  }

  return outputShiftRegister;
}

// MARK: - Converters

void transferADC(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3) {
  // delayNanoseconds(20000);

  // SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  // digitalWrite(CS_ADC, 0);

#if 0
  uint8_t word0 = SPI.transfer(byte0);
  uint8_t word1 = SPI.transfer(byte1);
  uint8_t word2 = SPI.transfer(byte2);
  uint8_t word3 = SPI.transfer(byte3);
#endif

#if 0
  uint32_t input = (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | (byte3 << 0);
  uint32_t output = SPI.transfer32(input);
  uint8_t word0 = (output >> 24) & 0xFF;
  uint8_t word1 = (output >> 16) & 0xFF;
  uint8_t word2 = (output >> 8) & 0xFF;
  uint8_t word3 = (output >> 0) & 0xFF;
#endif

  // delayNanoseconds(20000);
  // digitalWrite(CS_ADC, 1);
  // SPI.endTransaction();

#if 1
  uint8_t bytes[4] = { 0 };
  bytes[0] = byte0;
  bytes[1] = byte1;
  bytes[2] = byte2;
  bytes[3] = byte3;
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_ADC, 0);
  delayNanoseconds(500);

  SPI.transfer(bytes, 4);
  
  delayNanoseconds(500);
  digitalWrite(CS_ADC, 1);
  SPI.endTransaction();

  uint8_t word0 = bytes[0];
  uint8_t word1 = bytes[1];
  uint8_t word2 = bytes[2];
  uint8_t word3 = bytes[3];
#endif

  Serial.print(word0);
  Serial.print(" ");
  Serial.print(word1);
  Serial.print(" ");
  Serial.print(word2);
  Serial.print(" ");
  Serial.print(word3);
  Serial.println();
}

void transferDAC(uint8_t byte0, uint8_t byte1, uint8_t byte2) {
  delay(1);

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_DAC, 0);

  uint8_t word0 = SPI.transfer(byte0);
  uint8_t word1 = SPI.transfer(byte1);
  uint8_t word2 = SPI.transfer(byte2);

  delayMicroseconds(1);
  digitalWrite(CS_DAC, 1);
  SPI.endTransaction();

  Serial.print(word0);
  Serial.print(" ");
  Serial.print(word1);
  Serial.print(" ");
  Serial.print(word2);
  Serial.println();
}