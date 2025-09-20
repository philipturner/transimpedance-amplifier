#include <SPI.h>

uint8_t CS_DAC = 36;
uint8_t CS_ADC = 37;

void setup() {
  delay(50);

  pinMode(CS_DAC, OUTPUT);
  pinMode(CS_ADC, OUTPUT);
  digitalWrite(CS_DAC, 1);
  digitalWrite(CS_ADC, 1);

  delay(50);

  // Regular SPI usage works just fine;
  // we're testing bit-banging now.
  bitBang_SPI_begin();

  delay(50);

  transferDAC(0x03, 0x0A, 0x84);
  transferDAC(0x04, 0x00, 0x00);
  transferDAC(0x09, 0xFF, 0xFE);
  transferDAC(0x0A, 0x00, 0x05);
  transferDAC(0x10, 0xFF, 0xFF);
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
  delay(1);

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_ADC, 0);

  uint8_t word0 = SPI.transfer(byte0);
  uint8_t word1 = SPI.transfer(byte1);
  uint8_t word2 = SPI.transfer(byte2);
  uint8_t word3 = SPI.transfer(byte3);

  delayMicroseconds(1);
  digitalWrite(CS_ADC, 1);
  SPI.endTransaction();

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

  // SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_DAC, 0);

  uint8_t word0 = bitBang_SPI_transaction(byte0);
  uint8_t word1 = bitBang_SPI_transaction(byte1);
  uint8_t word2 = bitBang_SPI_transaction(byte2);

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