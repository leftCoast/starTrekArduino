#ifndef arduinoTrek_h
#define arduinoTrek_h

#include "textBuff.h"


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

// For debugging. Sends to serial monitor.
extern void out(char* str);		// No \n
extern void outln(char* str);		// Adds a \n

// It wants this.
#ifndef min
extern int min(int a, int b);
#endif

// Add to the end of your setup() function.
void arduinoTrekSetup(void);

// Add to the beginning of your loop() function.
void arduinoTrekLoop(void);


#endif