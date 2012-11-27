/// a single multi byte MIDI message
///
/// check status type and grab data:
/// 
/// if(midiArg.status == MIDI_NOTE_ON) {
///		cout << "note on " << midiArg.channel
///			 << midiArg.note << " " << midiArg.velocity
///			 << endl;
/// }
///
/// the message-specific types are only set for the appropriate
/// message types ie pitch is only set for noteon, noteoff, and
/// polyaftertouch messages
/// pulled from ofxMidi
///
#pragma once

#include "MidiConstants.h"
#include <vector>
#include <sstream>
#include <iomanip>

class MidiMessage {

public:

/// \section Variables

	MidiStatus status;
	int channel;		//< 1 - 16

	/// message-specific values,
	/// converted from raw bytes
	int pitch;			//< 0 - 127
	int velocity;		//< 0 - 127
	int control;		//< 0 - 127
	int value;			//< depends on message status type

	/// raw bytes
	std::vector<unsigned char> bytes;

	/// delta time since last message in ms
	double deltatime;

	/// the input port we received this message from
	///
	/// note: portNum will be -1 from a virtual port
	///
	int portNum;
	std::string portName;

/// \section Main

	MidiMessage();
	MidiMessage(std::vector<unsigned char>* rawBytes);
	MidiMessage(const MidiMessage& from);
	MidiMessage& operator=(const MidiMessage& from);

	/// clear the message contents, also resets status
	void clear();

/// \section Util

	/// get the raw message as a string in the format:
	///
	/// PortName: status channel [ raw bytes in hex ] deltatime
	///
	std::string toString();

	/// get a midi status byte as a string
	/// ie "Note On", "Note Off", "Control Change", etc
	static std::string getStatusString(MidiStatus status);
};