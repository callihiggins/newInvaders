#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetVerticalSync(true);
	ofBackgroundHex(0xfdefc2);
    ofEnableAlphaBlending();
    ofTrueTypeFont::setGlobalDpi(72);
    ofSetFrameRate(60);
    ofHideCursor();
    loaduser = false;
    user1load = false;
    user2load = false;
    player1win = false;
    player2win = false;
    senduser1 = false;
    senduser2 = false;
    player1lose = false;
    player2lose = false;
    instructions = false;
    whichuser = 0;
    drawGame = false;
    stopGame = false;
    lbullettimer = 0;
    rbullettimer = 0;
    instructionsmove = 1000;
    bulletinterval = 200;
    loaduser = false;
    drawusers = false;
    countdownnum = 400;
    countdownnumbool = false;
    startScreen = true;
    startGameBool = false;
    login = false;
    alpha = 125;
    alphaincrement = 1;
    moveRightDown = false;
    moveLeftDown = false;
    visitor82.loadFont("visitor1.ttf", 82, true, true);
	visitor82.setLineHeight(18.0f);
	visitor82.setLetterSpacing(1.037);
    visitor42.loadFont("visitor1.ttf", 38, true, true);
	visitor42.setLineHeight(18.0f);
	visitor42.setLetterSpacing(1.037);
    visitor200.loadFont("visitor1.ttf", 200, true, true);
	visitor200.setLineHeight(18.0f);
	visitor200.setLetterSpacing(1.037);
    visitor150.loadFont("visitor1.ttf", 150, true, true);
	visitor150.setLineHeight(18.0f);
	visitor150.setLetterSpacing(1.037);
    loadSettings("settings.xml");
    receiver.setup( port );
    sender.setup( sendhost, sendport );
	box2d.init();
  	box2d.setGravity(0, 10);
	//box2d.createGround();
	box2d.setFPS(30.0);
    bounds.set(40, 77, 1841, 920);
    box2d.createBounds(bounds);
    
    counter = 0;
    count = 0;
    paddlewidth = bounds.x + bounds.width/50;
	// register the listener so that we get the events
	ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &testApp::contactEnd);
    
    
    
    // load the 8 sfx soundfile
	for (int i=0; i<6; i++) {
		sound[i].loadSound("sfx/" +ofToString(i) + ".wav");
		sound[i].setMultiPlay(true);
		sound[i].setLoop(false);
	}
    
    vimeologo.loadImage("images/vimeo_logo.png");
    backgroundimg.loadImage("images/vimearcadeR.png");
    instructions1.loadImage("images/instructions1.png");
    instructions2.loadImage("images/instructions2.png");
    logo.loadImage("images/logo.png");
    
    //right now i'm making two vectors for each side of movie. if running slowly, can put this into one vector and do a test if both shapes have been deleted before stopping and deleting the movie
    for(int i=0; i< 13; i++){
        ofVideoPlayer * v = new ofVideoPlayer();
        v->loadMovie("movies/winner" + ofToString(i) +".mov");
        v->play();
        invaderVideos.push_back(v);
    }
    //    for(int i=0; i< 13; i++){
    //        ofVideoPlayer * v = new ofVideoPlayer();
    //        v->loadMovie("movies/fingers" + ofToString(i) +".mov");
    //        v->play();
    //        rightInvaderVideos.push_back(v);
    //    }
    rows = 4;
    columns = 4;
    yincrement = 25;
    xLincrement = 25;
    xRincrement = 25;
    xlimit = bounds.x + bounds.width - 50;
    xmin = bounds.x + 65;
    
}

//--------------------------------------------------------------
void testApp::contactStart(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        
        // if we collide with the ground we do not
        // want to play a sound. this is how you do that
        if((e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_polygon) || (e.a->GetType() == b2Shape::e_polygon && e.b->GetType() == b2Shape::e_circle )) {
            
            Data * aData = (Data*)e.a->GetBody()->GetUserData();
            Data * bData = (Data*)e.b->GetBody()->GetUserData();
            
            if(aData){
                if((aData->type == 0 && bData->type == 1)|| (bData->type == 0 && aData->type == 1)) {
                    // sound[aData->soundID].play();
                    //apply a force back at the ball when it hits a paddle
                    aData->hit = true;
                    bData->hit = true;
                    printf("paddle collision!");
                }
                
                if(aData->type == 0 && bData->type == 2){
                    aData->hit = true;
                    bData->hit = true;
                    printf("collision!");
                }
                
                if(bData->type == 0 && aData->type == 2){
                    aData->hit = true;
                    bData->hit = true;
                    printf("collision!");
                    
                    
                }
            }
            
        }
    }
    
}


//--------------------------------------------------------------
void testApp::contactEnd(ofxBox2dContactArgs &e) {
}



