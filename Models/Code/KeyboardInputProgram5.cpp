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

  // Test setup:
  //
  // 15 V / 1 GΩ = 15 nA
  // (0 V) - (15 nA)(100 MΩ) = -1.5 V
  //
  // ADC readings
  // - range 0b0000 (-12.288 V to 12.288 V) 
  //   - 0.438 full scale
  //   - translates to -1.524 V, rounding error 0.020 V
  // - range 0b0001 (-10.240 V to 10.240 V)
  //   - 0.425 full scale
  //   - translates to -1.536 V, rounding error 0.020 V
  // - varies randomly by <0.001 full scale between consecutive measurements
  //   - six-sigma noise < 20 mV

  // Safe SPI bitrate for ADC + ADC isolator system:
  //
  //  1 Mbps - YES
  //  2 Mbps - YES
  //  3 Mbps - YES
  //  4 Mbps - YES
  //  6 Mbps - YES
  //  8 Mbps - YES
  // 12 Mbps - YES
  // 16 Mbps - YES
  // 17 Mbps - YES
  // 18 Mbps - YES
  // 19 Mbps - YES
  // 20 Mbps - YES
  // 21 Mbps - YES
  // 22 Mbps - NO
  // 23 Mbps - NO
  // 24 Mbps - NO
  // 25 Mbps - NO
  // 30 Mbps - NO
  // 40 Mbps - NO
  // 50 Mbps - NO
  // 60 Mbps - NO
  // 65 Mbps - NO
  // 66 Mbps - NO
  // 67 Mbps - NO

  // All attempts to vary up the timing, vary the delay lengths,
  // and introduce extra delays did not change the results. At
  // 21 Mbps and below, it always works. At 22 Mbps and above,
  // it always fails.

  // Since the error comes from dropping the 1st bit, perhaps
  // switching to a different SPI mode (CPHA = 1) will fix it.
  // However, that complicates the code. The initial setup
  // functions would need to use a different SPI timing setup
  // than the measurements. For the moment, 21 Mbps is good
  // enough, so I won't investigate this.

  // Narrowing down the exact frequency to as many digits as practical:
  //
  // 21 Mbps - YES
  // 21.5 Mbps - YES
  // 21.6 Mbps - YES
  // 21.7 Mbps - YES
  // 21.8 Mbps - YES
  // 21.81 Mbps - YES
  // 21.815 Mbps - YES
  // 21.816 Mbps - YES
  // 21.817 Mbps - YES
  // 21.818 Mbps - YES
  // 21.8181 Mbps - YES
  // 21.81815 Mbps - YES
  // 21.818178 Mbps - YES
  // 21.818179 Mbps - YES
  // 21.818180 Mbps - YES
  // 21.818181 Mbps - NO
  // 21.818182 Mbps - NO
  // 21.818183 Mbps - NO
  // 21.818185 Mbps - NO
  // 21.81819 Mbps - NO
  // 21.8182 Mbps - NO
  // 21.8185 Mbps - NO
  // 21.819 Mbps - NO
  // 21.82 Mbps - NO
  // 21.83 Mbps - NO
  // 21.84 Mbps - NO
  // 21.85 Mbps - NO
  // 21.9 Mbps - NO
  // 22.0 Mbps - NO
  // 22.1 Mbps - NO
  // 22.2 Mbps - NO

  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_ADC, 0);
  delayNanoseconds(100);

  // 32 bits at 20 MHz, 1600 ns delay
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
// 'c' received - ADC performs a measurement
// 'd' received - ADC reports contents of ADS8689_RANGE_SEL_REG
// '0' received - ADC writes 0b0000 to ADS8689_RANGE_SEL_REG
// '1' received - ADC writes 0b0001 to ADS8689_RANGE_SEL_REG
void loop() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    if (incomingByte == 'c') {
      Serial.println("received command 'c'");
      responseC();
    } else if (incomingByte == 'd') {
      Serial.println("received command 'd'");
      responseD();
    } else if (incomingByte == '0') {
      Serial.println("received command '0'");
      responseB(0b0000);
    } else if (incomingByte == '1') {
      Serial.println("received command '1'");
      responseB(0b0001);
    }
  }
}

// MARK: - Keyboard Events

// Legend
//
// range 0b0000 | -12.288 V to 12.288 V
// range 0b0001 | -10.24 V to 10.24 V
// range 0b0010 | -6.144 V to 6.144 V
// range 0b0011 | -5.12 V to 5.12 V
// range 0b0100 | -2.56 V to 2.56 V
// range 0b1000 | 0 V to 12.288 V
// range 0b1001 | 0 V to 10.24 V
// range 0b1010 | 0 V to 6.144 V
// range 0b1011 | 0 V to 5.12 V

void responseB(uint8_t rangeCode) {
  // frame 0
  {
    ADCInput input;
    input.command = ADS8689_WRITE_FULL;
    input.registerAddress = ADS8689_RANGE_SEL_REG;
    input.data = uint16_t(rangeCode);
    transferADC(input);
  }
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
    Serial.println(output.data, 6); // force it to 6 decimal places
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
    input.registerAddress = ADS8689_RANGE_SEL_REG + 2;
    input.data = 0;
    transferADC(input);
  }

  // frame 1
  {
    ADCInput input;
    input.command = ADS8689_READ_HWORD;
    input.registerAddress = ADS8689_RANGE_SEL_REG;
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