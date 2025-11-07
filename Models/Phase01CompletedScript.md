Parameters from previous video:
- video size: 1920x1080
- video frame rate: 30 FPS
- background type for overlay: gray
- opacity for background overlay: 50%
- text font:
  - family: Monaco
  - size: 72
  - face: regular
  - color: white
- text offset:
  - alignment: left
  - anchor: bottom + left
  - position: X = 30, Y = 30
  - every caption in the video had 3 lines
- how to get text effect: top of window -> Titles -> Text
- speedup factor for fast-forward sections:
  - 800% typical
  - 1600% superfast
- freeze frame:
  - change clip speed -> freeze frame
  - trim edit mode not relevant to this problem?

[left lower corner]
[show just as the display starts rolling]
[fade in]
oscilloscope display with 1 second history

[middle of screen]
[gray transparent overlay]
device under test: 1 pF
stimulus: triangle wave
response: square wave

[middle of screen]
[gray transparent overlay]
 rising edge: -1.8 V (+5.4 nA)
falling edge: +1.5 V (-4.5 nA)

[left lower corner]
[fade in after ~1 second region of no text]
oscilloscope trace of 20 ms time interval

[left lower corner]
oscilloscope trace of 2 ms time interval

[middle of screen]
[gray transparent overlay]
[text in corners disappears temporarily]
frequency response: overshoot/undershoot/etc.

[left lower corner]
R_feedback: 330 MΩ
C_feedback: 169.5 fF
R_bandwidth: 1775 Ω
R_main_comp: ...
C_main_comp: 2.00 nF

[right lower corner]
ADC bandwidth limit: 15.0 kHz
TIA bandwidth limit: 44.8 kHz
R_f C_f pole: 2.85 kHz
R_c C_c zero: ...

| Turns | R_main_comp | R_c C_c zero | Description |
| ----- | ----------- | ------------ | ----------- |
| 8.0   | 15.0 kΩ     | 4.73 kHz     | undershoot  |
| 20.0  | 37.6 kΩ     | 2.02 kHz     | overshoot   |
| 14.0  | 26.3 kΩ     | 2.83 kHz     | good        |
| 14.5  | 27.3 kΩ     | 2.74 kHz     | fine-tuned  |

## Order of Frequency Response Captions

frequency response: undershoot
corners: 8.0 / 25 turns
corners: ...
corners: 20.0 / 25 turns
frequency response: overshoot
corners: 20.0 / 25 turns
corners: ...
corners: 14.0 / 25 turns
frequency response: good
corners: 14.0 / 25 turns
corners: ...
corners: 14.5 / 25 turns
frequency response: fine-tuned
fade into black screen
fade in the end screen text in two parts

## Font Sizes

frequency response text in corners:
- font size: 48
- split into two captions for left and right
- right uses 1890 px for X position
- right uses alignment: right
- right uses anchor right for horizontal

end screen text:
- font size: 120
- anchor bottom for vertical
- anchor center for horizontal
- use default (960 px) for X position
- use 330 px for Y position
- split into two captions, each 3 lines (with blanks) to enable 2-part fade

everything else:
- font size: 72
