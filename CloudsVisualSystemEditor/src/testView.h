#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "CloudsVisualSystemManager.h"

@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate, NSTokenFieldDelegate> {
	IBOutlet NSTokenField* currentKeywords;
	
	IBOutlet NSTableView* presetTable;
	IBOutlet NSTableView* clipTable;
	IBOutlet NSTableView* suppressedClipTable;
	IBOutlet NSTableView* allKeywordTable;
	IBOutlet NSTableView* allClipTable;
	IBOutlet NSTableView* clipPresetTable; //presets for the selected clip
	
	IBOutlet NSTextField* keywordPercent;
	IBOutlet NSTextField* clipPercent;
	
	IBOutlet NSTextField* notesText;
	IBOutlet NSComboBox* grade;
	IBOutlet NSButton* enabledBox;
	IBOutlet NSButton* oculusBox;
	IBOutlet NSButton* soundBox;

	IBOutlet NSButton* filterEnabledBox;
	IBOutlet NSButton* filterOculusBox;
	IBOutlet NSButton* filterGradeABox;

	bool shouldPlaySelectedRow;
	
	CloudsFCPParser parser;
	CloudsVisualSystemManager visualSystems;
	ofPtr<CloudsVisualSystem> currentVisualSystem;
	CloudsVisualSystemPreset* selectedPreset;
	vector<int> filteredPresetInds;
	
	vector<CloudsClip> associatedClips;
	vector<CloudsClip> suppressedClips;
	vector<string> associatedKeywords;
	
	vector<CloudsVisualSystemPreset> currentClipPresets;
	
	vector<int> sortedKeywordIndeces;
	vector<int> sortedClipIndeces;
	
	float percentKeywordsTagged;
	float percentClipsTagged;
}

- (int)selectedPresetIndex;

- (void)setup;
- (void)update;
- (void)draw;
- (void)exit;

- (void)keyPressed:(int)key;
- (void)keyReleased:(int)key;
- (void)mouseMoved:(NSPoint)p;
- (void)mouseDragged:(NSPoint)p button:(int)button;
- (void)mousePressed:(NSPoint)p button:(int)button;
- (void)mouseReleased:(NSPoint)p button:(int)button;
- (void)windowResized:(NSSize)size;

- (void) updateCounts;

- (IBAction) updateFilters:(id)sender;
- (IBAction) updatePresets:(id)sender;
- (IBAction) deletePreset:(id)sender;
- (IBAction) updateKeywords:(id)sender;
- (IBAction) suppressClip:(id)sender;
- (IBAction) unsuppressClip:(id)sender;

- (IBAction) linkClipToPreset:(id)sender;

- (IBAction) exportStandalone:(id)sender;

- (IBAction) playSelectedRow:(id)sender;
- (IBAction) playPreviousPreset:(id)sender;
- (IBAction) playNextPreset:(id)sender;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;
- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors;

- (void)playDoubleClickedRow:(id)sender;

- (NSArray *)tokenField:(NSTokenField *)tokenField
completionsForSubstring:(NSString *)substring
		   indexOfToken:(NSInteger)tokenIndex
	indexOfSelectedItem:(NSInteger *)selectedIndex;

- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor;
- (NSArray *)tokenField:(NSTokenField *)tokenField shouldAddObjects:(NSArray *)tokens atIndex:(NSUInteger)index;
- (void) updateAssociatedClips;
- (BOOL) hasKeyword:(NSString*) keyword;
- (vector<string>) entries:(vector<string>&)a sharedWith:(vector<string>&)b;

@end
