/* adapted from ofxMidi */
#pragma once

#include <vector>
#include <sstream>
#include <iomanip>

enum MidiStatus {

	MIDI_UNKNOWN			= 0x00,

	// channel voice messages
	MIDI_NOTE_OFF			= 0x80,
	MIDI_NOTE_ON			= 0x90,
	MIDI_CONTROL_CHANGE		= 0xB0,
	MIDI_PROGRAM_CHANGE		= 0xC0,
	MIDI_PITCH_BEND			= 0xE0,
	MIDI_AFTERTOUCH			= 0xD0,	// aka channel pressure
	MIDI_POLY_AFTERTOUCH	= 0xA0,	// aka key pressure

	// system messages
	MIDI_SYSEX				= 0xF0,
	MIDI_TIME_CODE			= 0xF1,
	MIDI_SONG_POS_POINTER	= 0xF2,
	MIDI_SONG_SELECT		= 0xF3,
	MIDI_TUNE_REQUEST		= 0xF6,
	MIDI_SYSEX_END			= 0xF7,
	MIDI_TIME_CLOCK			= 0xF8,
	MIDI_START				= 0xFA,
	MIDI_CONTINUE			= 0xFB,
	MIDI_STOP				= 0xFC,
	MIDI_ACTIVE_SENSING		= 0xFE,
	MIDI_SYSTEM_RESET		= 0xFF
};

class MidiMessage {

public:

	MidiStatus status;
	int channel;		// 1 - 16

	// message-specific values,
	// converted from raw bytes
	int pitch;			// 0 - 127
	int velocity;		// 0 - 127
	int control;		// 0 - 127
	int value;			// depends on message status type

	/// raw bytes
	std::vector<unsigned char> bytes;

	/// delta time since last message in ms
	double deltatime;

	// the input port we received this message from
	// note: portNum will be -1 from a virtual port
	int portNum;
	std::string portName;

	MidiMessage() { };
	MidiMessage(const MidiMessage& from);
	MidiMessage& operator=(const MidiMessage& from);
	void swap(MidiMessage &from);
	static MidiMessage* makeFrom(double deltatime, std::vector<unsigned char>* message);

	// clear the message contents, also resets status
	void clear();

	// get the raw message as a string in the format:
	//
	// PortName: status channel [ raw bytes in hex ] deltatime
	std::string toString();

	/// get a midi status byte as a string
	/// ie "Note On", "Note Off", "Control Change", etc
	static std::string getStatusString(MidiStatus status);
};