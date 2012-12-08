#include "Voice.h"

double Voice::noteToFreq[]={0, 8.661957, 9.177024, 9.722718, 10.3, 10.913383, 11.562325, 12.25, 12.978271, 13.75, 14.567617, 15.433853, 16.351599, 17.323914, 18.354048, 19.445436, 20.601723, 21.826765, 23.124651, 24.5, 25.956543, 27.5, 29.135235, 30.867706, 32.703197, 34.647827, 36.708096, 38.890873, 41.203445, 43.65353, 46.249302, 49., 51.913086, 55., 58.27047, 61.735413, 65.406395, 69.295654, 73.416191, 77.781746, 82.406891, 87.30706, 92.498604, 97.998856, 103.826172, 110., 116.540939, 123.470825, 130.81279, 138.591309, 146.832382, 155.563492, 164.813782, 174.61412, 184.997208, 195.997711, 207.652344, 220., 233.081879, 246.94165, 261.62558, 277.182617,293.664764, 311.126984, 329.627563, 349.228241, 369.994415, 391.995422, 415.304688, 440., 466.163757, 493.883301, 523.25116, 554.365234, 587.329529, 622.253967, 659.255127, 698.456482, 739.988831, 783.990845, 830.609375, 880., 932.327515, 987.766602, 1046.502319, 1108.730469, 1174.659058, 1244.507935, 1318.510254, 1396.912964, 1479.977661, 1567.981689, 1661.21875, 1760., 1864.655029, 1975.533203, 2093.004639, 2217.460938, 2349.318115, 2489.015869, 2637.020508, 2793.825928, 2959.955322, 3135.963379, 3322.4375, 3520., 3729.31, 3951.066406, 4186.009277, 4434.921875, 4698.63623, 4978.031738, 5274.041016, 5587.651855, 5919.910645, 6271.926758, 6644.875, 7040., 7458.620117, 7902.132812, 8372.018555, 8869.84375, 9397.272461, 9956.063477, 10548.082031, 11175.303711, 11839.821289, 12543.853516, 13289.75};
SynthSettings *Voice::settings;

void Voice::noteOn(int freq, int vel) {
	stamp = std::time(0);
	trigger = 1;
	velocity = vel;
	LFO1.phaseReset(0);
	pitch = freq;
	envelope.attackphase = 1;
	envelope.decayphase = 0;
	envelope.sustainphase = 0;
	envelope.releasephase = 0;
}

void Voice::noteOff(int notes) {
	if (notes == 0 || settings->polyphonic) {
		trigger = 0;
	}

	envelope.attackphase = 0;
	envelope.decayphase = 0;
	envelope.sustainphase = 0;
	envelope.releasephase = 1;
}

double Voice::play() {
	if (pitch == 0) return 0;

	double hertz = noteToFreq[pitch],
		   amp = (settings->LFO1amp == 0) ? 0 : settings->LFO1amp * (1 + LFO1out),
		   cutoff;

    LFO1out = LFO1.play(settings->LFO1wave, settings->LFO1freq);
    LFO2out = LFO2.play(settings->LFO2wave, settings->LFO2freq);
    VCO1out = VCO1.play(settings->OSC1wave, hertz * pow(2, settings->OSC1detune / 1200.0), settings->OSC1pw) * settings->OSC1volume * 0.5;
    VCO2out = VCO2.play(settings->OSC2wave, hertz * pow(2, settings->OSC2detune / 1200.0 + LFO1out * settings->LFO1amp), settings->OSC2pw) * settings->OSC2volume * 0.5;
    cutoff = settings->cutoff + (LFO2out * settings->LFO2amp);

    // clip the cutoff amount
    if (cutoff < 0) cutoff = 0;
    if (cutoff > 1) cutoff = 1;

    switch (settings->filter) {
    	case Filter::LOPASS:
    		VCFout = VCF.lopass(VCO1out + VCO2out, cutoff);
    		break;
    	case Filter::HIPASS:
    		VCFout = VCF.hipass(VCO1out + VCO2out, cutoff);
    		break;
    	case Filter::FILTER_OFF:
    	default:
    		VCFout = VCO1out + VCO2out;
    		break;
    }

    return envelope.adsr(VCFout * max(30, velocity) / 127, // force a minimum velocity of 30
    					settings->attack,
    					settings->decay,
    					settings->sustain,
    					settings->release,
    					1,
    					trigger);
}

bool Voice::isPlaying() {
	return envelope.amplitude > 0;
}
