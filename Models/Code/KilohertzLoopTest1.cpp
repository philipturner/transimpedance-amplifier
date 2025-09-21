IntervalTimer timer;

void setup() {
  bool worked = timer.begin(kilohertzLoopIteration, 100);
  Serial.print("worked: ");
  Serial.print(worked);
  Serial.println();
}

void loop() {
  
}

// Function to execute reliably with a consistent time
// base in the multiple kHz band.
//
// TODO: Adjust this to not cause hazards regarding interrupts.
// Also track the number of skipped loop iterations, and study
// the nature of timing infidelity immediately after program
// startup.
void kilohertzLoopIteration() {
  Serial.println(micros());
}