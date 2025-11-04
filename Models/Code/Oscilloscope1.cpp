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

////////////////////////////////////////////////////////////////////////////////

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
};
TimeStatistics timeStatistics;

uint32_t latestTimestamp;

void setup() {
  Serial.begin(0);
  latestTimestamp = micros();
  timer.begin(kilohertzLoopIteration, 20);
}

void loop() {
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

// Function to execute reliably with a consistent time
// base in the multiple kHz band.
void kilohertzLoopIteration() {
  uint32_t currentTimestamp = micros();
  uint32_t previousTimestamp = latestTimestamp;
  latestTimestamp = currentTimestamp;

  uint32_t jumpDuration = currentTimestamp - previousTimestamp;
  if (jumpDuration > 1000000) {
    timeStatistics.above1000000us_jumps += 1;
  } else if (jumpDuration > 100000) {
    timeStatistics.above100000us_jumps += 1;
  } else if (jumpDuration > 10000) {
    timeStatistics.above10000us_jumps += 1;
  } else if (jumpDuration > 1000) {
    timeStatistics.above1000us_jumps += 1;
  } else if (jumpDuration > 100) {
    timeStatistics.above100us_jumps += 1;
  } else if (jumpDuration > 20) {
    timeStatistics.above20us_jumps += 1;
  } else if (jumpDuration == 20) {
    timeStatistics.exactly20us_jumps += 1;
  } else if (jumpDuration < 20) {
    timeStatistics.under20us_jumps += 1;
  }
  timeStatistics.total_jumps += 1;
}
