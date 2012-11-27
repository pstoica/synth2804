#include "Synth2804.h"

//--------------------------------------------------------------
void Synth2804::setup(){
	ofEnableSmoothing();
	ofSetVerticalSync(true);
	ofBackground(209, 36, 43);
	ofSetLogLevel(OF_LOG_VERBOSE);

	int bufferSize		= 512;
	sampleRate 			= 44100;
	volume				= 1.0f;

	vector<std::string> waveforms = {
		"sinewave",
		"saw",
		"triangle",
		"square",
		"pulse",
		"noise"
	};

	vector<std::string> filters = {
		"lopass",
		"hipass",
		"none"
	};

	//soundStream.listDevices();

	//if you want to set the device id to be different than the default
	//soundStream.setDeviceID(1); 	//note some devices are input only and some are output only

	soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);

	Voice::settings = &settings;
	for (int i = 0; i < settings.maxVoices; i++) {
		Voice v;
		voices.push_back(v);
	}

	gui = unique_ptr<ofxUIScrollableCanvas>(new ofxUIScrollableCanvas(0,0,800,1000));		//ofxUICanvas(float x, float y, float width, float height)
	gui->setScrollAreaToScreen();
	gui->setScrollableDirections(false, true);

	gui->addWidgetDown(new ofxUILabel("Synth2804", OFX_UI_FONT_LARGE));
	gui->addWidgetDown(new ofxUIRotarySlider(50,0.0,1.0,&volume,"VOLUME"));
	gui->addWidgetRight(new ofxUIToggle("POLYPHONIC", &settings.polyphonic, 50, 50));
	gui->addWidgetDown(new ofxUISpacer(0, 10));
	gui->addWidgetDown(new ofxUILabel("ENVELOPE", OFX_UI_FONT_LARGE));
	gui->addWidgetDown(new ofxUIRotarySlider(50,0.0000000001,0.001,&settings.attack,"ATTACK"));
	gui->addWidgetRight(new ofxUIRotarySlider(50,0.9999,1.0,&settings.decay,"DECAY"));
	gui->addWidgetRight(new ofxUIRotarySlider(50,0.0,1.0,&settings.sustain,"SUSTAIN"));
	gui->addWidgetRight(new ofxUIRotarySlider(50,0.999,1.0,&settings.release,"RELEASE"));
	gui->addWidgetDown(new ofxUILabel("OSC1", OFX_UI_FONT_LARGE));
	gui->addWidgetDown(new ofxUIDropDownList("OSC1 WAVE", waveforms, 200));
	gui->addWidgetDown(new ofxUISlider("OSC1 DETUNE",-1200,1200,&settings.OSC1detune,200,20));
	gui->addWidgetEastOf(new ofxUIRotarySlider(50,0.0,1.0,&settings.OSC1volume,"OSC1 VOL"), "OSC1 WAVE");
	gui->addWidgetEastOf(new ofxUIRotarySlider(50,0.0,1.0,&settings.OSC1pw,"OSC1 PW"), "OSC1 VOL");
	gui->addWidgetDown(new ofxUISpacer(0, 10));
	gui->addWidgetDown(new ofxUILabel("OSC2", OFX_UI_FONT_LARGE));
	gui->addWidgetDown(new ofxUIDropDownList("OSC2 WAVE", waveforms, 200));
	gui->addWidgetDown(new ofxUISlider("OSC2 DETUNE",-1200,1200,&settings.OSC2detune,200,20));
	gui->addWidgetEastOf(new ofxUIRotarySlider(50,0.0,1.0,&settings.OSC2volume,"OSC2 VOL"), "OSC2 WAVE");
	gui->addWidgetEastOf(new ofxUIRotarySlider(50,0.0,1.0,&settings.OSC2pw,"OSC2 PW"), "OSC2 VOL");
	gui->addWidgetDown(new ofxUISpacer(0, 10));
	gui->addWidgetDown(new ofxUILabel("VCF", OFX_UI_FONT_LARGE));
	gui->addWidgetDown(new ofxUIRotarySlider(50,0.0,1.0,&settings.cutoff,"CUTOFF"));
	gui->addWidgetEastOf(new ofxUISpacer(35, 0, "SPACER"), "VCF");
	gui->addWidgetEastOf(new ofxUIRadio("TYPE", filters, OFX_UI_ORIENTATION_VERTICAL, 20, 20), "SPACER");
	gui->addWidgetDown(new ofxUISpacer(0, 10));
	gui->addWidgetDown(new ofxUILabel("OSC2 -> LFO1", OFX_UI_FONT_LARGE));
	gui->addWidgetDown(new ofxUIDropDownList("LFO1 WAVE", waveforms, 200));
	gui->addWidgetDown(new ofxUISlider("LFO1 FREQ",0.0,2.0,&settings.LFO1freq,200,20));
	gui->addWidgetDown(new ofxUISlider("LFO1 DEPTH",0.0,1.0,&settings.LFO1amp,200,20));
	gui->addWidgetEastOf(new ofxUIDropDownList("LFO2 WAVE", waveforms, 200), "LFO1 WAVE");
	gui->addWidgetNorthOf(new ofxUILabel("VCF -> LFO2", OFX_UI_FONT_LARGE), "LFO2 WAVE");
	gui->addWidgetSouthOf(new ofxUISlider("LFO2 FREQ",0.0,2.0,&settings.LFO2freq,200,20), "LFO2 WAVE");
	gui->addWidgetSouthOf(new ofxUISlider("LFO2 DEPTH",0.0,2.0,&settings.LFO2amp,200,20), "LFO2 FREQ");
	ofAddListener(gui->newGUIEvent, this, &Synth2804::guiEvent);

	ofSetFrameRate(60);
}


