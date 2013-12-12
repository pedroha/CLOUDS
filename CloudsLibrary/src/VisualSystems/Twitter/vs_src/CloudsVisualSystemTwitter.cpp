//
//  CloudsVisualSystemTwitter.cpp
//

#include "CloudsVisualSystemTwitter.h"
#include "CloudsRGBDVideoPlayer.h"

#include <Poco/URI.h>

bool dateSorter(Date const& lhs, Date const& rhs) {
    if (lhs.year != rhs.year)
        return lhs.year < rhs.year;
    if (lhs.month != rhs.month)
        return lhs.month < rhs.month;
    return lhs.day < rhs.day;
}

void CloudsVisualSystemTwitter::selfSetup()
{
    loadJSONData();
    parseClusterNetwork(getVisualSystemDataPath() +"/twitter.net");
    loadMesh();
    clusterShader.load(getVisualSystemDataPath()+"/shaders/cluster");
    sprite.loadImage(getVisualSystemDataPath()+"/dot.png");
    meshExpansion = 100;
    pointSize =10;
    std::sort(dateIndex.begin(), dateIndex.end(), &dateSorter);
//    currentDateIndex =50;
    currentDateIndex = dateIndex.size() -1;
    updateMesh(currentDateIndex);

}

void CloudsVisualSystemTwitter::selfBegin()
{
    
}

void CloudsVisualSystemTwitter::selfSetupGui()
{
	clusterGui = new ofxUISuperCanvas("CLUSTER PARAMS", gui);
    clusterGui->copyCanvasStyle(gui);
	clusterGui->copyCanvasProperties(gui);
	clusterGui->setName("Custom");
	clusterGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    clusterGui->addSlider("mesh expansion", 100, 10000, &meshExpansion);
	clusterGui->addSlider("point size", 1, 50, &pointSize);
	ofAddListener(clusterGui->newGUIEvent, this, &CloudsVisualSystemTwitter::selfGuiEvent);
	guis.push_back(clusterGui);
	guimap[clusterGui->getName()] = clusterGui;
}

void CloudsVisualSystemTwitter::loadJSONData(){

    ofFile f = ofFile();
    
    ofDirectory dir(getVisualSystemDataPath()+"tweets/");
    dir.listDir();
    if(dir.exists()){
        int size = dir.size();
        vector<ofFile>files= dir.getFiles();
        
        for(int i = 0; i< files.size(); i++){
            string filePath =getVisualSystemDataPath()+"tweets/" +files[i].getFileName();
            
            bool parsingSuccessful = result.openLocal(filePath);
            if (parsingSuccessful) {
                
                if(result.isMember("errors")) {
                    ofDrawBitmapString(result.getRawString(), 10, 14);
                }
                else if(result.isMember("Tweets")){
                    
                    Tweeter cur;
                    vector<Tweet> userTweets;
                    ofxJSONElement tweets = result["Tweets"];
                    
                    vector<string> names = ofSplitString(result["name"].asString(), ".")    ;
                    cur.name = "@" + names[0];
                    cur.ID = i;
                    
                    for(int j =0; j<tweets.size(); j ++){
                        Tweet t;
                        t.tweet = tweets[j]["Tweet"].asString();
                        
                        if(tweets[j]["Hashtag"].isValidIndex(0)){
                            ofxJSONElement hashTags = tweets[j]["Hashtag"];
                            
                            for(int k=0; k<hashTags.size(); k++){
                                t.hashtags.push_back(hashTags[k ].asString());
                            }
                        }
                        
                        if(tweets[j]["Users"].isValidIndex(0)){
                            ofxJSONElement users = tweets[j]["Users"];
                            
                            for(int k=0; k<users.size(); k++){
                                
                                if( !ofContains(cur.userLinks, users[k].asString())){
                                    t.mentionedUsers.push_back(users[k].asString());
                                    cur.userLinks.push_back(users[k].asString());
                                }
                            }
                        }
                        if(tweets[j].isMember("Date")){
                            bool alreadyExists = false;
                            ofxJSONElement date = tweets[j]["Date"];
                            t.tweetDate.day = date["Day"].asInt()   ;
                            t.tweetDate.month =date["Month"].asInt();
                            t.tweetDate.year =date["Year"].asInt();
                            
                            for(int i=0; i<dateIndex.size(); i++){
                                
                                if(dateIndex[i].year == t.tweetDate.year && dateIndex[i].month == t.tweetDate.month && dateIndex[i].day == t.tweetDate.day){
                                    alreadyExists = true;
                                    break;
                                }
                            }
                            if(! alreadyExists){
                                dateIndex.push_back(t.tweetDate);
                                dateIndexMap[getDateAsString(t.tweetDate)] = dateIndex.size()-1;
                            }
                        }
                        userTweets.push_back(t);
                    }
                    cur.tweets= userTweets;
                    tweeters.push_back(cur);
                } else {
                    cout  << "Failed to parse JSON" << endl;
                }
            }
        }
    }
    addUsersFromMentions();
}

