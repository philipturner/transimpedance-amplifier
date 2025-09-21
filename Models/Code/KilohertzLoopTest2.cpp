IntervalTimer timer;

struct TimeStatistics {
  uint32_t above1000000us_jumps = 0;
  uint32_t above100000us_jumps = 0;
  uint32_t above10000us_jumps = 0;
  uint32_t above1000us_jumps = 0;
  uint32_t above100us_jumps = 0;
  uint32_t exactly100us_jumps = 0;
  uint32_t under100us_jumps = 0;
  uint32_t total_jumps = 0;
};
TimeStatistics timeStatistics;

uint32_t latestTimestamp;

void setup() {
  latestTimestamp = micros();
  timer.begin(kilohertzLoopIteration, 100);
}

void loop() {
  delay(500);

  Serial.println();
  Serial.println(timeStatistics.above1000000us_jumps);
  Serial.println(timeStatistics.above100000us_jumps);
  Serial.println(timeStatistics.above10000us_jumps);
  Serial.println(timeStatistics.above1000us_jumps);
  Serial.println(timeStatistics.above100us_jumps);
  Serial.println(timeStatistics.exactly100us_jumps);
  Serial.println(timeStatistics.under100us_jumps);
  Serial.println(timeStatistics.total_jumps);

  // Results:
  //
  // above     1 s  - 0
  // above   100 ms - 0
  // above    10 ms - 0
  // above     1 ms - 0
  // above   100 μs - 3
  // exactly 100 μs - 589999
  // under   100 μs - 2
  //
  // total          - 590004
  //
  // Time base: 100 μs / 10 kHz
  // Test duration: 59 s 
  //
  // Next analysis should run at 100 kHz to see how
  // fidelity degrades with varying frequency.
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
  } else if (jumpDuration == 100) {
    timeStatistics.exactly100us_jumps += 1;
  } else if (jumpDuration < 100) {
    timeStatistics.under100us_jumps += 1;
  }
  timeStatistics.total_jumps += 1;
}