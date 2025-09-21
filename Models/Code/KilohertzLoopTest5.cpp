IntervalTimer timer;
uint32_t programStartTimestamp;
uint32_t counter = 0;

void setup() {
  programStartTimestamp = micros();
  timer.begin(kilohertzLoopIteration, 2);

  // test shorter times as well
  //   2 μs | 3/3 attempts work
  //   5 μs | 3/3 attempts work
  //  20 μs | 3/3 attempts work
  //  30 μs | 3/3 attempts work
  //  70 μs | 3/3 attempts work
  //  74 μs | 10/10 attempts work
  //  75 μs | 0/5 attempts work, stops at counter = 4 every time
  //  76 μs | 0/5 attempts work, stops at counter = 4 every time
  //  77 μs | 0/5 attempts work, stops at counter = 3 every time
  //  80 μs | 0/3 attempts work
  // 100 μs | 0/3 attempts work
  //   1 ms | 1/10 attempts work
  //  10 ms | 0/3 attempts work
  //  15 ms | 0/3 attempts work
  //  18 ms | 1/4 attempts work, one didn't print anything to console
  //  20 ms | 0/3 attempts work, strange return at counter = 47-48
  //  22 ms | 0/3 attempts work, strange return at counter = 43-45
  //  24 ms | 0/3 attempts work, strange return at counter = 39-40
  //  30 ms | 3/3 attempts work
  // 100 ms | 3/3 attempts work
  // 500 ms | 3/3 attempts work
  //
  // For the 100 μs tests, 4 lines (counter = 0, 1, 2, 3) are displayed
  // and then the program freezes. The freeze only happens when a fresh
  // 'Upload' is performed in the Arduino IDE. If the program launch
  // instead occurs from a button press, it proceeds to completion.
  // This is an important detail.
  //
  // The normal execution after a button press also happens with 1 ms.
  //
  // For the first 20 ms test, it did 0 through 3, then skipped to
  // 47 through 49. This conflicts with the notion that the Teensy
  // has frozen, and instead suggests errors in data transmission.
  // - 1st attempt: 0-3, 47-49
  // - 2nd attempt: 0-3, 48-49
  // - 3rd attempt: 0-3, 48-49
  //
  // The return time for 20-22 ms looks oddly like 1.000 seconds.
  // This pattern repeats for the 24 ms tests and the 1 undocumented
  // 26 ms test. It is likely covering up many failures in the 30 ms
  // test, making them appear like successes (without scrolling up).
  //
  // So an event occurring exactly 1 s after setup (which itself occurs
  // exactly 0.300000 s after program startup) causes numbers to appear
  // on the serial monitor again.
}

void loop() {
  
}

// Function to execute reliably with a consistent time
// base in the multiple kHz band.
//
// I am aware that the string manipulation here is not
// interrupt safe.
void kilohertzLoopIteration() {
  if (counter >= 50) {
    return;
  }

  uint32_t currentTimestamp = micros();
  uint32_t relativeTimestamp = currentTimestamp - programStartTimestamp;
  float timeInMs = float(relativeTimestamp) / float(1000);
  
  if (counter < 10) {
    Serial.print(" ");
  }
  Serial.print(counter);
  Serial.print(" ");
  Serial.println(timeInMs);

  counter += 1;
}