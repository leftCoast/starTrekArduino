# starTrekArduino
 A port of the old 1960s..1970s? Text Star Trek game to an Arduino based handheld.
 There is an issue here. Arduinos (8 bit ones) don't do the float to string thing. So your screen values come out as "?". This is NOT a problem when compiling for things like Teensy. They do the float to string thing like they are supposed to. Its all about allowing sprintf() stuff be the compiler.
