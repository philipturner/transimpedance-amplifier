#include <SPI.h>

// High-fidelity timer for Teensy
// 50 Ksps voltage measurement from 18-bit ADC
//
// 6000 ns breathing room for CONV
//         write CS low
//  100 ns breathing room to start SPI transfer
// 1600 ns transfer 32 bits @ 20 Mbps
// 6000 ns breathing room for ACQ
//         write CS high
// 6300 ns theoretical time left for other code
//
// WARNING: Good chance the periodic writes to the serial plotter will
// interrupt a loop iteration, especially with the long idle times.
// Little time left after 'kilohertzLoop' finishes for 'loop' to run.
// We will make the code robust to skipped frames.

// Displaying data to console
// - Measure timestamps, auto-detect skipped frames. Fill in skipped frames with
//   previous sensor value.
// - Neatly format the time and measured voltage, with a consistent spacing.
//   Display everything as text to the console, as we cannot display a UI
//   graph (unfortunately).
// - The exact subregion of the data stream to display is a non-trivial
//   problem, even for physical oscilloscopes.
// - Program two modes: text (high resolution), graphical (low resolution)
//
// Exploring the "Serial Plotter" tool right now.
//
// Plan:
//
// There are two display modes, averaging "a" and latest "l". They are
// enabled and switched by keyboard commands. Every 20 ms (50 Hz, 1000
// samples @ 50 KHz), "loop" is invoked. If the multiple of 20 ms is
// not even, it may backfill the history to appear without hitches.
//
// Averaging mode tracks the min, avg, and max of every group of 50
// samples. We take the counter incremented by the kilohertz loop as
// the source of truth for the current time. Only process fully
// completed groups of 50 samples, leaving the current (uncompleted)
// group unhandled.
//
// Latest mode takes a single snapshot of the last 1000 samples, then
// automatically toggles off. Write exactly 1000 samples, hopefully
// reducing the number of variables graphed from 3 to 1. Don't worry
// about transitions back from "l" to "a".
//
// "0" resets the mode to none, so nothing is displayed to the plotter.
//
// Test this functionality with a 1 kHz, +/-10 V sine wave following the
// true timestamp in the kilohertz loop. Then try a static 1.5 V input to
// the ADC, which varies temporally from random noise.
//
// Perhaps there are "data races" from the kilohertz loop interrupt
// routine. Anticipate these with a boolean "lock" variable in the data
// set. Initially, when we detect conflicts, the program will crash.
// Later, the kilohertz loop will return early, effectively creating a
// skipped frame. We want to hold the lock for as short as possible,
// perhaps only enough to copy relevant data into a safe buffer. Then
// transfer over USB serial with the lock not held.



// Teensy 4.1 has 1 MB of RAM. We'll use 200 KB to store 50,000 samples
// at 4 bytes/sample. This is a ring buffer, 1 second of history @ 50 Ksps.
// It's up to the code in 'loop()' to format outputs from this source of truth.
// We can logically separate the display code from the tight loop that
// actually gathers data.
//
// The internal loop will attempt to back-fill any skipped frames. It
// will write to the buffer in chronological order. If the number of
// skipped frames is unreasonably high, crash with an error.

// Fixing the X-axis limits to the Arduino IDE's serial plotter:
// https://www.open-electronics.org/how-to-adjust-x-and-y-axis-scale-in-arduino-serial-plotter-no-extra-software-needed/
// Successfully changed it from 50 to 1000 data points

// Eventually, we will probably need to split this into multiple files:
// https://arduino.github.io/arduino-cli/1.3/sketch-build-process/#pre-processing

// Tasks:
// - Split the program into multiple files.
//   - Call it "Oscilloscope2".
//   - Perhaps download the repo and do the Arduino project in a folder named
//     "Models/Code/Oscilloscope2".
//   - Hopefully ".h" files show up in the editor.
//   - Begin with a simple, fresh project that doesn't have any code from
//     "Oscilloscope1" copied over yet.
// - Figure out a good way to crash the program.

////////////////////////////////////////////////////////////////////////////////

// MARK: - SPI Utilities

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

