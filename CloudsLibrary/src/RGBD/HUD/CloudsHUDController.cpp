//
//  CloudsHUDController.cpp
//  CloudsPointcloudViewer
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDController.h"
#include "CloudsGlobal.h"
#include "CloudsInput.h"
#include "CloudsClip.h"
#include "CloudsSpeaker.h"
#include "CloudsVisualSystem.h"
#include "CloudsLocalization.h"


CloudsHUDController::CloudsHUDController(){
	hudGui = NULL;

    bSkipAVideoFrame = false;
    bDrawHud = true;
    bProjectExampleDisplayed = false;
    
    bQuestionDisplayed = false;
    bJustPaused = false;
    bJustUnpaused = false;
    bActJustStarted = false;
    bVisualSystemDisplayed = false;
    bClipIsPlaying = false;
    
    currentTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
    bIsScrollUpHover = false;
    bIsScrollDownHover = false;
    bIsScrollUpPressed = false;
    bIsScrollDownPressed = false;
    bIsHoldScrolling = false;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS].scrollPosition = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS].totalScrollHeight = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE].scrollPosition = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE].totalScrollHeight = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS].scrollPosition = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS].totalScrollHeight = 0;
    
    currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
 
    scrollPressedTime = 0;
    
    fakeConfirmHovered = false;
    fakeConfirmPressed = false;
    fakeConfirmClicked = false;

	isPlaying = false;
    
    scaleAmt = 1.0;
    margin = 40;
    
#ifdef OCULUS_RIFT
    // set defaults
    // there might be a better way of doing this...
    layerDistance[CLOUDS_HUD_QUESTION]         = 300;
    layerDistance[CLOUDS_HUD_LOWER_THIRD]      = 300;
    layerDistance[CLOUDS_HUD_PROJECT_EXAMPLE]  = 300;
    layerDistance[CLOUDS_HUD_MAP]              = 300;
    
    layerRotationH[CLOUDS_HUD_QUESTION]        = 0;
    layerRotationH[CLOUDS_HUD_LOWER_THIRD]     = 0;
    layerRotationH[CLOUDS_HUD_PROJECT_EXAMPLE] = 0;
    layerRotationH[CLOUDS_HUD_MAP]             = 0;
    
    layerRotationV[CLOUDS_HUD_QUESTION]        = 0;
    layerRotationV[CLOUDS_HUD_LOWER_THIRD]     = 0;
    layerRotationV[CLOUDS_HUD_PROJECT_EXAMPLE] = 0;
    layerRotationV[CLOUDS_HUD_MAP]             = 0;
    
    layerBillboard[CLOUDS_HUD_QUESTION]        = CLOUDS_HUD_BILLBOARD_CAMERA;
    layerBillboard[CLOUDS_HUD_LOWER_THIRD]     = CLOUDS_HUD_BILLBOARD_CAMERA;
    layerBillboard[CLOUDS_HUD_PROJECT_EXAMPLE] = CLOUDS_HUD_BILLBOARD_CAMERA;
    layerBillboard[CLOUDS_HUD_MAP]             = CLOUDS_HUD_BILLBOARD_CAMERA;
#endif
}

void CloudsHUDController::setup(){
	
	buildLayerSets();
    calculateFontSizes();

#ifdef MOUSE_INPUT
	ofAddListener(ofEvents().mouseMoved,this, &CloudsHUDController::mouseMoved);
	ofAddListener(ofEvents().mousePressed,this, &CloudsHUDController::mousePressed);
	ofAddListener(ofEvents().mouseReleased,this, &CloudsHUDController::mouseReleased);
#endif

	home.setup();
    
	//manually load reset triangle
	resetTriangle.addVertex(ofVec3f(1366.857,839.217));
	resetTriangle.addVertex(ofVec3f(1366.857,851.783));
	resetTriangle.addVertex(ofVec3f(1373.139,845.5));

	resetTriangle.setMode(OF_PRIMITIVE_TRIANGLES);
	 
}

void CloudsHUDController::populateProjectExample(const string& videoPath, const string& textLeft, const string& textRight, const string& textTop, bool forceOn) {
	if( isPlaying){
        videoPlayer.stop();
    }
    
    if( ofFile(videoPath).exists() ){
        isPlaying =  videoPlayer.loadMovie(videoPath);
        videoPlayer.play();
        
        bSkipAVideoFrame = true;
        
        hudLabelMap["ProjectExampleTextboxLeft"]->setText( textLeft, forceOn );
        hudLabelMap["ProjectExampleTextboxRight"]->setText( textRight, forceOn );
        hudLabelMap["ProjectExampleTextBoxTop"]->setText( textTop, forceOn );
        
        if( forceOn ){
            animateOn( CLOUDS_HUD_PROJECT_EXAMPLE );
        }
    }else{
        cout << "CloudsHUDController :: Project example video does not exist: " << videoPath << endl;
    }
}

