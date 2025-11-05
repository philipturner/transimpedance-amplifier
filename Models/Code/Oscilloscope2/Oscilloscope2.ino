#include "ADC.h"
#include "KeyboardInput.h"
#include "TimeStatistics.h"

IntervalTimer timer;

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

// TODO: Next, attempt to find a way to terminate
// the Teensy program.
void loop() {
  adcResponsivenessDiagnosticLoop();
}

// Function to execute reliably with a consistent time
// base in the multiple kHz band.
void kilohertzLoop() {
  uint32_t currentTimestamp = micros();
  uint32_t previousTimestamp = latestTimestamp;
  latestTimestamp = currentTimestamp;

  uint32_t jumpDuration = currentTimestamp - previousTimestamp;
  timeStatistics.integrate(jumpDuration);
}