#pragma once

#include "maximilian.h"
#include <ctime>

enum class Wave { SINE, SQUARE, TRI, SAW, PULSE, NOISE };

struct SynthSettings {
	double attack = 0.00001,
		   decay = 0.99,
		   sustain = 0.125,
		   release = 0.99999,
		   VCO1volume = 1,
		   VCO2volume = 1;
	bool polyphonic = true;
	int maxVoices = 16;
	double adsrEnv[8];
	std::string OSC1wave = "sinewave",
				OSC2wave = "squarewave",
				VCOwave = "sinewave",
				ENVwave = "sinewave";
};

class Voice {
public:
	maxiOsc VCO1, VCO2, LFO1, LFO2;
	double VCO1out, VCO2out, LFO1out, LFO2out, VCFout, ADSRout, pitch;
	int trigger, stamp;
	double velocity;
	maxiFilter VCF;
	maxiEnv envelope;
	SynthSettings *settings;
	bool operator<(Voice &v) {
		return this->stamp < v.stamp;
	}
	void noteOn(double freq, double vel) {
		std::cout << "on" << std::endl << std::flush;
		//envelope.trigger = 1;
		stamp = std::time(0);
		trigger = 1;
		velocity = vel;
		LFO1.phaseReset(0);
		pitch = freq;
	}
	void noteOff(int notes) {
		std::cout << "off" << std::endl << std::flush;
		if (notes == 0) {
			trigger = 0;
		} else if (settings->polyphonic) {
			trigger = 0;
		}

		envelope.attackphase = 0;
		envelope.decayphase = 0;
		envelope.sustainphase = 0;
		envelope.releasephase = 1;
	}
	double play() {
		//ADSRout=ADSR.line(8, adsrEnv);//our ADSR env has 8 value/time pairs.

		/*LFO1out=LFO1.sinebuf(0.2); //this lfo is a sinewave at 0.2 hz
        
        VCO1out=VCO1.pulse(pitch,0.6); //here's VCO1. it's a pulse wave at 55 hz, with a pulse width of 0.6
        VCO2out=VCO2.pulse((pitch)+LFO1out,0.2); //here's VCO2. it's a pulse wave at 110hz with LFO modulation on the frequency, and width of 0.2

        VCFout=VCF.lores((VCO1out+VCO2out)*0.5, 250+((pitch+LFO1out)*1000), 10);//now we stick the VCO's into the VCF, using the ADSR as the filter cutoff
        return VCFout * ADSRout / 6;*/
        if (pitch == 0) return 0;

        LFO1out = LFO1.sinebuf(0.8);
        VCO1out = VCO1.play(settings->OSC1wave, pitch) * settings->VCO1volume * 0.5;
        VCO2out = VCO2.play(settings->OSC2wave, pitch) * settings->VCO2volume * 0.5;
        VCFout = VCF.lores(VCO1out + VCO2out, 250+((8 +LFO1out)*1000), 10);
        return envelope.adsr(VCFout * velocity,
        					settings->attack,
        					settings->decay,
        					settings->sustain,
        					settings->release,
        					1,
        					trigger);
	}
};