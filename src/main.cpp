#include "Synth2804.h"
#include "ofAppGlutWindow.h"

//========================================================================

Synth2804 *synth;
RtMidiIn midiIn;

void midiReceived(double deltatime, std::vector<unsigned char> *message, void *userData) {
	synth->midiReceived(deltatime, message, userData);
}

int main( ){
	synth = new Synth2804;
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);			// <-------- setup the GL context
	midiIn.openVirtualPort("ofxMidiIn Input");
	midiIn.setCallback(&midiReceived);
	midiIn.ignoreTypes(false, false, false);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(synth);

	delete synth;

}
