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

void oscilloscopeDiagnosticLoop() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    if (incomingByte == 'a') {
      oscilloscopeMode = 'a';
    } else if (incomingByte == 'l') {
      // Right here, call a function to display the last 1000 samples.
      oscilloscopeMode = '0';
    } else if (incomingByte == '0') {
      oscilloscopeMode = '0';
    }
  }
  
  // TODO: Do the lock here.
  if (oscilloscopeMode == 'a') {
    delay(20);
  }
  oscilloscopeTimestamp = latestTimestamp;
}