//--------------------------------------------------------------
void testApp::update() {
	
	box2d.update();
    
    //OSC STUFF
    while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
		if ( m.getAddress() == "/joysticks" )
		{
			// both the arguments are int32's
			joystick1 = m.getArgAsInt32( 0 );
            joystick2 = m.getArgAsInt32( 1 );
        }
        if ( m.getAddress() == "/user" && login)
		{
            user = m.getArgAsString( 0 );
            username = m.getArgAsString( 1 );
            printf("got a user!");
            loaduser = true;
        }
        
        if ( m.getAddress() == "/leftshoot")
		{
            if(!startScreen && login){
                user = "images/guest.jpg";
                username = "Guest";
                loaduser = true;
            }
            if(startScreen){
                startScreen = false;
                instructions = true;
                break;
            }
            if(instructions){
                instructions = false; 
                login = true;
                instructionsmove = 1000;
            }

            
            
            if(drawGame){   
                sound[5].play();
                ofxBox2dCircle  c1;
                c1.setPhysics(0.1, 1.0, 0.1);
                c1.setup(box2d.getWorld(), players[0].getPosition().x, players[0].getPosition().y - players[0].getHeight()*2, 5);
                c1.setVelocity(0, -50);
                c1.setData(new Data());
                Data * sd1 = (Data*)c1.getData();
                sd1->hit	= false;		
                sd1->type = 0;
                leftbullets.push_back(c1);
                lbullettimer = 0;
            }
        }
        if ( m.getAddress() == "/rightshoot") {
            
            if(!startScreen && login){
               
                user = "images/guest.jpg";
                username = "Guest";
                loaduser = true;
            }
            if(startScreen){
                startScreen = false;
                //  login = true;
                instructions = true;
                break;
            }
            if(instructions){
                instructions = false; 
                login = true;
                instructionsmove = 1000;
            }
            
            if(drawGame){
                sound[5].play();
                ofxBox2dCircle  c2;
                c2.setPhysics(0.1, 1.0, 0.1);
                c2.setup(box2d.getWorld(), players[1].getPosition().x, players[1].getPosition().y - players[1].getHeight()*2, 5);
                c2.setVelocity(0, -50);
                c2.setData(new Data());
                Data * sd2 = (Data*)c2.getData();
                sd2->hit	= false;		
                sd2->type = 0;
                rightbullets.push_back(c2);
                rbullettimer = 0;
            }
        }
        
        if ( m.getAddress() == "/start") {
            startScreen = false;
            login = true;
        }
        
    }
    
    
    if(loaduser && login && whichuser == 0){
        sound[0].play();
        user1photo = user;
        user1.loadImage(user);
        username1 = username;
        whichuser = 1;
        user1load = true;
        loaduser = false;
        
    }
    
    if(loaduser && login && whichuser == 1){
        sound[0].play();
        user2photo = user;
        user2.loadImage(user);
        username2 = username;
        whichuser = 0;
        user2load = true;
        loaduser = false;
    }
    
    if(senduser1 ){
        ofxOscMessage m;
        m.setAddress( "/user" );
        m.addStringArg( user1photo );
        sender.sendMessage( m );
        senduser1 = false;            
    }
    if(senduser2){
        ofxOscMessage m;
        m.setAddress( "/user" );
        m.addStringArg( user2photo );
        sender.sendMessage( m );
        senduser2 = false;            
    }
    
    alpha  = alpha + alphaincrement;
    if(alpha > 200 || alpha < 100){
        alphaincrement = alphaincrement * -1;
    }
    
    if(countdownnumbool){
        printf("counting down: %d \n", countdownnum);
        countdownnum = countdownnum -1;
    }
    
    if(startGameBool){
        
        for(int i=0; i<invaderVideos.size(); i++){ 
            invaderVideos[i]->idleMovie();
        }
        //    for(int i=0; i<rightInvaderVideos.size(); i++){ 
        //        rightInvaderVideos[i]->idleMovie();
        //    }
        mapped_joystick1 = int(ofMap(joystick1, 0, 360, bounds.x + 20, bounds.x - 20 + bounds.width/2));
        mapped_joystick2 = int(ofMap(joystick2, 0, 360, bounds.x + 20 + bounds.width/2, bounds.x - 20 + bounds.width));
        
        
        
        b2Vec2 pos1 =  players[0].body->GetPosition();
        // b2Vec2 target1 = b2Vec2(mouseX/OFX_BOX2D_SCALE, pos1.y );
        b2Vec2 target1 = b2Vec2(mapped_joystick1/OFX_BOX2D_SCALE, pos1.y );
        target1.x = ofClamp(target1.x, bounds.x/OFX_BOX2D_SCALE, (bounds.x + (bounds.width/2))/OFX_BOX2D_SCALE - (paddlewidth/OFX_BOX2D_SCALE));
        b2Vec2 diff1 = b2Vec2(target1.x-pos1.x,target1.y-pos1.y);
        diff1.operator*=(2);
        players[0].body->SetLinearVelocity(diff1);
        
        b2Vec2 pos2 =  players[1].body->GetPosition();
        // b2Vec2 target2 = b2Vec2(mouseX/OFX_BOX2D_SCALE, pos2.y );
        b2Vec2 target2 = b2Vec2(mapped_joystick2/OFX_BOX2D_SCALE, pos2.y );
        target2.x = ofClamp(target2.x, (bounds.x + bounds.width/2)/OFX_BOX2D_SCALE + (paddlewidth/OFX_BOX2D_SCALE),(bounds.x + bounds.width)/OFX_BOX2D_SCALE );
        b2Vec2 diff2 = b2Vec2(target2.x-pos2.x,target2.y-pos2.y);
        diff2.operator*=(2);
        players[1].body->SetLinearVelocity(diff2);
        
        
        //SHIFT EVERYONE OVER 
        
        if(count%30 == 1){
            for(int i=0; i<leftInvaders1.size(); i++){
                leftInvaders1[i].setPosition(leftInvaders1[i].getPosition().x + xLincrement, leftInvaders1[i].getPosition().y);
                leftInvaders1[i].update();
            }
            for(int i=0; i<leftInvaders2.size(); i++){
                leftInvaders2[i].setPosition(leftInvaders2[i].getPosition().x + xLincrement, leftInvaders2[i].getPosition().y);
                leftInvaders2[i].update();
            }
            for(int i=0; i<leftInvaders3.size(); i++){
                leftInvaders3[i].setPosition(leftInvaders3[i].getPosition().x + xLincrement, leftInvaders3[i].getPosition().y);
                leftInvaders3[i].update();
            }
            for(int i=0; i<leftInvaders4.size(); i++){
                leftInvaders4[i].setPosition(leftInvaders4[i].getPosition().x + xLincrement, leftInvaders4[i].getPosition().y);
                leftInvaders4[i].update();
            }
            for(int i=0; i<rightInvaders1.size(); i++){
                rightInvaders1[i].setPosition(rightInvaders1[i].getPosition().x + xRincrement, rightInvaders1[i].getPosition().y);
                rightInvaders1[i].update();
            }
            for(int i=0; i<rightInvaders2.size(); i++){
                rightInvaders2[i].setPosition(rightInvaders2[i].getPosition().x + xRincrement, rightInvaders2[i].getPosition().y);
                rightInvaders2[i].update();
                
            }
            for(int i=0; i<rightInvaders3.size(); i++){
                rightInvaders3[i].setPosition(rightInvaders3[i].getPosition().x + xRincrement, rightInvaders3[i].getPosition().y);
                rightInvaders3[i].update();
                
            }
            for(int i=0; i<rightInvaders4.size(); i++){
                rightInvaders4[i].setPosition(rightInvaders4[i].getPosition().x + xRincrement, rightInvaders4[i].getPosition().y);
                rightInvaders4[i].update();
            }
            
        }
        
        
        //TEST X POSITIONS FOR BOTH SIDES
        if(rightInvaders4.size() > 0){
            for(int i=0; i<rightInvaders4.size(); i++){
                if (rightInvaders4[i].getPosition().x >= xlimit){  
                    moveRightDown = true;
                }
            }
        }
        else if(rightInvaders4.size() < 1 && rightInvaders3.size() > 0 ){
            for(int i=0; i<rightInvaders3.size(); i++){
                if (rightInvaders3[i].getPosition().x >= xlimit ){
                    moveRightDown = true;
                } 
            }
        }
        else if(rightInvaders4.size() < 1 && rightInvaders3.size() < 1 && rightInvaders2.size() > 0){
            for(int i=0; i<rightInvaders2.size(); i++){
                if (rightInvaders2[i].getPosition().x >= xlimit){
                    moveRightDown = true;
                }
            }
        }
        else if(rightInvaders4.size() < 1 && rightInvaders3.size() < 1 && rightInvaders2.size() < 1 && rightInvaders1.size() > 0){
            for(int i=0; i<rightInvaders1.size(); i++){
                if (rightInvaders1[i].getPosition().x >= xlimit)  {
                    moveRightDown = true;
                } 
                
            }
            
        }
        
        if(rightInvaders1.size() > 0){
            for(int i=0; i<rightInvaders1.size(); i++){
                if (rightInvaders1[i].getPosition().x <= xmin + bounds.x + bounds.width/2){  
                    moveRightDown = true;
                }
            }
        }
        else if(rightInvaders1.size() < 1 && rightInvaders2.size() > 0 ){
            for(int i=0; i<rightInvaders2.size(); i++){
                if (rightInvaders2[i].getPosition().x <= xmin + bounds.x + bounds.width/2 ){
                    moveRightDown = true;
                } 
            }
        }
        else if(rightInvaders1.size() < 1 && rightInvaders2.size() < 1 && rightInvaders3.size() > 0){
            for(int i=0; i<rightInvaders3.size(); i++){
                if (rightInvaders3[i].getPosition().x <= xmin + bounds.x + bounds.width/2){
                    moveRightDown = true;
                }
            }
        }
        else if(rightInvaders1.size() < 1 && rightInvaders3.size() < 1 && rightInvaders2.size() < 1 && rightInvaders4.size() > 0){
            for(int i=0; i<rightInvaders4.size(); i++){
                if (rightInvaders4[i].getPosition().x <= xmin + bounds.x + bounds.width/2)  {
                    moveRightDown = true;
                } 
                
            }
            
        }
        
        
        if(leftInvaders1.size() > 0){
            for(int i=0; i<leftInvaders1.size(); i++){
                if (leftInvaders1[i].getPosition().x <= xmin) {
                    moveLeftDown = true;
                } 
            }
        }
        else if(leftInvaders2.size() > 0 && leftInvaders1.size() < 1){
            for(int i=0; i<leftInvaders2.size(); i++){
                if (leftInvaders2[i].getPosition().x <= xmin){
                    moveLeftDown = true;
                } 
            }
        }
        else if(leftInvaders2.size() < 1 && leftInvaders1.size() < 1 && leftInvaders3.size() > 0){
            for(int i=0; i<leftInvaders3.size(); i++){
                if (leftInvaders3[i].getPosition().x <= xmin ) {
                    moveLeftDown = true;
                } 
                
            }
        }
        else if(leftInvaders2.size() < 1 && leftInvaders1.size() < 1 && leftInvaders3.size() < 1 && leftInvaders4.size() > 0){
            for(int i=0; i<leftInvaders4.size(); i++){
                if (leftInvaders4[i].getPosition().x <= xmin)  {
                    moveLeftDown = true;
                } 
                
            }
            
        }
        
        if(leftInvaders4.size() > 0){
            for(int i=0; i<leftInvaders4.size(); i++){
                if (leftInvaders4[i].getPosition().x >= xlimit - (bounds.x + bounds.width/2)) {
                    moveLeftDown = true;
                } 
            }
        }
        else if(leftInvaders3.size() > 0 && leftInvaders4.size() < 1){
            for(int i=0; i<leftInvaders3.size(); i++){
                if (leftInvaders3[i].getPosition().x >= xlimit - (bounds.x + bounds.width/2)){
                    moveLeftDown = true;
                } 
            }
        }
        else if(leftInvaders2.size() < 3 && leftInvaders1.size() < 4 && leftInvaders2.size() > 0){
            for(int i=0; i<leftInvaders2.size(); i++){
                if (leftInvaders2[i].getPosition().x >= xlimit - (bounds.x + bounds.width/2)) {
                    moveLeftDown = true;
                } 
            }
        }
        else if(leftInvaders2.size() < 1 && leftInvaders3.size() < 1 && leftInvaders4.size() < 1 && leftInvaders1.size() > 0){
            for(int i=0; i<leftInvaders1.size(); i++){
                if (leftInvaders1[i].getPosition().x >= xlimit - (bounds.x + bounds.width/2))  {
                    moveLeftDown = true;
                } 
            }
        }
        
        
        //SHIFT EVERYONE DOWN A ROW WHEN WE HIT THE EDGE
        if(moveLeftDown){
            printf("incremement!");
            for(int i=0; i<leftInvaders1.size(); i++){
                leftInvaders1[i].setPosition(leftInvaders1[i].getPosition().x + xLincrement*-1, leftInvaders1[i].getPosition().y + yincrement);
                leftInvaders1[i].update();
            }
            for(int i=0; i<leftInvaders2.size(); i++){
                leftInvaders2[i].setPosition(leftInvaders2[i].getPosition().x + xLincrement*-1, leftInvaders2[i].getPosition().y+ yincrement);
                leftInvaders2[i].update();
            }
            for(int i=0; i<leftInvaders3.size(); i++){
                leftInvaders3[i].setPosition(leftInvaders3[i].getPosition().x + xLincrement*-1, leftInvaders3[i].getPosition().y+ yincrement);
                leftInvaders3[i].update();
            }
            for(int i=0; i<leftInvaders4.size(); i++){
                leftInvaders4[i].setPosition(leftInvaders4[i].getPosition().x + xLincrement*-1, leftInvaders4[i].getPosition().y+ yincrement);
                leftInvaders4[i].update();
            }
            xLincrement = xLincrement * -1;
            moveLeftDown = false;    
        }
        if(moveRightDown){
            for(int i=0; i<rightInvaders1.size(); i++){
                rightInvaders1[i].setPosition(rightInvaders1[i].getPosition().x+ xRincrement*-1, rightInvaders1[i].getPosition().y+ yincrement);
                rightInvaders1[i].update();
            }
            for(int i=0; i<rightInvaders2.size(); i++){
                rightInvaders2[i].setPosition(rightInvaders2[i].getPosition().x + xRincrement*-1, rightInvaders2[i].getPosition().y+ yincrement);
                rightInvaders2[i].update();
                
            }
            for(int i=0; i<rightInvaders3.size(); i++){
                rightInvaders3[i].setPosition(rightInvaders3[i].getPosition().x + xRincrement*-1, rightInvaders3[i].getPosition().y+ yincrement);
                rightInvaders3[i].update();
                
            }
            for(int i=0; i<rightInvaders4.size(); i++){
                rightInvaders4[i].setPosition(rightInvaders4[i].getPosition().x + xRincrement*-1, rightInvaders4[i].getPosition().y+ yincrement);
                rightInvaders4[i].update();
            }
            xRincrement = xRincrement * -1;
            moveRightDown = false;
        }
        
        lbullettimer++;
        rbullettimer++;
        
        
        //INVADERS SEND BULLETS EVERY SO OFTEN
        if(int(ofRandom(0,bulletinterval))== 1 && leftInvaders1.size()>0){
            ofxBox2dCircle  c2;
            c2.setPhysics(0.1, 1.0, 0.1);
            c2.setup(box2d.getWorld(), leftInvaders1[leftInvaders1.size() - 1].getPosition().x, leftInvaders1[leftInvaders1.size() - 1].getPosition().y + leftInvaders1[3].getWidth()/2 + 40, 5);
            c2.setVelocity(0, 10);
            c2.setData(new Data());
            Data * sd2 = (Data*)c2.getData();
            sd2->hit	= false;		
            sd2->type = 0;
            invaderbullets.push_back(c2);
        }
        if(int(ofRandom(0,bulletinterval))== 1 && leftInvaders2.size()>0){
            ofxBox2dCircle  c2;
            c2.setPhysics(0.1, 1.0, 0.1);
            c2.setup(box2d.getWorld(), leftInvaders2[leftInvaders2.size() - 1].getPosition().x, leftInvaders2[leftInvaders2.size() - 1].getPosition().y + leftInvaders2[leftInvaders2.size() - 1].getWidth()/2 + 40, 5);
            c2.setVelocity(0, 10);
            c2.setData(new Data());
            Data * sd2 = (Data*)c2.getData();
            sd2->hit	= false;		
            sd2->type = 0;
            invaderbullets.push_back(c2);
        }
        if(int(ofRandom(0,bulletinterval))== 1  && leftInvaders3.size()>0){
            ofxBox2dCircle  c2;
            c2.setPhysics(0.1, 1.0, 0.1);
            c2.setup(box2d.getWorld(), leftInvaders3[leftInvaders3.size() - 1].getPosition().x, leftInvaders3[leftInvaders3.size() - 1].getPosition().y + leftInvaders3[leftInvaders3.size() - 1].getWidth()/2 + 40, 5);
            c2.setVelocity(0, 10);
            c2.setData(new Data());
            Data * sd2 = (Data*)c2.getData();
            sd2->hit	= false;		
            sd2->type = 0;
            invaderbullets.push_back(c2);
        }
        if(int(ofRandom(0,bulletinterval))== 1  && leftInvaders4.size()>0){
            ofxBox2dCircle  c2;
            c2.setPhysics(0.1, 1.0, 0.1);
            c2.setup(box2d.getWorld(), leftInvaders4[leftInvaders4.size() - 1].getPosition().x, leftInvaders4[leftInvaders4.size() - 1].getPosition().y + leftInvaders1[leftInvaders4.size() - 1].getWidth()/2 + 40, 5);
            c2.setVelocity(0, 10);
            c2.setData(new Data());
            Data * sd2 = (Data*)c2.getData();
            sd2->hit	= false;		
            sd2->type = 0;
            invaderbullets.push_back(c2);
        }
        if(int(ofRandom(0,bulletinterval))== 1  && rightInvaders1.size()>0){
            ofxBox2dCircle  c2;
            c2.setPhysics(0.1, 1.0, 0.1);
            c2.setup(box2d.getWorld(), rightInvaders1[rightInvaders1.size() - 1].getPosition().x, rightInvaders1[rightInvaders1.size() - 1].getPosition().y + rightInvaders1[rightInvaders1.size() - 1].getWidth()/2 + 40, 5);
            c2.setVelocity(0, 10);
            c2.setData(new Data());
            Data * sd2 = (Data*)c2.getData();
            sd2->hit	= false;		
            sd2->type = 0;
            invaderbullets.push_back(c2);
        }
        if(int(ofRandom(0,bulletinterval))== 1 && rightInvaders2.size()>0){
            ofxBox2dCircle  c2;
            c2.setPhysics(0.1, 1.0, 0.1);
            c2.setup(box2d.getWorld(), rightInvaders2[rightInvaders2.size() - 1].getPosition().x, rightInvaders2[rightInvaders2.size() - 1].getPosition().y + rightInvaders2[rightInvaders2.size() - 1].getWidth()/2 + 40, 5);
            c2.setVelocity(0, 10);
            c2.setData(new Data());
            Data * sd2 = (Data*)c2.getData();
            sd2->hit	= false;		
            sd2->type = 0;
            invaderbullets.push_back(c2);
        }
        if(int(ofRandom(0,bulletinterval))== 1 && rightInvaders3.size()>0){
            ofxBox2dCircle  c2;
            c2.setPhysics(0.1, 1.0, 0.1);
            c2.setup(box2d.getWorld(), rightInvaders3[rightInvaders3.size() - 1].getPosition().x, rightInvaders3[rightInvaders3.size() - 1].getPosition().y + rightInvaders3[rightInvaders3.size() - 1].getWidth()/2 + 40, 5);
            c2.setVelocity(0, 10);
            c2.setData(new Data());
            Data * sd2 = (Data*)c2.getData();
            sd2->hit	= false;		
            sd2->type = 0;
            invaderbullets.push_back(c2);
        }
        if(int(ofRandom(0,bulletinterval))== 1 && rightInvaders4.size()>0){
            ofxBox2dCircle  c2;
            c2.setPhysics(0.1, 1.0, 0.1);
            c2.setup(box2d.getWorld(), rightInvaders4[rightInvaders4.size() - 1].getPosition().x, rightInvaders4[rightInvaders4.size() - 1].getPosition().y + rightInvaders4[rightInvaders4.size() - 1].getWidth()/2 + 40, 5);
            c2.setVelocity(0, 10);
            c2.setData(new Data());
            Data * sd2 = (Data*)c2.getData();
            sd2->hit	= false;		
            sd2->type = 0;
            invaderbullets.push_back(c2);
        }
        
        
        //DELETE WHEN THERE'S A HIT
        
        for(int i=0; i<leftInvaders1.size(); i++){
            Data * theData = (Data*)leftInvaders1[i].getData();
            if(theData->hit == true){
                sound[1].play();
                theData->hit = false;  
                // leftInvaders1[i].movie->stop();
                //     delete leftInvaders1[i].movie;
                box2d.getWorld()->DestroyBody(leftInvaders1[i].body);
                //  leftInvaderVideos.erase(leftInvaderVideos.begin()+i);
                leftInvaders1.erase(leftInvaders1.begin()+i);  
            }
        }
        
        for(int i=0; i<leftInvaders2.size(); i++){
            Data * theData = (Data*)leftInvaders2[i].getData();
            if(theData->hit == true){
                 sound[1].play();
                theData->hit = false;  
                //   leftInvaders2[i].movie->stop();
                //  delete leftInvaders2[i].movie;
                box2d.getWorld()->DestroyBody(leftInvaders2[i].body);
                //   leftInvaderVideos.erase(leftInvaderVideos.begin()+i+rows);
                leftInvaders2.erase(leftInvaders2.begin()+i);  
            }
        }
        
        for(int i=0; i<leftInvaders3.size(); i++){
            Data * theData = (Data*)leftInvaders3[i].getData();
            if(theData->hit == true){
                 sound[1].play();
                theData->hit = false;  
                //     leftInvaders3[i].movie->stop();
                //      delete leftInvaders3[i].movie;
                box2d.getWorld()->DestroyBody(leftInvaders3[i].body);
                //       leftInvaderVideos.erase(leftInvaderVideos.begin()+i+rows*2);
                leftInvaders3.erase(leftInvaders3.begin()+i);  
            }
        }
        
        for(int i=0; i<leftInvaders4.size(); i++){
            Data * theData = (Data*)leftInvaders4[i].getData();
            if(theData->hit == true){
                 sound[1].play();
                theData->hit = false;  
                //        leftInvaders4[i].movie->stop();
                //   delete leftInvaders4[i].movie;
                box2d.getWorld()->DestroyBody(leftInvaders4[i].body);
                //  leftInvaderVideos.erase(leftInvaderVideos.begin()+i+rows*3);
                leftInvaders4.erase(leftInvaders4.begin()+i);  
            }
        }
        
        for(int i=0; i<rightInvaders1.size(); i++){
            Data * theData = (Data*)rightInvaders1[i].getData();
            if(theData->hit == true){
                 sound[1].play();
                theData->hit = false;  
                //    rightInvaders1[i].movie->stop();
                //   delete leftInvaders1[i].movie;
                box2d.getWorld()->DestroyBody(rightInvaders1[i].body);
                //     rightInvaderVideos.erase(rightInvaderVideos.begin()+i);
                rightInvaders1.erase(rightInvaders1.begin()+i);  
            }
        }
        
        for(int i=0; i<rightInvaders2.size(); i++){
            Data * theData = (Data*)rightInvaders2[i].getData();
            if(theData->hit == true){
                 sound[1].play();
                theData->hit = false;  
                //    rightInvaders2[i].movie->stop();
                //     delete leftInvaders2[i].movie;
                box2d.getWorld()->DestroyBody(rightInvaders2[i].body);
                //     rightInvaderVideos.erase(rightInvaderVideos.begin()+i+rows);
                rightInvaders2.erase(rightInvaders2.begin()+i);  
            }
        }
        
        for(int i=0; i<rightInvaders3.size(); i++){
            Data * theData = (Data*)rightInvaders3[i].getData();
            if(theData->hit == true){
                 sound[1].play();
                theData->hit = false;  
                //     rightInvaders3[i].movie->stop();
                //      delete rightInvaders3[i].movie;
                box2d.getWorld()->DestroyBody(rightInvaders3[i].body);
                //        rightInvaderVideos.erase(rightInvaderVideos.begin()+i+rows*2);
                rightInvaders3.erase(rightInvaders3.begin()+i);  
            }
        }
        
        for(int i=0; i<rightInvaders4.size(); i++){
            Data * theData = (Data*)rightInvaders4[i].getData();
            if(theData->hit == true){
                 sound[1].play();
                theData->hit = false;  
                //     rightInvaders4[i].movie->stop();
                //   delete rightInvaders4[i].movie;
                box2d.getWorld()->DestroyBody(rightInvaders4[i].body);
                //     rightInvaderVideos.erase(rightInvaderVideos.begin()+i+rows*3);
                rightInvaders4.erase(rightInvaders4.begin()+i);  
            }
        }
        
        for(int i=0; i<invaderbullets.size(); i++){
            Data * theData = (Data*)invaderbullets[i].getData();
            if(theData->hit == true|| invaderbullets[i].getPosition().y < bounds.y + 20 || invaderbullets[i].getPosition().y > bounds.y + bounds.height - 20 ){
                box2d.getWorld()->DestroyBody(invaderbullets[i].body);
                invaderbullets.erase(invaderbullets.begin()+i);  
                theData->hit = false;  
                invaderbullets[i].update();
            }
        }
        for(int i=0; i<leftbullets.size(); i++){
            Data * theData = (Data*)leftbullets[i].getData();
            if(theData->hit == true|| leftbullets[i].getPosition().y < bounds.y + 20 || leftbullets[i].getPosition().y > bounds.y + bounds.height - 20 ){
                box2d.getWorld()->DestroyBody(leftbullets[i].body);
                leftbullets.erase(leftbullets.begin()+i);  
                theData->hit = false;  
                leftbullets[i].update();
            }
        }
        for(int i=0; i<rightbullets.size(); i++){
            Data * theData = (Data*)rightbullets[i].getData();
            if(theData->hit == true|| rightbullets[i].getPosition().y < bounds.y + 20 || rightbullets[i].getPosition().y > bounds.y + bounds.height - 20 ){
                box2d.getWorld()->DestroyBody(rightbullets[i].body);
                rightbullets.erase(rightbullets.begin()+i);  
                theData->hit = false;  
                rightbullets[i].update();
            }
        }
        
        
        for(int i=0; i<players.size(); i++){
            Data * theData = (Data*)players[i].getData();
            if(theData->hit == true ){
                 sound[2].play();
                theData->hit = false;  
                theData->paddleopacity =  theData->paddleopacity - 85 ;
                
            }
            if(theData->paddleopacity == 0)
                theData->killed = true;
        }
        //IF YOU'VE BEEN HIT 3 TIMES
        Data * theData0 = (Data*)players[0].getData();
        if(theData0->killed == true){
            player2win = true;
            player1lose = true;
        }
        Data * theData1 = (Data*)players[1].getData();
        if(theData1->killed == true){
            player1win = true;
            player2lose = true;

        }
        //IF YOU KILL ARE YOUR INVADERS
        if(leftInvaders1.size() == 0 && leftInvaders2.size()==0 && leftInvaders3.size()==0 && leftInvaders4.size()==0){
            player1win = true;
            player1lose = true;
        }
        if(rightInvaders1.size() == 0 && rightInvaders2.size()==0 && rightInvaders3.size()==0 && rightInvaders4.size()==0){
            player2win = true;
            player2lose = true;
        }
        
        //IF THE INVADERS INVADE YOU
        if( rightInvaders1.size() > 0 ){
            if(rightInvaders1[rightInvaders1.size() - 1].getPosition().y + rightInvaders1[rightInvaders1.size() - 1].getHeight()  >= bounds.y + bounds.height-players[1].getHeight()){
                player1win = true;
                player2lose = true;

            }
        }
        if( rightInvaders2.size() > 0 ){
            if(rightInvaders2[rightInvaders2.size() - 1].getPosition().y + rightInvaders2[rightInvaders2.size() - 1].getHeight() >= bounds.y + bounds.height-players[1].getHeight()){
                player1win = true;
                player2lose = true;

            }
        }
        if( rightInvaders3.size() > 0 ){
            if(rightInvaders3[rightInvaders3.size() - 1].getPosition().y + rightInvaders3[rightInvaders3.size() - 1].getHeight() >= bounds.y + bounds.height-players[1].getHeight()){
                player1win = true;
                player2lose = true;

            }
        }
        if( rightInvaders4.size() > 0 ){
            if(rightInvaders4[rightInvaders4.size() - 1].getPosition().y + rightInvaders4[rightInvaders4.size() - 1].getHeight() >= bounds.y + bounds.height-players[1].getHeight()){
                player1win = true;
                player2lose = true;

            }
            
        }
        
        if( leftInvaders1.size() > 0 ){
            if(leftInvaders1[leftInvaders1.size() - 1].getPosition().y + leftInvaders1[leftInvaders1.size() - 1].getHeight()  >= bounds.y + bounds.height-players[0].getHeight()){
                player2win = true;
                player1lose = true;

            }
        }
        if( leftInvaders2.size() > 0 ){
            if(leftInvaders2[leftInvaders2.size() - 1].getPosition().y + leftInvaders2[leftInvaders2.size() - 1].getHeight()  >= bounds.y + bounds.height-players[0].getHeight()){
                player2win = true;
                player1lose = true;

            }
        }
        if( leftInvaders3.size() > 0 ){
            if(leftInvaders3[leftInvaders3.size() - 1].getPosition().y + leftInvaders3[leftInvaders3.size() - 1].getHeight() >= bounds.y + bounds.height-players[0].getHeight()){
                player2win = true;
                player1lose = true;

            }
        }
        if( leftInvaders4.size() > 0 ){
            if(leftInvaders4[leftInvaders4.size() - 1].getPosition().y + leftInvaders4[leftInvaders4.size() - 1].getHeight() >= bounds.y + bounds.height-players[0].getHeight()){
                player2win = true;
                player1lose = true;

            }
        }
        
        if(stopGame == true){
            for(int i = 0; i<leftInvaders1.size(); i++){
                box2d.world->DestroyBody(leftInvaders1[i].body);
                // delete leftInvaders1[i].movie;
            }
            for(int i = 0; i<leftInvaders2.size(); i++){
                box2d.world->DestroyBody(leftInvaders2[i].body);
                // delete leftInvaders2[i].movie;
            }
            for(int i = 0; i<leftInvaders3.size(); i++){
                box2d.world->DestroyBody(leftInvaders3[i].body);
                // delete leftInvaders3[i].movie;
            }
            for(int i = 0; i<leftInvaders4.size(); i++){
                box2d.world->DestroyBody(leftInvaders4[i].body);
                // delete leftInvaders4[i].movie;
            }
            for(int i = 0; i<rightInvaders1.size(); i++){
                box2d.world->DestroyBody(rightInvaders1[i].body);
                // delete rightInvaders1[i].movie;
            }
            for(int i = 0; i<rightInvaders2.size(); i++){
                box2d.world->DestroyBody(rightInvaders2[i].body);
                // delete rightInvaders2[i].movie;
            }
            for(int i = 0; i<rightInvaders3.size(); i++){
                box2d.world->DestroyBody(rightInvaders3[i].body);
                // delete rightInvaders3[i].movie;
            }
            for(int i = 0; i<rightInvaders4.size(); i++){
                box2d.world->DestroyBody(rightInvaders4[i].body);
                // delete rightInvaders4[i].movie;
            }
            for(int i = 0; i<invaderbullets.size(); i++){
                box2d.world->DestroyBody(invaderbullets[i].body);
            }
            for(int i = 0; i<rightbullets.size(); i++){
                box2d.world->DestroyBody(rightbullets[i].body);
            }
            for(int i = 0; i<leftbullets.size(); i++){
                box2d.world->DestroyBody(leftbullets[i].body);
            }
            for(int i = 0; i<players.size(); i++){
                box2d.world->DestroyBody(players[i].body);
                // delete rightInvaders4[i].movie;
            }    
            drawusers = false;
            startGameBool = false;
            leftInvaders1.clear();
            leftInvaders2.clear();
            leftInvaders3.clear();
            leftInvaders4.clear();
            rightInvaders1.clear();
            rightInvaders2.clear();
            rightInvaders3.clear();
            rightInvaders4.clear();
            players.clear();
            invaderbullets.clear();
            rightbullets.clear();
            leftbullets.clear();
        }
        
        
        count++;
    }
    if(counter>200){
        startScreen = true;    
        player1win = false;
        player2win = false;
        player2lose = false;
        player1lose = false;
        moveRightDown = false;
        moveLeftDown = false;
        count = 0;
        counter = 0;
        stopGame = false;
        yincrement = 25;
        xLincrement = 25;
        xRincrement = 25;

    }
    
    
}

