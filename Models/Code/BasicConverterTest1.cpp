#include <SPI.h>

// TODO: Upload this to the project repository
// instead of a GitHub gist.

uint8_t MOSI = 11;
uint8_t MISO = 12;
uint8_t SCK = 13;
uint8_t CS_DAC = 36;
uint8_t CS_ADC = 37;

void setup() {
  pinMode(CS_DAC, OUTPUT);
  pinMode(CS_ADC, OUTPUT);
  digitalWrite(CS_DAC, HIGH);
  digitalWrite(CS_ADC, HIGH);
  SPI.begin();
}

void loop() {

}

// // Execute 'Initialization Set Up' from the DAC81401 datasheet.
// writeDAC(0x03, 0x0A80);
// delay(100);
// writeDAC(0x04, 0x0000);
// delay(100);
// writeDAC(0x09, 0xFFFE);
// delay(100);
// writeDAC(0x0A, 0x0005);
// delay(50);
//
// // Write the digital input code.
// writeDAC(0x10, 0x1111);
// delay(50);

// Execute a transaction on the DAC.
void writeDAC(uint8_t registerAddress, uint16_t data) {
  digitalWrite(CS_DAC, LOW);
  delayNanoseconds(2000);
  
  // Begin the transaction with 1 MHz or 10 MHz and
  // SPI mode 2 (CPOL = 1, CPHA = 0).
  //
  // Possible cause of null results: SCK idle state being LOW instead of HIGH
  //
  // DAC would not register any SPI commands, explaining why VOUT was connected
  // to GND through a 8.3 kΩ resistor. Note that R1 is 4.7 kΩ (1%) and cannot
  // possibly explain the ~10 kΩ value matching the datasheet.
  //
  // VREFIO was showing 0 V across the capacitor, and impossible to register
  // a stable voltage differential between VREFIO and DVDD.
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE2));
  
  
  // Transfer the 'instruction cycle'.
  uint8_t W = 0b10000000;
  SPI.transfer(W | registerAddress);

  // Transfer the 'data cycle'.
  SPI.transfer(uint8_t(data >> 8));
  SPI.transfer(uint8_t(data & 0xFF));
  
  // End the transaction.
  SPI.endTransaction();

  delayNanoseconds(2000);
  digitalWrite(CS_DAC, HIGH);
}
