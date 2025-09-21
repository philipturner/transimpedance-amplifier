IntervalTimer timer;

struct TimeStatistics {
  uint32_t above1000000us_jumps = 0;
  uint32_t above100000us_jumps = 0;
  uint32_t above10000us_jumps = 0;
  uint32_t above1000us_jumps = 0;
  uint32_t above100us_jumps = 0;
  uint32_t above10us_jumps = 0;
  uint32_t above2us_jumps = 0;
  uint32_t exactly2us_jumps = 0;
  uint32_t under2us_jumps = 0;
  uint32_t total_jumps = 0;
};
TimeStatistics timeStatistics;

uint32_t latestTimestamp;

void setup() {
  latestTimestamp = micros();
  timer.begin(kilohertzLoopIteration, 2);
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
  Serial.println(timeStatistics.above2us_jumps);
  Serial.println(timeStatistics.exactly2us_jumps);
  Serial.println(timeStatistics.under2us_jumps);
  Serial.println(float(timeStatistics.total_jumps) / float(500000));

  // Time base: 2 μs / 500 kHz
  //
  //  9 above 2 μs, 4 under 2 μs, 25 s test duration
  //  6 above 2 μs, 2 under 2 μs, 65 s test duration
  //  8 above 2 μs, 2 under 2 μs, 25 s test duration
  //  8 above 2 μs, 3 under 2 μs,  3 s test duration, no more faults by 20 s
  // 12 above 2 μs, 5 under 2 μs,  1 s test duration, no more faults by 40 s
  //
  // No behavior deviating significantly from the above after 50 additional tests.
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
  } else if (jumpDuration > 2) {
    timeStatistics.above2us_jumps += 1;
  } else if (jumpDuration == 2) {
    timeStatistics.exactly2us_jumps += 1;
  } else if (jumpDuration < 2) {
    timeStatistics.under2us_jumps += 1;
  }
  timeStatistics.total_jumps += 1;
}