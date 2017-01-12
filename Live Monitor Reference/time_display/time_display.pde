void draw() {
  background(204);
  int s = second();  // Values from 0 - 59
  int m = minute();  // Values from 0 - 59
  int h = hour();    // Values from 0 - 23
  
  textSize(16);
  
  text(h, 10, 30);
  fill(0, 102, 153); 
  
  if (h <= 9)
    {
      text(h, 20, 30); 
      fill(0, 102, 153);
    }
  
  text(":", 33, 30);
  fill(0, 102, 153);
  
  text(m, 40, 30);
  fill(0, 102, 153);
  
  text(":", 62, 30);
  fill(0, 102, 153);
  
  text(s, 70, 30); 
  fill(0, 102, 153);
  
//  line(s, 0, s, 33);
//  line(m, 33, m, 66);
//  line(h, 66, h, 100);
}
