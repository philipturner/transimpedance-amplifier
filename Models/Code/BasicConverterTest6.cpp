void setup() {
  // [Default] This line is present, freeze after 3 iterations.
  // [Variation] This line is commented out, freeze after 4 iterations.
  Serial.println(); // allow easy distinction of different program runs
  
  for (int i = 0; i < 50; ++i) {
    // [Default] 100 μs, freeze after 3 iterations.
    // [Variation] 30 μs, program proceeds to completion.
    //
    // Exact threshold is 72.5-74.0 μs.
    // 73.0 μs - failed first time after between 20-50 iterations,
    //           but could not reproduce after 7 attempts
    // 73.5 μs - freeze after 4-5 iterations.
    delayMicroseconds(100);

    // [Default] 0, freeze after 3 iterations.
    // [Variation] ~100 character gibberish string, freeze after 3 iterations.
    Serial.println(0);
  }
  Serial.println("finished");
}

void loop() {

}
