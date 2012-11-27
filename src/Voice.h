#pragma once

#include "maximilian.h"
#include <ctime>
#include <math.h>

enum class Wave { SINE, SQUARE, TRI, SAW, PULSE, NOISE };

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
	int maxVoices = 16;
	double adsrEnv[8];
	std::string OSC1wave = "sinewave",
				OSC2wave = "squarewave",
				LFO1wave = "sinewave",
				LFO2wave = "sinewave",
				filter = "none";
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
	void noteOn(int freq, int vel) {
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
	void noteOff(int notes) {
		if (notes == 0 || settings->polyphonic) {
			trigger = 0;
		}

		envelope.attackphase = 0;
		envelope.decayphase = 0;
		envelope.sustainphase = 0;
		envelope.releasephase = 1;
	}
	double play() {
		if (pitch == 0) return 0;

		double hertz = noteToFreq[pitch],
			   amp = (settings->LFO1amp == 0) ? 0 : settings->LFO1amp * (1 + LFO1out),
			   cutoff;

        LFO1out = LFO1.play(settings->LFO1wave, settings->LFO1freq);
        LFO2out = LFO2.play(settings->LFO2wave, settings->LFO2freq);
        VCO1out = VCO1.play(settings->OSC1wave, hertz * pow(2, settings->OSC1detune / 1200.0), settings->OSC1pw) * settings->OSC1volume * 0.5;
        VCO2out = VCO2.play(settings->OSC2wave, hertz * pow(2, settings->OSC2detune / 1200.0 + LFO1out * settings->LFO1amp), settings->OSC2pw) * settings->OSC2volume * 0.5;
        cutoff = settings->cutoff + (LFO2out * settings->LFO2amp);
        if (cutoff < 0) cutoff = 0;
        if (cutoff > 1) cutoff = 1;
        if (settings->filter == "lopass") {
        	VCFout = VCF.lopass(VCO1out + VCO2out, cutoff);
        } else if (settings->filter == "hipass") {
			VCFout = VCF.hipass(VCO1out + VCO2out, cutoff);
        } else {
        	VCFout = VCO1out + VCO2out;
        }

        return envelope.adsr(VCFout * max(30, velocity) / 127,
        					settings->attack,
        					settings->decay,
        					settings->sustain,
        					settings->release,
        					1,
        					trigger);
	}
	bool isPlaying() {
		return envelope.amplitude > 0;
	}
};