void CloudsVisualSystemTwitter::parseClusterNetwork(string fileName){
	ofBuffer pajekFile = ofBufferFromFile(fileName);
	bool findingNodes = false;
    bool findingEdges = false;;
	while(!pajekFile.isLastLine()){
		string line = pajekFile.getNextLine();
		
		if(line == "" || line.at(0) == '%'){
			continue;
		}
		
		if (line.find("*Vertices") != string::npos ) {
			findingNodes = true;
			continue;
            
		}
		if (line.find("*Edgeslist") != string::npos ) {
			findingNodes = false;
            findingEdges = true;
            continue;
		}
		
		if(findingNodes){
			vector<string> components = ofSplitString(line, " ");
            
            int id = ofToInt(components[0]);
            
            Tweeter& tweeter = getTweeterByID(tweeters, id);
            
			int numcomp = components.size();
            max = ofVec3f(0,0,0);
            
			tweeter.position = ofVec3f(ofToFloat(components[2]),
                                       ofToFloat(components[3]),
                                       ofToFloat(components[4])*10);
		}
        
        if(findingEdges){
			vector<string> components = ofSplitString(line, " ");
            int id = ofToInt(components[0]);
            //428 4 8 9 11 15 17 18
            
            Tweeter& tweeter = getTweeterByID(tweeters, id);
            for(int i =1; i< components.size()-1; i++){
                if(tweeter.ID != ofToInt(components[i]) ){
                    tweeter.linksById.push_back(ofToInt(components[i]));
                }
                else{
                    cout<<"Error! "<<tweeter.name<<"  : "<<tweeter.ID<<" index "<< ofToInt(components[i])<<endl;
                }
            }
        }
        
	}
}

void CloudsVisualSystemTwitter::updateMesh(int index){
    
    for(int i= 0; i<nodeMesh.getColors().size(); i++){
        nodeMesh.setColor(i, ofFloatColor(0.,0.,0.,0.));
    }
    
    for(int i= 0; i<edgeMesh.getColors().size(); i++){
        ofFloatColor c = edgeMesh.getColor(i);
        if(c.b == 0){
                edgeMesh.setColor(i, c.a*0.001);
        }
        
    }
    
    vector<Tweeter> current = getTweetersForDate(index);
    if(current.size() > 0 ){
        
        for (int i = 0; i<current.size(); i++) {
            
            
            vector<Tweet> tweetsOnDate = current[i].getTweetsByDate(dateIndex[index]);
            
            for(int k=0; k<tweetsOnDate.size();k ++){
                
                
                
                for(int l=0; l<tweetsOnDate[k].mentionedUsers.size(); l++){
                    int user = getUserIdByName(tweetsOnDate[k].mentionedUsers[l]);
                    if(user != -1){
                        
                        
                        Tweeter& t  = getTweeterByID(tweeters,user);
                        
                        
                        if(lineIndexPairs.find(make_pair(current[i].name, t.name)) != lineIndexPairs.end()){
                            
                            pair<int, int> currentIndeces = lineIndexPairs[make_pair(current[i].name, t.name)];
                            edgeMesh.setColor(currentIndeces.first, ofFloatColor(1.,0.,0.,1.));
                            edgeMesh.setColor(currentIndeces.second, ofFloatColor(1.0,0.,0.,1.));
                        }
                        else if(lineIndexPairs.find(make_pair(t.name,current[i].name)) != lineIndexPairs.end()){
                            
                            pair<int, int> currentIndeces = lineIndexPairs[make_pair(current[i].name, t.name)];
                            edgeMesh.setColor(currentIndeces.first, ofFloatColor(1.,0.,0.,1.));
                            edgeMesh.setColor(currentIndeces.second, ofFloatColor(1.0,0.,0.,1.));
                        }
                        else{
                            cout<<"link between "<<current[i].name<<" and "<<t.name<<" not found in map"<<endl;
                        }
                        
                    }
                    
                }
            }
        }
        
        for (int i=0; i<current.size(); i++) {
            nodeMesh.setColor(current[i].nodeVertexIndex, ofFloatColor(1.0,1.0,1.0,1.));
        }
    }
    
}

