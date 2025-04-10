//
// Created by Administrator on 25-4-1.
//

#include "SAM2695Synth.h"


SAM2695Synth::SAM2695Synth()
:_serial(nullptr)
,_softSerial(nullptr)
,_pitch(60)
,_velocity(90)
,_bpm(BPM_DEFAULT)
{

}

SAM2695Synth::~SAM2695Synth()
{
    if(_serial)
    {
        delete _serial;
        _serial = nullptr;
    }
    if(_softSerial)
    {
        delete _softSerial;
        _softSerial = nullptr;
    }
}

// Get the singleton, ensuring there is only one object.
SAM2695Synth& SAM2695Synth::getInstance()
{
    static SAM2695Synth instance;
    return instance;
}

// Initializes the SAM2695Synth object with a specified serial interface and baud rate
// This version is called when the 'begin' function is invoked with custom serial, baud, RX, and TX settings.
// Parameters:
//   serial - Pointer to the HardwareSerial object for communication (e.g., Serial1, Serial2) 
//            //default Serial2
//   baud - Baud rate for serial communication (e.g., 9600, 115200) 
//           //default 115200
//   RX - The pin number used for receiving data (RX)
//            //default 42
//   TX - The pin number used for transmitting data (TX)
//            //default 43
void SAM2695Synth::begin(HardwareSerial* serial, int baud, uint8_t RX, uint8_t TX)
{
    _serial = serial;
    _serial->begin(baud, SERIAL_8N1, RX, TX);
}

void SAM2695Synth::begin(SoftwareSerial *serial , int baud)
{
    _softSerial = serial;
    if(_softSerial != nullptr)
    {
        _softSerial->begin(baud);
    }
}

// Sets the instrument for a specific MIDI bank and channel by sending appropriate MIDI control and program change messages
// This version is called when the 'setInstrument' function is invoked with user-defined bank, channel, and value parameters.
// Parameters:
//   bank - The bank number (0-127) to select the instrument from.
//   channel - The MIDI channel (0-15) to which the instrument should be assigned.
//   value - The program change value (instrument number) to be applied on the specified channel and bank.
void SAM2695Synth::setInstrument(uint8_t bank, uint8_t channel, uint8_t value)
{
    uint8_t CMD_CONTROL_CHANGE_1[] = {
        (uint8_t)(MIDI_CMD_CONTROL_CHANGE | (channel & 0x0f)), 0x00, bank};

    sendCMD(CMD_CONTROL_CHANGE_1, sizeof(CMD_CONTROL_CHANGE_1));

    uint8_t CMD_PROGRAM_CHANGE_2[] = {
        (uint8_t)(MIDI_CMD_PROGRAM_CHANGE | (channel & 0x0f)), value};
    sendCMD(CMD_PROGRAM_CHANGE_2, sizeof(CMD_PROGRAM_CHANGE_2));
}

// Sends a MIDI "Note On" message to trigger a note on a specific MIDI channel
// This will play the note with the specified pitch and velocity on the given channel.
// Parameters:
//   channel - The MIDI channel (0-15) on which to send the "Note On" message.
//   pitch - The MIDI pitch value (0-127) representing the note to be played (e.g., 60 for Middle C).
//   velocity - The velocity (0-127) indicating how hard the note is struck (higher values for louder sounds).
void SAM2695Synth::setNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    if(-1 == pitch)
        pitch = _pitch;
    if(-1 == velocity)
        velocity = _velocity;
    uint8_t CMD_NOTE_ON[] = {(uint8_t)(MIDI_COMMAND_ON | (channel & 0x0f)),
                             pitch, velocity};
    sendCMD(CMD_NOTE_ON, sizeof(CMD_NOTE_ON));
    return ;
}

