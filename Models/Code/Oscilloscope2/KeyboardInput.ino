#include "KeyboardInput.h"

#include "ADC.h"
#include "RingBuffer.h"
#include "TimeStatistics.h"

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

void adcResponsivenessDiagnosticLoop() {
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

char oscilloscopeMode = '0';
float oscilloscopeCopiedSamples[1000];

void oscilloscopeGuardedCode(bool shouldDisplayLatest) {
  uint32_t startSlotID = (oscilloscopeTimestamp - startTimestamp) / 20;
  uint32_t endSlotID = (latestTimestamp - startTimestamp) / 20;

  if (oscilloscopeMode == 'a') {
    
  } else if (shouldDisplayLatest) {

  }

  oscilloscopeTimestamp = latestTimestamp;
}

void oscilloscopeDiagnosticLoop() {
  delay(20);
  
  bool shouldDisplayLatest = false;
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    if (incomingByte == 'a') {
      oscilloscopeMode = 'a';
    } else if (incomingByte == 'l') {
      oscilloscopeMode = '0';
      shouldDisplayLatest = true;
    } else if (incomingByte == '0') {
      oscilloscopeMode = '0';
    }
  }
  
  // Make the guarded portion very small and do not
  // invoke 'Serial.print' here.
  oscilloscopeLock = true;
  oscilloscopeGuardedCode(shouldDisplayLatest);
  oscilloscopeLock = false;
  
}