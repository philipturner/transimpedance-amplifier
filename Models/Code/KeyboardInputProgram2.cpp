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
  //delayNanoseconds(3000);
  delay(1);

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_DAC, 0);
  //delayNanoseconds(100);

  // 24 bits at 25 MHz, 960 ns delay
  SPI.transfer(input.registerAddress);
  SPI.transfer(uint8_t(input.data >> 8));
  SPI.transfer(uint8_t(input.data >> 0));

  delayNanoseconds(1000);
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
  delay(10);
  pinMode(CS_DAC, OUTPUT);
  pinMode(CS_ADC, OUTPUT);
  digitalWrite(CS_DAC, 1);
  digitalWrite(CS_ADC, 1);
  delay(10);

  SPI.begin();
  delay(10);

  // Clear the DAC SPICONFIG register, except for the one reserved bit.
  {
    DACInput input;
    input.registerAddress = 0x03;
    input.data = 0x0A84;
    transferDAC(input);
  }
  delay(10);

  // Clear the DAC GENCONFIG register, turning on the voltage reference.
  {
    DACInput input;
    input.registerAddress = 0x04;
    input.data = 0x0000;
    transferDAC(input);
  }
  delay(10);

  // Clear the DAC DACPWDWN register, turning on the output.
  {
    DACInput input;
    input.registerAddress = 0x09;
    input.data = 0xFFFF;
    transferDAC(input);
  }
  delay(10);

  // Configure the DAC DACRANGE register to span from -5 V to 5 V.
  {
    DACInput input;
    input.registerAddress = 0x0A;
    input.data = 0x0005;
    transferDAC(input);
  }
  delay(10);

  {
    DACInput input;
    input.registerAddress = 0x10;
    input.data = 0x6666;
    transferDAC(input);
  }

  Serial.begin(0);
  Serial.println(); // allow easy distinction of different program runs
  Serial.println("Serial Monitor has initialized.");
}

// Keyboard events:
// 'a' received - DAC outputs -1 V
// 'b' received - DAC outputs +1 V
// 'c' received - ADC performs a measurement (2 SPI transactions)
// 'd' received - ADC reports contents of ADS8689_ALARM_H_TH_REG (3 SPI transactions)
void loop() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    if (incomingByte == 'a') {
      Serial.println("received command 'a'");
      outputDACVoltage(-1.0);
    } else if (incomingByte == 'b') {
      Serial.println("received command 'b'");
      outputDACVoltage(1.0);
    } else if (incomingByte == 'c') {
      Serial.println("received command 'c'");
      responseC();
    } else if (incomingByte == 'd') {
      Serial.println("received command 'd'");
      responseD();
    }
  }
}

// MARK: - Keyboard Events

void outputDACVoltage(float voltage) {
  Serial.print("DAC voltage: ");
  Serial.println(voltage);

  float fractionFullScale = voltage - float(-5);
  fractionFullScale /= float(5) - float(-5);
  Serial.print("DAC code (fraction of full-scale): ");
  Serial.println(fractionFullScale);

  DACInput input;
  input.registerAddress = 0x10;
  input.data = uint16_t(fractionFullScale * 65536);
  transferDAC(input);
  Serial.print("DAC code: ");
  Serial.println(input.data);
}

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