// Sends a MIDI "Note Off" message to stop a note on a specific MIDI channel
// This will stop the note with the specified pitch on the given channel. 
// The velocity is typically set to 0 for "Note Off" messages.
// Parameters:
//   channel - The MIDI channel (0-15) on which to send the "Note Off" message.
//   pitch - The MIDI pitch value (0-127) representing the note to be stopped (e.g., 60 for Middle C).
//   velocity - The velocity (0-127) which is usually ignored for "Note Off" messages but can be set to any value. In this case, it's always set to 0.
void SAM2695Synth::setNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    uint8_t CMD_NOTE_OFF[] = {(uint8_t)(MIDI_COMMAND_OFF | (channel & 0x0f)),
                              pitch, 0x00};
    sendCMD(CMD_NOTE_OFF, sizeof(CMD_NOTE_OFF));
}

// Sends a MIDI "All Notes Off" message to stop all notes on a specific MIDI channel
// This command turns off all active notes on the given MIDI channel by sending a control change 
// message with controller number 123 (All Notes Off) and value 0 (to turn off the notes).
// Parameters:
//   channel - The MIDI channel (0-15) on which to send the "All Notes Off" message.
void SAM2695Synth::setAllNotesOff(uint8_t channel)
{
    uint8_t CMD_CONTROL_CHANGE[] = {
        (uint8_t)(MIDI_CMD_CONTROL_CHANGE | (channel & 0x0f)), 0x7b, 0x00};
    sendCMD(CMD_CONTROL_CHANGE, sizeof(CMD_CONTROL_CHANGE));
}

// Plays a chord by sending MIDI "note on" messages for each active note
// This function iterates through all the notes in the provided chord and sends a 
// "note on" message for each note that is active (i.e., has its isOn flag set to true).
// Parameters:
//   chord - The chord data containing the notes to be played, their MIDI pitch, 
//           and velocity, along with the MIDI channel to use for the playback.
void SAM2695Synth::playChord(const musicData& chord)
{
    for (int i = 0; i < sizeof(chord.notes) / sizeof(chord.notes[0]); ++i) {
        if (chord.notes[i].isOn) {
            setNoteOn(chord.channel, chord.notes[i].pitch, chord.velocity);
        }
    }
}

// Sets the pitch value for the synthesizer.
// This value will be used when the pitch is not explicitly provided during note playback.
// Parameters:
//   pitch - The MIDI pitch value (0-127) to be set as the default pitch for the synthesizer.
void SAM2695Synth::setPitch(uint8_t pitch)
{
    _pitch = pitch;
}

// Retrieves the current pitch value of the synthesizer.
// This value is the default pitch that is used when the pitch is not explicitly provided during note playback.
// Returns:
//   The current MIDI pitch value (0-127) that is set for the synthesizer.
uint8_t SAM2695Synth::getPitch() const
{
    return _pitch;
}

// Sets the volume level for a specific MIDI channel.
// This function sends a "Control Change" message to set the volume (controller number 7) 
// for the specified channel to the provided level.
// Parameters:
//   channel - The MIDI channel (0-15) on which to set the volume.
//   level   - The volume level to be set (0-127), where 0 is the lowest volume and 127 is the highest.
void SAM2695Synth::setVolume(uint8_t channel, uint8_t level)
{
    uint8_t CMD_CONTROL_CHANGE[] = {
        (uint8_t)(MIDI_CMD_CONTROL_CHANGE | (channel & 0x0f)), 0x07, level};
    sendCMD(CMD_CONTROL_CHANGE, sizeof(CMD_CONTROL_CHANGE));
}

// Increases the pitch value by 1, ensuring it does not exceed the maximum allowed pitch.
// If the pitch exceeds the predefined maximum value (BANK0_FX4atmosphere), 
// it is set to that maximum value.
// This function ensures the pitch stays within the valid range.
// No parameters are needed.
void SAM2695Synth::increasePitch()
{
    _pitch++;
    if(_pitch > NOTE_C8) _pitch = NOTE_C8;
}

// Decreases the pitch value by 1, ensuring it does not go below the minimum allowed pitch.
// If the pitch goes below the predefined minimum value (BANK1_Clav), 
// it is set to that minimum value.
// This function ensures the pitch stays within the valid range.
// No parameters are needed.
void SAM2695Synth::decreasePitch()
{
    _pitch--;
    if(_pitch < NOTE_B0) _pitch = NOTE_B0;
}

