

extern int runsst(int, char**);  // The Start Trek guts..


// Setup and fire up.
void setup() {
  randomSeed(analogRead(0));
  Serial.begin(115200);
  for (int lp = 0; lp < 12; lp++) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("SST");
  runsst(0, NULL);
}


// Nothing to loop?
void loop() { }


// Block 'till we get a char. I guess this is the waiting room.
int getch() {
  while (!Serial.available());
  return Serial.read();
}


// And where it all goes out..
void proutn(char *s) { Serial.print(s);}
