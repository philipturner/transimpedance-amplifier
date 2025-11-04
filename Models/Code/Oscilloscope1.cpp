// High-fidelity timer for Teensy
//
// TODO: Annotate the timing of operations within the 50 kHz loop.

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

////////////////////////////////////////////////////////////////////////////////

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

  // Do any other setup here.

  latestTimestamp = micros();
  timer.begin(kilohertzLoop, 20);
}

// MARK: - Loop

void loop() {
  timeFidelityDiagnosticLoop();
}

// Responsiveness test
//
// Set the range select register to some different values,
// read back the result.
//
// During normal program operation, the responsiveness test does
// not execute. The range select register is set to 0 before the kilohertz
// loop starts.
void adcResponsiveDiagnosticLoop() {
  // TODO: Copy over code once it can compile.
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
