#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 3840, 1080, OF_WINDOW);
	ofRunApp(new testApp());
	
}