void CloudsHUDController::buildLayerSets(){
	
	//configure layers
    for(int i = 0; i < CLOUDS_HUD_ALL; i++){
        CloudsHUDLayerSet layerSet = CloudsHUDLayerSet(i);
        CloudsHUDLayer* layer = new CloudsHUDLayer();
        layer->load(GetCloudsDataPath() + "HUD/SVG/" + filenameForLayer(layerSet));
        layers[layerSet] = layer;
        allLayers.push_back( layer );
    }
                    
    /*
    CloudsHUDLayer* homeLayer = new CloudsHUDLayer();
    homeLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_HOME");
    layers[CLOUDS_HUD_HOME] = homeLayer;
    allLayers.push_back( homeLayer );
    
    CloudsHUDLayer* lowerThirdLayer = new CloudsHUDLayer();
    lowerThirdLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_LOWER_THIRD");
    layers[CLOUDS_HUD_LOWER_THIRD] = lowerThirdLayer ;
    allLayers.push_back( lowerThirdLayer );
    
    CloudsHUDLayer* questionLayer = new CloudsHUDLayer();
    questionLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_QUESTION");
    layers[CLOUDS_HUD_QUESTION] = questionLayer;
    allLayers.push_back( questionLayer );
    
    CloudsHUDLayer* pauseLayer = new CloudsHUDLayer();
    pauseLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_PAUSE");
    layers[CLOUDS_HUD_PAUSE] = pauseLayer;
    allLayers.push_back( pauseLayer );
    
    CloudsHUDLayer* projectExampleLayer = new CloudsHUDLayer();
    projectExampleLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_PROJECT_EXAMPLE");
    layers[CLOUDS_HUD_PROJECT_EXAMPLE] = projectExampleLayer;
    allLayers.push_back( projectExampleLayer );
    
    CloudsHUDLayer* nextLayer = new CloudsHUDLayer();
    nextLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_NEXT");
    layers[CLOUDS_HUD_NEXT] = nextLayer;
    allLayers.push_back( nextLayer );
    
    CloudsHUDLayer* researchListLayer = new CloudsHUDLayer();
    researchListLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_RESEARCH_LIST");
    layers[CLOUDS_HUD_RESEARCH_LIST] = researchListLayer;
    allLayers.push_back( researchListLayer );
    
    CloudsHUDLayer* researchNavLayer = new CloudsHUDLayer();
    researchNavLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_RESEARCH_NAV");
    layers[CLOUDS_HUD_RESEARCH_NAV] = researchNavLayer;
    allLayers.push_back( researchNavLayer );
    
    CloudsHUDLayer* researchShuffleLayer = new CloudsHUDLayer();
    researchShuffleLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_RESEARCH_SHUFFLE");
    layers[CLOUDS_HUD_RESEARCH_SHUFFLE] = researchShuffleLayer;
    allLayers.push_back( researchShuffleLayer );
    
    CloudsHUDLayer* aboutLayer = new CloudsHUDLayer();
    aboutLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_ABOUT");
    layers[CLOUDS_HUD_ABOUT] = aboutLayer;
    allLayers.push_back( aboutLayer );
    */
    
    for( int i = 0; i < allLayers.size(); i++ ){
        
        //TODO: filled meshes shouldn't be done this way
		for( int s = 0; s < allLayers[i]->svg.getMeshes().size(); s++){
			ofVboMesh& m = allLayers[i]->svg.getMeshes()[s].mesh;
			for(int v = 0; v < m.getNumVertices(); v++){
				m.addNormal(ofVec3f(ofRandomuf(),0,0));
			}
		}
        
        allLayers[i]->duration = 1.5;
//        allLayers[i]->delayTime = 0;
//        allLayers[i]->startPoint = ofVec2f(allLayers[i]->svg.getWidth(),0);
//        allLayers[i]->endPoint   = ofVec2f(0,allLayers[i]->svg.getHeight());
    }
    
    home.bounds = layers[CLOUDS_HUD_HOME]->svg.getMeshByID("HomeButtonFrame")->bounds;
    home.bounds.scaleFromCenter(1.5);
    
    bioBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("BioFrame")->bounds;
    
    svgVideoBounds = layers[CLOUDS_HUD_PROJECT_EXAMPLE]->svg.getMeshByID("ProjectExampleFrame")->bounds;
	videoBounds = svgVideoBounds;
    
    researchScrollBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("ListBacking")->bounds;
    researchScrollUpBounds   = layers[CLOUDS_RESEARCH]->svg.getMeshByID("ListScrollUpBacking")->bounds;
    researchScrollDownBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("ListScrollDownBacking")->bounds;
    
    hudBounds.set( 0, 0, allLayers[0]->svg.getWidth(), allLayers[0]->svg.getHeight() );
    
    //	cout << "HUD BOUNDS " << hudBounds.width << " / " << hudBounds.height << endl;
    //  cout << "SCREEN " << ofGetScreenWidth() << " / " << ofGetScreenHeight() << endl;
}


string CloudsHUDController::filenameForLayer(CloudsHUDLayerSet layer){
    switch (layer) {
        case CLOUDS_HUD_HOME:
            return "CLOUDS_HUD_HOME.svg";
        case CLOUDS_HUD_QUESTION:
            return "CLOUDS_HUD_QUESTION.svg";
        case CLOUDS_HUD_LOWER_THIRD:
            return "CLOUDS_HUD_LOWER_THIRD.svg";
        case CLOUDS_HUD_PROJECT_EXAMPLE:
            return "CLOUDS_HUD_PROJECT_EXAMPLE.svg";
        case CLOUDS_HUD_PAUSE:
            return "CLOUDS_HUD_PAUSE.svg";
        case CLOUDS_HUD_NEXT:
            return "CLOUDS_HUD_NEXT.svg";
        case CLOUDS_RESEARCH:
            return "CLOUDS_RESEARCH.svg";
        case CLOUDS_RESEARCH_SHUFFLE:
            return "CLOUDS_RESEARCH_SHUFFLE.svg";
        case CLOUDS_RESEARCH_RESUME:
            return "CLOUDS_RESEARCH_RESUME.svg";
        case CLOUDS_ABOUT_BACKERS:
            return "CLOUDS_ABOUT_BACKERS.svg";
        case CLOUDS_ABOUT_CAST:
            return "CLOUDS_ABOUT_CAST.svg";
        case CLOUDS_ABOUT_CREDITS:
            return "CLOUDS_ABOUT_CREDITS.svg";
        case CLOUDS_ABOUT_INFO:
            return "CLOUDS_ABOUT_INFO.svg";
        case CLOUDS_ABOUT_MAIN:
            return "CLOUDS_ABOUT_MAIN.svg";
        case CLOUDS_ABOUT_SETTINGS:
            return "CLOUDS_ABOUT_SETTINGS.svg";
        default:
            break;
    }
    
    ofLogError("CloudsHUDController::filenameForLayer") << "No filename for layer " << layer;
    return "ERROR!";
}

