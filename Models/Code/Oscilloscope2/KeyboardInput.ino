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
  uint32_t previousSlotID = (oscilloscopeTimestamp - startTimestamp) / 20;
  uint32_t currentSlotID = (latestTimestamp - startTimestamp) / 20;

  if (shouldDisplayLatest) {
    // WARNING: Modulo operator may have undefined behavior for
    // negative integers. Force the number to be positive.
    int32_t endSlotID = currentSlotID + 1;
    int32_t startSlotID = endSlotID - 1000;
    endSlotID += 50000;
    startSlotID += 50000;
    if (startSlotID < 0) {
      Serial.println("Invalid slot ID.");
      exit(0);
    }

    for (int32_t slotID = startSlotID; slotID < endSlotID; ++slotID) {
      int32_t copiedSampleID = slotID - startSlotID;
      float sample = ringBuffer.samples[slotID % 50000];
      oscilloscopeCopiedSamples[copiedSampleID] = sample;
    }
  } else if (oscilloscopeMode == 'a') {

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