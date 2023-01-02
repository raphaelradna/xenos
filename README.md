# Xenos

Xenos is a virtual instrument plug-in that implements and extends the Dynamic Stochastic Synthesis (DSS) algorithm invented by Iannis Xenakis. It is open-source, cross-platform, and can be built in a number of plugin formats.

![](Extra/xenosInterface.png "The Xenos interface")

Features include:
- Authentic DSS engine
- Xenharmonic pitch quantizer
- Custom scale import in the [Scala](https://www.huygens-fokker.org/scala/) format
- Ten stochastic distributions with up to two parameters each
- First- and second-order random walks
- Variable number of segments per wave cycle
- Variable amplitude envelope
- Polyphonic (64 voices by default)
- MIDI implementation (notes, sustain, pitch bend)
- External MIDI controller assignment
- Parameter automation
- Simple and streamlined interface
- Free and open source

## Build

1. [Download](https://juce.com/get-juce/download) JUCE
2. Clone or [download](https://github.com/raphaelradna/xenos/archive/refs/heads/main.zip) Xenos
3. Open Xenos.jucer in the Projucer
4. Export the project for your IDE and platform (e.g. Xcode (maxOS) or Visual Studio 2019); see [here](https://docs.juce.com/master/tutorial_new_projucer_project.html) for help
5. Compile Xenos using your IDE (or make on Linux)
6. Copy the plug-in binary (e.g., Xenos.component, Xenos.vst3, etc.) to the proper location for your platform and host software
7. Open a suitable plug-in host application, add Xenos on a virtual instrument track, and enjoy!

## Pre-built releases and user manual coming soon!