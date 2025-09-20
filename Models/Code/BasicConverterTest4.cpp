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

  // Set the DAC output voltage to -1 V
  transferDAC(0x03, 0x00, 0x80);
  transferDAC(0x04, 0x00, 0x00);
  transferDAC(0x09, 0xFF, 0xFE);
  transferDAC(0x0A, 0x00, 0x05);
  transferDAC(0x10, 0x66, 0x66);

  // 100 MΩ transimpedance amplifier

  // DUT =   25 MΩ | voltage = 3.962 V
  // DUT =   50 MΩ | voltage = 1.979 V
  // DUT =  100 MΩ | voltage = 0.994 V
  // DUT =  250 MΩ | voltage = 0.396 V
  // DUT =  500 MΩ | voltage = 0.198 V
  // DUT = 1000 MΩ | voltage = 0.101 V
}

void loop() {

}

// MARK: - Converters

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