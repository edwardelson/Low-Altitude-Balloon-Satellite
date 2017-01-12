import processing.serial.*;

Serial myPort;

float angle;
float vertical;

void setup()
{
  size(displayWidth, displayHeight);
  
  //SERIAL COMMUNICATION CONFIGURATION
  println(Serial.list());
  myPort = new Serial(this, Serial.list()[0], 9600);
 
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
}

void draw()
{
  background(255);
 
  translate(1000, 380);

  // draw the compass background
  ellipseMode(CENTER);
  fill(50);
  stroke(10);
  strokeWeight(2);
  ellipse(150,150,300,300);

  // draw the lines and dots
  translate(150,150);  // translate the lines and dots to the middle of the compass
  rotate(0);
  noStroke();
  fill(51, 255, 51);

  int radius = 120;

  for(int degC = 5; degC < 360; degC += 10) //Compass dots
  {
    float angleC = radians(degC);
    float xC = 0 + (cos(angleC)* radius);
    float yC = 0 + (sin(angleC)* radius);
    ellipse(xC,yC, 3, 3);
  }

  for( int degL = 10; degL < 370; degL += 10) //Compass lines
  {
    float angleL = radians(degL);
    float x = 0 + (cos(angleL)* 145);
    float y = 0 + (sin(angleL)* 145);
  
    if( degL==90 || degL==180 || degL==270 || degL==360)
    {
     stroke(51, 255, 51);
     strokeWeight(4);
    }
    else
    {
      stroke(234,144,7);
      strokeWeight(2);
    }
    line(0,0, x,y);
  }

  fill(102, 102, 102); 
  noStroke();
  ellipseMode(CENTER);
  ellipse(0,0, 228,228); //draw a filled circle to hide the lines in the middle

  textAlign(CENTER);

  // Draw the letters
  fill(250);
  text("N", 1, -90);
  textSize(22);
  rotate(radians(90));
  text("E", 0, -90);
  rotate(radians(90));
  text("S", 0, -90);
  rotate(radians(90));
  text("W", 0, -90);
  rotate(radians(90));
  
  textAlign(CENTER);
  fill(0);
  text((angle), 0, 180);
  println(angle);

  //draw the needle

  rotate(radians(360-angle));
  stroke(234,144,7);
  strokeWeight(3);

  triangle(-10, 0, 10, 0, 0, -85);
  fill(234,144,7);
  triangle(-10, 0, 10, 0, 0, 60);

}

void serialEvent (Serial myPort)
{
   String inString = myPort.readStringUntil('\n');
 
   if (inString != null) {
   // trim off any whitespace:
   inString = trim(inString);}
   
   // convert to an int and map to the screen height:
   float[] inByte = float(split(inString, ",")); 
//   inByte[0] = map(inByte[0], 0, 1023, 0, 250);
//   inByte[1] = map(inByte[1], 0, 1023, 0, 250);
   angle = inByte[0];
   vertical = inByte[1];
   

}