//--------------------------------------------------------------
void Synth2804::update(){

}

//--------------------------------------------------------------
void Synth2804::draw(){
}


//--------------------------------------------------------------
void Synth2804::keyPressed  (int key){
	if (key == '0') {
		settings.attack = 1;
		settings.OSC1detune = 1;
		settings.OSC2detune = 1;
	}
}

//--------------------------------------------------------------
void Synth2804::keyReleased  (int key){

}

//--------------------------------------------------------------
void Synth2804::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void Synth2804::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void Synth2804::mousePressed(int x, int y, int button){
}


//--------------------------------------------------------------
void Synth2804::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void Synth2804::windowResized(int w, int h){

}

//--------------------------------------------------------------
void Synth2804::audioOut(float * output, int bufferSize, int nChannels){
	for (int i = 0; i < bufferSize; i++){
		int numVoices = 1;
		mix = 0;
		for (Voice &v : voices) {
			double out = v.play();
			mix += out;
			numVoices += (v.envelope.trigger == 1 || v.envelope.amplitude > 0.00000001);
		}

		output[i*nChannels    ] = mix * volume / settings.maxVoices;
		output[i*nChannels + 1] = mix * volume / settings.maxVoices;
	}
}

//--------------------------------------------------------------
void Synth2804::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void Synth2804::dragEvent(ofDragInfo dragInfo){

}

void Synth2804::midiReceived(double deltatime, std::vector<unsigned char> *message, void *userData) {
	MidiMessage midiData;
	midiData.makeFrom(deltatime, message);
	std::cout << midiData.toString() << std::endl;

	if (midiData.status == MIDI_NOTE_ON) {
		if (settings.polyphonic) {
			auto voice = find_if (voices.begin(), voices.end(), [&midiData](Voice &v) {
				return v.pitch == midiData.pitch;
			});

			if (voice != voices.end()) {
				voice->noteOn(midiData.pitch, midiData.velocity);
			} else {
				min_element(voices.begin(), voices.end())->noteOn(midiData.pitch, midiData.velocity);
			}
		} else {
			voices[0].noteOn(midiData.pitch, midiData.velocity);
		}

		notes++;
	} else if (midiData.status == MIDI_NOTE_OFF) {
		notes = max(0, notes - 1);

		auto voice = find_if (voices.begin(), voices.end(), [&midiData](Voice &v) {
			return v.pitch == midiData.pitch;
		});
		voice->noteOff(notes);
	} else if (midiData.status == MIDI_CONTROL_CHANGE) {
		switch (midiData.control) {
		case 0x1:
			settings.attack = convertMidiValue(midiData.value, 0.0000000001, 0.001);
			break;
		case 0x2:
			settings.decay = convertMidiValue(midiData.value, 0.9999, 1.0);
			break;
		case 0x3:
			settings.sustain = convertMidiValue(midiData.value, 0, 1);
			break;
		case 0x4:
			settings.release = convertMidiValue(midiData.value, 0.999,1.0);
			break;
		case 0x5:
			volume = convertMidiValue(midiData.value, 0, 1);
			break;
		case 0x6:
			settings.OSC1volume = convertMidiValue(midiData.value, 0, 1);
			break;
		case 0x7:
			settings.OSC2volume = convertMidiValue(midiData.value, 0, 1);
			break;
		case 0x8:
			settings.cutoff = convertMidiValue(midiData.value, 0, 1);
			break;
		}
	}
}

void Synth2804::exit() {
	ofSoundStreamClose();
}

void Synth2804::guiEvent(ofxUIEventArgs &e) {
	if (e.widget->getName() == "OSC1 WAVE") {
		ofxUIDropDownList *list = (ofxUIDropDownList *) e.widget;
		vector<ofxUIWidget *> &selected = list->getSelected();
		for(int i = 0; i < selected.size(); ++i) {
			settings.OSC1wave = selected[i]->getName();
		}
	} else if (e.widget->getName() == "OSC2 WAVE") {
		ofxUIDropDownList *list = (ofxUIDropDownList *) e.widget;
		vector<ofxUIWidget *> &selected = list->getSelected();
		for(int i = 0; i < selected.size(); ++i) {
			settings.OSC2wave = selected[i]->getName();
		}
	} else if (e.widget->getName() == "LFO1 WAVE") {
		ofxUIDropDownList *list = (ofxUIDropDownList *) e.widget;
		vector<ofxUIWidget *> &selected = list->getSelected();
		for(int i = 0; i < selected.size(); ++i) {
			settings.LFO1wave = selected[i]->getName();
		}
	} else if (e.widget->getName() == "LFO2 WAVE") {
		ofxUIDropDownList *list = (ofxUIDropDownList *) e.widget;
		vector<ofxUIWidget *> &selected = list->getSelected();
		for(int i = 0; i < selected.size(); ++i) {
			settings.LFO2wave = selected[i]->getName();
		}
	} else if (e.widget->getName() == "lopass" || e.widget->getName() == "hipass" || e.widget->getName() == "none") {
		settings.filter = e.widget->getName();
	} else if (e.widget->getName() == "POLYPHONIC") {
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
		settings.maxVoices = toggle->getValue() ? 16 : 1;
		for (Voice &v : voices) v.pitch = 0;
	}
}

float Synth2804::convertMidiValue(float num, float newMin, float newMax) {
	return (((num) * (newMax - newMin)) / (127)) + newMin;
}