void CloudsVisualSystemTwitter::loadMesh(){
    int  currentIndex =0;
    for(int j=0; j<tweeters.size(); j++){
        
        for (int k=0; k<tweeters[j].linksById.size(); k++) {
            
    
                edgeMesh.addVertex(tweeters[j].position);
                edgeMesh.addNormal(ofVec3f(0,0,0));
                edgeMesh.addColor(ofFloatColor(0.0,0.0,1.0,0.1));
                tweeters[j].edgeVertexIndex = currentIndex;
                
                currentIndex++;
                Tweeter& t  = getTweeterByID(tweeters, tweeters[j].linksById[k]);
                
                edgeMesh.addVertex(t.position);
                edgeMesh.addNormal(ofVec3f(0,0,0));
                edgeMesh.addColor( ofFloatColor(0.0,0.0,1.0,0.1));
                t.edgeVertexIndex = currentIndex;
            
                links.insert(make_pair(tweeters[j].ID, tweeters[j].linksById[k]));
                lineIndexPairs[make_pair(tweeters[j].name, t.name) ] = make_pair(currentIndex-1, currentIndex);
            currentIndex++;

        }
        edgeMesh.setMode(OF_PRIMITIVE_LINES);

    }
    currentIndex = 0;
    for(int j=0; j<tweeters.size(); j++){
        
        nodeMesh.addVertex(tweeters[j].position);
        nodeMesh.addNormal(ofVec3f(0,0,0));
        nodeMesh.addColor(ofFloatColor(0.0,0.0,0.0,1.0));
        tweeters[j].nodeVertexIndex = currentIndex;
        
        
        currentIndex++;
        nodeMesh.setMode(OF_PRIMITIVE_POINTS);
        
        
    }
}

void CloudsVisualSystemTwitter::addUsersFromMentions(){
    
    vector<string> names;
    
    for (int i= 0; i < tweeters.size(); i++) {
        names.push_back(tweeters[i].name);
    }
    
    for (int i= 0; i < tweeters.size(); i++) {
        for(int j=0; j<tweeters[i].userLinks.size(); j++){
            
            if(! ofContains(names, tweeters[i].userLinks[j])){
                numberOfMentions[tweeters[i].userLinks[j]] ++;
            }
        }
    }
    
    map<string,int>::iterator it;
    for(it = numberOfMentions.begin() ; it != numberOfMentions.end() ; it++){
        
        if(it->second > 3){
            Tweeter t = Tweeter(it->first, tweeters.size());
            tweeters.push_back(t);
        }
    }
}


void CloudsVisualSystemTwitter::createPajekNetwork(){
    stringstream ss;
    cout<<"Creating paejk file"<<endl;
    ss<<"*Vertices "<<tweeters.size()<<endl;
    
    for (int i= 0; i < tweeters.size(); i++) {
        ss<<tweeters[i].ID<<"  \""<<tweeters[i].name<<"\""<<endl;
    }
    
    ss<<"*EdgesList "<<tweeters.size()<<endl;
    for(int j =0; j<tweeters.size(); j++){
        
        string edges;
        if(tweeters[j].userLinks.size() > 0){
            for(int k=0; k< tweeters[j].userLinks.size(); k++){
                
                if(getUserIdByName(tweeters[j].userLinks[k]) != -1){
                    edges += ofToString(getUserIdByName(tweeters[j].userLinks[k])) +  " ";
                }
            }
            ss<<tweeters[j].ID<<" "<<edges<<endl;
        }
    }
    ofBuffer b = ofBuffer(ss);
    ofBufferToFile(getVisualSystemDataPath() + "/twitter.net",b);
}

int CloudsVisualSystemTwitter:: getUserIdByName(string name){
    
    for (int i=0; i<tweeters.size(); i++) {
        if(tweeters[i].name == name){
            return tweeters[i].ID;
        }
    }
    return -1;
}

