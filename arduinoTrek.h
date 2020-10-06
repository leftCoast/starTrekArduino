#ifndef arduinoTrek_h
#define arduinoTrek_h

#include <SD.h>
#include <textBuff.h>
#include <lists.h>

#define COMBUFF_BYTES	80
#define REPLYBUFF_BYTES	750

// Our output buffer that the ported code will see.
extern textBuff*	trekComBuffer;

// Their reply buffer.
extern textBuff*	trekReplyBuffer;

// Hack to get at the "real" makemoves(). That one is "hidden". Dumb..
extern void doMakeMoves(void);

// Block 'till we get a char. I guess this is the waiting room.
extern int getch();

// And where it all goes out.. Well for now, we'll stuff it in here.
extern void proutn(char* s);

// Sometimes they jjust want a char sent out.
extern void proutCh(char c);

// Ok, they want this string to be printed s-l-o-w-l-y This'll do that.
extern void prouts(char* s);

// save game, start from saved..

class dirItem :	public linkListObj {

	public:
					dirItem(char* inName);
	virtual		~dirItem(void);
	
				char*	getName();
				
				char*	mName;
};


class dirList :	public linkList {

	public:
						dirList(void);
	virtual			~dirList(void);
	
						int readDir(char* path);
						char* getDirItem(int index);
};


extern bool openForSave(char* fullpath);

extern void writeData(char* data,int numBytes);

extern bool openForRead(char* fullPath);

extern void readData(char* data,int numBytes);

extern void closeFile(void);


// For debugging. Sends to serial monitor.
extern void out(char* str);		// No \n
extern void outln(char* str);		// Adds a \n

// It wants this.
#ifndef min
extern int min(int a, int b);
#endif

// Add to the end of your setup() function.
void arduinoTrekSetup(char* comList);

// Add to the beginning of your loop() function.
void arduinoTrekLoop(void);


#endif