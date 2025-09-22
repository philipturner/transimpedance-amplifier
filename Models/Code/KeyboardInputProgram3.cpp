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

  pinMode(CS_DAC, OUTPUT);
  pinMode(CS_ADC, OUTPUT);
  digitalWrite(CS_DAC, 1);
  digitalWrite(CS_ADC, 1);

  SPI.begin(); 
}

// Keyboard events:
// 'c' received - ADC performs a measurement (2 SPI transactions)
// 'd' received - ADC reports contents of ADS8689_ALARM_H_TH_REG (3 SPI transactions)
void loop() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    if (incomingByte == 'c') {
      Serial.println("received command 'c'");
      responseC();
    } else if (incomingByte == 'd') {
      Serial.println("received command 'd'");
      responseD();
    }
  }
}

// MARK: - Keyboard Events

// Data transfer process:
//
// frame 0 | input conversion command (NOP) | output ignored
// frame 1 | NOP                            | output receive ADC code

void responseC() {
  // frame 0
  {
    ADCInput input;
    input.command = ADS8689_NOP;
    input.registerAddress = 0;
    input.data = 0;
    transferADC(input);
  }

  // frame 1
  {
    ADCInput input;
    input.command = ADS8689_NOP;
    input.registerAddress = 0;
    input.data = 0;
    uint32_t rawData = transferADC(input);

    ADCOutputConversion output(rawData);
    Serial.print("ADC code (fraction of full-scale): ");
    Serial.println(output.data);
  }
}

// Data transfer process:
//
// frame 0 | input read highest 16 bits | output ignored
// frame 1 | input read lowest 16 bits  | output receive highest 16 bits
// frame 2 | NOP                        | output receive lowest 16 bits

void responseD() {
  // frame 0
  {
    ADCInput input;
    input.command = ADS8689_READ_HWORD;
    input.registerAddress = ADS8689_ALARM_H_TH_REG + 2;
    input.data = 0;
    transferADC(input);
  }

  // frame 1
  {
    ADCInput input;
    input.command = ADS8689_READ_HWORD;
    input.registerAddress = ADS8689_ALARM_H_TH_REG;
    input.data = 0;
    uint32_t rawData = transferADC(input);

    ADCOutputHWORD output(rawData);
    Serial.print("Highest 16 bits of register: ");
    Serial.println(output.data);
  }

  // frame 2
  {
    ADCInput input;
    input.command = ADS8689_NOP;
    input.registerAddress = 0;
    input.data = 0;
    uint32_t rawData = transferADC(input);

    ADCOutputHWORD output(rawData);
    Serial.print("Lowest 16 bits of register: ");
    Serial.println(output.data);
  }
}