vector<Tweeter> CloudsVisualSystemTwitter::getTweetersForDate(int index){
    vector<Tweeter> tOnDate;
    
    for(int k=0; k<tweeters.size(); k++){
        if(tweeters[k].hasTweetOnDate(dateIndex[index])){
            tOnDate.push_back(tweeters[k]);
        }
    }
    
    return tOnDate;
}

void CloudsVisualSystemTwitter::drawTweetsForDate(int index){
    
    for(int k=0; k<tweeters.size(); k++){
        vector<Tweet> tweets =  tweeters[k].getTweetsByDate(dateIndex[index]);
        
        if(tweets.size() > 0 ){
            
            for(int j = 0; j<tweets.size(); j++){
                ss<<tweets[j].tweet<<" "<<getDateAsString(dateIndex[index])<<endl;
            }
        }
    }
    
}
string CloudsVisualSystemTwitter::getDateAsString(Date d){
    string dateString;
    
    dateString += ofToString(d.day) + " , ";
    dateString += ofToString(d.month) + " , ";
    dateString += ofToString(d.year);
    return dateString;
}


Tweeter& CloudsVisualSystemTwitter::getTweeterByID(vector<Tweeter>& tweeters, int _id ){
    
    for(int i=0; i< tweeters.size(); i++){
        if(tweeters[i].ID == _id){
            return tweeters[i];
        }
    }
}

void CloudsVisualSystemTwitter::CompareDates(Date d1,Date d2){
    
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfGuiEvent(ofxUIEventArgs &e)
{
    
}
//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemTwitter::selfSetupSystemGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemTwitter::selfSetupRenderGui(){
    
}

void CloudsVisualSystemTwitter::guiRenderEvent(ofxUIEventArgs &e){
	
}


//--------------------------------------------------------------
// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemTwitter::selfPresetLoaded(string presetPath)
{
	
}



//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemTwitter::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
//normal update call
void CloudsVisualSystemTwitter::selfUpdate()
{
    if(  ofGetFrameNum() %2 <4){
        currentDateIndex %= dateIndex.size();
        currentDateIndex++;
        updateMesh(currentDateIndex);
    }

    
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemTwitter::selfDraw()
{
    
    glDisable(GL_DEPTH_TEST);
    //     ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    ofSetBackgroundColor(0,0,0);
    ofScale(10, 10);
    
    glPointSize(3);
    
    nodeMesh.draw();
    edgeMesh.draw();
    
    
    
}

// draw any debug stuff here
void CloudsVisualSystemTwitter::selfDrawDebug()
{
    
}

//--------------------------------------------------------------
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemTwitter::selfDrawBackground()
{
    ofSetColor(ofColor::whiteSmoke);
    string cur = getDateAsString( dateIndex[currentDateIndex]);
    ofPushMatrix();
    ofDrawBitmapString(cur , 0,0);
    ofRotateY(90);
    ofPopMatrix();
    //    listFont.drawString(ss.str(), 0, 0);
    //    ofDrawBitmapString("TEST", ofGetWidth()/2,ofGetHeight()/2);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemTwitter::selfEnd()
{
    
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemTwitter::selfExit()
{
    
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemTwitter::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key =='k'){
        std::sort(dateIndex.begin(), dateIndex.end(), &dateSorter);
        
        for(int i =0; i<dateIndex.size(); i++){
            cout<<dateIndex[i].day<<" : "<<dateIndex[i].month<<" : "<<dateIndex[i].year<<endl;
        }
    }
    if(args.key == 'j'){
        cout<<ss.str()<<endl;
    }
    if(args.key == OF_KEY_LEFT){
        if(currentDateIndex > dateIndex.size()){
            
        }
        else{
            currentDateIndex++;
        }
        
        cout<<getDateAsString(dateIndex[currentDateIndex])<<endl;
        updateMesh(currentDateIndex);
    }
    if(args.key == OF_KEY_RIGHT){
        if(currentDateIndex <0){
            
        }
        else{
            currentDateIndex--;
        }
        
        cout<<getDateAsString(dateIndex[currentDateIndex])<<endl;
        
        updateMesh(currentDateIndex);
        
    }
}
void CloudsVisualSystemTwitter::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemTwitter::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemTwitter::selfMouseReleased(ofMouseEventArgs& data){
	
}
