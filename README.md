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
- Lack of understanding about electrial performance of reversible DUT connection point (DUT = resistor, capacitor)
- Need to elaborate on requirements for ventilation from acid fumes, and other specific equipment to order for hand soldering
- Need to resolve the issues with incorrect/unnecessary bypass/decoupling capacitors
- Need to resolve the missing/incorrect KiCad symbols and footprints for each IC. Organize the footprints on a physical PCB model and become familiar with the specific pin functions & positions.
- Need to clarify the bothersome trace width variable

<b>Fast-track work on the physical prototype.</b> Ensure that all fabrication equipment, testing equipment, and circuit parts are delivered by August 12, 2025. In any leftover time until August 12, I can return to the three simulation tasks.