void CloudsHUDController::calculateFontSizes(){
    // temporary allocate
    int minFontSize = 1;
    int maxFontSize = 70;
#ifdef OCULUS_RIFT
	//string fontPath = GetCloudsDataPath() + "font/Blender-MEDIUM.ttf";
	string fontPath = GetMediumFontPath();
#else
	//string fontPath = GetCloudsDataPath() + "font/Blender-THIN.ttf";
	string fontPath = GetThinFontPath();
#endif
    
    
    for(int i = minFontSize; i < maxFontSize; i++){
        ofxFTGLFont *tmp = new ofxFTGLFont();
        tmp->loadFont(fontPath , i );
        tempFontList.push_back( tmp );
    }
    
    //BIO
    ////first name
    getLabelForLayer("BylineFirstNameTextBox_1_", fontPath, 50, true);
    ////last name
    getLabelForLayer("BylineLastNameTextBox", fontPath, 50, true);
    ////title
    getLabelForLayer("BylineTopicTextBoxBottom", fontPath);
    ////location
    getLabelForLayer("BylineTopicTextBoxTop", fontPath);
    getLabelForLayer("VSCreditsTextBoxTop", fontPath);
    getLabelForLayer("VSCreditsTextBoxBottom", fontPath);
    getLabelForLayer("QuestionTextBox_1_", fontPath);
    
    getLabelForLayer("ProjectExampleTextboxLeft", fontPath);
    getLabelForLayer("ProjectExampleTextboxRight", fontPath);
    getLabelForLayer("ProjectExampleTextBoxTop", fontPath);
    
    //pause
    getLabelForLayer("ExploreTextBox", fontPath);
    getLabelForLayer("SeeMoreTextBox", fontPath);
    getLabelForLayer("NextButtonTextBox", fontPath);
    getLabelForLayer("BioTitleTextBox", fontPath);
    getLabelForLayer("BioTextBox", fontPath,35,false,true); //use layout
    
    
    getLabelForLayer("ResetButtonTextBox", fontPath);
    //research stuff
    ResearchTopicListLabel = getLabelForLayer("ListTextBoxes", fontPath);
    
    //research navigation
    getLabelForLayer("MapTextBox", fontPath);
    getLabelForLayer("PeopleTextBox", fontPath);
    getLabelForLayer("VisualsTextBox", fontPath);
    getLabelForLayer("RSResetButtonTextBox", fontPath);
    getLabelForLayer("ShuffleButtonTextBox", fontPath);

    //about text boxes
    getLabelForLayer("AboutTextBox", fontPath);
    
    getLabelForLayer("CreditsList1TextBox", fontPath, 35,false,true);
    getLabelForLayer("CreditsList2TextBox", fontPath, 35,false,true);
    getLabelForLayer("CreditsList3TextBox", fontPath, 35,false,true);

    getLabelForLayer("CastList1TextBox", fontPath, 35,false,true);
    getLabelForLayer("CastList2TextBox", fontPath, 35,false,true);
    getLabelForLayer("CastList3TextBox", fontPath, 35,false,true);

    getLabelForLayer("BackersList1TextBox", fontPath, 35,false,true);
    getLabelForLayer("BackersList2TextBox", fontPath, 35,false,true);
    getLabelForLayer("BackersList3TextBox", fontPath, 35,false,true);
    
    // cleanup!
    for( int i=0; i<tempFontList.size(); i++ ){
        delete tempFontList[i];
    }
    tempFontList.clear();
    
    
    scrollIncrement = hudLabelMap["ListTextBoxes"]->bounds.height * 1.5;
    
    hudLabelMap["MapTextBox"]->setText(GetTranslationForString("MAP"), false);
    hudLabelMap["PeopleTextBox"]->setText(GetTranslationForString("PEOPLE"), false);
    hudLabelMap["VisualsTextBox"]->setText(GetTranslationForString("VISUALS"), false);
    hudLabelMap["RSResetButtonTextBox"]->setText(GetTranslationForString("EXIT"), false); //this one may change...
    
    hudLabelMap["ResetButtonTextBox"]->setText(GetTranslationForString("EXIT"), false);
    hudLabelMap["NextButtonTextBox"]->setText(GetTranslationForString("NEXT"), false);
    hudLabelMap["ExploreTextBox"]->setText(GetTranslationForString("EXPLORE THE MAP"), false);
    hudLabelMap["SeeMoreTextBox"]->setText(GetTranslationForString("SEE MORE OF THIS PERSON"), false); //todo dynmic name
    
    hudLabelMap["ShuffleButtonTextBox"]->setText(GetTranslationForString("SHUFFLE ALL"), false); //todo dynmic name
    
    hudLabelMap["BioTitleTextBox"]->setText(GetTranslationForString("BIO"), false);
    
    hudLabelMap["BioTextBox"]->layout->setLineLength(hudLabelMap["BioTextBox"]->bounds.width);
    
//    hudLabelMap["CreditsList1TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/credits1.txt").getText(), false);
//    hudLabelMap["CreditsList2TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/credits2.txt").getText(), false);
//    hudLabelMap["CreditsList3TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/credits3.txt").getText(), false);

//    hudLabelMap["CastList1TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/cast1.txt").getText(), false);
//    hudLabelMap["CastList2TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/cast2.txt").getText(), false);
//    hudLabelMap["CastList3TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/cast3.txt").getText(), false);
    
    ofBuffer backers = ofBufferFromFile(GetCloudsDataPath() + "about/backers.txt");
    int i = 0;
    string columns[3];
    while(!backers.isLastLine()){
        columns[i++ % 3 ] += backers.getNextLine() +"\n";
    }
    
//    hudLabelMap["BackersList1TextBox"]->setText(columns[0], false);
//    hudLabelMap["BackersList2TextBox"]->setText(columns[1], false);
//    hudLabelMap["BackersList3TextBox"]->setText(columns[2], false);
    
}

CloudsHUDLabel* CloudsHUDController::getLabelForLayer(const string& layerName,
                                                      const string& fontPath,
                                                      int kerning,
                                                      bool caps,
                                                      bool useLayout)
{
    
    for( int i = 0; i < CLOUDS_HUD_ALL; i++ ){
        
        SVGMesh* textMesh = layers[(CloudsHUDLayerSet)i]->svg.getMeshByID( layerName );
        
        if( textMesh == NULL ){
            continue;
        }
        
        textMesh->visible = false;
        
        float maxHeight = textMesh->bounds.height;
        
        CloudsHUDLabel *newLabel = new CloudsHUDLabel();
        // make a layout
        if(useLayout){
            ofxFTGLSimpleLayout *newLayout = new ofxFTGLSimpleLayout();
            newLayout->loadFont( fontPath, 12 );
            newLayout->setLineLength( 999 );
            newLabel->setup( newLayout, textMesh->bounds );
        }
        else{
            int fontSize = getFontSizeForMesh( textMesh );
            if( layerName == "ListTextBoxes"){
                fontSize -= 2;
            }
            
            ofxFTGLFont *newFont = new ofxFTGLFont();
            newFont->setLetterSpacing(kerning * .08);
            newFont->loadFont( fontPath, fontSize );
            newLabel->setup( newFont, textMesh->bounds );
        }
        // make a label
        newLabel->caps = caps;
        
        hudLabelMap[layerName] = newLabel;
        hudLayerLabels[(CloudsHUDLayerSet)i].push_back(newLabel);
        
        return newLabel;
        
    }
    
    ofLogError("CloudsHUDController::getLayoutForLayer") << "Mesh not found " << layerName;
    
    return NULL;
    
}

int CloudsHUDController::getFontSizeForMesh( SVGMesh* textMesh ){
    if( !textMesh ){
        ofLogError("CloudsHUDController::getFontSizeForMesh") << "Text box not found";
        return 0;
    }
    
    int fontSize = 0;
    float textBoxHeight = textMesh->bounds.height;
    
    for( int k=0; k<tempFontList.size()-1; k++){
        if( tempFontList[k]->getStringBoundingBox("W",0,0).height <= textBoxHeight && tempFontList[k+1]->getStringBoundingBox("W",0,0).height > textBoxHeight ){
            fontSize = 1 + k;
            break;
        }
    }
    
    return fontSize;
}

void CloudsHUDController::actBegan(CloudsActEventArgs& args){
	bDrawHud = true;
	bActJustStarted = true;
    bVisualSystemDisplayed = false;
}

void CloudsHUDController::actEnded(CloudsActEventArgs& args){

    animateOff( CLOUDS_HUD_HOME );
	animateOff( CLOUDS_HUD_LOWER_THIRD );
// 	animateOff( CLOUDS_HUD_QUESTION );
	animateOff( CLOUDS_HUD_PROJECT_EXAMPLE );
	animateOff( CLOUDS_HUD_PAUSE );
    
    bVisualSystemDisplayed = false;
    
}
//////////TODO: these need to animate out
void CloudsHUDController::clearQuestion(){
	hudLabelMap["QuestionTextBox_1_"]->animateOut(true);
    animateOff(CLOUDS_HUD_QUESTION);

    bQuestionDisplayed = false;
}

void CloudsHUDController::clearClip(){
    hudLabelMap["BylineFirstNameTextBox_1_"]->animateOut(true);
    hudLabelMap["BylineLastNameTextBox"]->animateOut(true);
    hudLabelMap["BylineTopicTextBoxBottom"]->animateOut(true);
    hudLabelMap["BylineTopicTextBoxTop"]->animateOut(true);
    hudLabelMap["BioTextBox"]->animateOut(true);

    bClipIsPlaying = false;
    if(!bClipIsPlaying && !bVisualSystemDisplayed){
        animateOff(CLOUDS_HUD_LOWER_THIRD);
    }
}
////////////////////

