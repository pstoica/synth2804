#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "RtMidi.h"
#include "Voice.h"
#include "MidiMessage.h"
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

        float convertMidiValue(float num, float newMin, float newMax);

		ofSoundStream soundStream;
		std::unique_ptr<ofxUIScrollableCanvas> gui;

		int		sampleRate;
		float 	volume;

		//------------------- maximilian

		SynthSettings settings;
		vector<Voice> voices;
		int notes;
		double mix;
};
