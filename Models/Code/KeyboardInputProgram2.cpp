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

// MARK: - Data Structures

struct DACInput {
  uint8_t registerAddress;
  uint16_t data;
};

struct ADCInput {
  uint8_t command;
  uint8_t registerAddress;
  uint16_t data;
};

struct ADCOutputHWORD {
  uint16_t data;

  ADCOutputHWORD(uint32_t rawData) {
    data = rawData >> 16;
  }
};

struct ADCOutputConversion {
  // Fractional value from 0.0 to 1.0 full-scale.
  float data;

  ADCOutputConversion(uint32_t rawData) {
    uint32_t integer18Bit = rawData >> 14;
    uint32_t denominator = 1 << 18;
    data = float(integer18Bit) / float(denominator);
  }
};

// MARK: - Utility Functions

void transferDAC(DACInput input) {
  uint8_t bytes[3];
  bytes[0] = input.registerAddress;
  bytes[1] = uint8_t(input.data >> 8);
  bytes[2] = uint8_t(input.data >> 0);

  // Guarantee that the DAC output wait time has passed.
  delayNanoseconds(3000);

  SPI.beginTransaction(SPISettings(25000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_DAC, 0);
  delayNanoseconds(100);

  // 24 bits at 25 MHz, 960 ns delay
  SPI.transfer(bytes, 3);

  delayNanoseconds(100);
  digitalWrite(CS_DAC, 1);
  SPI.endTransaction();
}

uint32_t transferADC(ADCInput input) {
  uint8_t bytes[4];
  bytes[0] = input.command << 1;
  bytes[1] = input.registerAddress;
  bytes[2] = uint8_t(input.data >> 8);
  bytes[3] = uint8_t(input.data >> 0);
  
  // Guarantee that enough conversion time has passed.
  delayNanoseconds(6000);

  SPI.beginTransaction(SPISettings(25000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_ADC, 0);
  delayNanoseconds(100);

  // 32 bits at 25 MHz, 1280 ns delay
  SPI.transfer(bytes, 4);

  // Minimize EMI from digital signals while analog is acquiring.
  delayNanoseconds(6000);
  digitalWrite(CS_ADC, 1);
  SPI.endTransaction();

  uint32_t output = 0;
  output = (output << 8) | bytes[0];
  output = (output << 8) | bytes[1];
  output = (output << 8) | bytes[2];
  output = (output << 8) | bytes[3];
  return output;
}

// MARK: - Top-Level Program

void setup() {
  Serial.begin(0);
  Serial.println(); // allow easy distinction of different program runs
  Serial.println("Serial Monitor has initialized.");
  Serial.flush();

  SPI.begin();

  // Clear the DAC SPICONFIG register, except for the one reserved bit.
  {
    DACInput input;
    input.registerAddress = 0x03;
    input.data = 0x0080;
    transferDAC(input);
  }

  // Clear the DAC GENCONFIG register, turning on the voltage reference.
  {
    DACInput input;
    input.registerAddress = 0x04;
    input.data = 0x0000;
    transferDAC(input);
  }

  // Clear the DAC DACPWDWN register, turning on the output.
  {
    DACInput input;
    input.registerAddress = 0x09;
    input.data = 0xFFFE;
    transferDAC(input);
  }

  // Configure the DAC DACRANGE register to span from -5 V to 5 V.
  {
    DACInput input;
    input.registerAddress = 0x0A;
    input.data = 0b0101;
    transferDAC(input);
  }

  Serial.println("Setup process has completed.");
}

// Keyboard events:
// 'a' received - DAC outputs -1 V
// 'b' received - DAC outputs +1 V
// 'c' received - ADC performs a measurement (2 SPI transactions)
// 'd' received - ADC reports contents of ADS8689_ALARM_H_TH_REG (3 SPI transactions)
void loop() {
  
}
