/** 
 * This is a basic example code. The purpose of the code is to send MIDI signals via serial
 * from the XIAO series development board to make the SAM2695 chip produce sound.
 * You will need to define the serial port and its pins. This example uses the XIAO_ESP32S3.
 * For pin definitions, please refer to https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/
 * 
 * In this example, it demonstrates how to play chords.
 * You need to first define the structure required for the chord, which includes:
 *      Channel
 *      Note combination (the chord consists of multiple notes), with the second note being a flag that controls whether the note plays
 *      Volume
 *      Index (the index is not used in this example)
 *      Tempo
 * Button A is responsible for playing a chord on channel 1, which consists of two notes.
 * Button B is responsible for playing a chord on channel 2, which consists of four notes.
 */

#include <Arduino.h>
#include "SAM2695Synth.h"
#include "Button.h"
 
#ifdef __AVR__
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(2, 3); // RX, TX
    #define COM_SERIAL SSerial
    #define SHOW_SERIAL Serial
    SAM2695Synth<SoftwareSerial> synth = SAM2695Synth<SoftwareSerial>::getInstance();
#endif

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) ||  defined(ARDUINO_XIAO_RA4M1) 
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(D7, D6); // RX, TX
    #define COM_SERIAL SSerial
    #define SHOW_SERIAL Serial
    SAM2695Synth<SoftwareSerial> synth = SAM2695Synth<SoftwareSerial>::getInstance();
#endif

#if  defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32S3)
    #define COM_SERIAL Serial0
    #define SHOW_SERIAL Serial
    SAM2695Synth<HardwareSerial> synth = SAM2695Synth<HardwareSerial>::getInstance();
#endif

#ifdef SEEED_XIAO_M0
    #define COM_SERIAL Serial1
    #define SHOW_SERIAL Serial
    SAM2695Synth<Uart> synth = SAM2695Synth<Uart>::getInstance();
#elif defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
    #define COM_SERIAL Serial1
    #define SHOW_SERIAL SerialUSB
    SAM2695Synth<Uart> synth = SAM2695Synth<Uart>::getInstance();
#endif

#if defined(NRF52840_XXAA)
    #ifdef USE_TINYUSB
    #include <Adafruit_TinyUSB.h>
    #endif
    #define COM_SERIAL Serial1
    #define SHOW_SERIAL Serial

    SAM2695Synth<Uart> synth = SAM2695Synth<Uart>::getInstance();
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
    #define BUTTON_A_PIN 4
    #define BUTTON_B_PIN 3
    #define BUTTON_C_PIN 2 
    #define BUTTON_D_PIN 1 
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    #define BUTTON_A_PIN 5
    #define BUTTON_B_PIN 4
    #define BUTTON_C_PIN 3 
    #define BUTTON_D_PIN 2 
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
    #define BUTTON_A_PIN 21
    #define BUTTON_B_PIN 2
    #define BUTTON_C_PIN 1 
    #define BUTTON_D_PIN 0
#else //You need to refer to the corresponding manual...
    #define BUTTON_A_PIN 0 //You need to refer to the corresponding manual...
    #define BUTTON_B_PIN 1 //You need to refer to the corresponding manual...
    #define BUTTON_C_PIN 2 //You need to refer to the corresponding manual...
    #define BUTTON_D_PIN 3 //You need to refer to the corresponding manual...
#endif

//Define the structure needed for the button
BtnState btnA = {HIGH, HIGH, 0, 0, false};
BtnState btnB = {HIGH, HIGH, 0, 0, false};

//Example of multi-track chord data definition
// This structure controls playback on channel 9, with only two notes being played. 
// The sound is set to the default sound, and the duration can be adjusted as needed.
const musicData chordTwoNotes =
{
    CHANNEL_1,                          //play on channel 9
    {
        {NOTE_C2, true},                //NOTE_C2 on
        {NOTE_FS2, true},               //NOTE_FS2 on
        {NOTE_E6, false},               //NOTE_E6 off
        {NOTE_CS5, false},              //NOTE_CS5 off
    },
    VELOCITY_DEFAULT ,                  //default velocity(64)
    0,                                  // index Not used here
    BPM_DEFAULT*8,                      //interval time
};

const musicData chordFourNotes =
{
    CHANNEL_2,                          // play on channel 0
    {
        {NOTE_F2, true},                // NOTE_F2 on
        {NOTE_B2, true},                // NOTE_B2 on
        {NOTE_E6, true},                // NOTE_E6 on
        {NOTE_CS5, true},               // NOTE_CS5 on
    },
    VELOCITY_DEFAULT ,                  // default velocity(64)
    0,                                  // index Not used here
    BPM_DEFAULT*4,                      // interval time
};

bool chordTwoNotesFlag = false;         // on or off chord_twoNotes
bool chordFourNotesFlag = false;        // on or off chord_fourNotes
unsigned long preMillisTwoNotes = 0;    // Record the time of the last MIDI signal sent on chord_twoNotes
unsigned long preMillisFourNotes= 0;    // Record the time of the last MIDI signal sent on const musicData chord_fourNotes =


void setup()
{
    //serial init to usb
    SHOW_SERIAL.begin(USB_SERIAL_BAUD_RATE);
    // Synth initialization. Since a hardware serial port is used here, the software serial port is commented out.
    synth.begin(COM_SERIAL, MIDI_SERIAL_BAUD_RATE);
    synth.setInstrument(0,CHANNEL_0,unit_synth_instrument_t::GrandPiano_1);
    // Initialize the buttons you are using.
    initButtons(BUTTON_A_PIN);
    initButtons(BUTTON_B_PIN);

    delay(1000);
}

void loop()
{
    detectButtonEvents(BUTTON_A_PIN, btnA, shortPressFlag_A, longPressFlag_A, releaseFlag_A);
    detectButtonEvents(BUTTON_B_PIN, btnB, shortPressFlag_B, longPressFlag_B, releaseFlag_B);
    if(shortPressFlag_A)
    {
        shortPressFlag_A = false;
        chordTwoNotesFlag = !chordTwoNotesFlag;
    }
    if(shortPressFlag_B)
    {
        shortPressFlag_B = false;
        chordFourNotesFlag = !chordFourNotesFlag;
    }
    play();
}

void play()
{
    unsigned long currentMillis = millis();
    if(chordTwoNotesFlag)
    {
        if (currentMillis - preMillisTwoNotes >= chordTwoNotes.delay)
        {
            preMillisTwoNotes = currentMillis;
            synth.playChord(chordTwoNotes);
        }
    }

    if(chordFourNotesFlag)
    {
        if (currentMillis - preMillisFourNotes >= chordFourNotes.delay)
        {
            preMillisFourNotes = currentMillis;
            synth.playChord(chordFourNotes);
        }
    }
}