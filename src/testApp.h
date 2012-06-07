#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxOsc.h"
#include "ofxVectorMath.h"
#include "ofxXmlSettings.h"

#define OF_ADDON_USING_OFXOSC
#define N_SOUNDS 6
// listen on port 12345
//port is 9002 at nyu
//#define PORT 12346 
#define NUM_MSG_STRINGS 20
#include <stdio.h>
#include <stdlib.h>

//128.122.151.183 - nyu
//#define HOST "localhost"
//"192.168.130.241"
//"localhost"
//define PORT 12345


class Data {
public:
	int	 soundID;
	bool hit;
    ofColor color;
	int		id;
    int type;
    int lifespan;
    int paddleopacity;
    bool killed;
    
};

class Invader : public ofxBox2dRect {
	
public:
    ofVideoPlayer * movie;
    
    void setupTheCustomData() {
		static int i;
		static int colors[] = {0xcae72b, 0xe63b8f, 0x2bb0e7, 0x00FFFF, 0xFF00FF, 0x99FF66, 0xFFCCFF, 0x9900FF, 0x33CCCC, 0x00FFCC, 0x666633, 0xCC0066};
        // we are using a Data pointer because 
		// box2d needs to have a pointer not 
		// a referance
		setData(new Data());
		Data * theData = (Data*)getData();
        theData->color.setHex(colors[i]);
        theData->id = i;
        theData->soundID = ofRandom(0, N_SOUNDS);
        theData->hit	= false;		
		theData->type= 2;
        theData->lifespan = 255;
        printf("setting the custom data!\n");
		i ++;
        i = i% 13;
	}
	
	void draw() {
        
		Data* theData = (Data*)getData();
		if(theData) {
			
			// Evan though we know the data object lets just 
			// see how we can get the data out from box2d
			// you would use this when using a contact listener
			// or tapping into box2d's solver.
			
			float width = getWidth()*2;
			ofPushMatrix();
            ofSetRectMode(OF_RECTMODE_CENTER);
			ofTranslate(getPosition());
			ofRotateZ(getRotation());
            ofSetColor(255,221,21);
            ofRect(0,0,width + 20, width + 20);
            ofSetColor(255);
            movie->draw(0,0,width,width);
            //  ofRect(0,0, width, width);
			ofDrawBitmapString(ofToString(theData->id), -5, 5);
			ofPopMatrix();
		}
    }
    
};



// -------------------------------------------------
class testApp : public ofBaseApp {
	
public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
    void startGame();
	void resized(int w, int h);
	void loadSettings(string fileString);
	// this is the function for contacts
	void contactStart(ofxBox2dContactArgs &e);
	void contactEnd(ofxBox2dContactArgs &e);
    
	// when the ball hits we play this sound
	ofSoundPlayer  sound[N_SOUNDS];
	ofxBox2d						box2d;			  //	the box2d world
	ofRectangle bounds;
    vector		<ofxBox2dCircle>	invaderbullets;		  //	default box2d circles
    vector		<ofxBox2dCircle>	leftbullets;		  //	default box2d circles
    vector		<ofxBox2dCircle>	rightbullets;		  //	default box2d circles
    vector		<ofxBox2dRect>	players;
	vector		<ofVideoPlayer*>	invaderVideos;
    vector		<ofVideoPlayer*>	rightInvaderVideos;
    vector		<Invader>  leftInvaders1;
    vector		<Invader>  leftInvaders2;
    vector		<Invader>  leftInvaders3;
    vector		<Invader>  leftInvaders4;
    vector		<Invader>  leftInvaders5;
    vector		< Invader>  rightInvaders1;
    vector		<Invader>  rightInvaders2;
    vector		<Invader>  rightInvaders3;
    vector		<Invader>  rightInvaders4;
    vector		<Invader>  rightInvaders5;
    ofTrueTypeFont	visitor82;
    ofTrueTypeFont	visitor42;
    ofTrueTypeFont	visitor200;
    ofTrueTypeFont	visitor150;
    ofImage vimeologo, backgroundimg, instructions1, instructions2, logo;
    int score1, score2, counter, count, joystick1, joystick2, mapped_joystick1, mapped_joystick2, numInvaders, columns, rows, yincrement, xLincrement, xRincrement, xlimit, xmin, paddlewidth, bulletcounter, paddleopacity, lbullettimer, bulletinterval, rbullettimer, instructiontimer, instructionsmove ;
    ofImage user1, user2;
    ofxOscSender sender;
    ofxOscReceiver	receiver;
    int port, sendport;
    char * host;
    char * sendhost;
    string user, username, username1, username2, user1photo, user2photo;
    bool loaduser, user1load, user2load, moveRightDown, moveLeftDown, stopGame, instructions;
    int whichuser, countdownnum, alpha, alphaincrement;
    int paddleattraction;
    b2Vec2 pveloc;
    bool fullscreen,  startGameBool, drawGame, player1win, player2win, player2lose, player1lose, startScreen,  countdown, countdownnumbool, drawusers, login, senduser1, senduser2,;
};

