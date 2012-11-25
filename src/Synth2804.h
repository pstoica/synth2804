#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "RtMidi.h"
#include "Voice.h"
#include <queue>

class Synth2804 : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void audioOut(float * input, int bufferSize, int nChannels);
		void midiReceived(double deltatime, std::vector<unsigned char> *message, void *userData);

		void exit(); 
void guiEvent(ofxUIEventArgs &e);

		ofSoundStream soundStream;
		ofxUICanvas *gui;

		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;

		vector <float> lAudio;
		vector <float> rAudio;

		//------------------- maximilian

		/*Synth2804() : adsrEnv{1,5,0.125,100,0.125,200,0,1000}, notes(0), maxVoices(16), polyphonic(true), mix(0), voice() { };*/
		SynthSettings settings;
		int notes;
		double mix;
		vector<Voice> voices;
		
		maxiOsc test1;
};