// Increases the velocity of the synthesizer and adjusts the volume for all MIDI channels.
// This function increases the current velocity by a predefined step value, ensuring the velocity does not exceed the maximum allowed value.
// Then, it updates the volume for all MIDI channels (0 to 15) based on the new velocity value.
// The velocity is used to control the volume, where higher velocity corresponds to a higher volume level.
// The velocity value is capped at a maximum defined by VELOCITY_MAX.
// No parameters are required for this function.
void SAM2695Synth::increaseVelocity()
{
    _velocity += VELOCITY_STEP;
    if(_velocity > VELOCITY_MAX) _velocity = VELOCITY_MAX;
    for(int i = CHANNEL_0;i<=CHANNEL_15;i++){
        setVolume(i,_velocity);
    }
}

// Decreases the velocity of the synthesizer and adjusts the volume for all MIDI channels.
// This function decreases the current velocity by a predefined step value, ensuring the velocity does not fall below the minimum allowed value.
// Then, it updates the volume for all MIDI channels (0 to 15) based on the new velocity value.
// The velocity value is capped at a minimum defined by VELOCITY_MIN.
// No parameters are required for this function.
void SAM2695Synth::decreaseVelocity()
{
    _velocity -= VELOCITY_STEP;
    if(_velocity < VELOCITY_MIN) _velocity = VELOCITY_MIN;
    for(int i = CHANNEL_0;i<=CHANNEL_15;i++){
        setVolume(i,_velocity);
    }
}

// Increases the beats per minute (BPM) for the synthesizer.
// This function increments the current BPM by a predefined step value (BPM_STEP),
// and then updates the BPM setting for the synthesizer accordingly.
// The new BPM value is calculated by adding the BPM_STEP to the current BPM value.
// No parameters are required for this function.
void SAM2695Synth::increaseBpm()
{
    setBpm(_bpm + BPM_STEP);
}

// Decreases the beats per minute (BPM) by a defined step (BPM_STEP).
// The new BPM value is calculated by subtracting BPM_STEP from the current BPM value, 
// and then the setBpm function is called to apply the updated BPM.
// No parameters are needed.
void SAM2695Synth::decreaseBpm()
{
    setBpm(_bpm - BPM_STEP);
}

// Sets the beats per minute (BPM) to the specified value, ensuring it stays within the valid range (40 to 240).
// If the provided BPM is less than the minimum allowed value (BPM_MIN, 40), it is set to BPM_MIN.
// If the provided BPM is greater than the maximum allowed value (BPM_MAX, 240), it is set to BPM_MAX.
// Otherwise, the BPM is set to the specified value.
// The valid range for BPM is between 40 and 240.
void SAM2695Synth::setBpm(uint8_t bpm)
{
    if(bpm < BPM_MIN)
        bpm = BPM_MIN;
    else if(bpm > BPM_MAX)
        bpm = BPM_MAX;
    _bpm = bpm;
}

// Returns the current beats per minute (BPM) value.
// This function retrieves the current BPM value stored in the _bpm variable.
// The BPM value is returned as an unsigned 8-bit integer (uint8_t).
uint8_t SAM2695Synth::getBpm() const
{
    return _bpm;
}

// Sends a command to the serial interface.
// This function sends a byte array (cmd) of a specified length (len) through the serial port.
// It uses the _serial object's write method to transmit the command data.
// Parameters:
// - cmd: A pointer to a byte array containing the command data to be sent.
// - len: The length of the byte array to be sent.
void SAM2695Synth::sendCMD(byte* cmd, int len)
{
    if(_serial != nullptr){
        _serial->write(cmd, len);
    }
    if(_softSerial != nullptr){
        _softSerial->write(cmd, len);
    }
    if(_serial == nullptr && _softSerial == nullptr){
        Serial.println("hardware serial and software serial error! please check!");
    }
}
