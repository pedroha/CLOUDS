#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	
	parser.setup("../../../CloudsData/fcpxml/");
    parser.parseLinks("../../../CloudsData/links/clouds_link_db.xml");
	parser.setCombinedVideoDirectory("/Users/focus/Desktop/RGBD_Compiled");
	
	storyEngine.setup();
	storyEngine.network = &parser;
	storyEngine.maxTimesOnTopic = 4;
	storyEngine.combinedClipsOnly = true;
	
	player.setup(storyEngine);
	
	float randomClip = ofRandom(parser.getAllClips().size() );
	storyEngine.seedWithClip( parser.getRandomClip(true) );
	
}

//--------------------------------------------------------------
void testApp::update(){
	player.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	player.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}