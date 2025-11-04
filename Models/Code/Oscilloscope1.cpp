// High-fidelity timer for Teensy
//
// TODO: Annotate the timing of operations within the 50 kHz loop.

// Displaying data to console
// - Measure timestamps, auto-detect skipped frames. Fill in skipped frames with
//   previous sensor value.
// - Neatly format the time and measured voltage, with a consistent spacing.
//   Display everything as text to the console, as we cannot display a UI
//   graph (unfortunately).
// - The exact subregion of the data stream to display is a non-trivial
//   problem, even for physical oscilloscopes.
// - Program two modes: text (high resolution), graphical (low resolution)
//
// Exploring the "Serial Plotter" tool right now.

// Teensy 4.1 has 1 MB of RAM. We'll use 200 KB to store 50,000 samples
// at 4 bytes/sample. This is a ring buffer, 1 second of history @ 50 Ksps.
// It's up to the code in 'loop()' to format outputs from this source of truth.
// We can logically separate the display code from the tight loop that
// actually gathers data.
//
// The internal loop will attempt to back-fill any skipped frames. It
// will write to the buffer in chronological order. If the number of
// skipped frames is unreasonably high, crash with an error.

// Fixing the X-axis limits to the Arduino IDE's serial plotter:
// https://www.open-electronics.org/how-to-adjust-x-and-y-axis-scale-in-arduino-serial-plotter-no-extra-software-needed/
// Successfully changed it from 50 to 1000 data points (TODO)

////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
}

int i = 0;

void loop() {
  i = (i + 1) % 360;

  float y1 = 1 * sin(i * M_PI / 180);
  float y2 = 2 * sin((i + 90)* M_PI / 180);
  float y3 = 5 * sin((i + 180)* M_PI / 180);

  Serial.print(y1);
  Serial.print("\t"); // a space ' ' or  tab '\t' character is printed between the two values.
  Serial.print(y2);
  Serial.print("\t"); // a space ' ' or  tab '\t' character is printed between the two values.
  Serial.println(y3); // the last value is followed by a carriage return and a newline characters.

  delay(1);
}
