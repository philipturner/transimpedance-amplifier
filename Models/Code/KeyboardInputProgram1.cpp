void setup() {
  // The top of the Serial Monitor tab has a text input field
  // saying "Message (Enter to send message to ...)".
  Serial.begin(0);
  Serial.println("Enter a message: ");
}

void loop() {
  if (Serial.available() > 0) {
    // Does 'char' vs. 'int8_t' vs. 'uint8_t' change how
    // text is handled in 'Serial.print()'?
    //
    // 'char' - characters, terminated with blank
    // 'int8_t' - integers, terminated with 10
    // 'uint8_t' - integers, terminated with 10
    //
    // The terminating character changes with
    // selection of "New Line" or similar to the
    // right of the text input field. The ASCII
    // code for '\n' is 10. It might be useful
    // to select "No Line Ending" for some programs.
    char incomingByte = Serial.read();
    Serial.print("Received: ");
    Serial.println(incomingByte);
  }
}
