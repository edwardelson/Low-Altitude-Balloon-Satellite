import processing.serial.*;
import org.gwoptics.graphics.graph2D.Graph2D;
import org.gwoptics.graphics.graph2D.traces.ILine2DEquation;
import org.gwoptics.graphics.graph2D.traces.RollingLine2DTrace;

class eq implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    return temperature;
  }    
}

class eq2 implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    return altitude;
  }    
}

class eq3 implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    return pressure;
  }    
}

class eq4 implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    return humidity;
  }    
}

Serial myPort; 

RollingLine2DTrace deg, m, pa, per;
Graph2D temp;
Graph2D alt;
Graph2D pres;
Graph2D hum;


float temperature;
float altitude;
float pressure;
float humidity;

void setup()
{
  size(displayWidth, displayHeight);
  
  ////SERIAL COMMUNICATION CONFIGURATION
  println(Serial.list());
  myPort = new Serial(this, Serial.list()[0], 9600);
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
  
  ////GRAPH TRACE CONFIGURATION
  
  deg = new RollingLine2DTrace(new eq() ,100,0.005f);
  deg.setTraceColour(0, 255, 0);
  deg.setLineWidth(2);
  
  m = new RollingLine2DTrace(new eq2(),100,0.005f);
  m.setTraceColour(255, 0, 0);
  m.setLineWidth(2);
  
  pa  = new RollingLine2DTrace(new eq3() ,100,0.005f);
  pa.setTraceColour(0, 0, 255);
  pa.setLineWidth(2);
  
  per = new RollingLine2DTrace(new eq4(),100,0.005f);
  per.setTraceColour(100, 100, 100);
  per.setLineWidth(2);
  
  ////LIVE GRAPH CONFIGURATION
  
  temp = new Graph2D(this, 350, 250, false); //SIZE OF GRAPH
  temp.addTrace(deg);
  temp.setYAxisMax(50);
  temp.setXAxisLabel("Time");
  temp.setYAxisLabel("Temperature (Celcius)");
  temp.setXAxisLabelFont("serif", 15, true);
  temp.setYAxisLabelFont("serif", 15, true);
  temp.position.y = 40;
  temp.position.x = 100-20;
  temp.setYAxisTickSpacing(5);
  temp.setXAxisMax(5f);
  temp.setNoBorder();
  
  alt = new Graph2D(this, 350, 250, false); //SIZE OF GRAPH
  alt.addTrace(m);
  alt.setYAxisMax(100);
  alt.setXAxisLabel("Time");
  alt.setYAxisLabel("Altitude (meter)");
  alt.setXAxisLabelFont("serif", 15, true);
  alt.setYAxisLabelFont("serif", 15, true);
  alt.position.y = 400;
  alt.position.x = 100-20;
  alt.setYAxisTickSpacing(10);
  alt.setXAxisMax(5f);
  alt.setNoBorder();
  
  pres = new Graph2D(this, 350, 250, false); //SIZE OF GRAPH
  pres.addTrace(pa);
  pres.setYAxisMin(100000);
  pres.setYAxisMax(101000);
  pres.setXAxisLabel("Time");
  pres.setYAxisLabel("Pressure (Pascal)");
  pres.setXAxisLabelFont("serif", 15, true);
  pres.setYAxisLabelFont("serif", 15, true);
  pres.position.y = 40;
  pres.position.x = 560-20;
  pres.setYAxisTickSpacing(100);
  pres.setXAxisMax(5f);
  pres.setNoBorder();
  
  hum = new Graph2D(this, 350, 250, false); //SIZE OF GRAPH
  hum.addTrace(per);
  hum.setYAxisMax(100);
  hum.setXAxisLabel("Time");
  hum.setYAxisLabel("Humidity (%)");
  hum.setXAxisLabelFont("serif", 15, true);
  hum.setYAxisLabelFont("serif", 15, true);
  hum.position.y = 400;
  hum.position.x = 560-20;
  hum.setYAxisTickSpacing(10);
  hum.setXAxisMax(5f);
  hum.setNoBorder();
  
}

void draw()
{
  background(255);
  temp.draw();
  alt.draw();
  pres.draw();
  hum.draw();

}

void serialEvent (Serial myPort)
{
   String inString = myPort.readStringUntil('\n');
 
   if (inString != null) {
   // trim off any whitespace:
   inString = trim(inString);}
   
   // convert to an int and map to the screen height:
   float[] inByte = float(split(inString, ",")); 
   temperature = inByte[0];
   altitude = inByte[1];
   pressure = inByte[2];
   humidity = inByte[3];
   
}