struct ADC {
  static uint32_t transfer(ADCInput input) {
    uint8_t bytes[4];
    bytes[0] = input.command << 1;
    bytes[1] = input.registerAddress;
    bytes[2] = uint8_t(input.data >> 8);
    bytes[3] = uint8_t(input.data >> 0);
    
    // Guarantee that enough conversion time has passed.
    delayNanoseconds(6000);

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

  static void nop() {
    ADCInput input;
    input.command = ADS8689_NOP;
    input.registerAddress = 0;
    input.data = 0;
    transfer(input);
  }

  static float readConversionCode() {
    ADCInput input;
    input.command = ADS8689_NOP;
    input.registerAddress = 0;
    input.data = 0;
    uint32_t rawData = transfer(input);

    ADCOutputConversion output(rawData);
    return output.data;
  }

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
  static void writeRangeSelect(uint8_t rangeCode) {
    ADCInput input;
    input.command = ADS8689_WRITE_FULL;
    input.registerAddress = ADS8689_RANGE_SEL_REG;
    input.data = uint16_t(rangeCode);
    transfer(input);
  }

  // Data transfer process:
  //
  // frame 0 | input read highest 16 bits | output ignored
  // frame 1 | input read lowest 16 bits  | output receive highest 16 bits
  // frame 2 | NOP                        | output receive lowest 16 bits
  static uint32_t readRangeSelect() {
    uint32_t upperBits;
    uint32_t lowerBits;

    // frame 0
    {
      ADCInput input;
      input.command = ADS8689_READ_HWORD;
      input.registerAddress = ADS8689_RANGE_SEL_REG + 2;
      input.data = 0;
      transfer(input);
    }

    // frame 1
    {
      ADCInput input;
      input.command = ADS8689_READ_HWORD;
      input.registerAddress = ADS8689_RANGE_SEL_REG;
      input.data = 0;
      uint32_t rawData = transfer(input);

      ADCOutputHWORD output(rawData);
      upperBits = uint32_t(output.data);
    }

    // frame 2
    {
      ADCInput input;
      input.command = ADS8689_NOP;
      input.registerAddress = 0;
      input.data = 0;
      uint32_t rawData = transfer(input);

      ADCOutputHWORD output(rawData);
      lowerBits = uint32_t(output.data);
    }

    return (upperBits << 16) | lowerBits;
  }
};

// MARK: - Time Tracking Utilities

IntervalTimer timer;

struct TimeStatistics {
  uint32_t above1000000us_jumps = 0;
  uint32_t above100000us_jumps = 0;
  uint32_t above10000us_jumps = 0;
  uint32_t above1000us_jumps = 0;
  uint32_t above100us_jumps = 0;
  uint32_t above20us_jumps = 0;
  uint32_t exactly20us_jumps = 0;
  uint32_t under20us_jumps = 0;
  uint32_t total_jumps = 0;

  void integrate(uint32_t jumpDuration) {
    if (jumpDuration > 1000000) {
      this->above1000000us_jumps += 1;
    } else if (jumpDuration > 100000) {
      this->above100000us_jumps += 1;
    } else if (jumpDuration > 10000) {
      this->above10000us_jumps += 1;
    } else if (jumpDuration > 1000) {
      this->above1000us_jumps += 1;
    } else if (jumpDuration > 100) {
      this->above100us_jumps += 1;
    } else if (jumpDuration > 20) {
      this->above20us_jumps += 1;
    } else if (jumpDuration == 20) {
      this->exactly20us_jumps += 1;
    } else if (jumpDuration < 20) {
      this->under20us_jumps += 1;
    }
    this->total_jumps += 1;
  }
};
TimeStatistics timeStatistics;

uint32_t latestTimestamp;

// MARK: - Setup

void setup() {
  Serial.begin(0);
  Serial.println(); // allow easy distinction of different program runs
  Serial.println("Serial Monitor has initialized.");

  pinMode(CS_DAC, OUTPUT);
  pinMode(CS_ADC, OUTPUT);
  digitalWrite(CS_DAC, 1);
  digitalWrite(CS_ADC, 1);
  SPI.begin(); 

  ADC::writeRangeSelect(0b0000);

  latestTimestamp = micros();
  timer.begin(kilohertzLoop, 20);
}

// MARK: - Loop

void loop() {
  adcResponsiveDiagnosticLoop();
}

void timeFidelityDiagnosticLoop() {
  delay(500);

  Serial.println();
  Serial.println(timeStatistics.above1000000us_jumps);
  Serial.println(timeStatistics.above100000us_jumps);
  Serial.println(timeStatistics.above10000us_jumps);
  Serial.println(timeStatistics.above1000us_jumps);
  Serial.println(timeStatistics.above100us_jumps);
  Serial.println(timeStatistics.above20us_jumps);
  Serial.println(timeStatistics.exactly20us_jumps);
  Serial.println(timeStatistics.under20us_jumps);
  Serial.println(timeStatistics.total_jumps);
}

// Responsiveness test
//
// Keyboard events:
// 'c' received - ADC performs a measurement
// 'd' received - ADC reports contents of ADS8689_RANGE_SEL_REG
// '0' received - ADC writes 0b0000 to ADS8689_RANGE_SEL_REG
// '1' received - ADC writes 0b0001 to ADS8689_RANGE_SEL_REG
//
// During normal program operation, the responsiveness test does
// not execute. The range select register is set to 0 before the kilohertz
// loop starts.
void adcResponsiveDiagnosticLoop() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    if (incomingByte == 'c') {
      Serial.println("received command 'c'");

      float voltage = ADC::readConversionCode();
      Serial.print("ADC code (fraction of full-scale): ");
      Serial.println(voltage, 6); // force it to 6 decimal places
    } else if (incomingByte == 'd') {
      Serial.println("received command 'd'");
      
      uint32_t rangeCode = ADC::readRangeSelect();
      Serial.print("Contents of range select register: ");
      Serial.println(rangeCode);
    } else if (incomingByte == '0') {
      Serial.println("received command '0'");
      
      ADC::writeRangeSelect(0b0000);
    } else if (incomingByte == '1') {
      Serial.println("received command '1'");
      
      ADC::writeRangeSelect(0b0001);
    }
  }
}

// MARK: - Kilohertz Loop

// Function to execute reliably with a consistent time
// base in the multiple kHz band.
void kilohertzLoop() {
  uint32_t currentTimestamp = micros();
  uint32_t previousTimestamp = latestTimestamp;
  latestTimestamp = currentTimestamp;

  uint32_t jumpDuration = currentTimestamp - previousTimestamp;
  timeStatistics.integrate(jumpDuration);
}
