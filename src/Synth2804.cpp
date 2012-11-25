#include "Synth2804.h"

//--------------------------------------------------------------
void Synth2804::setup(){
	ofEnableSmoothing(); 
	ofSetVerticalSync(true);
	ofBackground(34, 34, 34);
	ofSetLogLevel(OF_LOG_VERBOSE);

	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 320-xInit; 

	// 2 output channels,
	// 0 input channels
	// 22050 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)

	int bufferSize		= 512;
	sampleRate 			= 44100;
	volume				= 0.1f;
	bNoise 				= false;

	vector<std::string> waveforms = {
		"sinewave",
		"coswave",
		"saw",
		"triangle",
		"square",
		"pulse",
		"noise"
	};

	settings.attack = 1;


	lAudio.assign(bufferSize, 0.0);
	rAudio.assign(bufferSize, 0.0);

	//soundStream.listDevices();

	//if you want to set the device id to be different than the default
	//soundStream.setDeviceID(1); 	//note some devices are input only and some are output only

	soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);

	for (int i = 0; i < settings.maxVoices; i++) {
		Voice v;
		v.settings = &settings;
		voices.push_back(v);
	}

	gui = new ofxUICanvas(0,0,640,640);		//ofxUICanvas(float x, float y, float width, float height)

	gui->addWidgetDown(new ofxUILabel("Synth2804", OFX_UI_FONT_LARGE));
	gui->addWidgetDown(new ofxUIRotarySlider(50,0.0,1.0,volume,"VOLUME"));
	gui->addWidgetRight(new ofxUIRotarySlider(50,0.00000001,0.001,settings.attack,"ATTACK"));
	gui->addWidgetRight(new ofxUIRotarySlider(50,0.0,1.0,settings.decay,"DECAY"));
	gui->addWidgetRight(new ofxUIRotarySlider(50,0.0,1.0,settings.sustain,"SUSTAIN"));
	gui->addWidgetRight(new ofxUIRotarySlider(50,0.999,1.0,settings.release,"RELEASE"));
	gui->addWidgetDown(new ofxUIDropDownList("OSC1 WAVE", waveforms, 100, 304, 40));
	ofAddListener(gui->newGUIEvent, this, &Synth2804::guiEvent);
	gui->loadSettings("GUI/guiSettings.xml");

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

}

//--------------------------------------------------------------
void Synth2804::keyReleased  (int key){

}

//--------------------------------------------------------------
void Synth2804::mouseMoved(int x, int y ){
	int width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height-y) / height);
}

//--------------------------------------------------------------
void Synth2804::mouseDragged(int x, int y, int button){
	int width = ofGetWidth();
	pan = (float)x / (float)width;
}

//--------------------------------------------------------------
void Synth2804::mousePressed(int x, int y, int button){
	bNoise = true;
}


//--------------------------------------------------------------
void Synth2804::mouseReleased(int x, int y, int button){
	bNoise = false;
}

//--------------------------------------------------------------
void Synth2804::windowResized(int w, int h){

}

//--------------------------------------------------------------
void Synth2804::audioOut(float * output, int bufferSize, int nChannels){	
	for (int i = 0; i < bufferSize; i++){
		mix = 0;
		for (Voice &v : voices) {
			mix += v.play();
		}

		lAudio[i] = output[i*nChannels    ] = mix * volume;
		rAudio[i] = output[i*nChannels + 1] = mix * volume;
	}
}

//--------------------------------------------------------------
void Synth2804::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void Synth2804::dragEvent(ofDragInfo dragInfo){

}

void Synth2804::midiReceived(double deltatime, std::vector<unsigned char> *message, void *userData) {
  unsigned char name, channel, note;
  double velocity;
  std::string nameString;
  name = (message->at(0)) >> 4;
  channel = (message->at(0)) & 0xF;
  note = message->at(1);
  velocity = message->at(2) / 127.;
  double hertz;
  convert midiConverter;

  hertz = midiConverter.mtof(note);

  std::cout << "Name: " << name << " Channel: " << (int) channel << " Note: " << hertz << " Velocity: " << velocity << std::endl;
  if (name == 0x9) { // noteOn
  	if (settings.polyphonic) {
  		auto voice = find_if (voices.begin(), voices.end(), [hertz](Voice &v) {
  			return (int) v.pitch == (int) hertz;
	  	});

	  	if (voice != voices.end()) {
	  		voice->noteOn(hertz, velocity);
	  	} else {
	  		min_element(voices.begin(), voices.end())->noteOn(hertz, velocity);
	  	}
  	} else {
  		max_element(voices.begin(), voices.end())->noteOn(hertz, velocity);
  	}

  	notes++;
  } else if (name == 0x8) { // noteOff
  	auto voice = find_if (voices.begin(), voices.end(), [hertz](Voice &v) {
  		return (int) v.pitch == (int) hertz;
  	});
  	voice->noteOff(notes);
  } else if (name == 0xB) {
  	std::cout << "control change" << std::endl;
  }
}

void Synth2804::exit() {
	gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
}

void Synth2804::guiEvent(ofxUIEventArgs &e) {
    if (e.widget->getName() == "VOLUME") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        volume = slider->getScaledValue();
    } else if (e.widget->getName() == "OSC1 WAVE") {
    	ofxUIDropDownList *list = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = list->getSelected(); 
        for(int i = 0; i < selected.size(); ++i) {
            settings.OSC1wave = selected[i]->getName();
        }
    } else if (e.widget->getName() == "ATTACK") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        settings.attack = slider->getScaledValue();
        std::cout << slider->getScaledValue() << std::endl;
    } else if (e.widget->getName() == "DECAY") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        settings.decay = slider->getScaledValue();
        std::cout << slider->getScaledValue() << std::endl;
    } else if (e.widget->getName() == "SUSTAIN") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        settings.sustain = slider->getScaledValue();
        std::cout << slider->getScaledValue() << std::endl;
    } else if (e.widget->getName() == "RELEASE") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        settings.release = slider->getScaledValue();
        std::cout << slider->getScaledValue() << std::endl;
    }
}
