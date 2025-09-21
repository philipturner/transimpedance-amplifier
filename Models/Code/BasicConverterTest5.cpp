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
  Serial.println(); // allow easy distinction of different program runs
  
  delay(50);

  pinMode(CS_DAC, OUTPUT);
  pinMode(CS_ADC, OUTPUT);
  digitalWrite(CS_DAC, 1);
  digitalWrite(CS_ADC, 1);

  delay(50);

  bitBang_SPI_begin();

  delay(50);

  transferADC(ADS8689_READ_HWORD << 1, ADS8689_ALARM_H_TH_REG + 2, 0, 0);
  transferADC(ADS8689_READ_HWORD << 1, ADS8689_ALARM_H_TH_REG + 2, 0, 0);
  transferADC(ADS8689_READ_HWORD << 1, ADS8689_ALARM_H_TH_REG + 0, 0, 0);
  transferADC(ADS8689_READ_HWORD << 1, ADS8689_ALARM_H_TH_REG + 0, 0, 0);
  transferADC(ADS8689_READ_HWORD << 1, ADS8689_ALARM_H_TH_REG + 2, 0, 0);
  transferADC(ADS8689_READ_HWORD << 1, ADS8689_ALARM_H_TH_REG + 2, 0, 0);
}

void loop() {

}

// MARK: - Bit Banging

// Running the following tests:
// - Regular SPI @ 1 Mbps, 4-byte batched transfer
// - Regular SPI @ 8 Mbps, 1-byte chunks
// - Bit-banged SPI @ 1 Mbps theoretical rate

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

void transferDAC(uint8_t byte0, uint8_t byte1, uint8_t byte2) {
  delay(1); // 1 ms delay, ensures DAC wait period between updates

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_DAC, 0);

  uint8_t word0 = SPI.transfer(byte0);
  uint8_t word1 = SPI.transfer(byte1);
  uint8_t word2 = SPI.transfer(byte2);

  delayMicroseconds(1); // 1 μs delay after last SCK falling capture
  digitalWrite(CS_DAC, 1);
  SPI.endTransaction();

  Serial.print(word0);
  Serial.print(" ");
  Serial.print(word1);
  Serial.print(" ");
  Serial.print(word2);
  Serial.println();
}

void transferADC(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3) {
  // Allowed delay before the Teensy unexpectedly crashes:
  // - 37 μs (3/3 attempts work)
  // - 38 μs (1/3 attempts work, all fail between checkpoint 5 and 6th ADC transfer)
  // - 39 μs (0/3 attempts work, all fail between checkpoint 2 and 3rd ADC transfer)
  //
  // Remove the checkpoint Serial diagnostics:
  // - 39 μs (1/3 attempts work, all fail between 3rd and 4th ADC transfer)
  //
  // Rearrange the order of SPI operations, add 1 μs delay after CS falling edge:
  // - 37 μs (1/1 attempts work)
  // - 38 μs (0/1 attempts work)
  // - 39 μs (0/1 attempts work)
  // - 40 μs (0/1 attempts work)
  //
  // Switch from 1 Mbps to 8 Mbps:
  // - 40 μs (3/3 attempts work)
  // - 50 μs (3/3 attempts work)
  // - 60 μs (3/3 attempts work)
  // - 65 μs (1/1 attempts work)
  // - 66 μs (1/1 attempts work)
  // - 67 μs (0/1 attempts work)
  // - 68 μs (0/1 attempts work)
  // - 70 μs (0/3 attempts work)
  //
  // Switch from 8 Mbps to 25 Mbps:
  // - 67 μs (1/1 attempts work)
  // - 68 μs (1/1 attempts work)
  // - 69 μs (1/3 attempts work, all fail between 4th and 5th ADC transfer)
  // - 70 μs (0/1 attempts work, all fail between 3rd and 4th ADC transfer)
  //
  // Switch from 4-byte batched transfer to 1-byte chunks:
  // - 69 μs (3/3 attempts work)
  // - 70 μs (0/3 attempts work, all fail between 3rd and 4th ADC transfer)
  //
  // Migration to 1 Mbps bit-banging:
  // - 30 μs (1/1 attempts work)
  // - 32 μs (1/1 attempts work)
  // - 33 μs (1/1 attempts work)
  // - 34 μs (3/3 attempts work)
  // - 35 μs (0/3 attempts work, all fail between 3rd and 4th ADC transfer)
  // - 40 μs (0/1 attempts work, all fail between 3rd and 4th ADC transfer)
  //
  // VREFIO on ADC after a textbook Teensy crash: 4.098 V
  // After probing the bypass capacitor for the digital side of the ADC
  // isolator, the Teensy fired up again (pin 13 / SCK blinked) and the
  // last 3 lines transmitted to the Serial Monitor. Interesting behavior.
  // The bypass capacitor read ~3.3 V, although it was measured after
  // the fire up happened.
  delayMicroseconds(100);

  digitalWrite(CS_ADC, 0);

  uint8_t word0 = bitBang_SPI_transaction(byte0);
  uint8_t word1 = bitBang_SPI_transaction(byte1);
  uint8_t word2 = bitBang_SPI_transaction(byte2);
  uint8_t word3 = bitBang_SPI_transaction(byte3);
  
  delayMicroseconds(1); // 1 μs delay after last SCK falling capture
  digitalWrite(CS_ADC, 1);

  Serial.print(word0);
  Serial.print(" ");
  Serial.print(word1);
  Serial.print(" ");
  Serial.print(word2);
  Serial.print(" ");
  Serial.print(word3);
  Serial.println();
}