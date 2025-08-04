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

Phase 0 of the [APM Roadmap](https://github.com/philipturner/apm-roadmap)

Objective: Gather as much data as possible about the TIA sensor of an STM, using both theory and experiment.

Deadline: August 31, 2025 for the first physical prototype

Table of Contents:
- [July 26, 2025](#july-26-2025)
- [July 27, 2025](#july-27-2025)
- [July 28, 2025](#july-28-2025)
- [July 29, 2025](#july-29-2025)
- [August 3, 2025](#august-3-2025)

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