void CloudsHUDController::clearVisualSystem(){

    hudLabelMap["VSCreditsTextBoxTop"]->animateOut(true);
    hudLabelMap["VSCreditsTextBoxBottom"]->animateOut(true);

    bVisualSystemDisplayed = false;

    if(!bClipIsPlaying && !bVisualSystemDisplayed){
        animateOff(CLOUDS_HUD_LOWER_THIRD);
    }
}

void CloudsHUDController::clipBegan(CloudsClipEventArgs& args){
	respondToClip(args.chosenClip);
}

void CloudsHUDController::clipEnded(){
    clearClip();
}

void CloudsHUDController::visualSystemBegan(CloudsVisualSystemEventArgs& args){
    respondToSystem(args.preset);
}

void CloudsHUDController::visualSystemEnded(CloudsVisualSystemEventArgs& args){

    clearVisualSystem();
    if(bActJustStarted && bClipIsPlaying){
        animateOn(CLOUDS_HUD_LOWER_THIRD);
        animateOn(CLOUDS_HUD_HOME);
        animateOn(CLOUDS_HUD_NEXT);
    }
    bActJustStarted = false;
    
}

void CloudsHUDController::questionProposed(CloudsQuestionEventArgs& args){

}

void CloudsHUDController::questionSelected(CloudsQuestionEventArgs& args){
    populateQuestion( args.question, true);
}

void CloudsHUDController::topicChanged(CloudsTopicEventArgs& args){

}

void CloudsHUDController::preRollRequested(CloudsPreRollEventArgs& args){
	
}

void CloudsHUDController::respondToClip(CloudsClip* clip){
	
    if(!bVisualSystemDisplayed){
        bActJustStarted = false;
    }
    
    currentSpeaker = CloudsSpeaker::speakers[ clip->person ];
    
    populateLowerThird(currentSpeaker.firstName, currentSpeaker.lastName, currentSpeaker.location2, currentSpeaker.title, currentSpeaker.byline1 );
    
    if(!bActJustStarted){
        if(!hudOpenMap[CLOUDS_HUD_LOWER_THIRD] ){
            animateOn( CLOUDS_HUD_LOWER_THIRD );
        }
        
        if(!hudOpenMap[CLOUDS_HUD_HOME]){
            animateOn( CLOUDS_HUD_HOME );
        }
        
        if( !hudOpenMap[CLOUDS_HUD_NEXT] ){
            animateOn( CLOUDS_HUD_NEXT );
        }
    }
    
    //PROJECT EXAMPLE
    if(!bVisualSystemDisplayed && clip->hasProjectExample && clip->projectExample.exampleVideos.size() ){
        CloudsProjectExample example = clip->projectExample;
        string videoPath = example.exampleVideos[ (int)ofRandom(0, example.exampleVideos.size()) ];
        populateProjectExample( videoPath, example.creatorName, "", example.title, true );
        bProjectExampleDisplayed = true;
    }
    else{
        animateOff(CLOUDS_HUD_PROJECT_EXAMPLE);
        bProjectExampleDisplayed = false;
    }
    
    bClipIsPlaying = true;
 
}

void CloudsHUDController::respondToSystem(const CloudsVisualSystemPreset& preset){
    
    populateVisualSystem(preset.credits.line1, preset.credits.line2 );
    
    if(!bActJustStarted && !hudOpenMap[CLOUDS_HUD_LOWER_THIRD] ){
        animateOn( CLOUDS_HUD_LOWER_THIRD );
    }
    
    bVisualSystemDisplayed = true;
    animateOff(CLOUDS_HUD_PROJECT_EXAMPLE);
}

void CloudsHUDController::questionHoverOn(const string& question,bool animate){
	populateQuestion(question, true, animate);
}

//void CloudsHUDController::questionHoverOff(){
//	animateOff( CLOUDS_HUD_QUESTION );
//}

void CloudsHUDController::populateQuestion(const string& question, bool forceOn, bool animate){
    if(question == ""){
//		animateOff( CLOUDS_HUD_QUESTION );
        clearQuestion();
	}
	else{
		hudLabelMap["QuestionTextBox_1_"]->setText( question, forceOn );
		if( forceOn ){
			if(animate){
				animateOn( CLOUDS_HUD_QUESTION );
			}
			else{
				hudLabelMap["QuestionTextBox_1_"]->instantIn();
			}
		}
        bQuestionDisplayed = true;
	}
}

//BIO
void CloudsHUDController::populateLowerThird(const string& firstName,
                                             const string& lastName,
                                             const string& title,
                                             const string& location,
                                             const string& textbox)
{
 
    CloudsHUDLabel* firstNameLabel  = hudLabelMap["BylineFirstNameTextBox_1_"];
    CloudsHUDLabel* lastNameLabel  = hudLabelMap["BylineLastNameTextBox"];
    
    bool forceOn = hudOpenMap[CLOUDS_HUD_LOWER_THIRD];
    firstNameLabel->setText( firstName, forceOn );
    lastNameLabel->setText( lastName, forceOn );
    
    int firstNameRight = firstNameLabel->getRightEdge();
    int lastNameRight = lastNameLabel->getRightEdge();
    int rightEdge = 0;
    
    if(firstNameRight > lastNameRight){
        rightEdge = firstNameRight;
    }
	else{
        rightEdge = lastNameRight;
	}

//    cout<< "right edge: " << rightEdge << endl;
    
    //move these over to float left of name
    CloudsHUDLabel* locationLabel = hudLabelMap["BylineTopicTextBoxTop"];
    CloudsHUDLabel* titleLabel = hudLabelMap["BylineTopicTextBoxBottom"];
    locationLabel->bounds.x = rightEdge + margin;
    titleLabel->bounds.x = rightEdge + margin;
    
    locationLabel->setText( location, forceOn );
    titleLabel->setText( title, forceOn );
    
    hudLabelMap["BioTextBox"]->setText(textbox, false);

    
    //description
    ////reset to default
    //get rid of this for now
//    CloudsHUDLabel* descLabel = hudLabelMap["BylineBodyCopyTextBox"];
//    descLabel->bounds = defaultBioBounds;
//    descLabel->layout->setLineLength(defaultBioBounds.width);
//    int descLeftEdge = descLabel->bounds.getLeft();
    
//    if(locationLabel->getRightEdge() > titleLabel->getRightEdge()){
//        rightEdge = locationLabel->getRightEdge();
//	}
//	else{
//        rightEdge = titleLabel->getRightEdge();
//	}
//
//    if(rightEdge + margin >= descLeftEdge){
//        descLabel->bounds.x = rightEdge+margin;
//        descLabel->layout->setLineLength(defaultBioBounds.width - (descLabel->bounds.x - defaultBioBounds.x));
//    }
//    
//    descLabel->setText( textbox, forceOn );
    

}

