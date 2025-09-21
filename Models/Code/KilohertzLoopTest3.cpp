IntervalTimer timer;

struct TimeStatistics {
  uint32_t above1000000us_jumps = 0;
  uint32_t above100000us_jumps = 0;
  uint32_t above10000us_jumps = 0;
  uint32_t above1000us_jumps = 0;
  uint32_t above100us_jumps = 0;
  uint32_t above10us_jumps = 0;
  uint32_t exactly10us_jumps = 0;
  uint32_t under10us_jumps = 0;
  uint32_t total_jumps = 0;
};
TimeStatistics timeStatistics;

uint32_t latestTimestamp;

void setup() {
  latestTimestamp = micros();
  timer.begin(kilohertzLoopIteration, 10);
}

void loop() {
  delay(500);

  Serial.println();
  Serial.println(timeStatistics.above1000000us_jumps);
  Serial.println(timeStatistics.above100000us_jumps);
  Serial.println(timeStatistics.above10000us_jumps);
  Serial.println(timeStatistics.above1000us_jumps);
  Serial.println(timeStatistics.above100us_jumps);
  Serial.println(timeStatistics.above10us_jumps);
  Serial.println(timeStatistics.exactly10us_jumps);
  Serial.println(timeStatistics.under10us_jumps);
  Serial.println(timeStatistics.total_jumps);

  // Results:
  //
  // above     1 s  - 0
  // above   100 ms - 0
  // above    10 ms - 0
  // above     1 ms - 0
  // above   100 μs - 0
  // above    10 μs - 3
  // exactly  10 μs - 6800062
  // under    10 μs - 2
  //
  // total          - 6800067
  //
  // Time base: 10 μs / 100 kHz
  // Test duration: 68 s

  // One of the first tests (first time the program loaded, before
  // restarting with a physical button press), the statistics had
  // a few more faults. 4 above 10 μs, 3 below μs, 6 s test duration.
  //
  // All of the faults for each test (both 6 s and 68 s) happen
  // within the 1st second. This is not a guarantee that an error
  // will never happen, but it does show the typical behavior of
  // the error distribution.
  //
  // I can rely on integer tracking of micros(), without any
  // floating-point jitter problems from tracking the 120 Hz
  // time base on molecular-renderer on macOS. At least with
  // time timestamp query being placed carefully at the very
  // start of the loop iteration, as done here.
  //
  // Perhaps one day, the centerpoint of the jitter lands on
  // the +/- right on the boundary of a rounding point. To
  // test for this, I should run another test at 500 kHz.
  // Execute the test a very large number of times, to prove
  // the flaky rounding problem will not happen beyond any
  // reasonable doubt.
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
  } else if (jumpDuration > 10) {
    timeStatistics.above10us_jumps += 1;
  } else if (jumpDuration == 10) {
    timeStatistics.exactly10us_jumps += 1;
  } else if (jumpDuration < 10) {
    timeStatistics.under10us_jumps += 1;
  }
  timeStatistics.total_jumps += 1;
}