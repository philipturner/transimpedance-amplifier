# Transimpedance Amplifier

<p align="center">
&nbsp;
  <img src="./Documentation/HomeBuiltSTM/Control_Loop_Image1.png" width="20.35%">
&nbsp;&nbsp;
  <img src="./Documentation/HomeBuiltSTM/CustomDesign_Schematic.png" width="24.70%">
&nbsp;&nbsp;
  <img src="./Documentation/HomeBuiltSTM/CustomDesign_Calibration.png" width="44.95%">
&nbsp;
</p>

Phase 0.1 of the [APM Roadmap](https://github.com/philipturner/apm-roadmap)

Objective: Gather as much data as possible about the TIA sensor of an STM, using both theory and experiment.

Deadline: September 30, 2025 for the first physical prototype

Table of Contents:
- [July 26, 2025](#july-26-2025)
- [July 27, 2025](#july-27-2025)
- [July 28, 2025](#july-28-2025)
- [July 29, 2025](#july-29-2025)
- [August 3, 2025](#august-3-2025)
- [August 4, 2025](#august-4-2025)
- [August 5, 2025](#august-5-2025)
- [August 6, 2025](#august-6-2025)
- [August 7, 2025](#august-7-2025)
- [August 8, 2025](#august-8-2025)
- [August 9, 2025](#august-9-2025)
- [August 16, 2025](#august-16-2025)
- [August 18, 2025](#august-18-2025)
- [August 20, 2025](#august-20-2025)
- [August 22, 2025](#august-22-2025)
- [August 23, 2025](#august-23-2025)
- [August 26, 2025](#august-26-2025)
- [August 27, 2025](#august-27-2025)
- [August 28, 2025](#august-28-2025)
- [August 29, 2025](#august-29-2025)
- [September 1, 2025](#september-1-2025)
- [September 7, 2025](#september-7-2025)
- [September 8, 2025](#september-8-2025)
- [September 9, 2025](#september-9-2025)
- [September 10, 2025](#september-10-2025)
- [September 11, 2025](#september-11-2025)
- [September 12, 2025](#september-12-2025)
- [September 15, 2025](#september-15-2025)
- [September 16, 2025](#september-16-2025)
- [September 17, 2025](#september-17-2025)
- [September 18, 2025](#september-18-2025)
- [September 19, 2025](#september-19-2025)

## July 26, 2025

Three simulation tasks, in chronological order
1) Understand/simulate why coarse tip approach takes so long
    - Understand specific algorithms for moving the coarse motor (approach & retract) without frictional backlash causing a tip crash
    - Understand maximum speeds of piezo motors
2) Simulate imperfect DAC-driven cancellation with a fixed value compensation capacitor
    - Use the tip-sample capacitance of a plausible geometry, derived with FEA
    - Include a finite insulation resistance
3) Fully understand the art of cabling analog lines in/out of UHV chambers + instrumentation amplifiers + ground loops
    - Include EMI and digital coupling from the bias DAC + ADC + SPI ports + power supply system

Notes about simulations:
- Make compromises to reduce the number of variables in each simulation, making it practical to set up
    - The simulations are not particularly computationally intensive, often 1-variable analytical functions, Fourier transforms, and integrals of power spectral density.
    - Likely using a combination of Google Sheets and single-file scripts hosted on GitHub Gist, hyperlinked to this repo.
- Exploit recently acquired skills in finite element analysis and the theory of EMI/grounding/shielding
- <b>Early IRL hardware tests may fail some requirements for system performance</b>, which are assumed or optimized for in the simulations

---

Simultaneously performing a low-cost physical test with a badly shielded IRL model
- Gather as much data, as early as possible
- Pipeline the phases of design elaboration
- Later stages include electromagnetic shielding, interfaces with automated electronics for the I/O voltages, then the logical transition to Phase 0.2

Primary blockers to first physical prototype:
- Lack of understanding about electrical performance of reversible DUT connection point (DUT = resistor, capacitor)
- Need to elaborate on requirements for ventilation from acid fumes, and other specific equipment to order for hand soldering
- Need to resolve the issues with incorrect/unnecessary bypass/decoupling capacitors
  - It is a very bad idea to omit all the capacitors, without at least giving it some rigorous consideration
- Need to resolve the missing/incorrect KiCad symbols and footprints for each IC. Organize the footprints on a physical PCB model and become familiar with the specific pin functions & positions.
- Need to clarify the bothersome trace width variable

<b>Fast-track work on the physical prototype.</b> Ensure that all fabrication equipment, testing equipment, and circuit parts are delivered by August 12, 2025. In any leftover time until August 12, I can return to the three simulation tasks.

An appropriate name for this is ["STM Experiment #3"](https://www.youtube.com/watch?v=IUPDecnuzO4). The experiments may not logically build on each other. For example, a single 1000 V linear amplifier doesn't logically follow a capacitive displacement sensor. However, it can still be catalogued as `n + 1` to show the iterative nature of design elaboration & exploration.

## July 27, 2025

One of the most important tests should be feedback loop stability. With the SPICE simulations, I can predict what values for input capacitance will send the circuit into oscillations. Pick a conservative value(s) for a capacitor DUT that satisfies this condition, without making the current exceed its dynamic range (simply reduce the frequency of the signal).

The first prototype will test two TIA variants. The first will be the standard OPA627 + 100 MΩ design used in most DIY STM projects. This will give a baseline for tracing failures, if the complex 2-stage design does not work at all.

I should see whether testing equipment, such as a handheld oscilloscope, can generate an input function. This would remove the need for a hard-coded triangle wave generator on the PCB. However, I might include the triangle wave generator anyway, for completeness and/or more debugging options. It also provides experience designing with digital/nonlinear components (comparator & Schmitt trigger), which may couple into the analog components against my wishes. There should be a simple way to disconnect the generator from power pins. It could be helpful to make the generator's capacitor swappable, so the frequency can change.

## July 28, 2025

I am starting to read large swaths of the IPC 7351 standard, February 2005 revision. This project will resolve my frustration with non-standardization of resistor & capacitor footprints. It substantiates a growing realization across many engineering projects, that complex systems have many variables. They demand significant amounts of data to wrap my head around. I am not satisfied with arbitrary guesses and binary yes/no "it worked" tests, with no explanation of how this choice contributed to a reduction in UHV chamber cost.

> My objective is obtain better control over the usability, performance, and cost of my end product(s).

I feel more comfortable creating my own footprints from scratch, where I can trust every parameter used to derive every dimension. I will compare my footprints to those from vendors, SnapEDA, and KiCad libraries.

## July 29, 2025

A few more things I want to do, before the 1st prototype:
- Run a SPICE simulation of the stability of a feedback loop with OPA627 + 100 MΩ. Check the limits of stability with the parasitic capacitance of the resistor.
  - Include a model of OPA637 to understand why that's not okay.
  - If OPA637 passes in the simulations, the test board will include a third TIA, OPA637 + 100 MΩ + no parallel capacitor.
    - Even if it doesn't pass, include the actual combination IRL, to prove that it oscillates when tested!
    - Unless this could plausibly impair the operation of the other amplifiers, and there's no way to switch off power to the OPA637.
    - Will severe oscillation conditions cause damage to the component and/or its power supply?
- Get more rigorous calculations of the parasitics for the components, and of the potentiometers.
- Design custom footprints for the capacitors and trimpots as well, in the first step. I previously planned to just wrap my head around the ICs + large resistors first. Instead, I should get the entire system specified at the highest (symbolic) level, and elaborate on the physical layout once that's fixed.
  - Bypass/decoupling capacitors are intentionally omitted at this stage.
  - The exact resistor values for the AD8615 voltage divider are omitted at this stage. Also not worrying about whether the two polarities of the divider cross through the GND net.
- Calculate the expected noise (e.g. Johnson noise from resistors in 2nd stage), bandwidth of the oscilloscope ADC, and consider electromagnetic shielding.
  - Consider an artificial bandwidth limiter with a transfer function similar to the ADS8699. This probably requires another IC. Good practice in selecting arbitrary op-amps for real world situations of need.
  - Calculate the leakage current at the physical current input junction + corresponding reversible DUT port. Compare the leakage current to the noise floor.
- Prepare a list of through-hole DUT components. Elaborate on whether it's a single DUT port for all 2&ndash;3 amplifiers, or a separate port for each.

I just learned/remembered that OPA627 and OPA637 are very old and expensive (\~$30). I'll have to revise these plans, to use newer and cheaper chips like OPA828 (\~$7). Or scrap the entire concept of additional 1-stage amplifiers to save time, depending on motivation.

## August 3, 2025

This project is on track to blow far north of its time budget. I'll have to compromise.

![Capacitive Displacement Sensing (OneNote)](./Documentation/New/CapacitiveDisplacementSensing_OneNote.jpg)

Permitted:
- Delete the oscilloscope from the workflow (yay!)
- Read [MT-015](https://www.analog.com/media/en/training-seminars/tutorials/MT-015.pdf), [MT-017](https://www.analog.com/media/en/training-seminars/tutorials/MT-017.pdf), [MT-019](https://www.analog.com/media/en/training-seminars/tutorials/MT-019.pdf), [MT-021](https://www.analog.com/media/en/training-seminars/tutorials/MT-021.pdf), [MT-090](https://www.analog.com/media/en/training-seminars/tutorials/MT-090.pdf) to understand the chips I'm working with
- Make an entirely custom footprint for every electronic part

Disallowed:
- Run a simulation to understand whether quantization noise and time jitter makes capacitance compensation unviable, with my novel approach
- Use any chips that require reflow soldering
- The analog triangle wave generator from the fast low-noise TIA paper. We're already synthesizing arbitrary waveforms with the DAC, with adjustable frequencies.

The near-term deliverable will pair the Teensy with a single DAC channel (DAC81401) and a single ADC channel (ADS8699), with a nerfed system bandwidth of 15 kHz. I really wanted a higher frequency to debug the first step. Unfortunately, the 500 kHz chip, ADS86861W, requires reflow soldering. I'll defer contemplating the ramifications of this omission to a later date.

---

Comments on the previous journal entry:
- Johnson noise is comparable to e<sub>n</sub>C noise, at 15 kHz bandwidth + 2nd order rolloff.
  - 6σ noise from capacitance is <b>5.2 pA</b>, provided just the 9.2 pF input capacitance of the AD8615 (conservative estimate, summing the differential and common-mode capacitance)
  - 6σ noise from capacitance is <b>24.7 pA</b>, when moved outside the UHV chamber, adding 35 pF to the capacitance to model a coaxial cable
  - 6σ Johnson noise from a 100 MΩ resistor is <b>10.9 pA</b>
  - 6σ Johnson noise from a 300 MΩ resistor is <b>6.3 pA</b>
  - 6σ Johnson noise from a 1 GΩ resistor is <b>3.5 pA</b>
- The noise signal appears as ~1 mV at the ADC.
  - 1-stage with OPA828ID:
    - 15.0 pF
    - 100 MΩ
    - 6σ noise envelope is <b>1.2 mV</b>
  - 2-stage with AD8615:
    - 9.2 pF
    - 300 MΩ
    - 6σ noise envelope is <b>2.4 mV</b>
- Don't need to construct an artificial 15 kHz filter out of discrete components. I'm using the actual ADS8699 chip in the circuit board now.
- 15 kHz bandwidth is more than enough for the STM feedback loop. If anything, I would use digital filtering to reduce it to the single kHz range, reducing the 6σ current noise to ~1 pA.

Source: [Transimpedance Amplifier (Google Sheets)](https://docs.google.com/spreadsheets/d/1QVJOiaRJbdRXN7CXii3z91K5A64h3VmgLCsRjpDpIrQ/edit?usp=sharing)

## August 4, 2025

<p align="center">
&nbsp;
  <img src="./Documentation/New/August4_Part1.jpg" width="40.16%">
&nbsp;&nbsp;
  <img src="./Documentation/New/August4_Part2.jpg" width="49.84%">
&nbsp;
</p>

## August 5, 2025

It is becoming very clear that I want to run this simulation about capacitive displacement sensing.

The simulation will have an analytical (frequency domain) and numerical (time domain) part.
- Analytical stuff:
  - Theoretical model of the distribution of quantization noise among the DACs (evenly across the Nyquist bandwidth)
    - Ensure the sine wave frequency is indivisible by the quantization frequency (100 kSPS)
    - Concerned about the variable-voltage approach not being able to perfectly cancel the quantization noise
  - Model of the integrated e<sub>n</sub>C noise after lock-in amplification
  - Model of the transfer function for the 2-stage TIA, which might change with input capacitance (?)
- Numerical stuff:
  - FP64 for the simulations to avoid concerns about rounding error, over long time intervals for lock-in amplification
    - Time stepping resolution 1&ndash;2 orders of magnitude better than the 100 kSPS sampling rate
  - Exact algorithm for a digital filter approximating a first-order or second-order lowpass
    - Validate that the filters have the expected gain falloff, by running a sweep over sine wave frequencies
    - The filter is affected by the time step size
  - Simulate the two DACs being out of sync, which probably cannot be represented in an analytical frequency domain model
- A way to include the effects of shot noise from background capacitance into the results of the numerical part

Imperfections covered in the simulation:
- Inaccurate matching of a fixed-value capacitor to the tip-sample capacitance (up to a factor of 10 in either direction)
- Overloading the maximum TIA voltage due to the transfer function exceeding the ADC @ 15 kHz. Specifically, overloading from signals in the 15&ndash;30 kHz range.
- Quantization of the two DAC signals causing a strange division of the Gaussian quantization errors (?)
- The two DACs being out of sync (not yet figured out the exact amount of delay to test)

Imperfections not covered:
- Time jitter in the ADC and/or DACs
- Switching transients in the DACs
- Interference from acoustic noise
- Interference from a finite insulation resistance in the compensation capacitor

First step:
- Attempt to find a formula for the RMS current of two cancelling signals, slightly phase shifted
- Address the low-hanging fruit, of a frequency domain analysis of the effect of quantization noise

Second step:
- Refresh myself on the analytical formulas for transfer function of the fast low-noise TIA, stability conditions, and location of the 2nd pole
- Figure out the exact potentiometer values appropriate for this circuit
- For simplicity (to save time), the simulation will assume the potentiometers are perfectly matched, to an infinite resolution

Since I am in the car right now, I cannot work on the first step. Therefore, I will proceed with specifying the near-term experiment with reduced scope.

---

Interesting observations:
- ADS8681W is not even in stock, although Mouser expects a new batch in late August
- Only Mouser has stock of DAC81402, DigiKey does not
- Amazon Basics 6LR61 is probably sufficient for the 9V batteries

## August 6, 2025

I understand how the AH2550 / AH2700 works now. Used this knowledge to understand possible limitations in my custom metrology setup.

![August 6](./Documentation/New/August6.png)

[Capacitive Displacement Sensing (Google Sheets)](https://docs.google.com/spreadsheets/d/1fhiZoWPt_2P-jLdGjmx1e9VDYEp9JpBjLP0ZZkT_46Y/edit?usp=sharing)

Done with the investigation of capacitive displacement sensing. Modulation voltage is 10 V, carrier frequency is 10 kHz. Pushing the limits of the DAC and ADC. Digitization rate in the DAC assumed 50 kSPS to give a conservative estimate for quantization noise.

Assuming the two capacitances can be compensated to 99.9% accuracy, static magnitude and shot noise of the background signal are not limiting factors. The ratio of 1 LSB in the ADC to the static background signal (~565 pA) is 1:1800.

Dominant issue is dynamic quantization noise. In the AH2550/AH2700, the quantization errors from the two stimulus signals (near-)perfectly cancel. In my design, they do not.

## August 7, 2025

I did some work on the voltage limiter for the 2nd stage of the 2-part TIA. I went with copying the part models from the latest iteration of the design by Schmid et al. There is an extremely large space of manufacturers and part variants, for diodes and BJTs that are almost the same.

I was about to inspect more details of these parts, and the conditions they're subjected to, to understand more about what's going on. Instead, I deferred that task to a later date. Just as elaboration on the specific decoupling network for each IC, is out of scope for today. Except for the fact that AD8615's decoupling network uses a 1 kΩ potentiometer and a 3.3V Zener.

Thanks to Michael Schmid for filling me in on the recommended decoupling network, thus saving a lot of time. Chips like the ADC and DAC, or other parts where decoupling is critical to operation, typically have a manufacturer's suggested configuration on the datasheet.

[Discrete Semiconductors (Google Sheets)](https://docs.google.com/spreadsheets/d/1FA-BcQ8bDqJqKHbvW08lza3Lz3hW-CrOkspl6eD3IWo/edit?usp=sharing)

I am returning to the KiCad project, currently in the stage of specifying the symbolic schematic. I've specifically declared PCB geometry and footprints out of scope. When downloading part symbols from vendor websites, I neglect the footprints and 3D models. However, I'm aware of the specific package. For example, I chose the THT package over the SMT package for the voltage regulators. Although the part becomes larger, the pin count reduces from 8 to 3; the pin mapping is more direct. Everywhere else, I prefer an SMT package if possible.

I will rename the hierarchical sheets for the transimpedance amplifiers, avoiding words that start with a number. I will also attempt to create a deeper level of hierarchy for the 2 stages of the 2-part TIA. Then, I will copy as many part symbols as possible, for everything except small resistors, small capacitors, and reversible connection ports.

Tomorrow, I should be able to start specifying in/out lines for each module. For example, distributing regulated voltages from the power module to each client module. The low-frequency decoupling capacitor and voltage divider for the AD8615's ±5 V lines will probably be encapsulated within the sub-sheet of the 2-stage TIA. All other regulators have their networks specified in the power module.

> A deep investigation is warranted for the subtle detail of a 0.01 μF C2 on the LM78L datasheet / 0.1 μF on LM79L. Approaching the severity of declaring all footprints for every circuit part "custom" to achieve control of the little details. Again, beyond the scope of the tasks outlined above.

## August 8, 2025

Today, my objective was to understand more about the parasitic capacitance of the TIA feedback resistor.

![August 8](./Documentation/New/August8.jpg)

_Cross-section view of the Elmer simulation results, showing where the electrostatic energy is concentrated. The sharp ends of the conductors contain 90% of the energy associated with capacitance._

I created a multi-material model of the Vishay 0805 <b>thick film</b> chip resistor. Each conductor is 10	μm thick and wraps around from top to bottom. The bulk of the component is a slab of alumina, parameterized with ε<sub>r</sub> = 9.1. The resistive element was replaced with air. After several hours of troubleshooting, I got the simulation to work. The initial result, ~40 fF, was unexpectedly small. I tried refining the mesh, to see whether the result would change.

| Node Count | Element Order | Mutual Capacitance (C<sub>12</sub>) |
| ---------: | ------------: | ----------------------------------: |
| ~5,000     | 1             | 39.61 fF                            |
| ~14,000    | 1             | 37.83 fF                            |
| ~40,000    | 1             | 36.93 fF                            |
| ~150,000   | 1             | 36.46 fF                            |

In his [ultra-low noise qPlus AFM sensor design](https://doi.org/10.1063/1.4993737), Geissibl used an 0805 <b>thick film</b> chip resistor ([datasheet](https://www.te.com/usa-en/product-1625862-1.datasheet.pdf)). He reported 150 fF, which agrees with the generally\* assumed 100 fF for transimpedance amplifiers. I simulated an almost identical geometry, and was off by a factor of 4.

> \*Art of Electronics, fast low-noise TIA paper

Vishay [measured the parasitics](https://www.vishay.com/docs/60107/freqresp.pdf) of <b>thin film</b> chip resistors, going as large as 0603. The capacitances follow a predictable scaling law. I'll develop a model that accepts any resistor geometry, and matches them to the Vishay data.

| Source           | Case Size | L    | W    | H    | T2   |
| ---------------- | --------- | ---: | ---: | ---: | ---: |
| Vishay (2009)    | 0201      | 0.51 | 0.25 | n/a  | 0.20 |
| Vishay (2009)    | 0402      | 1.02 | 0.51 | n/a  | 0.29 |
| Vishay (2009)    | 0603      | 1.63 | 0.81 | n/a  | 0.49 |
| Vishay D/CRCW e3 | 0402      | 1.00 | 0.50 | 0.35 | 0.20 |
| Vishay D/CRCW e3 | 0603      | 1.55 | 0.85 | 0.45 | 0.30 |
| Vishay D/CRCW e3 | 0805      | 2.00 | 1.25 | 0.50 | 0.30 |
| TE               | 0805      | 2.00 | 1.20 | 0.40 | 0.30 |

_Part dimensions from three literature sources. All dimensions are in millimeters._

Use thick film 0603 as an "origin" or normalized value, from which other capacitances are scaled. It is the case size I will use in the PCB. Scaling behavior comes mostly from lengthening and separating the sharp ends of the conductors. Scaling of height could both increase and decrease capacitance, making it troublesome to model. Therefore, we don't need to worry about the lack of height data for Vishay (2009).

I developed two fittings of the data ([source](https://docs.google.com/spreadsheets/d/1QVJOiaRJbdRXN7CXii3z91K5A64h3VmgLCsRjpDpIrQ/edit?usp=sharing)). One extrapolates based on the parallel plate model. The other extrapolates based on the parallel wire model. Here are the final values for capacitance, after correcting for the discrepancy between "flip chip" and "wrap around".

| Source           | Case Size | parallel plate | parallel wire |
| ---------------- | --------- | ---: | ---: |
| Vishay (2009)    | 0201      | 33.9	fF | 28.2	fF |
| Vishay (2009)    | 0402      | 35.3	fF | 41.0 fF |
| Vishay (2009)    | 0603      | 60.3	fF | 60.3 fF |
| Vishay D/CRCW e3 | 0402      | 24.9	fF | 37.8 fF |
| Vishay D/CRCW e3 | 0603      | 45.4	fF | 59.0 fF |
| Vishay D/CRCW e3 | 0805      | 66.7	fF | 81.1 fF |
| TE               | 0805      | 61.4	fF | 77.9 fF |

The Vishay data predicts ~74 fF, double the simulation result.

<b>Option 1:</b> Continue trying things in the simulator, to raise the lower bound for capacitance with an 0603 case size. This single parasitic value is the reason for the entire 2nd stage of the TIA. If the cutoff frequency is ~13 kHz, the ADS8699 cannot facilitate tuning.

<b>Option 2:</b> Switch to the larger 0805 size, which will certainly have more capacitance than 0603 and match previous expectations.

Conclusion: Go with option 2, reworking the equations to handle an unknown value in the range of 74&ndash;250 fF. If this makes the design unworkable, revert to option 1. Accept reduced tunability at a tradeoff of faster time to completion. If the range of 74&ndash;150 fF is more tunable than 150&ndash;250 fF, this is very good.

> Realization: Simulation work will only raise the lower bound to capacitance. Marginal improvements to the lower bound will cost disproportionately more effort. There is no upper bound. Poor PCB design will add parasitic coupling to the lower bound.
>
> C<sub>f</sub> &ge; 74 fF
>
> The primary goal here is to maximize the lower bound, to avoid a situation where the first cutoff frequency is under 15 kHz. The secondary goal is to maximize tunability, by restricting the range of uncertainty.

---

Tracking the cutoff frequency throughout this investigation:

| R<sub>f</sub> | C<sub>f</sub> | f<sub>c</sub> | Case Size |
| ------------: | ------------: | ------------: | --------- |
| 100 MΩ | 100 fF | 15.9 kHz | n/a |
|   1 GΩ | 100 fF |  1.6 kHz | n/a |
| 300 MΩ | 100 fF |  5.3 kHz | 0603 |
| 300 MΩ |  40 fF | 13.2 kHz | 0805 |
| 300 MΩ |  30 fF | 17.7 kHz | 0603 |
| 300 MΩ |  52 fF | 10.2 kHz | 0603 |
| 300 MΩ |  74 fF |  7.2 kHz | 0805 |
| 330 MΩ |  74 fF |  6.5 kHz | 0805 |

330 MΩ exists in both 0805 and 0603 sizes. 300 MΩ is exclusive to 0603. I will revise the symbolic schematic to use 330 MΩ, 5% in the 0805 size, with the respective purchase link on DigiKey.

## August 9, 2025

I decided to try a few more FEM simulations, and immediately found a surprising result.

| Node Count | Region 1 | Region 2 | Region 3 | C<sub>12</sub> |
| ---------: | -------: | -------: | -------: | -------------: |
| 7963       | 1.000 mm | 0.150 mm | 0.050 mm | 163.39 fF      |
| 19741      | 0.667 mm | 0.100 mm | 0.033 mm | 153.63 fF      |
| 72073      | 0.400 mm | 0.060 mm | 0.020 mm | 146.14 fF      |

Adding a ground plane on the opposite side of the 2-layer PCB:

| Node Count | C<sub>12</sub> | C<sub>13</sub> | C<sub>23</sub> |
| ---------: | -------------: | -------------: | -------------: |
| 17143      | 60.44 fF       | 170.51 fF      | 169.94 fF      |
| 45255      | 58.25 fF       | 165.37 fF      | 165.21 fF      |
| 169417     | 56.04 fF       | 161.22 fF      | 161.29 fF      |

The literature advises against placing a ground plane directly underneath the resistor. Changing the ground plane to a ring:

| Node Count | C<sub>12</sub> | C<sub>13</sub> | C<sub>23</sub> |
| ---------: | -------------: | -------------: | -------------: |
| 15255      | 72.29 fF       | 131.96 fF      | 131.47 fF      |
| 40545      | 70.27 fF       | 126.24 fF      | 126.52 fF      |
| 155345     | 67.65 fF       | 121.74 fF      | 122.10 fF      |

I'm screenshotting E-field intensity instead of electrostatic energy density now. The latter is proportional to the former, just squared. This choice makes it easier to see important details without calibrating the color legend.

<p align="center">
&nbsp;
  <img src="./Documentation/August9/Resistor_0805_Mounted_Y.jpg" width="45.00%">
&nbsp;&nbsp;
  <img src="./Documentation/August9/Resistor_0805_Mounted_X.jpg" width="45.00%">
&nbsp;
</p>

_Exact same setup as yesterday, but with more conductor material and a dielectric slab underneath._

<p align="center">
&nbsp;
  <img src="./Documentation/August9/Resistor_0805_GND_Plane_Y.jpg" width="44.99%">
&nbsp;&nbsp;
  <img src="./Documentation/August9/Resistor_0805_GND_Plane_X.jpg" width="45.01%">
&nbsp;
</p>

_Filling the opposite side of the dielectric with a conductor. The 250 μm via makes the next simulation easier to set up._

<p align="center">
&nbsp;
  <img src="./Documentation/August9/Resistor_0805_GND_Ring_Y.jpg" width="44.86%">
&nbsp;&nbsp;
  <img src="./Documentation/August9/Resistor_0805_GND_Ring_X.jpg" width="45.14%">
&nbsp;
</p>

_Changing the plane to a ring. Outer dimension measures 5 mm, ring thickness is 500 μm._

### Summarizing Today's Data

| Ground Coupling Severity        | C<sub>12</sub> | C<sub>12</sub> \|\| C<sub>23</sub> | C<sub>12</sub> \|\| (C<sub>13</sub> series C<sub>23</sub>) |
| ------------------------------- | --------: | --------: | --------: |
| None                            | 146.14 fF | n/a       | n/a       |
| Poorly designed keepout zone    | 67.65 fF  | 189.75 fF | 128.61 fF |
| Complete overlap with GND plane | 56.04 fF  | 217.33 fF | 136.67 fF |

Once the PCB layout is finalized, I'll run another FEM simulation of the ground conductor geometry. I'll make sure C<sub>12</sub> exceeds 80\* fF. This is a slight improvement over 74 fF, instead of a major degradation to 67 fF. The average (expected) value to target is 150 fF. The upper bound is somewhere between 200&ndash;300 fF, based on my discretion.

> \*Expect this condition to be met after draconian measures to isolate the resistor.

Continuing the FEM simulations for another day was a good choice. I am now more confident in the numbers stated in the literature. I can proceed with more certainty about expected performance and less extreme design margins.

## August 16, 2025

The August 31 deadline seems unrealistic now, but it was a good motivator to get me working again. I'm proceeding at the correct pace to achieve the end goal. Every day, I resolve a different unknown variable and the hardware design becomes more specific.

I'm using basic linear algebra to study the 5 coupled variables for component values in the TIA compensation network: [TransimpedanceAmplifierCalibration.swift](https://gist.github.com/philipturner/219ffce07112c4575081fc9615dd2323)

## August 18, 2025

![August 18, Part 1](./Documentation/New/August18_Part1.png)

Common properties of KiCad default footprints:
- 0.20 mm keepout zone around copper pads (thin red line)
- 0.25 mm distance between courtyard and copper/part outline
- 0.10 mm line width in F.Fab for part outline
- 0.12 mm line width in silkscreen layer for fiducial outline
- 0.11 mm distance between centers of lines for F.Fab and silkscreen
- 0.15 mm text thickness
- 1.00 mm text height
- 1.00 mm text width

Do the official vendor footprints (Texas Instruments, Analog Devices) match these conventions?

![August 18, Part 2](./Documentation/New/August18_Part2.png)

Observations:
- The text size is exactly 0.15 mm, 1.00 mm, 1.00 mm. I wonder whether this is an industry standard or just something KiCad sets.
- 0.20 mm thin red line around copper pads exists for all footprints. I wonder whether this is something KiCad sets.
- Line width in F.Fab varies, either ~0.03 mm (could be 1 mil) or 0.15 mm.
- Line width in silkscreen layer is 0.15 mm.
- Distance between F.Fab and silkscreen is impossible to predict.
- Distance between F.Fab and courtyard varies wildly. For some products, the three footprints differ in the courtyard being 0.10 mm, 0.25 mm, or 0.50 mm. For some other products, there is no courtyard at all!

I will examine how much the dimensional tolerances vary, across ICs with identical packages. Then, I will make custom footprints for RESC and CAPC from scratch. I will decide how to approach the numerous specific chips once I have the data stated previously.

---

I decided on the following physical dimensions for the landing pads.

| Part      | Pitch   | Z        | G       | X       |
| --------- | ------: | -------: | ------: | ------: |
| CAPC0603  | n/a     | 2.88 mm  | 0.56 mm | 1.08 mm |
| RESC0603  | n/a     | 2.85 mm  | 0.56 mm | 1.13 mm |
| RESC0805  | n/a     | 3.36 mm  | 0.97 mm | 1.56 mm |
| AD8615    | 0.95 mm | 4.19 mm  | 0.82 mm | 0.69 mm |
| OP37G     | 1.27 mm | 7.35 mm  | 2.74 mm | 0.69 mm |
| OPA828    | 1.27 mm | 7.35 mm  | 2.74 mm | 0.69 mm |
| DAC81401  | 0.65 mm | 7.75 mm  | 4.16 mm | 0.44 mm |
| ADS8699   | 0.65 mm | 7.75 mm  | 4.16 mm | 0.44 mm |
| ISO7641FM | 1.27 mm | 11.76 mm | 6.92 mm | 0.69 mm |

## August 20, 2025

![August 20](./Documentation/New/August20.png)

## August 22, 2025

Deliverables for today:
- Organize the footprint libraries into: TwoTerminal, ThreeTerminal, IC, ThroughHole
- Collect more purchase links and elaborate on more IC local decoupling networks.
  - Start with finalizing the trimpot variant.
  - Then, the discrete semiconductors.
  - Finally, the 0805 capacitors with tight dimensional tolerance.
- If possible, finalize the Schottky diode model as well. See whether all diode models can share a common footprint:
  - 1N4448W
  - LM4040D10
  - LM4040D33
  - Chosen Schottky diode model

I did not get through all the deliverables today. I was looking for a purchase link for LM79L15, and realized the TO-92-3 version was not in stock. To sort through options for alternative products, I tabulated the total current load at every regulator. The LM78L15 and LM79L15 have ~45 mA, which is below the rated 100 mA. However, the power dissipation is ~0.6 W, almost at the limit of safe operating area. These chips would be elevated 100°C above ambient temperature!

I decided to switch the 15 V, -15 V, and 5 V (Converter) lines to the higher-current versions, in a TO-220 package. This should reduce the temperature rise by a factor of 2.6&ndash;4, and put the wattage far below the rated limit. I will download fresh symbols from the manufacturer and update the schematic accordingly.

I may switch the PreAmp's power supplies over to TO-220 as well, for convenience. This choice reduces the time spent on footprint design. We're already using through-hole components, either way.

---

I also want to switch to the Bourns 3296 model, which is through-hole. It feels more comforting, knowing that the mechanical bond to the PCB is stronger. It also isn't significantly larger on the thickness dimension (4.8 mm vs 4.3 mm for SMT version).

This choice makes life a little bit easier. The footprints are through-hole, and thus probably easier to design. They also sit at the cost minimum compared to the SMT version.

| Model | Cost (Average) |
| ----- | -------------: |
| 3269W | $4.26          |
| 3296W | $2.36          |
| 3296Y | $2.16          |

## August 23, 2025

Many great developments today. I found a decoupling network for every IC that I agree with. I overhauled all the bypass capacitors, to a simple set of 3 capacitor values (100 nF, 1 μF, 10 μF). Boosted confidence that I can start fabricating the physical prototype close to the original August 31 deadline.

TODO list for the rest of today:
- Purchase links for regulators
- Purchase links for new trimpots (Y variant)
- Footprints for 0805 capacitors
- Purchase links for Schottky diodes
- Finalize all decoupling networks

## August 26, 2025

Next steps: figure out the footprints for linear regulators, potentiometers, and Teensy 4.1.

Reminded myself that I can just use jumper wires for the reversible connections between TIAs and the ADC. Less complex than a mechanical switch, and more flexible. For example, I can have both amplifiers disconnected from the ADC, so it directly senses the DAC output. Might also minimize some parasitics (capacitance, insulation resistance) of a switch on the current input to a TIA.

![August 26](./Documentation/New/August26.jpg)

I decided on Sullins Connector Solutions as the vendor for pin headers, pin sockets, and jumpers.

## August 27, 2025

![August 27, Part 1](./Documentation/New/August27_Part1.jpg)

This arrangement doesn't work. The connectors don't fit across the dimension of two separate batteries.

To make everything easier, we'll try a separate approach. Each of the 9 V batteries has a dedicated +/- connector. This connector is soldered directly onto the PCB. It costs slightly more than the battery itself, but saves a lot of design effort and space.

![August 27, Part 2](./Documentation/New/August27_Part2.png)

In KiCad, I'll use an isolated battery (not connected terminal-to-terminal with neighbors) to represent the connector. It would already be strange to specify the battery purchase link separately, as I'm getting all 8 in a single pack. Instead, each symbol will use the purchase link for the connector. There will be an additional dedicated footprint for this connector.

## August 28, 2025

I stumbled upon a very important insight for the PCB layout phase: [Source](https://www.eevblog.com/forum/kicad/connecting-two-nets/msg5608951/#msg5608951)

> I agree that the ratsnest does not help you match a specific capacitor to a specific pin on a specific part before you have the components placed the way you want them. I usually get capacitors at least close to the right component without specific effort because during initial component placement I'm in the habit of <b>drag-selecting groups of related components in the schematic</b> in order to get them selected in the schematic, and then using "Pack and Move Components" (should be mapped to the "P" hotkey) in the PCB to group them together. I usually stick each related group by itself somewhere off-board to see how it connects to other groups, and then <b>start moving entire unrouted and randomly arranged groups</b> to begin figuring out my placement strategy. Once I begin placing individual components within a group, the group at least contains the right capacitors (and only the right capacitors) for the IC(s) in that group. This will be less cumbersome if you have enough screen space to have the schematic and PCB tools open side by side.

## August 29, 2025

![August 29, Part 1](./Documentation/New/August29_Part1.jpg)

I can strategically place the two modules, to evenly distribute battery weight across the board. This makes it more mechanically / gravitationally stable, improving ease of use. I could even place them underneath, turning the batteries into the mechanical supports for the board. This choice would leave more room on the top for visually debuggable components.

![August 29, Part 2](./Documentation/New/August29_Part2.jpg)

_Arrangement to stop the USB cord from sagging and yanking the PCB off the table._

Interesting links and ideas for future reference:
- https://www.matsusada.com/product/matrix/high-voltage-amplifiers.html
- https://www.mouser.com/c/semiconductors/amplifier-ics/operational-amplifiers-op-amps/?m=Apex%20Microtechnology&supply%20voltage%20-%20max=900%20V~~1.25%20kV&instock=y&sort=pricing&rp=semiconductors%2Famplifier-ics%2Foperational-amplifiers-op-amps%7C~Supply%20Voltage%20-%20Max
- Using a DC-DC switching converter as the 1st stage in a series of filters for the power supply

## September 1, 2025

![PCB Render](./Documentation/New/PCB_Render.jpg)

## September 7, 2025

Things are not looking good. I am having an argument about who is going to fund the bill of materials, and its implications for my wider financial security. I was going to purchase everything today, but things just went off schedule.

There is a good chance I will purchase everything out of pocket. DIY projects don't really need crowdfunding; they're the scale of money accessible to any average person. It's the big things like getting a salary so you can spend 100% of your time working on something, that requires serious and secure funding. I started reading the Wikipedia series on [corporate law](https://en.wikipedia.org/wiki/Corporate_law).

On the hobby side, I am feeling burnt out on Minecraft. I might be gravitating back toward the hobby of Swift programming for "nanotech CAD", which is just for personal enjoyment and not financial stability.

## September 8, 2025

I am going to order all equipment today, out of pocket. Previously, I used a crowd-funding/group-funding approach, where 2-3 people split the bill for an order of ~$100. Looks like that approach no longer scales when I need $800 purchases on a timely deadline. That's where you benefit from liquid funds you have direct control over, in your own bank account. Money buys time.

[Bill of Materials (Google Sheets)](https://docs.google.com/spreadsheets/d/1QVJOiaRJbdRXN7CXii3z91K5A64h3VmgLCsRjpDpIrQ/edit?gid=1651612763#gid=1651612763)

![September 8, Part 1](./Documentation/New/September8_Part1.png)

I will be reimbursed for all of this, and receive back pay for the person hours spent on the Transimpedance Amplifier project, once I have financial stability. I will never resort to working part-time in unrelated jobs such as the AI field. I will always spend my time how I see fit, for my own priorities, on nobody else's schedule. Real world hardware is what gets serious funding.

### Confirmation Emails

![Emails - OSH Park](./Documentation/Orders/Email/OSH_Park.png)

![Emails - DigiKey](./Documentation/Orders/Email/DigiKey.png)

![Emails - SparkFun](./Documentation/Orders/Email/SparkFun.png)

![Emails - Amazon](./Documentation/Orders/Email/Amazon.png)

### OSH Park Invoice

<p align="center">
&nbsp;
  <img src="./Documentation/Orders/OSHPark/OSH_Park_Invoice_1.png" width="44.44%">
&nbsp;&nbsp;
  <img src="./Documentation/Orders/OSHPark/OSH_Park_Invoice_2.png" width="45.56%">
&nbsp;
</p>

### DigiKey Invoice

| Page | Screenshot |
| ---: | ---------- |
| 1    | ![](./Documentation/Orders/DigiKey/Page1.png) |
| 2    | ![](./Documentation/Orders/DigiKey/Page2.png) |
| 3    | ![](./Documentation/Orders/DigiKey/Page3.png) |
| 4    | ![](./Documentation/Orders/DigiKey/Page4.png) |
| 5    | ![](./Documentation/Orders/DigiKey/Page5.png) |
| 6    | ![](./Documentation/Orders/DigiKey/Page6.png) |
| 7    | ![](./Documentation/Orders/DigiKey/Page7.png) |
| 8    | ![](./Documentation/Orders/DigiKey/Page8.png) |
| 9    | ![](./Documentation/Orders/DigiKey/Page9.png) |
| 10   | ![](./Documentation/Orders/DigiKey/Page10.png) |

### Amazon Invoice

<p align="center">
&nbsp;
  <img src="./Documentation/Orders/Amazon/Amazon_Invoice_1.png" width="44.96%">
&nbsp;&nbsp;
  <img src="./Documentation/Orders/Amazon/Amazon_Invoice_2.png" width="45.04%">
&nbsp;
</p>

## September 9, 2025

I just had a brilliant idea, and was very delighted by it. However, I think it's best to continue my break from the hardware stuff. It's bad for my state of mind, to suddenly switch contexts like this. Whether the system eigenfrequency is sufficient, is one of many make-or-break unknowns.

Setting up an FEM simulation in FreeCAD of Phase IV system eigenfrequency
- Worst-case option needing 500 nm range on all coarse axes, and the fine Z axis
- 2 kinematic mounts stacked on top of each other for X and Y
- Unable to recycle coarse axes as fine axes
- Put this in the transimpedance-amplifier repo for convenience

On the other hand, perhaps I can simplify the system geometry to make the simulation more accessible. The nanoasperities of the kinematic mounts should not be explicitly modeled. I don't know what their effect is on the ultimate result, but there are probably more important big ideas. I think I can finish this entire simulation tonight.

| Material | Young's Modulus | Density | Speed of Sound |
| -------- | --------------: | ------: | -------------: |
| Lithium Niobate | 181&ndash;202 GPa | 4.64 g/mL | 6250&ndash;6560 m/s |
| Steel           | 190-210 GPa | 7.75-8.05 g/mL | 4950&ndash;5110 m/s |
| Aluminum Oxide  | 370-400 GPa | 3.99 g/mL | 9630&ndash;10010 m/s |
| Aluminum        | 69-70 GPa | 2.70 g/mL | 5060&ndash;5090 m/s |
| Epoxy           | 3&ndash;30 GPa | 1.1&ndash;1.43 g/mL | 1650&ndash;4580 m/s |
| Copper          | 110-128 GPa | 8.96 g/mL | 3500&ndash;3800 m/s |

It might be valid to just use the material parameters for aluminum, which I used in previous eigenfrequency simulations. Not messing with multi-material FEM simulations, which are a pain to set up and seem to break the FEM simulator for eigenfrequency.

I am using the Aluminum-Generic default in FreeCAD, which sets the Young's modulus to 70 GPa.

<p align="center">
&nbsp;
  <img src="./Documentation/New/September9_Part1.png" width="45.49%">
&nbsp;&nbsp;
  <img src="./Documentation/New/September9_Part2.png" width="44.51%">
&nbsp;
</p>

_Render of the 1st eigenmode with constraints included (6.9 kHz). The mesh has 33,190 nodes and 18,521 volumes. The entire assembly measures 65 x 50 x 100 mm._

Without any constraints:

| Eigenmode Number | Frequency |
| :--------------: | --------: |
| 1                | 3340 Hz   |
| 2                | 5231 Hz   |
| 3                | 7086 Hz   |

With the supporting outer surfaces constrained, as in real life:

| Eigenmode Number | Frequency |
| :--------------: | --------: |
| 1                | 6901 Hz   |
| 2                | 8025 Hz   |
| 3                | 11528 Hz  |

Eigenfrequency of a solid cube with the assembly's XYZ dimensions:

| Cube Side Length | Stiffness | Mass | Frequency |
| ---------------: | --------: | ---: | --------: |
| 0.065 m  | 4.55 GN/m | 0.741 kg | 12471 Hz |
| 0.050 m  | 3.50 GN/m | 0.338 kg | 16196 Hz |
| 0.100 m  | 7.00 GN/m | 2.700 kg |  8104 Hz |

## September 10, 2025

I forgot to order high-voltage insulated gloves. I am treating this project like a Phase I&ndash;II system, with regular exposure to ~1000 VDC. The official Class 0 with 1000 VAC / 1500 VDC seems sufficient. The next rank, Class 1, goes all the way to 7500 V, probably with proportionally greater thickness. It is critical to minimize thickness, because I need the dexterity to work with my equipment.

I chose a reputable vendor on Amazon, who specializes in manufacturing PPE. Their product lineup included two contenders, 3000 V and 8000 V rated, both called "Class 1". I chose the 3000 V option because it's enough for Phase II. However, Phase III and beyond will use an ion pump, which could reach 7000 V. I will worry about high-voltage dangers of the Phase III system in the distant future.

![September 10, Part 1](./Documentation/New/September10_Part1.jpg)

I retroactively included the high-voltage gloves in the BOM spreadsheet, under a "Late Purchases" category. I tabulated it as an "Amazon 2" order to simplify the accounting of different orders.

---

I read the datasheet for the Apex PA94 (900 V, 140 MHz) and the application note "General Operating Procedures". A first glance at the safe operating area has us worried, with 21 W quiescent power dissipation possibly overheating the device. The application note highlights that this specific concern is false for the high-voltage, high-speed products like PA94. I can operate it at 900 V and low output current with no problems.

The amplifier requires a bipolar ±450 V supply, not a unipolar 900 V. This is because the two input pins cannot be more than 450 V away from either supply pin. So the old idea of having a GND line and a 900 V line, then multiplexing them to double the effective range, cannot work. There's still an option to have two Apex op amps per piezo actuator (one driving each set of alternating plates), but this defeats the whole point of the original idea. So we've resolved the unknown regarding whether piezo range can be doubled for free.

Assume a conservative 800 (±400) V to avoid exceeding absolute maximum ratings, as a result of component tolerances. We have now resolved several unknowns regarding the system mechanics:

| Actuator | Piezo Plates | Range | DAC INL Imprecision |
| -------- | -----------: | ----: | ------------------: |
| Coarse X | 3 x 10       | 544 nm | ±8 pm |
| Coarse Y | 3 x 10       | 544 nm | ±8 pm |
| Coarse Z | 3 x 10       | 544 nm | ±8 pm |
| Fine X   | 6            | 326 nm | ±5 pm |
| Fine Y   | 6            | 326 nm | ±5 pm |
| Fine Z   | 12           | 652 nm | ±10 pm |

If the coarse piezo can also perform fine motion without sliding or variation of nm/V calibration, we can eliminate the Fine X and Fine Y piezos in Phase IV. The Fine Z piezo would remain, to be conservative.

The plate count is 30/54/114 for Phase I/II/IV. Assuming the current price for 10 mm, X-cut plates from Crystal Substrates ($27.60 each), that is $828/1460/3146. But this number doesn't include spares needed for failed piezo stack fabrication attempts. We should employ modular, reversibly bondable design techniques to allow recycling of piezoactuators from partially broken complete systems. That will minimize the number of spares needed.

### Sensors

| Device | Type | Phase 0.2 | Phase I | Phase II | Phase IV |
| ------ | ---- | :-------: | :-----: | :------: | :------: |
| Coarse X | DAC output | | | | ✅ |
| Coarse Y | DAC output | | | | ✅ |
| Coarse Z | DAC output | | ✅ | ✅ | ✅ |
| Fine X   | DAC output | | | ✅ | TBD |
| Fine Y   | DAC output | | | ✅ | TBD |
| Fine Z   | DAC output | ✅ | | ✅ | ✅ |
| Bias     | DAC output | TBD | TBD | ✅ | ✅ |
| V<sub>comp</sub> calibration | MDAC | TBD | TBD | ✅ | TBD |
| Tunneling current | ADC input | TBD | TBD | ✅ | ✅ |
| AD7745            | I2C interface | TBD | TBD | | | |
| Michelson interferometer X | TBD | | | | ✅ |
| Michelson interferometer Y | TBD | | | | ✅ |
| Michelson interferometer Z | TBD | | | | ✅ |

In the DAC81404, the full-scale output range for each DAC channel is independently programmable. This avoids any constraint that the bias voltage might need to use a different DAC than the piezos. The piezo DACs use the ±20 V mode, amplifying with a 20 V/V gain factor to ±400 V. The bias uses the ±10 V mode.

The AD7745 must be reversibly bondable to the Islam-Beamish junction. It cannot be used simultaneously with the custom TIA-based capacitance sensor. It also cannot be fed into a UHV chamber (too much parasitic capacitance on the coaxial cables). It is mostly a test, like a simpler backup option. In much the same way as the 100 MΩ amplifier in Phase 0.1. There are still unknowns regarding the mechanical configuration of the compensation capacitor, especially with an Islam-Beamish junction. The custom capacitance sensor was designed for a standard STM tunneling junction, where the compensation capacitance may differ from the tip-sample capacitance by an order of magnitude. The AD7745 has a built-in compensation capacitance (7-bit CAPDAC) and thus may be more suited for an Islam-Beamish junction.

The custom capacitance sensor is a <b>substitute for the Z-axis Michelson interferometer</b> from Phase IV. It cannot detect the surface position on first-time tip approach. Instead, one must hope the cryogenic UHV chamber supports optical windows to perform coarse approach by eye. Assume this needs to be automated through computer vision (CCD camera) at some point. Rather, these coarse position readouts allow repeatable approach during hundreds of round trips after the 1st tip approach. They require pre-existing knowledge of the exact position where the tip contacts the surface. The X and Y interferometers mostly prevent accumulation of drift (dead reckoning errors) that could make the exact line separating two Si sample wafers unknown. There will be extreme stress on the reliability of the interferometers, to prevent a single 2-pi ambiguity error from happening over hour or day-long mechanosynthetic build sequences. I think the whole coarse nanopositioning system will be fast enough that the bottleneck is inverted mode tip registration.

The PA95 looks like a better choice than the PA94 for my application. An order of magnitude less quiescent current. It's still reasonably affordable ($283 vs $215) and in stock (333 vs 408). The GBP is 10 MHz, so with 20 V/V, the first-order rolloff starts at 500 kHz. Still orders of magnitude greater than the mechanical eigenfrequency. The slew rate is 30 V/μs, so all 800 V could be traveled in 27 μs. For comparison, the time period of an oscillation at 6.9 kHz is 145 μs. A ±400 V sinewave at 6.9 kHz has a peak velocity of 17 V/μs. It's an open question whether 30 V/μs is enough to outpace the eigenfrequency or defeat static friction. I can purchase both options during Phase I, see whether PA95 is sufficient, then incorporate the less power-hungry option into Phase II.

With less power dissipation, there's less severe need to pull heat away as fast as possible. I can pack components closer together, have a smaller power supply, and possibly the tempco/ppm drift will be smaller. I will design a power supply for Phase I that can supply a single PA94 or six PA95's (the PA94 is more than all 6 combined). If the worst case is true, and slew rate is not enough (seems unlikely but you can never know for sure), I will redesign a new power supply from scratch for Phase II.

## September 11, 2025

![September 11, Part 1](./Documentation/New2/September11_Part1.jpg)

![September 11, Part 2](./Documentation/New2/September11_Part2.jpg)

![September 11, Part 3](./Documentation/New2/September11_Part3.jpg)

![September 11, Part 4](./Documentation/New2/September11_Part4.jpg)

## September 12, 2025

![September 12, Part 1](./Documentation/New2/September12_Part1.jpg)

![September 12, Part 2](./Documentation/New2/September12_Part2.jpg)

![September 12, Part 3](./Documentation/New2/September12_Part3.jpg)

![September 12, Part 4](./Documentation/New2/September12_Part4.jpg)

![September 12, Part 5](./Documentation/New2/September12_Part5.jpg)

![September 12, Part 6](./Documentation/New2/September12_Part6.jpg)

![September 12, Part 7](./Documentation/New2/September12_Part7.jpg)

[Using a $2 desoldering pump (YouTube)](https://www.youtube.com/shorts/qrw1d5Ns6gg)

[Teensy responds to EMI from a Human Finger Capacitor (YouTube)](https://www.youtube.com/watch?v=SzrVnSTCt6Q)

[Teensy does a countdown (YouTube)](https://www.youtube.com/watch?v=PW8NTNBJ09g)

[TeensyCountdown.cpp (GitHub Gist)](https://gist.github.com/philipturner/74579bf0ccc845478f56cf372457cc6a)

## September 15, 2025

I did not take the time to format the images for today's entry. Here is the link to the folder with them:

[September15](./Documentation/September15)

## September 16, 2025

Every single TSSOP on the 2nd practice board had 1-2 shorts. It’s flash flooding, so I had to pack up early before attempting the DAC81401 and ADS8699 on the main board. Parents are putting sand bags next to the garage doors.

![September 16, Part 1](./Documentation/September16/September16_Part1.jpg)

![September 16, Part 2](./Documentation/September16/September16_Part2.jpg)

![September 16, Part 3](./Documentation/September16/September16_Part3.jpg)

## September 17, 2025

By some unbelievable miracle, I got it right on the 3rd attempt at the main board. I will have to omit the 1 uF bypass capacitor from the ADC AVDD terminal, as I air-wired over a pad that got burnt off. And swap out the now heavily-oxidized soldering iron tip.

![September 17, Part 1](./Documentation/September17/September17_Part1.jpg)

![September 17, Part 2](./Documentation/September17/September17_Part2.jpg)

## September 18, 2025

Risky move, but I ended up soldering that 1 uF capacitor just fine.

![September 18, Part 1](./Documentation/September18/September18_Part1.jpg)

![September 18, Part 2](./Documentation/September18/September18_Part2.jpg)

Finished all surface-mount components

![September 18, Part 3](./Documentation/September18/September18_Part3.jpg)

Burnt off one of the pads for the DAC isolator. By sheer luck, it was the MISO channel, which is technically not needed. I thought everything was just fine, 100% of components done, then this unexpected nightmare happened at the very last moment.

![September 18, Part 4](./Documentation/September18/September18_Part4.jpg)

Tried forcing the soldering iron in between 2 leads to fix a short, which made it worse. Tried salvaging it with solder wick, which made it worse. Ended up shredding all the leads and prying the carcass of the chip, taking a delicate pad with it. You can see the pad (red)!

![September 18, Part 5](./Documentation/September18/September18_Part5.jpg)

One wrong move, and I have to wait several days to make progress again. And I don’t get the luxury of quitting and moving on to another stage, without succeeding in Phase 0.1. The stress is so bad that it doesn’t even phase me anymore.

![September 18, Part 6](./Documentation/September18/September18_Part6.jpg)

![September 18, Part 7](./Documentation/September18/September18_Part7.jpg)

![September 18, Part 8](./Documentation/September18/September18_Part8.jpg)

## September 19, 2025

Every single chip’s power pins give the correct voltage. Including both isolators and the TIAs (installed all jumpers). Tuned the PreAmp’s V+ to 2.503 V, proving adjustability to within 0.06%. V- is fixed at -1.475 V. Proceeding with SPI driver code for DAC and ADC.

![September 19, Part 1](./Documentation/September19/September19_Part1.jpg)