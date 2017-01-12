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
float angle;
float bat11, bat12, bat21, bat22;

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
  temp.setXAxisLabelFont("serif", 20, true);
  temp.setYAxisLabelFont("serif", 20, true);
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
  alt.setXAxisLabelFont("serif", 20, true);
  alt.setYAxisLabelFont("serif", 20, true);
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
  pres.setXAxisLabelFont("serif", 20, true);
  pres.setYAxisLabelFont("serif", 20, true);
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
  hum.setXAxisLabelFont("serif", 20, true);
  hum.setYAxisLabelFont("serif", 20, true);
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
  
  ////draw battery reading
    
     fill(255, 0, 0);
     text("Bat 1 Cell 1:", 1050, 40);
  
     fill(50, 200, 0);
     text("Bat 1 Cell 2:", 1050, 120);
     
     fill(0, 0, 255);
     text("Bat 2 Cell 1:", 1050, 200);
     
     fill(100, 200, 30);
     text("Bat 2 Cell 2:", 1050, 280);
     
     
     fill(255, 0, 0);
     text((bat11), 1170, 40);
  
     fill(50, 200, 0);
     text((bat12), 1170, 120);
     
     fill(0, 0, 255);
     text((bat21), 1170, 200);
     
     fill(100, 200, 30);
     text((bat22), 1170, 280);
     
     
     fill(255, 0, 0);
     text("Volt", 1250, 40);
  
     fill(50, 200, 0);
     text("Volt", 1250, 120);
     
     fill(0, 0, 255);
     text("Volt", 1250, 200);
     
     fill(100, 200, 30);
     text("Volt", 1250, 280);
    
  ////draw compass
    
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
   temperature = inByte[0];
   altitude = inByte[1];
   pressure = inByte[2];
   humidity = inByte[3];
   bat11 = inByte[4];
   bat12 = inByte[5];
   bat21 = inByte[6];
   bat22 = inByte[7];
   angle = inByte[8];
   
}