//--------------------------------------------------------------
void testApp::draw() {
    ofBackground(0); 
    ofSetColor(255);
    ofSetRectMode(OF_RECTMODE_CORNER);
    backgroundimg.draw(0,0, ofGetWidth(), ofGetHeight());
    
    if(startScreen){
        ofSetColor(255);
        ofSetRectMode(OF_RECTMODE_CENTER);
        logo.draw(bounds.x + bounds.width/2, bounds.y + bounds.height/2 );
        ofSetColor(255,221,21);
       
    }
    
    if(instructions){
        instructionsmove--;
        ofSetColor(255);
        if (instructionsmove > 700) {
            ofSetRectMode(OF_RECTMODE_CENTER);
            instructions1.draw(bounds.x + bounds.width/2, bounds.y + bounds.height/2);
        }
        if (instructionsmove < 700 && instructionsmove > 300) {
            ofSetRectMode(OF_RECTMODE_CENTER);
            instructions2.draw(bounds.x + bounds.width/2, bounds.y + bounds.height/2);
        }
        if (instructionsmove < 300 ) {
            login = true; 
            instructions = false;
            instructionsmove = 1000;
        }
    }        
    
    
    
if(!user1load && login && !startScreen){
    //    printf("alpha: %d: \n", alpha);
    ofSetColor(235,0, 139, alpha);
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofRect(bounds.x,bounds.y + 82, bounds.width/2, bounds.height - 82);
    ofSetColor(255);
    visitor82.drawString("Left Player", bounds.x + bounds.width/4 - visitor82.stringWidth("Left Player")/2, bounds.y + bounds.height/2 - 150);
    visitor82.drawString("Tap Your Pass", bounds.x + bounds.width/4 - visitor82.stringWidth("Tap Your Pass!")/2, bounds.y + bounds.height/2 -75);
    
    visitor42.drawString("Or press the button to play as a guest", bounds.x + 30, bounds.y + bounds.height/2 + 50);
}

if(user1load && !user2load && login){
     ofSetColor(235,0, 139, alpha);
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofRect(bounds.x + bounds.width/2, bounds.y + 82, bounds.width/2, bounds.height - 82);
    ofSetColor(255);
    user1.draw(bounds.x + bounds.width/4 - 50, bounds.height/2, 100, 100);
    visitor82.drawString(username1, bounds.x + bounds.width/4 - visitor82.stringWidth(username1)/2, bounds.y + bounds.height/2 + 120);
    ofSetColor(255);
    visitor82.drawString("Right Player", bounds.x + bounds.width/2 + bounds.width/4 - visitor82.stringWidth("Right Player")/2, bounds.y + bounds.height/2 - 150);
    visitor82.drawString("Tap Your Pass", bounds.x + bounds.width/2 + bounds.width/4 - visitor82.stringWidth("Tap Your Pass")/2, bounds.y + bounds.height/2 - 75);
    visitor42.drawString("Or press the button to play as a guest", bounds.x + 30 + bounds.width/2, bounds.y + bounds.height/2 + 50);
    
}
    
    if(user1load &&  user2load){
        countdownnumbool = true;
        ofSetColor(255);
        user1.draw(bounds.x + bounds.width/4 - 50, bounds.height/2, 100, 100);
        user2.draw(bounds.x + bounds.width/4 - 50 + bounds.width/2, bounds.height/2, 100, 100);
        visitor82.drawString(username1, bounds.x + bounds.width/4 - visitor82.stringWidth(username1)/2, bounds.y + bounds.height/2 + 120);
        visitor82.drawString(username2, bounds.x + bounds.width/4 - visitor82.stringWidth(username2)/2 + bounds.width/2, bounds.y + bounds.height/2 + 120);
        if(countdownnum == 375)
            sound[4].play();
        if(countdownnum == 300)
            sound[4].play();
        if(countdownnum > 300){
            visitor200.drawString("3", bounds.x + 10 + bounds.width/2- visitor200.stringWidth("3")/2, bounds.y + bounds.height/2 + visitor200.stringHeight("3")/2  + 35);
        }
        if(countdownnum == 200)
            sound[4].play();
        if(countdownnum > 200 &&countdownnum < 300 ){
            visitor200.drawString("2", bounds.x + 10 + bounds.width/2 - visitor200.stringWidth("2")/2, bounds.y + bounds.height/2 + visitor200.stringHeight("2")/2 + 35);
        }
        if(countdownnum == 100)
            sound[4].play();
        if(countdownnum > 100 && countdownnum < 200){
            visitor200.drawString("1", bounds.x + + 25 + bounds.width/2 - visitor200.stringWidth("1!")/2, bounds.y + bounds.height/2 + visitor200.stringHeight("1")/2 + 35);
        }
        if(countdownnum > 0 && countdownnum < 100){
            visitor200.drawString("GO!", bounds.x + 15 + bounds.width/2 - visitor200.stringWidth("GO!")/2, bounds.y + bounds.height/2 + visitor200.stringHeight("GO!")/2 + 30);
        }
        if (countdownnum == 0 && countdownnumbool){
            countdownnumbool = false;
            countdownnum = 400;
            startGame();
            drawusers = true;
            login = false;
            user1load = false;
            user2load = false;
        }
        
    }
    
    if(drawGame){
        
             
        
        //DRAW THE INVADERS
        
        for(int i=0; i<leftInvaders1.size(); i++){
            leftInvaders1[i].draw();
        }
        for(int i=0; i<leftInvaders2.size(); i++){
            leftInvaders2[i].draw();
        }
        for(int i=0; i<leftInvaders3.size(); i++){
            leftInvaders3[i].draw();
        }
        for(int i=0; i<leftInvaders4.size(); i++){
            leftInvaders4[i].draw();
        }
        for(int i=0; i<rightInvaders1.size(); i++){
            rightInvaders1[i].draw();
        }
        for(int i=0; i<rightInvaders2.size(); i++){
            rightInvaders2[i].draw();
        }
        for(int i=0; i<rightInvaders3.size(); i++){
            rightInvaders3[i].draw();
        }
        for(int i=0; i<rightInvaders4.size(); i++){
            rightInvaders4[i].draw();
        }
        for(int i = 0; i<invaderbullets.size(); i++){
            ofSetColor(255);
            vimeologo.draw(invaderbullets[i].getPosition().x, invaderbullets[i].getPosition().y, 20,20);
        }
        for(int i = 0; i<leftbullets.size(); i++){
            ofSetColor(255);
            user1.draw(leftbullets[i].getPosition().x, leftbullets[i].getPosition().y, 20,20);
        }
        for(int i = 0; i<rightbullets.size(); i++){
            ofSetColor(255);
            user2.draw(rightbullets[i].getPosition().x, rightbullets[i].getPosition().y, 20,20);
        }
        
    }
    if(player1win && !player1lose){
        ofSetColor(255,221,21);
        visitor150.drawString("YOU WIN!", bounds.x + bounds.width/4 - visitor150.stringWidth("YOU WIN!")/2, bounds.y + bounds.height/2 - 120);
        visitor150.drawString("YOU LOSE!", bounds.x + bounds.width/4 - visitor150.stringWidth("YOU LOSE!")/2 + bounds.width/2, bounds.y + bounds.height/2 - 120);        
        ofSetColor(255);
        ofSetRectMode(OF_RECTMODE_CORNER);
        user1.draw(bounds.x + bounds.width/4 - 50,  bounds.height/2 + 20, 100, 100);
        user2.draw(bounds.x + bounds.width/4 - 50 + bounds.width/2,  bounds.height/2 + 20, 100, 100);
        ofSetColor(255,221,21);
        visitor82.drawString(username1, bounds.x + bounds.width/4 - visitor82.stringWidth(username1)/2, bounds.y + bounds.height/2 + 120);
        visitor82.drawString(username2, bounds.x + bounds.width/4 - visitor82.stringWidth(username2)/2 + bounds.width/2, bounds.y + bounds.height/2 + 120);
        
        stopGame = true;
        counter++;
        printf("counter: %d\n", counter);
        drawGame = false;
        
    }
    if(player2win && !player2lose && counter ==10){
        senduser2 = true;
        sound[3].play();
    }
    
    if(player1win && !player1lose && counter ==10){
        senduser1 = true;
        sound[3].play();
    }
    if(player2win && !player2lose){
        ofSetColor(255,221,21);
        visitor150.drawString("YOU LOSE!", bounds.x + bounds.width/4 - visitor150.stringWidth("YOU LOSE!")/2, bounds.y + bounds.height/2 - 120);
        visitor150.drawString("YOU WIN!", bounds.x + bounds.width/4 - visitor150.stringWidth("YOU WIN!")/2 + bounds.width/2, bounds.y + bounds.height/2 - 120);        
        ofSetColor(255);
        ofSetRectMode(OF_RECTMODE_CORNER);
        user1.draw(bounds.x + bounds.width/4 - 50,  bounds.height/2 + 20, 100, 100);
        user2.draw(bounds.x + bounds.width/4 - 50 + bounds.width/2,  bounds.height/2 + 20, 100, 100);
        ofSetColor(255,221,21);
        visitor82.drawString(username1, bounds.x + bounds.width/4 - visitor82.stringWidth(username1)/2, bounds.y + bounds.height/2 + 120);
        visitor82.drawString(username2, bounds.x + bounds.width/4 - visitor82.stringWidth(username2)/2 + bounds.width/2, bounds.y + bounds.height/2 + 120);
        
        stopGame = true;
        counter++;
        printf("counter: %d\n", counter);
        drawGame = false;
               
    }
    
    if(player1lose && player2lose){
        ofSetColor(255,221,21);
        visitor150.drawString("YOU LOSE!", bounds.x + bounds.width/4 - visitor150.stringWidth("YOU LOSE!")/2, bounds.y + bounds.height/2 - 140);
        visitor150.drawString("YOU LOSE!", bounds.x + bounds.width/4 - visitor150.stringWidth("YOU LOSE!")/2 + bounds.width/2, bounds.y + bounds.height/2 - 140);        
        ofSetColor(255);
        ofSetRectMode(OF_RECTMODE_CORNER);
        user1.draw(bounds.x + bounds.width/4 - 50,  bounds.height/2 + 20, 100, 100);
        user2.draw(bounds.x + bounds.width/4 - 50 + bounds.width/2,  bounds.height/2 + 20, 100, 100);
        ofSetColor(255,221,21);
        visitor82.drawString(username1, bounds.x + bounds.width/4 - visitor82.stringWidth(username1)/2, bounds.y + bounds.height/2 + 120);
        visitor82.drawString(username2, bounds.x + bounds.width/4 - visitor82.stringWidth(username2)/2 + bounds.width/2, bounds.y + bounds.height/2 + 120);
        
        stopGame = true;
        counter++;
        printf("counter: %d\n", counter);
        drawGame = false;
    }
    
    
    ofSetColor(255, 255, 255);
    
    
    //DRAW PROFILE PHOTOS AS PLAYERS
    if(drawusers){
        Data * data1 = (Data*)players[0].getData();
        ofSetColor(255,255,255, data1->paddleopacity);
        user1.draw(players[0].getPosition().x, players[0].getPosition().y - 110, 40, 40);
        ofSetColor(255,221,21, data1->paddleopacity);
        ofRect(players[0].getPosition().x, players[0].getPosition().y - 50, 80, 80);
        ofSetColor(255,221,21);
        visitor82.drawString("LIVES ",bounds.x+20, bounds.y +65);
        ofSetColor(255);
        if(data1->paddleopacity == 255){
            user1.draw(bounds.x + visitor82.stringWidth("LIVES ") + 180, bounds.y+45, 40,40);    
            user1.draw(bounds.x + visitor82.stringWidth("LIVES ") + 130, bounds.y+45, 40,40);                
            user1.draw(bounds.x + visitor82.stringWidth("LIVES ") + 80, bounds.y+45, 40,40);    
        }
        
        if(data1->paddleopacity == 170){
            user1.draw(bounds.x + visitor82.stringWidth("LIVES ") + 80, bounds.y+45, 40,40);    
            user1.draw(bounds.x + visitor82.stringWidth("LIVES ") + 130, bounds.y+45, 40,40);
        }
        if(data1->paddleopacity == 85){
            user1.draw(bounds.x + visitor82.stringWidth("LIVES ") + 80, bounds.y+45, 40,40);
        }
        
        Data * data2 = (Data*)players[1].getData();
        ofSetColor(255,255,255, data2->paddleopacity);
        user2.draw(players[1].getPosition().x, players[1].getPosition().y - 110, 40, 40);
        ofSetColor(255,221,21, data2->paddleopacity);
        ofRect(players[1].getPosition().x, players[1].getPosition().y - 50, 80, 80);
        ofSetColor(255,221,21);
        visitor82.drawString("LIVES ", bounds.x + bounds.width/2 + 20, bounds.y +65);
        ofSetColor(255);
        if(data2->paddleopacity == 255){
            user2.draw(bounds.x + visitor82.stringWidth("LIVES ") + 180 + bounds.width/2, bounds.y+45, 40,40);
            user2.draw(bounds.x + visitor82.stringWidth("LIVES ") + 80 + bounds.width/2, bounds.y+45, 40,40);
            user2.draw(bounds.x + visitor82.stringWidth("LIVES ") + 130 + bounds.width/2, bounds.y+45, 40,40);            }
        
        if(data2->paddleopacity == 170){
            user2.draw(bounds.x + visitor82.stringWidth("LIVES ") + 80 + bounds.width/2, bounds.y+45, 40,40);
            user2.draw(bounds.x + visitor82.stringWidth("LIVES ") + 130 + bounds.width/2, bounds.y+45, 40,40);                        }
        if(data2->paddleopacity == 85){
            user2.draw(bounds.x + visitor82.stringWidth("LIVES ") +80 + bounds.width/2, bounds.y+45, 40,40);
        }
        
        
        
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
    
    /*  if ((key == 'L' || key == 'l') && lbullettimer>50){
     ofxBox2dCircle  c1;
     c1.setPhysics(0.1, 1.0, 0.1);
     c1.setup(box2d.getWorld(), players[0].getPosition().x, players[0].getPosition().y - players[0].getHeight()*2, 5);
     c1.setVelocity(0, -50);
     c1.setData(new Data());
     Data * sd1 = (Data*)c1.getData();
     sd1->soundID = ofRandom(0, N_SOUNDS);
     sd1->hit	= false;		
     sd1->type = 0;
     leftbullets.push_back(c1);
     lbullettimer = 0;
     }
     if ((key == 'R' || key == 'r') && rbullettimer>50 ){
     ofxBox2dCircle  c2;
     c2.setPhysics(0.1, 1.0, 0.1);
     c2.setup(box2d.getWorld(), players[1].getPosition().x, players[1].getPosition().y - players[1].getHeight()*2, 5);
     c2.setVelocity(0, -50);
     c2.setData(new Data());
     Data * sd2 = (Data*)c2.getData();
     sd2->soundID = ofRandom(0, N_SOUNDS);
     sd2->hit	= false;		
     sd2->type = 0;
     rightbullets.push_back(c2);
     rbullettimer = 0;
     }
     
     if(key == 's' && startScreen == true){
     startScreen = false;
     login = true;
     
     }*/
    
       
}

void testApp::startGame(){
    count = 0;
    startGameBool = true;
    drawGame = true;
    ofxBox2dRect player1;
    player1.setPhysics(0.1, 1.0, 0.0);
    player1.setup(box2d.getWorld(), bounds.x, bounds.y + bounds.height-10, paddlewidth, paddlewidth, b2_kinematicBody);
    player1.setData(new Data());
    //  paddle1.body->SetUserData(paddle1);
    Data * sd1 = (Data*)player1.getData();
    sd1->hit	= false;		
    sd1-> paddleopacity = 255;
    sd1-> killed = false;;
    sd1->type = 1;
    players.push_back(player1);	
    
    ofxBox2dRect player2;
    player2.setPhysics(0.1, 1.0, 0.0);
    player2.setup(box2d.getWorld(), bounds.x + bounds.width, bounds.y + bounds.height-10, paddlewidth, paddlewidth, b2_kinematicBody);
    //   paddle2.body->SetUserData(paddle2);
    player2.setData(new Data());
    Data * sd2 = (Data*)player2.getData();
    sd2->hit	= false;
    sd2->killed = false;
	sd2-> paddleopacity = 255;
	sd2->type = 1;
    players.push_back(player2);	
    
    for(int i=0; i < 4; i++){
        Invader v;
        v.setPhysics(1.0, 0.5, 0.3);
        v.setup(box2d.getWorld(), float(((bounds.x + bounds.width/2 - 60)/columns)+ (bounds.x + bounds.width/2)), float(((bounds.y + bounds.height/3)/(rows))*i + bounds.y+110), 40,40, b2_staticBody);
        v.setupTheCustomData();
        v.movie = invaderVideos[ofRandom(0,12)];
        rightInvaders1.push_back(v);
    }
    
    for(int i=0; i < rows; i++){
        Invader v;
        v.setPhysics(1.0, 0.5, 0.3);
        v.setup(box2d.getWorld(), float(((bounds.x + bounds.width/2 - 60)/columns)*2 + (bounds.x + bounds.width/2)), float(((bounds.y + bounds.height/3)/(rows))*i + bounds.y+110), 40,40, b2_staticBody);
        v.setupTheCustomData();
        v.movie = invaderVideos[ofRandom(0,12)];
        rightInvaders2.push_back(v);
    }
    
    for(int i=0; i < rows; i++){
        Invader v;
        v.setPhysics(1.0, 0.5, 0.3);
        v.setup(box2d.getWorld(), float(((bounds.x + bounds.width/2 - 60)/columns)*3 + (bounds.x + bounds.width/2 )), float(((bounds.y + bounds.height/3)/(rows))*i + bounds.y+110), 40,40, b2_staticBody);
        v.setupTheCustomData();
        v.movie = invaderVideos[ofRandom(0,12)];
        rightInvaders3.push_back(v);
    }
    
    
    for(int i=0; i < rows; i++){
        Invader v;
        v.setPhysics(1.0, 0.5, 0.3);
        v.setup(box2d.getWorld(), float(((bounds.x + bounds.width/2 - 60)/columns)*4 + (bounds.x + bounds.width/2)), float(((bounds.y + bounds.height/3)/rows)*i + bounds.y+110), 40,40, b2_staticBody);
        v.setupTheCustomData();
        v.movie = invaderVideos[ofRandom(0,12)];
        rightInvaders4.push_back(v);
    }
    
    
    for(int i=0; i < rows; i++){
        Invader v;
        v.setPhysics(1.0, 0.5, 0.3);
        v.setup(box2d.getWorld(), float(((bounds.x + bounds.width/2 - 60)/columns)), float(((bounds.y + bounds.height/3)/rows)*i + bounds.y+110), 40,40, b2_staticBody);
        v.setupTheCustomData();
        v.movie = invaderVideos[ofRandom(0,12)];
        leftInvaders1.push_back(v);
    }
    
    for(int i=0; i < rows; i++){
        Invader v;
        v.setPhysics(1.0, 0.5, 0.3);
        v.setup(box2d.getWorld(), float(((bounds.x + bounds.width/2 - 60)/columns)*2), float(((bounds.y + bounds.height/3)/rows)*i + bounds.y+110), 40,40, b2_staticBody);
        v.setupTheCustomData();
        v.movie = invaderVideos[ofRandom(0,12)];
        leftInvaders2.push_back(v);
    }
    
    
    for(int i=0; i < rows; i++){
        Invader v;
        v.setPhysics(1.0, 0.5, 0.3);
        v.setup(box2d.getWorld(), float(((bounds.x + bounds.width/2 - 60)/columns)*3), float(((bounds.y + bounds.height/3)/rows)*i + bounds.y+110), 40,40, b2_staticBody);
        v.setupTheCustomData();
        v.movie = invaderVideos[ofRandom(0,12)];
        leftInvaders3.push_back(v);
    }
    
    
    for(int i=0; i < rows; i++){
        Invader v;
        v.setPhysics(1.0, 0.5, 0.3);
        v.setup(box2d.getWorld(), float(((bounds.x + bounds.width/2 - 60)/columns)*4), float(((bounds.y + bounds.height/3)/rows)*i + bounds.y+110), 40,40, b2_staticBody);
        v.setupTheCustomData();
        v.movie = invaderVideos[ofRandom(0,12)];
        leftInvaders4.push_back(v);
    }
    
    
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
    ofxOscMessage m;
    m.setAddress( "/user" );
    m.addStringArg( "http://b.vimeocdn.com/ps/215/215646_100.jpg" );
    sender.sendMessage( m );
  	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::resized(int w, int h){
}

void testApp::loadSettings(string fileString){
	string host_address;
    string send_host_address;
	string host_address1;
	string host_address2;
	string filename;
	
	//--------------------------------------------- get configs
    ofxXmlSettings xmlReader;
	bool result = xmlReader.loadFile(fileString);
	if(!result) printf("error loading xml file\n");
	
	
	host_address = xmlReader.getValue("settings:master:address","test",0);
	port = xmlReader.getValue("settings:master:port",5204,0);
	host = (char *) malloc(sizeof(char)*host_address.length());
    strcpy(host, host_address.c_str());
    
    send_host_address = xmlReader.getValue("settings:send:address","test",0);
    sendport = xmlReader.getValue("settings:send:port",5204,0);
	sendhost = (char *) malloc(sizeof(char)*host_address.length());
	strcpy(sendhost, send_host_address.c_str());
    
    
	filename = xmlReader.getValue("settings:movie:","test",0);
	
	
    int w = xmlReader.getValue("settings:dimensions:width", 640, 0);
	int h = xmlReader.getValue("settings:dimensions:height", 480, 0);
	
    ofSetWindowShape(w, h);
    
	
	if(xmlReader.getValue("settings:go_fullscreen", "false", 0).compare("true") == 0) {
		fullscreen = true;
		ofSetFullscreen(true);
	}
}

