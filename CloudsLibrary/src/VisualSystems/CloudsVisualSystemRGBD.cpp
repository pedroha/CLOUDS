
#include "CloudsVisualSystemRGBD.h"

//--------------------------------------------------------------
string CloudsVisualSystemRGBD::getSystemName(){
	return "RGBD";
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetup(){
	currentFlowPosition = 0;
	generatePointGrid();
	generateScanlines();
	connectionGenerator.numParticles = 400;
	connectionGenerator.setMinDistance(20);
	connectionGenerator.setup();
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupGuis(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfUpdate(){
	if(refreshScanlineMesh){
		generateScanlines();
	}
	if(refreshPointcloud){
		generatePointGrid();
	}
	if(numRandomPoints != randomPoints.getNumVertices()){
		generateRandomPoints();
	}
	currentFlowPosition += cloudFlow;
	sharedRenderer->flowPosition = currentFlowPosition;
	
	connectionGenerator.update();
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generatePointGrid(){
	
	pointGrid.clear();
	
	pointVerticalSpace = MAX(pointVerticalSpace, 1);
	pointHorizontalSpace = MAX(pointHorizontalSpace, 1);
	for(float y = 0; y <= 480; y+= pointVerticalSpace){
		for(float x = 0; x <= 640; x+= pointHorizontalSpace){
			pointGrid.addVertex(ofVec3f(x,y,0));
		}
	}
	pointGrid.setMode(OF_PRIMITIVE_POINTS);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generateRandomPoints(){
	if(numRandomPoints == 0){
		randomPoints.clear();
	}
	else if(numRandomPoints < randomPoints.getNumVertices() ){
		randomPoints.getVertices().erase(randomPoints.getVertices().begin(), randomPoints.getVertices().begin() + (randomPoints.getNumVertices() - numRandomPoints) );
	}
	while(numRandomPoints > randomPoints.getNumVertices()){
		randomPoints.addVertex( ofVec3f(ofRandom(640),ofRandom(480),0) );
	}
	
	randomPoints.setMode(OF_PRIMITIVE_POINTS);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generateScanlines(){

	verticalScanLines.clear();
	horizontalScanLines.clear();
	
	if(scanlineSimplify.x <= 0) scanlineSimplify.x = 1;
	if(scanlineSimplify.y <= 0) scanlineSimplify.y = 1;
	
	int height = 480;
	int width = 640;
	
	//VERTICAL
	for (float xstep = 0; xstep <= width-scanlineSimplify.x; xstep += scanlineSimplify.x){
		for (float ystep = 0; ystep <= height-scanlineSimplify.y; ystep += scanlineSimplify.y){
			verticalScanLines.addVertex( ofVec3f(xstep, ystep, 0) );
			verticalScanLines.addVertex( ofVec3f(xstep, ystep+scanlineSimplify.y, 0) );
		}
	}
	
	//HORIZONTAL
	for (float ystep = 0; ystep <= height-scanlineSimplify.y; ystep += scanlineSimplify.y){
		for (float xstep = 0; xstep <= width-scanlineSimplify.x; xstep += scanlineSimplify.x){
			
			float ystepOffset = ofRandom(-scanlineSimplify.y/2,scanlineSimplify.y/2);
			
			horizontalScanLines.addColor(ofFloatColor(ofRandom(1.)));
			horizontalScanLines.addVertex( ofVec3f(xstep, ystep+ystepOffset, 0) );
			
			horizontalScanLines.addColor(ofFloatColor(ofRandom(1.)));
			horizontalScanLines.addVertex( ofVec3f(xstep+scanlineSimplify.x, ystep+ystepOffset, 0) );
		}
	}

	verticalScanLines.setMode( OF_PRIMITIVE_LINES );
	horizontalScanLines.setMode( OF_PRIMITIVE_LINES );
	
	if(sharedRenderer != NULL){
		sharedRenderer->setSimplification(scanlineSimplify);
	}
	else{
		ofLogError() << "CloudsVisualSystemRGBD::generateScanlines -- renderer not set up!";
	}
	
	refreshScanlineMesh = false;
}

void CloudsVisualSystemRGBD::selfDrawBackground(){
	
}

void CloudsVisualSystemRGBD::selfDrawDebug(){
	
}

void CloudsVisualSystemRGBD::selfSceneTransformation(){

}

void CloudsVisualSystemRGBD::selfDraw(){
	if(sharedRenderer != NULL && hasSpeaker){
		
		ofPushStyle();
		ofPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		
		//Enable smooth lines and screen blending
		glDisable(GL_DEPTH_TEST);
		
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		
		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		
		//move the pointcloud
		ofTranslate(0,0,pointcloudOffsetZ);
		ofScale(pointcloudScale,pointcloudScale,pointcloudScale);
		
		//set up the renderer so that any geometry within 640x480 space
		//can be prjected onto the pointcloud
		sharedRenderer->bindRenderer();
		
		sharedRenderer->getShader().setUniform1f("flowPosition", currentFlowPosition);

		if(drawPoints){
			//draw the points
			glPointSize(pointSizeMin);
			ofSetColor(255*randomPointAlpha);
			randomPoints.draw();
			ofSetColor(255*pointGridAlpha);
			pointGrid.draw();
		}
		
		
		if(drawScanlines){
			
			//draw the lines
			ofSetLineWidth(verticalScanlineThickness);
			ofSetColor(255*verticalScanlineAlpha);
			verticalScanLines.draw();
			
			sharedRenderer->getShader().setUniform1f("flowPosition", 0);
			
			ofSetColor(255*horizontalScanlineAlpha);
			ofSetLineWidth(horizontalScanlineThickness);
			horizontalScanLines.draw();
			
		
		}
		
		sharedRenderer->getShader().setUniform1f("flowPosition", 0);
//		connectionGenerator.draw();

		sharedRenderer->unbindRenderer();
		
		glPopAttrib();
		
		ofPopMatrix();
		ofPopStyle();
	}
}

void CloudsVisualSystemRGBD::selfExit(){
	
}

void CloudsVisualSystemRGBD::selfBegin(){

}

void CloudsVisualSystemRGBD::selfEnd(){
	
}

void CloudsVisualSystemRGBD::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){

	}
}

void CloudsVisualSystemRGBD::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemRGBD::selfMouseDragged(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMouseReleased(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfGuiEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupSystemGui(){
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::guiSystemEvent(ofxUIEventArgs &e){
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupRenderGui(){
	
	rdrGui->addSlider("CLOUD SCALE", .001,  0.5, &pointcloudScale);
	rdrGui->addSlider("CLOUD OFFSET",   0, -800, &pointcloudOffsetZ);

	rdrGui->addLabel("POINTS");
	rdrGui->addToggle("DRAW POINTS", &drawPoints);
	rdrGui->addSlider("POINTSIZE MAX", 0, 3, &pointSizeMax);
	rdrGui->addSlider("POINTSIZE MIN", 0, 3, &pointSizeMin);
	rdrGui->addSpacer();
	rdrGui->addSlider("POINT GRID ALPHA", 0, 1.0, &pointGridAlpha);
	rdrGui->addSlider("POINTGRID VERTICAL SPACE", 1, 10, &pointVerticalSpace);
	rdrGui->addSlider("POINTGRID HORIZONTAL SPACE", 1, 10, &pointHorizontalSpace);
	rdrGui->addSpacer();
	rdrGui->addSlider("NUM RANDOM POINTS", 0, 500000.0f, &numRandomPoints);
	rdrGui->addSlider("RANDOM POINT ALPHA", 0, 1.0f, &randomPointAlpha);
	
	rdrGui->addLabel("LINES");
	rdrGui->addToggle("DRAW LINES", &drawScanlines);
	rdrGui->addSlider("VERTICAL LINE SPACE", .5, 12, &scanlineSimplify.x);
	rdrGui->addSlider("VERTICAL LINE ALPHA", 0, 1.0, &verticalScanlineAlpha);
	rdrGui->addSlider("VERTICAL LINE THICKNESS", 0, 2.0, &verticalScanlineThickness);
	rdrGui->addSpacer();
	rdrGui->addSlider("HORIZONTAL LINE SPACE", .5, 12, &scanlineSimplify.y);
	rdrGui->addSlider("HORIZ. LINE ALPHA", 0, 1.0, &horizontalScanlineAlpha);
	rdrGui->addSlider("HORIZ. LINE THICKNESS", 0, 2.0, &horizontalScanlineThickness);
	
	rdrGui->addLabel("FLOW");
	rdrGui->addSlider("CLOUD FLOW", -5, 5, &cloudFlow);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::guiRenderEvent(ofxUIEventArgs &e){
	
	cout << "GUI EVENT WITH WIDGET " << e.widget->getName();
	
	if(e.widget->getName() == "VERTICAL LINE SPACE"){
		refreshScanlineMesh = true;
	}
	else if(e.widget->getName() == "HORIZONTAL LINE SPACE"){
		refreshScanlineMesh = true;
	}
	else if(e.widget->getName() == "POINTGRID VERTICAL SPACE"){
		refreshPointcloud = true;
	}
	else if(e.widget->getName() == "POINTGRID HORIZONTAL SPACE"){
		refreshPointcloud = true;
	}
}

