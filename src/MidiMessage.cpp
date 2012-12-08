#include "MidiMessage.h"
#include <iostream>

// -----------------------------------------------------------------------------
MidiMessage::MidiMessage(const MidiMessage& from) :
	status(from.status), channel(from.channel), pitch(from.pitch),
	velocity(from.velocity), control(from.control), value(from.value),
	bytes(from.bytes), deltatime(from.deltatime), portNum(from.portNum),
	portName(from.portName) { }

void MidiMessage::swap(MidiMessage &from) {
	std::swap(status, from.status);
	std::swap(channel, from.channel);
	std::swap(pitch, from.pitch);
	std::swap(velocity, from.velocity);
	std::swap(control, from.control);
	std::swap(value, from.value);
	std::swap(bytes, from.bytes);
	std::swap(deltatime, from.deltatime);
	std::swap(portNum, from.portNum);
	std::swap(portName, from.portName);
	bytes.swap(from.bytes);
}

// -----------------------------------------------------------------------------
MidiMessage& MidiMessage::operator=(const MidiMessage& from) {
	MidiMessage temp(from);
	this->swap(temp);

	return *this;
}

// -----------------------------------------------------------------------------
void MidiMessage::clear() {
	status = MIDI_UNKNOWN;
	channel = 0;
	pitch = 0;
	velocity = 0;
	control = 0;
	value = 0;
	deltatime = 0;
	portNum = -1;
	portName = "";
	bytes.clear();
}

// -----------------------------------------------------------------------------
std::string MidiMessage::toString() {
	std::stringstream stream;
	stream << (int) bytes[0] << std::endl;
	stream << portName << ": " << getStatusString(status) << " "
		   << channel << " [ ";
	for(unsigned int i = 0; i < bytes.size(); ++i) {
		stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
			   << (int) bytes[i] << " "
			   << std::dec << std::nouppercase << std::setw(1) << std::setfill(' ');
	}
	stream << "] " << deltatime;
	return stream.str();
}

// -----------------------------------------------------------------------------
std::string MidiMessage::getStatusString(MidiStatus status) {
	switch(status) {
		case MIDI_NOTE_OFF:
			return "Note Off";
		case MIDI_NOTE_ON:
			return "Note On";
		case MIDI_CONTROL_CHANGE:
			return "Control Change";
		case MIDI_PROGRAM_CHANGE:
			return "Program Change";
		case MIDI_PITCH_BEND:
			return "Pitch Bend";
		case MIDI_AFTERTOUCH:
			return "Aftertouch";
		case MIDI_POLY_AFTERTOUCH:
			return "Poly Aftertouch";
		case MIDI_SYSEX:
			return "Sysex";
		case MIDI_TIME_CODE:
			return "Time Code";
		case MIDI_SONG_POS_POINTER:
			return "Song Pos";
		case MIDI_SONG_SELECT:
			return "Song Select";
		case MIDI_TUNE_REQUEST:
			return "Tune Request";
		case MIDI_SYSEX_END:
			return "Sysex End";
		case MIDI_TIME_CLOCK:
			return "Time Clock";
		case MIDI_START:
			return "Start";
		case MIDI_CONTINUE:
			return "Continue";
		case MIDI_STOP:
			return "Stop";
		case MIDI_ACTIVE_SENSING:
			return "Active Sensing";
		case MIDI_SYSTEM_RESET:
			return "System Reset";
		default:
			return "Unknown";
	}
}

//static MidiMessage *makeFrom(double deltatime, std::vector<unsigned char>* message);

// -----------------------------------------------------------------------------
MidiMessage* MidiMessage::makeFrom(double deltatime, std::vector<unsigned char>* message) {
	MidiMessage *temp = new MidiMessage;

	for(unsigned int i = 0; i < message->size(); ++i)
		temp->bytes.push_back(message->at(i));

	temp->status = (MidiStatus) (message->at(0) & 0xF0);
	temp->channel = (int) (message->at(0) & 0x0F)+1;

	temp->deltatime = deltatime; // * 1000; // convert s to ms

	switch(temp->status) {
		case MIDI_NOTE_ON :
		case MIDI_NOTE_OFF:
			temp->pitch = (int) message->at(1);
			temp->velocity = (int) message->at(2);
			break;
		case MIDI_CONTROL_CHANGE:
			temp->control = (int) message->at(1);
			temp->value = (int) message->at(2);
			break;
		case MIDI_PROGRAM_CHANGE:
		case MIDI_AFTERTOUCH:
			temp->value = (int) message->at(1);
			break;
		case MIDI_PITCH_BEND:
			temp->value = (int) (message->at(2) << 7) +
								(int) message->at(1); // msb + lsb
			break;
		case MIDI_POLY_AFTERTOUCH:
			temp->pitch = (int) message->at(1);
			temp->value = (int) message->at(2);
			break;
		default:
			break;
	}

	return temp;
}