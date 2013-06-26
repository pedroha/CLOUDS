#pragma once

#define CLOUDS_NO_VS 1

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "CloudsD3Exporter.h"
#include "ClipSelector.h"

//@class ViewerApp;
@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate, NSTokenFieldDelegate> {

    IBOutlet ClipSelector* linkerA;
    IBOutlet ClipSelector* linkerB;
    
    CloudsFCPParser parser;
	ofVideoPlayer preview;
	CloudsD3Exporter exporter;
    
    bool updatePhysics;
    vector<string> selectedKeywordsSource;
    vector<string> selectedKeywordsTarget;
    CloudsClip currentPlayingClip;
    BOOL clipLoaded;

    vector<CloudsClip> selectedClipsTarget;
    vector<CloudsClip> selectedClipsSource;
    vector<CloudsLink> currentClipLinks;
    vector<CloudsLink> currentSuppressedLinks;

	bool playingPlaylist;
	int currentPlaylistIndex;
	
    int clipEndFrame;

	ofTrueTypeFont font;
	bool autoProgressStory;
	float timeOfNextStory;
	float storyStartTime;
	
	bool movieFileMissing;
	bool onPlaylist;
}

@property(nonatomic,readonly) int clipEndFrame;
@property(nonatomic,readonly) bool movieFileMissing;
@property(nonatomic,readonly) ofVideoPlayer& preview;

- (void)setup;
- (void)update;
- (void)draw;
- (void)exit;

- (void) playClip:(CloudsClip&)clip;

- (IBAction) setXMLFolder:(id)sender;
- (IBAction) refreshXML:(id)sender;
- (IBAction) saveLinks:(id)sender;

//- (IBAction) deleteLink:(id)sender;
//- (IBAction) regenerateGraph:(id)sender;

- (IBAction) togglePlay:(id)sender;
//- (IBAction) nextOnPlaylist:(id)sender;
//- (IBAction) prevOnPlaylist:(id)sender;
//- (IBAction) playCurrentPlaylist:(id)sender;
//- (IBAction) addClip:(id)sender;
//- (IBAction) removeLink:(id)sender;
//- (IBAction) unsuppressLink:(id)sender;
//- (IBAction) suppressLinkModifier:(id)sender;
//- (IBAction) previewLinks:(id)sender;
//- (IBAction) previewSuppressed:(id)sender;
//- (IBAction) stopPreview:(id)sender;


- (IBAction)linkFromLeftToRight:(id)sender;
- (IBAction)linkFromRightToLeft:(id)sender;
- (IBAction)suppressFromLeftToRight:(id)sender;
- (IBAction)suppressFromRighttoLeft:(id)sender;

//- (IBAction) linkLast:(id)sender;
//- (IBAction) suppressLast:(id)sender;
//- (IBAction) setStartText:(id)sender;

- (float) clipPercentComplete;

- (void) linkClip:(CloudsClip) source toClip:(CloudsClip) target;
- (void) suppressLink:(CloudsClip)source toClip:(CloudsClip) target;

- (void)keyPressed:(int)key;
- (void)keyReleased:(int)key;
- (void)mouseMoved:(NSPoint)p;
- (void)mouseDragged:(NSPoint)p button:(int)button;
- (void)mousePressed:(NSPoint)p button:(int)button;
- (void)mouseReleased:(NSPoint)p button:(int)button;
- (void)windowResized:(NSSize)size;

//- (BOOL)tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;

//- (CloudsClip&) selectedClip;
//- (CloudsClip&) selectedClipFromPlaylist;

//- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
//- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
//- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;
//- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors;
//- (NSArray *)tokenField:(NSTokenField *)tokenField completionsForSubstring:(NSString *)substring indexOfToken:(NSInteger)tokenIndex indexOfSelectedItem:(NSInteger *)selectedIndex;


@end