void CloudsHUDController::populateVisualSystem(const string& creditLine1,
                                               const string& creditLine2)
{
    
    CloudsHUDLabel* topBox  = hudLabelMap["VSCreditsTextBoxTop"];
    CloudsHUDLabel* bottomBox  = hudLabelMap["VSCreditsTextBoxBottom"];

    //if the HUD is open, force the text on
    bool forceOn = hudOpenMap[CLOUDS_HUD_LOWER_THIRD];
    topBox->setText( creditLine1, forceOn );
    bottomBox->setText( creditLine2, forceOn );
    
//    int topLineRight = topBox->getRightEdge();
//    int bottomLineRight  = bottomBox->getRightEdge();
//    int rightEdge = 0;
//    
//    if(firstNameRight > lastNameRight){
//        rightEdge = firstNameRight;
//    }
//	else{
//        rightEdge = lastNameRight;
//	}
//    
    //    cout<< "right edge: " << rightEdge << endl;
    
    //move these over to float left of name
//    CloudsHUDLabel* locationLabel = hudLabelMap["VSCreditsTextBoxTop"];
//    CloudsHUDLabel* titleLabel = hudLabelMap["VSCreditsTextBoxBottom"];
//    locationLabel->bounds.x = rightEdge + margin;
//    titleLabel->bounds.x = rightEdge + margin;
    
//    //TODO: do we want to add things into this local
//    locationLabel->setText( "", forceOn );
//    titleLabel->setText( "", forceOn );
    
//    //description
//    ////reset to default
//    CloudsHUDLabel* descLabel = hudLabelMap["BylineBodyCopyTextBox"];
//    descLabel->bounds = defaultBioBounds;
//    descLabel->layout->setLineLength(defaultBioBounds.width);
//    int descLeftEdge = descLabel->bounds.getLeft();
//    
//    if(locationLabel->getRightEdge() > titleLabel->getRightEdge()){
//        rightEdge = locationLabel->getRightEdge();
//	}
//	else{
//        rightEdge = titleLabel->getRightEdge();
//	}
//    
//    if(rightEdge + margin >= descLeftEdge){
//        descLabel->bounds.x = rightEdge+margin;
//        descLabel->layout->setLineLength(defaultBioBounds.width - (descLabel->bounds.x - defaultBioBounds.x));
//    }
//    
//    //TODO: do we want bio text
//    descLabel->setText( "", forceOn );

    
}

ofVec2f CloudsHUDController::getSize(bool bScaled){
    return ofVec2f(hudBounds.width, hudBounds.height) * (bScaled? scaleAmt : 1.0);
}

ofVec2f CloudsHUDController::getCenter(bool bScaled){
    return ofVec2f(hudBounds.width * 0.5, hudBounds.height * 0.5) * (bScaled? scaleAmt : 1.0);
}

void CloudsHUDController::update(){

	for(int i = 0; i < allLayers.size(); i++){
		allLayers[i]->update();
	}
//	float scaleToWidth  = CloudsVisualSystem::getStaticRenderTarget().getWidth()  - 20; //20 for hardcoded bleed
//	float scaleToHeight = CloudsVisualSystem::getStaticRenderTarget().getHeight() - 20;
	float scaleToWidth  = ofGetWidth();
	float scaleToHeight = ofGetHeight();
    
	float xScale = scaleToWidth/hudBounds.width;
	float yScale = scaleToHeight/hudBounds.height;
    
	bool xDominantScale = xScale < yScale;
    scaleAmt	= xDominantScale ? xScale : yScale;
	scaleOffset = xDominantScale ? 
		ofVec2f(0, scaleToHeight- hudBounds.height*scaleAmt)*.5 :
		ofVec2f(scaleToWidth - hudBounds.width*scaleAmt, 0)*.5;

   if( isPlaying){
	   	if(! videoPlayer.isPlaying()){
			videoPlayer.play();
		}
        if( videoPlayer.isFrameNew() ){
            bSkipAVideoFrame = false;
            
            videoBounds.set(0, 0, videoPlayer.getWidth(), videoPlayer.getHeight() );
            videoBounds.scaleTo( svgVideoBounds );
        }

        videoPlayer.update();
    }

    hudLabelMap["NextButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_NEXT]->svg.getMeshByID("NextButtonBacking")->bounds;
    hudLabelMap["NextButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["NextButtonTextBox"]->baseInteractiveBounds);
 	
    researchScrollUpBoundsScaled   = getScaledRectangle(researchScrollUpBounds);
    researchScrollDownBoundsScaled = getScaledRectangle(researchScrollDownBounds);
    researchScrollBoundsScaled = getScaledRectangle(researchScrollBounds);
    
    home.interactiveBounds = getScaledRectangle(home.bounds);
    home.update();
    if( home.wasActivated() ){
        if( !hudOpenMap[CLOUDS_HUD_PAUSE] ){
            pause();
        }
        else{
            unpause();
        }
    }
    
    if( hudOpenMap[CLOUDS_RESEARCH] ){
        updateScroll();
        
        /////TEMP
        fakeConfirmSelectionBounds.x = ofGetWidth()/2 - 150;
        fakeConfirmSelectionBounds.y = ofGetHeight()/2 - 75;
        fakeConfirmSelectionBounds.width = 300;
        fakeConfirmSelectionBounds.height = 150;
        fakeConfirmSelectionBounds = getScaledRectangle(fakeConfirmSelectionBounds);
        
        updateResearchNavigation();
    }
    ///////////////////////////////

}

void CloudsHUDController::updateScroll(){
    if(bIsScrollDownPressed || bIsScrollUpPressed){
        float timeSincePress = ofGetElapsedTimef() - scrollPressedTime;
        bool scrolled = ofGetMousePressed() && ( ( bIsHoldScrolling && timeSincePress > .5) || (!bIsHoldScrolling && timeSincePress > .75) );
        if(scrolled){
            float newScrollPosition = currentResearchList->scrollPosition + scrollIncrement * (bIsScrollUpPressed ? -1 : 1);
            currentResearchList->scrollPosition = ofClamp(newScrollPosition, 0, currentResearchList->totalScrollHeight - researchScrollBounds.height);
            bIsHoldScrolling = true;
        }
    }
    
    
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        CloudsHUDResearchButton& b = currentResearchList->buttons[i];
        b.visible = b.top >= currentResearchList->scrollPosition + scrollIncrement * .5 &&
                    b.top < currentResearchList->scrollPosition + researchScrollBounds.height - scrollIncrement;
        //cout << "button top is " << b.top << " list scroll is " << currentResearchList->scrollPosition << " with height " << currentResearchList->totalScrollHeight << endl;
        if(b.visible){
            b.selectRect = ofRectangle(researchScrollBounds.x,
                                       researchScrollBounds.y + b.top - currentResearchList->scrollPosition,
                                       researchScrollBounds.width, 15);

            b.selectRectScaled = getScaledRectangle( b.selectRect );
        }
    }

}

bool CloudsHUDController::isItemSelected(){
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        if(currentResearchList->buttons[i].clicked){
            return true;
        }
    }
    return false;
}

string CloudsHUDController::getSelectedItem(){
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        if(currentResearchList->buttons[i].clicked){
            return currentResearchList->buttons[i].tag;
        }
    }
    return "";
}

