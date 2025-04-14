# Seeed Arduino MIDIMaster

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## 💫 Project Overview

Seeed Arduino MIDIMaster is a professional-grade MIDI synthesizer development library designed specifically for the XIAO Serial microcontroller paired with the SAM2695 audio synthesis chip. This library provides comprehensive MIDI control capabilities, enabling creators, musicians, and developers to easily build complex musical projects, from simple monophonic melodies to multi-track chord performances.

![XIAO MIDI Synth Board](https://files.seeedstudio.com/wiki/XIAO_Series/MIDISynth.jpg)

## ✨ Core Features

### Sound & Expression
- **Professional Sound Library** - Built-in collection of 127 high-quality instrument sounds spanning pianos, strings, brass, percussion, and more
- **Pitch Control** - Precise scale adjustment system with support for real-time semitone shifting
- **Volume Dynamics** - Programmable volume control with 127 levels of fine adjustment
- **Tempo Adjustment** - Flexible BPM control for music rhythm variations

### Advanced Playback Features
- **16-Channel Multi-track Playback** - Support for controlling 16 independent MIDI channels simultaneously for complex arrangements
- **Chord Synthesis** - Advanced chord playback with support for arbitrary note combinations and chord complexity control
- **Professional Drum Kit** - Channel 9 dedicated drum machine functionality with various percussion sounds
- **Dynamic Expression** - Support for note velocity, vibrato, portamento and other MIDI expression parameters

### System Architecture
- **State Machine Management** - Efficient state machine design for clear, logical button operation
- **LED Status Indication** - Intuitive LED feedback system with different blinking patterns for different modes
- **Event Handling System** - Robust event processing with support for short press, long press and complex interactions
- **Reliable Serial Communication** - Support for both hardware and software serial interfaces, offering multiple connection options

## 🔧 Hardware Requirements

- **Processor:** Seeed XIAO Serial microcontroller
- **Synthesis Chip:** SAM2695 audio synthesis driver board
- **Connection:** XIAO expansion board (optional)
- **Power:** USB power or 3.7V lithium battery

## 📦 Installation Guide

### Arduino IDE Installation
```
1. Open Arduino IDE，eg: XIAO ESP32S3
2. Select Tools > Board Manager, install "Seeed XIAO ESP32S3"
3. Select Sketch > Include Library > Manage Libraries
4. Search for and install the "Seeed_Arduino_MIDIMaster" library
5. For software serial functionality, also install "EspSoftwareSerial" library
```

### PlatformIO Installation
```ini
# platformio.ini
[env:seeed_xiao_esp32s3]
platform = espressif32
board = seeed_xiao_esp32s3
framework = arduino
lib_deps = 
    seeed/Seeed_Arduino_MIDIMaster
    plerup/EspSoftwareSerial
```

## 📝 Usage Examples

### Basic MIDI Note Playing
```cpp
#include <SAM2695Synth.h>

SAM2695Synth synth;

void setup() {
  synth.begin();
  synth.setInstrument(0, 0);  // Set channel 0 to piano sound
}

void loop() {
  // Play middle C (note 60)
  synth.noteOn(0, 60, 100);
  delay(500);
  synth.noteOff(0, 60);
  delay(500);
}
```

### Multi-track Chord Performance
```cpp
#include <SAM2695Synth.h>

SAM2695Synth synth;

void setup() {
  synth.begin();
  synth.setInstrument(0, 0);   // Piano
  synth.setInstrument(1, 24);  // Nylon Guitar
  synth.setInstrument(9, 0);   // Drum channel
}

void loop() {
  // C major triad - Piano
  synth.noteOn(0, 60, 100); // C
  synth.noteOn(0, 64, 100); // E
  synth.noteOn(0, 67, 100); // G
  
  // Guitar accompaniment
  synth.noteOn(1, 48, 80);
  
  // Bass drum
  synth.noteOn(9, 36, 120);
  
  delay(1000);
  
  // Turn off all notes
  synth.allNotesOff();
  delay(500);
}
```

## 📚 API Documentation Summary

- `begin()` - Initialize the synthesizer
- `setInstrument(channel, instrument)` - Set the instrument sound for specified channel
- `noteOn(channel, note, velocity)` - Trigger specified note
- `noteOff(channel, note)` - Stop specified note
- `increaseBpm()/decreaseBpm()` - Adjust music speed
- `increasePitch()/decreasePitch()` - Adjust pitch
- `increaseVelocity()/decreaseVelocity()` - Adjust volume
- See full documentation for more advanced features

## ❓ Frequently Asked Questions

**Q: Is XIAO ESP32C3 supported?**  
A: Yes, this library is compatible with all XIAO series development boards, including ESP32C3, though some advanced features may be limited by processing power.

**Q: How do I connect external audio output?**  
A: The SAM2695 driver board provides a 3.5mm audio output that can be directly connected to headphones or powered speakers.

**Q: How many notes can be played simultaneously?**  
A: The SAM2695 chip supports up to 64 polyphonic voices, theoretically allowing up to 64 simultaneous notes.

## 🤝 Contribution Guidelines

Community contributions are welcome! If you'd like to participate in this project:

1. Fork this repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Submit a Pull Request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details

---

**Seeed Studio** | [Official Website](https://www.seeedstudio.com/) | [Technical Support](https://forum.seeedstudio.com/)

