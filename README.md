<img width="4080" height="3060" alt="WhatsApp Image 2026-09-14 at 21 59 07" src="https://github.com/user-attachments/assets/9e2dcb0b-11a9-4075-9a0b-3012a2309232" />
# EEG-Based Brain–Computer Interface (BCI)

### Designing an 8-Channel EEG Acquisition System Using ADS1299 and ESP32

I'm working on developing a custom EEG acquisition board to capture electrical signals from the human brain and explore their use in Brain–Computer Interface (BCI) applications.

The project combines electronics, PCB design, embedded systems, and signal processing. My main focus is designing a low-noise PCB around the ADS1299 and interfacing it with an ESP32 for multichannel neural signal acquisition.

The long-term goal is to explore how brain signals can be processed and translated into commands for interacting with computers or external devices, particularly for assistive technology.

## Project Overview

The system uses the ADS1299, a low-noise analog front end designed for biopotential measurements, along with an ESP32 for communication and data handling.

Some of the main areas I'm working on include:

- Designing a custom PCB for 8-channel EEG signal acquisition.
- Interfacing the ADS1299 with the ESP32 using SPI communication.
- Improving signal quality through careful PCB layout, grounding, and noise reduction techniques.
- Designing electrode input protection and reference and bias circuitry.
- Developing firmware for multichannel data acquisition.
- Exploring EEG filtering, feature extraction, and machine learning for BCI applications.

## Hardware Components

| Component        | Purpose                                                                    |
| ---------------- | -------------------------------------------------------------------------- |
| ADS1299          | Acquires and digitizes low-amplitude biopotential signals                  |
| ESP32            | Controls acquisition and handles data communication                        |
| EEG electrodes   | Capture electrical activity from the scalp                                 |
| Custom PCB       | Integrates the analog front end, microcontroller, and supporting circuitry |
| Power regulation | Provides the required supply voltages                                      |

## Hardware Communication

The ADS1299 communicates with the ESP32 through SPI. Additional control signals manage acquisition and indicate when new data is available.

| Signal      | ESP32 GPIO |
| ----------- | ---------: |
| SCLK        |    GPIO 18 |
| MOSI / DIN  |    GPIO 23 |
| MISO / DOUT |    GPIO 19 |
| CS          |     GPIO 5 |
| DRDY        |     GPIO 4 |
| START       |    GPIO 27 |
| RESET       |    GPIO 33 |
| PWDN        |    GPIO 32 |

These are the planned pin assignments and will be verified during hardware testing.

## Low-Noise PCB Design

One of the main challenges in this project is designing a PCB that can handle very small EEG signals without introducing excessive noise.

I'm focusing on component placement, short analog signal paths, appropriate grounding, power supply decoupling, and reducing interference from digital switching circuits.

I'm also working on electrode input protection and reference and bias circuitry, which are important for reliable EEG acquisition.

The aim is to build a compact and practical hardware platform that can support further experimentation with neural signals.

## EEG Signal Processing

Once the hardware can acquire reliable signals, the next step is to process and analyze the data.

The planned signal-processing pipeline includes:

- **Band-pass filtering:** Reduce unwanted low- and high-frequency components.
- **50 Hz notch filtering:** Reduce power-line interference where necessary.
- **Artifact analysis:** Identify noise caused by eye blinks, muscle activity, and movement.
- **Power spectral density:** Use Welch's method to analyze the frequency content of EEG signals.
- **Band-power extraction:** Study delta, theta, alpha, beta, and gamma frequency bands.
- **Classification:** Explore LDA and other machine learning algorithms to classify EEG features.

These techniques will help me understand the acquired signals and explore their potential use in a basic BCI system.

## Technologies and Tools

- **Hardware:** ADS1299, ESP32, EEG electrodes
- **PCB Design:** Schematic capture, footprint selection, PCB layout, DRC/ERC
- **Embedded Systems:** ESP32 firmware and SPI communication
- **Signal Processing:** Digital filtering, PSD estimation, EEG band-power analysis
- **Machine Learning:** Feature extraction and EEG classification
- **Programming:** C/C++ and Python

## Current Progress

- [x] Hardware architecture and component selection
- [x] Initial schematic and PCB design
- [ ] PCB fabrication and assembly
- [ ] Power supply and hardware verification
- [ ] ADS1299 communication and configuration
- [ ] Multichannel signal acquisition and validation
- [ ] EEG data visualization
- [ ] Signal-processing pipeline
- [ ] EEG feature extraction and classification

I'll update this section as the project progresses and each stage is tested.

## Motivation

I'm interested in the intersection of electronics, neuroscience, and artificial intelligence. This project gives me an opportunity to work on a real hardware system while learning about neural signals, low-noise circuit design, embedded programming, and machine learning.

In the future, I hope to explore how this platform could support simple assistive interfaces and other experimental BCI applications.

## Collaboration

I'm open to feedback, suggestions, and collaboration from people interested in EEG, brain–computer interfaces, embedded systems, PCB design, and neural signal processing.

If you're working on something similar, feel free to connect or contribute.

## Disclaimer

This is an educational and research project, not a certified medical device. Testing involving human participants requires appropriate electrical safety precautions, suitable isolation, and compliance with applicable research ethics requirements.

---

**Project:** Multichannel Neural Signal Acquisition for Brain–Computer Interface

**Hardware:** ADS1299 and ESP32

**PCB Revision:** Rev 1.0

**Status:** Prototype development