bool CloudsHUDController::isItemConfirmed(){
    return fakeConfirmClicked;
}

void CloudsHUDController::clearSelection(){
    fakeConfirmClicked = false;    
}

void CloudsHUDController::updateResearchNavigation(){

    //get the backing
    hudLabelMap["MapTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("MapHoverBacking")->bounds;
    hudLabelMap["PeopleTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("PeopleHoverBacking")->bounds;
    hudLabelMap["VisualsTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("VSHoverBacking")->bounds;
    hudLabelMap["RSResetButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("RSResetButtonBacking")->bounds;
    
    //set the interaction regions
    hudLabelMap["MapTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["MapTextBox"]->baseInteractiveBounds);
    hudLabelMap["PeopleTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["PeopleTextBox"]->baseInteractiveBounds);
    hudLabelMap["VisualsTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["VisualsTextBox"]->baseInteractiveBounds);
    hudLabelMap["RSResetButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["RSResetButtonTextBox"]->baseInteractiveBounds);
}

void CloudsHUDController::showAbout(){
    //TODO: !!
    //animateOn(CLOUDS_HUD_ABOUT);
}

void CloudsHUDController::hideAbout(){
    //TODO: !!
    //animateOff(CLOUDS_HUD_ABOUT);
}

void CloudsHUDController::pause(){

    //TODO: save the current HUD state before pause

    
    //set up the sizing
    ofRectangle backingBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreBackingHover")->bounds;
    float textRightEdge = hudLabelMap["ExploreTextBox"]->getRightEdge();
    float exploreMapWidth = (textRightEdge - backingBounds.x) + margin;
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreBackingHover")->bounds.width = exploreMapWidth;
    float seeMoreX = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreBackingHover")->bounds.getMaxX();
    float seeMoreMaxX  = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds.getMaxX();
    float seeMoreWidth = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds.getWidth();
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds.x = seeMoreX;
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds.width = (seeMoreMaxX - seeMoreX);

    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreTitleDivide")->mesh.getVertices()[0].x = seeMoreX;
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreTitleDivide")->mesh.getVertices()[1].x = seeMoreX;
    
    hudLabelMap["SeeMoreTextBox"]->bounds.x = seeMoreX + margin;

    hudLabelMap["ResetButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ResetButtonBacking")->bounds;
    hudLabelMap["ExploreTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreBackingHover")->bounds;
    hudLabelMap["SeeMoreTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds;
    
    //set the interaction regions
    hudLabelMap["ResetButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ResetButtonTextBox"]->baseInteractiveBounds);
    hudLabelMap["ExploreTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ExploreTextBox"]->baseInteractiveBounds);
    hudLabelMap["SeeMoreTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["SeeMoreTextBox"]->baseInteractiveBounds);
 
    animateOff( CLOUDS_HUD_QUESTION );
    animateOff( CLOUDS_HUD_NEXT );
    if(bProjectExampleDisplayed){
        animateOff( CLOUDS_HUD_PROJECT_EXAMPLE );
    }

    animateOn( CLOUDS_HUD_PAUSE );

    bJustPaused = true;
    bJustUnpaused = false;

}

void CloudsHUDController::unpause(){
    animateOff( CLOUDS_HUD_PAUSE );
    if(bQuestionDisplayed){
        animateOn( CLOUDS_HUD_QUESTION );
    }
//    if(!bClipIsPlaying && !bVisualSystemDisplayed){
//        animateOff( CLOUDS_HUD_LOWER_THIRD );
//    }
    if(bProjectExampleDisplayed){
        animateOn( CLOUDS_HUD_PROJECT_EXAMPLE );
    }
    
    //if(bClipIsPlaying){
    animateOn( CLOUDS_HUD_NEXT );
    //}
    bJustUnpaused = true;
    bJustPaused = false;
    
}

void CloudsHUDController::setTopics(const set<string>& topics){
    CloudsHUDResearchList& topicList = researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
    topicList.buttons.clear();
    topicList.buttons.resize(topics.size());
    int i = 0;
    for(set<string>::iterator it = topics.begin(); it != topics.end(); it++){
        topicList.buttons[i].top = i * scrollIncrement;
        topicList.buttons[i].tag = *it;
        topicList.buttons[i].label = *it;
        i++;
    }
    
    topicList.totalScrollHeight = topicList.buttons.back().top + scrollIncrement;
    topicList.scrollPosition = 0;
}

void CloudsHUDController::populateSpeakers(){
    CloudsHUDResearchList& peopleList = researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE];
    peopleList.buttons.clear();
    peopleList.buttons.resize(CloudsSpeaker::speakers.size());
    int i = 0;
    for(map<string,CloudsSpeaker>::iterator it = CloudsSpeaker::speakers.begin(); it != CloudsSpeaker::speakers.end(); it++){
        peopleList.buttons[i].top = i * scrollIncrement;
        peopleList.buttons[i].tag = it->first;
        peopleList.buttons[i].label = it->second.firstName + " " + it->second.lastName;
        i++;
    }
    
    peopleList.totalScrollHeight = peopleList.buttons.back().top + scrollIncrement;
    peopleList.scrollPosition = 0;
}

void CloudsHUDController::setVisuals(vector<string> visuals){
    CloudsHUDResearchList& visualsList = researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS];
    visualsList.buttons.clear();
    visualsList.buttons.resize(visuals.size());
    for(int i = 0; i < visuals.size(); i++){
        visualsList.buttons[i].top = i * scrollIncrement;
        visualsList.buttons[i].tag = visuals[i];
        visualsList.buttons[i].label = visuals[i];
    }
    
    visualsList.totalScrollHeight = visualsList.buttons.back().top + scrollIncrement;
    visualsList.scrollPosition = 0;
}

void CloudsHUDController::mouseMoved(ofMouseEventArgs& args){

    for (map<string, CloudsHUDLabel*>::iterator it = hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        it->second->mouseMoved(ofVec2f(args.x,args.y));
    }
    
    for(map<CloudsHUDLayerSet, CloudsHUDLayer*>::iterator it = layers.begin(); it != layers.end(); it++){
        if(it->second->isOpen()){
            if(getScaledRectangle( it->second->svg.getBounds()).inside(args.x,args.y)){
                it->second->hoverOn();
            }
            else {
                it->second->hoverOff();
            }
        }
    }
 
    if(hudOpenMap[CLOUDS_RESEARCH]){
        bIsScrollUpHover = researchScrollUpBoundsScaled.inside(args.x, args.y);
        bIsScrollDownHover = researchScrollDownBoundsScaled.inside(args.x, args.y);
        
        if(researchScrollBoundsScaled.inside(args.x, args.y)){
            
            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                if(currentResearchList->buttons[i].visible){
                    currentResearchList->buttons[i].hovered = currentResearchList->buttons[i].selectRectScaled.inside(args.x, args.y);
                }
            }
        }
        
        //TEMP
        fakeConfirmHovered = fakeConfirmSelectionBounds.inside(args.x, args.y);
    }

}

void CloudsHUDController::mousePressed(ofMouseEventArgs& args){
	
    for(map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->mousePressed(ofVec2f(args.x,args.y));
    }

    if( hudOpenMap[CLOUDS_HUD_HOME] && home.hitTest(args.x, args.y) ){
        home.activate();
    }
    
    if(hudOpenMap[CLOUDS_RESEARCH]){
        bIsScrollUpPressed = researchScrollUpBoundsScaled.inside(args.x, args.y);
        bIsScrollDownPressed = researchScrollDownBoundsScaled.inside(args.x, args.y);
        
        scrollPressedTime = ofGetElapsedTimef();
        if(researchScrollBoundsScaled.inside(args.x, args.y)){
            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                currentResearchList->buttons[i].clicked = false;
                if(currentResearchList->buttons[i].visible){
                    currentResearchList->buttons[i].pressed = currentResearchList->buttons[i].selectRectScaled.inside(args.x, args.y);
                }
            }
        }
        
        //TEMP
        fakeConfirmPressed = fakeConfirmSelectionBounds.inside(args.x, args.y);

    }
}

void CloudsHUDController::mouseReleased(ofMouseEventArgs& args){

    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->mouseReleased(ofVec2f(args.x,args.y));
    }
    
    if(hudOpenMap[CLOUDS_RESEARCH]){
        if(bIsScrollUpPressed && researchScrollUpBoundsScaled.inside(args.x, args.y)){
            float newScrollPosition = currentResearchList->scrollPosition - scrollIncrement;
            currentResearchList->scrollPosition = ofClamp(newScrollPosition, 0, currentResearchList->totalScrollHeight - researchScrollBounds.height);
        }
        if(bIsScrollDownPressed && researchScrollDownBoundsScaled.inside(args.x, args.y)){
            float newScrollPosition = currentResearchList->scrollPosition + scrollIncrement;
            currentResearchList->scrollPosition = ofClamp(newScrollPosition, 0, currentResearchList->totalScrollHeight - researchScrollBounds.height);
        }        
        bIsHoldScrolling = false;
        
        if(researchScrollBoundsScaled.inside(args.x, args.y)){

            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                if(currentResearchList->buttons[i].visible){
                   currentResearchList->buttons[i].clicked = currentResearchList->buttons[i].pressed &&
                                                             currentResearchList->buttons[i].selectRectScaled.inside(args.x, args.y);
                    currentResearchList->buttons[i].pressed = false;
                }
            }
        }
        
        //TEMP
        fakeConfirmClicked = fakeConfirmPressed && fakeConfirmSelectionBounds.inside(args.x, args.y);
        fakeConfirmPressed = false;
    }
}


bool CloudsHUDController::isResetHit(){
    return hudLabelMap["ResetButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isResearchResetHit(){
    return hudLabelMap["RSResetButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isNextHit(){
    return hudLabelMap["NextButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isExploreMapHit(){
    bool selected = hudLabelMap["ExploreTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
    }
    return selected;
}

bool CloudsHUDController::isSeeMorePersonHit(){
    bool selected = hudLabelMap["SeeMoreTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_PEOPLE;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE];
    }
    return selected;
}

bool CloudsHUDController::selectedMapTab(){
    bool selected = hudLabelMap["MapTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
    }
    return selected;
}

bool CloudsHUDController::selectedPeopleTab(){
    bool selected = hudLabelMap["PeopleTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_PEOPLE;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE];
    }
    return selected;
}

bool CloudsHUDController::selectedVisualsTab(){
    bool selected = hudLabelMap["VisualsTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_VISUALS;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS];
    }
    return selected;
}
    
void CloudsHUDController::setHudEnabled(bool enable){
	bDrawHud = enable;
}

bool CloudsHUDController::isHudEnabled(){
    return bDrawHud;
}

bool CloudsHUDController::isPaused(){
    return hudOpenMap[CLOUDS_HUD_PAUSE];
}

bool CloudsHUDController::didPause(){
    bool ret = bJustPaused;
    bJustPaused = false;
    return ret;
}

bool CloudsHUDController::didUnpause(){
    bool ret = bJustUnpaused;
    bJustUnpaused = false;
    return ret;
}

void CloudsHUDController::draw(){
    
    if( !bDrawHud ){
        return;
	}
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableAlphaBlending();
	ofSetLineWidth(1);
    ofTranslate( (ofGetWindowSize() - getSize() ) * 0.5 );
    ofScale( scaleAmt, scaleAmt );
    
    if( videoPlayer.isPlaying() ){
        ofSetColor(255, 255, 255, 255*0.7);
        if( !bSkipAVideoFrame ){
			videoPlayer.draw( videoBounds.x, videoBounds.y, videoBounds.width, videoBounds.height );
        }
        ofSetColor(255, 255, 255, 255);
    }

    for(map<CloudsHUDLayerSet, CloudsHUDLayer*>::iterator it = layers.begin(); it != layers.end(); it++){
//        drawLayer(CloudsHUDLayerSet(i));
        drawLayer(it->first);
    }
    
    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->draw();
    }

	if (hudOpenMap[CLOUDS_HUD_HOME]){
		home.draw();
    }
    
    if(hudOpenMap[CLOUDS_RESEARCH]){
        
        ofPushStyle();
        //test to see bound locations
//        ofSetColor(255, 0, 0);
//        ofRect(researchScrollUpBounds);
//        ofRect(researchScrollDownBounds);
        
        
        //begin test for fake confirm
        if(isItemSelected()){
            if(fakeConfirmClicked){
                ofSetColor(255, 0, 0, 50);
            }
            else if(fakeConfirmPressed){
                ofSetColor(255, 200, 0, 50);
            }
            else if(fakeConfirmHovered) {
                ofSetColor(255, 20, 255, 50);
            }
            else{
                ofNoFill();
                ofSetColor(255,255,255,50);
            }
            ofRect(fakeConfirmSelectionBounds);
            
        }
        
        ofPopStyle();
        ///end test
        
        drawList();
        
    }
 

    //TODO: intelligent widgets
	if(hudLabelMap["ResetButtonTextBox"]->isClicked()){
		ofSetColor(200,30,0,200);
		resetTriangle.draw();
	}

	ofPopMatrix();
	ofPopStyle();


}

void CloudsHUDController::drawLayer(CloudsHUDLayerSet layer){
    layers[layer]->draw();
}

void CloudsHUDController::drawList(){
    ofPushMatrix();
    ofTranslate(0, -currentResearchList->scrollPosition);
 
    ofPushStyle();
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        if(currentResearchList->buttons[i].visible){
            //TODO: better coloring system
            if(currentResearchList->buttons[i].clicked){
                ofSetColor(255, 180, 0);
            }
            else if(currentResearchList->buttons[i].pressed){
                ofSetColor(255, 180, 0, 200);
            
            }
            else if(currentResearchList->buttons[i].hovered){
                ofSetColor(255);
            }
            else{
                ofSetColor(255, 200);
            }
            
            //TODO: maybe different styles for different
            ResearchTopicListLabel->font->drawString(currentResearchList->buttons[i].label,
                                              hudLabelMap["ListTextBoxes"]->bounds.x,
                                              researchScrollBounds.y + currentResearchList->buttons[i].top + scrollIncrement * .5);
            
        }
    }
    ofPopStyle();
    
    ofPopMatrix();
}

#ifdef OCULUS_RIFT
void CloudsHUDController::draw3D(ofCamera* cam, ofVec2f offset){
    
    if( !bDrawHud ){
        return;
	}
    
	ofPushStyle();
	ofPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
 
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    ofEnableAlphaBlending();
	ofSetLineWidth(1);

    drawLayer3D(CLOUDS_HUD_QUESTION, cam, offset);
    drawLayer3D(CLOUDS_HUD_LOWER_THIRD, cam, offset);
	drawLayer3D(CLOUDS_HUD_PROJECT_EXAMPLE, cam, offset);
	drawLayer3D(CLOUDS_HUD_MAP, cam, offset);
	
    glPopAttrib();
    
	ofPopMatrix();
	ofPopStyle();
}

void CloudsHUDController::drawLayer3D(CloudsHUDLayer layer, ofCamera* cam, ofVec2f& offset){
    ofPushMatrix();
    
    // Hook up to the camera to keep the layer steady.
    ofMatrix4x4 baseRotation;
    ofTranslate(cam->getPosition());
    baseRotation.makeRotationMatrix(cam->getOrientationQuat());
    ofMultMatrix(baseRotation);
    
    ofVec3f camPos = ofVec3f();  //cam->getPosition();
    
    // Calculate the base position.
    static ofVec3f yAxis = ofVec3f(0.0, 1.0, 0.0);
    static ofVec3f xAxis = ofVec3f(1.0, 0.0, 0.0);
//    ofVec3f basePos = camPos + (cam->getLookAtDir().getScaled(layerDistance[layer]));
//    ofVec3f basePos(0, 0, -layerDistance[layer]);
    ofVec3f basePos(offset.x, offset.y, -layerDistance[layer]);
    basePos.rotate(layerRotationH[layer], camPos, yAxis);
    basePos.rotate(layerRotationV[layer], camPos, xAxis);
    
    // Get the total layer bounds.
    ofRectangle layerBounds;
	for(int i = 0; i < layerSets[layer].size(); i++){
        if (i == 0) layerBounds = layerSets[layer][i]->svg.getBounds();
        else layerBounds.growToInclude(layerSets[layer][i]->svg.getBounds());
	}
    
    // Translate to the layer center pos.
    ofVec3f layerPos = basePos + (getCenter(false) - layerBounds.getCenter());
    ofTranslate(layerPos);

	//JG Yebizo Festival -- Commenting out other billboard modes for now
    //if (layerBillboard[layer] == CLOUDS_HUD_BILLBOARD_OCULUS) {
    //    // Billboard rotation using the Oculus orientation.
    //    float angle;
    //    ofVec3f axis;
    //    CloudsVisualSystem::getOculusRift().getOrientationQuat().getRotate(angle, axis);
    //    ofRotate(angle, axis.x, axis.y, axis.z);
    //    ofScale(-1, 1, 1);
    //}
//    else if (layerBillboard[layer] == CLOUDS_HUD_BILLBOARD_CAMERA) {
        // Billboard rotation using the camera.
        ofNode node;
        node.setPosition(layerPos);
        node.lookAt(camPos);
        ofVec3f axis;
        float angle;
        node.getOrientationQuat().getRotate(angle, axis);
        ofRotate(angle, axis.x, axis.y, axis.z);
//    }
//    else {
////        ofRotateY(layerRotationH[layer]);
////        ofRotateX(layerRotationV[layer]);
//        ofScale(-1, 1, 1);
//    }
    
    // Debug circle.
//    ofSetColor(255);
//    ofCircle(0, 0, 25);
    
    // Transform for rendering the layer.
    ofScale(-scaleAmt, -scaleAmt, 1);
    ofTranslate(-layerBounds.getCenter());

    // Draw the video player if we're on the right layer.
    if (layer == CLOUDS_HUD_PROJECT_EXAMPLE && videoPlayer.isPlaying()) {
        ofSetColor(255, 255, 255, 255*0.7);
        if( !bSkipAVideoFrame ){
//            videoPlayer.draw( videoBounds.x, videoBounds.y, videoBounds.width, videoBounds.height );
        }
    }
    
    // Draw the layer.
    ofSetColor(255);
    drawLayer(layer);
    
    // Draw the home button if we're on the right layer.
    if (layer == CLOUDS_HUD_LOWER_THIRD && hudOpenMap[CLOUDS_HUD_LOWER_THIRD]) {
        home.draw();
    }
    
    // Draw the associated text labels.
    for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
        bool bFound = false;
        for(int i = 0; i < layerSets[layer].size(); i++){
            if (layerSets[layer][i]->svg.getMeshByID(it->first) != NULL) {
                bFound = true;
                break;
            }
        }
        if (bFound) {
            (it->second)->draw();
        }
    }
    
    ofPopMatrix();
}
#endif

void CloudsHUDController::animateOn(CloudsHUDLayerSet layer){
	
    for (map<CloudsHUDLayerSet, CloudsHUDLayer* >::iterator it = layers.begin(); it != layers.end(); ++it) {
        if (layer == it->first || layer == CLOUDS_HUD_ALL) {
            hudOpenMap[it->first] = true;
            it->second->start();
        }
    }
    
    // animate in text, this is sub-optimal
    if( layer == CLOUDS_HUD_ALL ){
        for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
            (it->second)->animateIn( true );
        }
    }
    else{
        for(int i = 0; i < hudLayerLabels[layer].size(); i++){
            hudLayerLabels[layer][i]->animateIn( true );
        }
    }
}

void CloudsHUDController::animateOff(){
    animateOff(CLOUDS_HUD_ALL);
}

void CloudsHUDController::animateOff(CloudsHUDLayerSet layer){
    
    if(layer == CLOUDS_HUD_LOWER_THIRD){
        cout << "ANIMATING OUT LOWER THIRDS" << endl;
    }
    
    if(layer == CLOUDS_HUD_QUESTION){
        cout << "ANIMATING OUT QUESTION" << endl;
    }
    
    if (isPlaying) {
		isPlaying = false;
        videoPlayer.stop();
        videoPlayer.close();
    }

    for (map<CloudsHUDLayerSet, CloudsHUDLayer* >::iterator it = layers.begin(); it != layers.end(); ++it) {
        if (layer == it->first || layer == CLOUDS_HUD_ALL) {
            hudOpenMap[it->first] = false;
            it->second->close();
        }
    }
    
    // animate out text, this is sub-optimal
    if( layer == CLOUDS_HUD_ALL ){
        for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
            (it->second)->animateOut();
        }
    }
    else{
        for(int i = 0; i < hudLayerLabels[layer].size(); i++){
            hudLayerLabels[layer][i]->animateOut();
        }
    }
    
    
}

ofRectangle CloudsHUDController::getScaledRectangle(const ofRectangle& rect){
    ofRectangle outrect;
    outrect.x      = rect.x * scaleAmt + scaleOffset.x;
	outrect.y      = rect.y * scaleAmt + scaleOffset.y;
	outrect.width  = rect.width  * scaleAmt;
	outrect.height = rect.height * scaleAmt;
    return outrect;
}

