import processing.serial.*;
import org.gwoptics.graphics.graph2D.Graph2D;
import org.gwoptics.graphics.graph2D.traces.ILine2DEquation;
import org.gwoptics.graphics.graph2D.traces.RollingLine2DTrace;

class eq implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    return horizontal;
  }    
}

class eq2 implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    return vertical;
  }    
}

class eq3 implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    if(mousePressed)
      return 400;
    else
      return 0;
  }    
}

Serial myPort; 

RollingLine2DTrace r,r2,r3;
RollingLine2DTrace s,s2,s3;
Graph2D g;
Graph2D h;

float horizontal;
float vertical;
  
void setup()
{
  size(displayWidth, displayHeight);
  
  //SERIAL COMMUNICATION CONFIGURATION
  println(Serial.list());
  myPort = new Serial(this, Serial.list()[0], 9600);
 
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
  
  //LIVE GRAPH CONFIGURATION
  r  = new RollingLine2DTrace(new eq() ,100,0.005f);
  r.setTraceColour(0, 255, 0);
  r.setLineWidth(2);
  
  r2 = new RollingLine2DTrace(new eq2(),100,0.005f);
  r2.setTraceColour(255, 0, 0);
  
  s  = new RollingLine2DTrace(new eq() ,100,0.005f);
  s.setTraceColour(0, 255, 0);
  
  s2 = new RollingLine2DTrace(new eq2(),100,0.005f);
  s2.setTraceColour(255, 0, 0);
  
   
  g = new Graph2D(this, 450, 250, false); //SIZE OF GRAPH
  g.setYAxisMax(50); //Y-AXIS
  g.addTrace(r);


  g.position.y = 40;
  g.position.x = 100;
  g.setYAxisTickSpacing(5);
  g.setXAxisMax(5f);
  g.setNoBorder();
  
  h = new Graph2D(this, 450, 250, false);
  h.setXAxisLabel("Time (sec)");
  h.setYAxisLabel("Voltage");
  h.setYAxisMax(100);
  h.setYAxisLabelFont("serif", 15, true);
  h.setNoBorder();

 
  h.addTrace(s2);

  h.position.y = 400;
  h.position.x = 100;
  h.setYAxisTickSpacing(10);
  h.setXAxisMax(5f);
  

}

void draw()
{
  background(255);
  g.draw();
  h.draw();
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
   horizontal = inByte[0];
   vertical = inByte[1];
   

}

