#include "idlers.h"        // Sets up an idle loop so things can run in background.
#include "sst.h"           // Main definition file for the Star Trek game.
#include "textBuff.h"      // Ring buffer for text. Kind'a like streams. Lighter, just text.
#include "arduinoTrek.h"   // Glue code lives in here along with it's buddy arduinoTrek.cpp



// *****************************************************
//                      starTrekUpdater .h stuff..
// *****************************************************

class starTrekUpdater : public idler {

  public:
            starTrekUpdater(void);
  virtual   ~starTrekUpdater(void);

  virtual  void  idle(void);         
};



// *****************************************************
//                      starTrekUpdater .cpp stuff..
// *****************************************************


starTrekUpdater::starTrekUpdater(void) { hookup(); }


starTrekUpdater::~starTrekUpdater(void) {  }


// The game sits in getChar() forever looking for input. We use this background idle time to..
// First, find what the game has outputted and feed that to the screen.
// Then, see if the user has sent us anything. If so, feed that to the game.
void starTrekUpdater::idle(void) {

   char  aChar;
   
   while(!trekReplyBuffer->empty()) {              // While the reply buffer is not empty..
      Serial.print(trekReplyBuffer->readChar());   // Send each char to the serial monitor.
   }
   while(Serial.available()) {                     // While there are command chars available..
      aChar = Serial.read();                       // Grab each char.
      trekComBuffer->addChar(aChar);               // Send it to the game.
      Serial.print(aChar);                         // Echo it to the screen.
   }
}



// *****************************************************
//                      The sketch..
// *****************************************************
//
// A couple of notes.
// 
// You will see some gibberish on the screen it looks like : [2J
// This is the game using an escape sequence telling the display
// its being played on to clear the screen.
//
// You can use the freeze and thaw function to save and restore games.
// *IF* you..
// Setup this with a functional SD drive.
// Setup the SD card with a /starTrek/ directory.
// Successfully call SD.begin().
// Set SDDriveReady = true;
// 
// *****************************************************

starTrekUpdater* ourUpdater;

void setup(void) {

   ourUpdater = new starTrekUpdater(); // This is what actually runs everything through the monitor.
   arduinoTrekSetup("");               // The initialization bit for the game.
}

void loop(void) {

   idle();
   arduinoTrekLoop();                              // This blocks 'till the gane finishes.
   quickExit = false;                              // Quick exit is for "windowed" versions where close box is hit.
   prout((char*)"Do you want to play again?");     // Lie, loop forces you to play again and again forever!
   if (!ja()) {												// The game's call for yes/no.
      skip(2);                                     // the game's call for skipping(x) lines.
      prout((char*)"May the Great Bird of the Galaxy roost upon your home planet.");
      sleep(4000);                                 // Give you some time to read it.
   }
}