void setup() {
//  PFont metaBold;
//  metaBold = loadFont("fonts/LucidaBrightItalic.vlw.gz"); 
//  setFont(metaBold, 44); 

}

void draw() {
  int d = day();    // Values from 1 - 31
  int m = month();  // Values from 1 - 12
  int y = year();   // 2003, 2004, 2005, etc.
  String s = String.valueOf(d);
  
  text(s, 18, 10);
  s = String.valueOf(m);
  text("/", 32, 10);
  text(s, 46, 10); 
  s = String.valueOf(y);
  text("/", 60, 10);
  text(s, 74, 10);
}
