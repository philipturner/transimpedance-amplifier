#pragma once

void timeFidelityDiagnosticLoop();

// Responsiveness test
//
// Keyboard events:
// 'c' received - ADC performs a measurement
// 'd' received - ADC reports contents of ADS8689_RANGE_SEL_REG
// '0' received - ADC writes 0b0000 to ADS8689_RANGE_SEL_REG
// '1' received - ADC writes 0b0001 to ADS8689_RANGE_SEL_REG
//
// During normal program operation, the responsiveness test does
// not execute. The range select register is set to 0 before the kilohertz
// loop starts.
void adcResponsivenessDiagnosticLoop();

// 'a' received - min/avg/max over 1 ms intervals, showing 1 s of history
// 'l' received - take a snapshot of 20 ms of the raw data stream
// '0' received - turn off any plotting
void oscilloscopeDiagnosticLoop();