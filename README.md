# c12880maPi
Interface software for a c12880ma + PureEngneering interface + Waveshare ARPI600 with the Raspberry Pi

## Overview

This will be the initial steps in operating a [Hamamatsu C12880MA](https://www.hamamatsu.com/eu/en/product/type/C12880MA/index.html) spectrometer on a Raspberry Pi. PureEngineering developed a [breakout board](https://www.pureengineering.com/projects/c12666ma) to interface easily with the pinout of Arduino Unos. They also provided an initial interface .ino file for reading data from the device. While this a good step for initial evaluation of the spectrometer at low cost, I wanted to push this further.  Ideally, I wanted to develop a self-contained solution which could be remotely triggered, store data locally, flexibly change settings and run the spectrometer at its limits. Combining this spectrometer with a Raspberry Pi would easily allow for most of these characteristics.

The Raspberry Pi natively does not have an ADC and the intensity readout of the C12880MA is in analog (even though it is digitially controlled). The breakout board then requires some other circuit to read in the analog data. Rather than develop on a protoboard or with a prototype PCB, the first step would be to replicate the Arduino hardware for Raspberry Pi and while still using the breakout board. The [ARPI600 by Waveshare](https://www.waveshare.com/arpi600.htm) does exactly that.

This repository contains the C++ code needed to communicate with the C12880MA and to read data through the ARPI600 (a TC1543 ADC). It also contains a CLI and GUI to automate and evaluate its operation. Hopefully this will lead to the development of a custom PCB with a more advanced ADC for better time and bit resolution, or this project's inclusion into something else.

