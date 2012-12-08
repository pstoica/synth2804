#pragma once

#include "maximilian.h"
#include <ctime>
#include <math.h>

struct SynthSettings {
	float attack = 1.0,
		   decay = 1.0,
		   sustain = 1.0,
		   release = 0.99999,
		   cutoff = 1.0,
		   resonance = 10,
		   OSC1pw = 0.2,
		   OSC2pw = 0.2,
		   OSC1volume = 1,
		   OSC2volume = 1,
		   OSC1detune = 0,
		   OSC2detune = 0,
		   LFO1freq,
		   LFO2freq,
		   LFO1amp,
		   LFO2amp;
	bool polyphonic = true;
	int maxVoices = 8;
	double adsrEnv[8];
	Wave OSC1wave = Wave::SINE,
		 OSC2wave = Wave::SAW,
		 LFO1wave = Wave::SINE,
		 LFO2wave = Wave::SINE;
	Filter filter = Filter::FILTER_OFF;
};

class Voice {
public:
	static double noteToFreq[];
	static SynthSettings *settings;
	maxiOsc VCO1, VCO2, LFO1, LFO2;
	double VCO1out, VCO2out, LFO1out, LFO2out, VCFout, ADSRout;
	int trigger, stamp, pitch = 0;
	int velocity;
	maxiFilter VCF;
	maxiEnv envelope;
	bool operator<(Voice &v) {
		return this->stamp < v.stamp;
	}
	Voice() { };
	void noteOn(int freq, int vel);
	void noteOff(int notes);
	double play();
	bool isPlaying();
};
