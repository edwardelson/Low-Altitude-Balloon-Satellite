//COORDINATOR COM3 DEFAULT: RECEIVE
//Balloon Satellite
//Edward Elson Kosasih (A0112046X)
//EG1310

#include <XBee.h>
#define VERSION "1.1"
#include <Wire.h>
#include <LCDi2cR.h>
#include <Math.h>
#include <SD.h>

#define pi 22/7;

/////////MEMORY CARD CONFIGURATION
File myFile;
int cardCancel = 0;

////////Thumb Joystick Configuration
int vertical = 0;
int horizontal = 0;
double verShow;
double horShow;
double select = 5.0;
int pinS = 5;
int pinH = 6;
int pinV = 7;

////////LCD CONFIGURATION
LCDi2cR lcd = LCDi2cR(4,20,0x63,0);
uint8_t rows = 4;
uint8_t cols = 20;

////////XBEE CONFIGURATION
XBee xbee = XBee();

uint8_t payload[] = {0, 0, 0, 0, 0};

XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x408D4BA7); // end_2, XB24-B, 1347   

ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
XBeeResponse response   = XBeeResponse();
ZBRxResponse        rx  = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

int pin3 	= 12; // yellow / green
int pin2  	= 11; // red
int pin1  	= 10; // green / yellow
int dataReceive;

////RECEIVED DATA CONFIGURATION
int temperatureGet;
double temperature;
long pressure;
int altitudeGet;
double altitude;

int magneticX;
int magneticY;
int magneticZ;
double magnetic;
double compass;

int humidityGet;
double humidity;
int bat11Get;
double bat11;
int bat12Get;
double bat12;
int bat21Get;
double bat21;
int bat22Get;
double bat22;


void setup() 
{
  ////LCD CONFIGURATION
  lcd.init();
  lcd.clear();
  lcd.print("KEYSTATE 0");
 
  ////PC CONFIGURATION
  Serial.begin(9600);

  ////XBEE CONFIGURATION
  Serial2.begin(9600);
  xbee.setSerial(Serial2);

  onStatus(1, 1, 1);

  pinMode(pin3, OUTPUT);
  pinMode(pin2,  OUTPUT);
  pinMode(pin1,   OUTPUT);

  ////MEMORY CARD CONFIGURATION
  pinMode(53, OUTPUT);  
  
  if (!SD.begin(53))
  {
    cardCancel = 1;
  }
 
  if (!(SD.exists("data.csv")))
  {
    myFile = SD.open("data.csv", FILE_WRITE);
    myFile.close();
  }
  
}

void loop() 
{
  onStatus(1, 0, 0);

  xbee.readPacket();

  if (xbee.getResponse().isAvailable())
  {
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) 
    {
      xbee.getResponse().getZBRxResponse(rx);

      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) 
      {

      } 
      else 
      {
      }

      onStatus(1, 0, 1);

      temperatureGet = rx.getData(0) << 8 | rx.getData(1);
      temperature = temperatureGet/100.00;

      pressure = (long) rx.getData(2) <<24 | (long) rx.getData(3) <<16 | (long) rx.getData(4) <<8 | (long) rx.getData(5);

      altitudeGet = rx.getData(6) << 8 | rx.getData(7);
      altitude = altitudeGet/100.00;

      magneticX = rx.getData(8) << 8 | rx.getData(9);
      magneticY = rx.getData(10) << 8 | rx.getData(11);
      magneticZ = rx.getData(12) << 8 | rx.getData(13);
     
      magnetic = sqrt(pow(magneticX, 2) + pow(magneticY, 2) + pow(magneticZ, 2));
      
	  calculateCompass();

      humidityGet = rx.getData(14) << 8 | rx.getData(15);
      humidity = (5000.0 / 31.0)*((humidityGet/1024.0) - 0.16);

      bat11Get = rx.getData(16) << 8 | rx.getData(17);
      bat11 = bat11Get * 5.0 / 1024.0;
      
      bat12Get = rx.getData(18) << 8 | rx.getData(19);
      bat12 = 2 * (bat12Get * 5.0 / 1024.0) - bat11;

      bat21Get = rx.getData(20) << 8 | rx.getData(21);
      bat21 = bat21Get * 5.0 / 1024.0;
   
      bat22Get = rx.getData(22) << 8 | rx.getData(23);
      bat22 = 2 * (bat22Get * 5.0 / 1024.0) - bat21;

	  //*****SAVE TO MEMORY****//
		if (cardCancel == 0)
		saveMemory();

//      lcd.setCursor(1,0);
//      lcd.print(compass);
	  
//      //PRINT IN LCD
//      lcd.clear();
//
//      lcd.print ("Temp: ");
//      lcd.print(temperature, 2);
//      lcd.print(" deg C");
//
//      lcd.setCursor(1,0);
//
//      lcd.print ("Pressure: ");
//      lcd.print(pressure);
//      lcd.println(" Pa");
//
//      lcd.setCursor(2,0);
//
//      lcd.print ("Altitude: ");
//      lcd.print(altitude, 2);
//      lcd.println(" m");
//      
//      lcd.setCursor(3,0);
//
//      lcd.print ("Humidity: ");
//      lcd.print(humidity);
//      lcd.println(" %");

      //*****SEND TO PROCESSING*****//
      Serial.print(temperature);
      Serial.print(",");
      Serial.print(altitude);
      Serial.print(",");
      Serial.print(pressure);
      Serial.print(",");
      Serial.print(humidity);      
      Serial.print(",");
      Serial.print(bat11);
      Serial.print(",");
      Serial.print(bat12);
      Serial.print(",");
      Serial.print(bat21);
      Serial.print(",");
      Serial.print(bat22);      
      Serial.print(",");
      Serial.println(compass);     

      
      delay(1000);
      
      // transmit data only if this module have receive data from the remote module
      transmit();
    } 
    else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE)
    {
      onStatus(1, 1, 0);
      xbee.getResponse().getModemStatusResponse(msr);

      if (msr.getStatus() == ASSOCIATED) 
      {

      } 
      else if (msr.getStatus() == DISASSOCIATED) 
      {

      }
      else 
      {

      }
    }
    else
    {
      onStatus(1, 1, 0);    
      }
    }

  }

  // In this state, this system will transmit data to remote Xbee
  // exit the transmission state only if the remote module have receive the data
  // first check whether keypad is pressed or not.

  void transmit()
  {
    onStatus(0, 0, 0);

    // READ KEYPAD
    int keyState = 0;

    // keyState 0 = send data as per normal
    // keyState 1 = send data read from joystick

    readJoystick();
    // when SELECT button is pressed, voltage read will be 0.0 (connected to ground)

    // SELECT button is pressed. Release pressing when "you are now in keyState 1" is shown on screen. 
    if ((select < 1) and (select >= 0.0))
    {
      keyState = 1;
      lcd.clear();
      lcd.print("KEYSTATE 1");

      onStatus(0, 1, 0);

      delay(1000);
    }

    while (keyState == 1)  
    { 

      //transmit data
      dataReceive = 0;

      while (dataReceive == 0)
      {
		onStatus(0, 1, 1);	
		readJoystick();
		
        horShow = horizontal * 5.0/1024.0;
		
		//display data properly
		if ((horShow >= 2.45) && (horShow <= 2.48)) horShow = 0;
		else 	horShow = - 2.45 + horShow;
  
		horShow = horShow * 2.5; // NEED to increase the rotating speed

        verShow = vertical * 5.0/1024.0;

		if ((verShow >= 2.50) && (verShow <= 2.53))     verShow = 0;
		else 	verShow = 2.53 - verShow;
  
		verShow = verShow * 2.5; //set the increasing speed
		
        lcd.clear();
        
        lcd.print("KEYSTATE 1");
        
        lcd.setCursor(1,0);
        lcd.print("Horizontal: ");
	lcd.print(horShow);
     
	lcd.setCursor(2,0);

		lcd.print("Vertical: ");
		lcd.print(verShow);
     
		lcd.setCursor(3,0);
		lcd.print("Select: ");
		lcd.print(select);
      
        
        payload[0] = 1; // send the keyState to remote XBee
        payload[1] = horizontal >> 8 & 0xff;
        payload[2] = horizontal;
        payload[3] = vertical >> 8 & 0xff;
        payload[4] = vertical;
        
        
        xbee.send(zbTx);
        checkData(); // check whether data have been received. If yes, dataReceive will be inverted to 1.
      }

    //user press SELECT button to leave the keystate 1	
    if ((select < 1) and (select >= 0.0))
      {
        keyState = 0;
        lcd.clear();
        lcd.print("KEYSTATE 0");
        delay(1000);     
      }    
    }
	
    // transmit data as per normal
    // keystate 0

    onStatus(0, 0, 1);

    dataReceive = 0;

    //keep on sending the same data until it is received by remote Xbee
    while (dataReceive == 0)
    {
      onStatus(0, 1, 1);
	  
      payload[0] = 0;
      payload[1] = 0;
      payload[2] = 0;
      payload[3] = 0;
      payload[4] = 0;

      xbee.send(zbTx);

      checkData();
    }
  }
  
//calculate the value of compass heading
void calculateCompass()
{
  double angle = (atan(magneticX * 1.0 / magneticY))*180*7/22;

  if (magneticY > 0) compass = 270 - angle;
  else if (magneticY < 0)
  {
	compass = 90 - angle;
  }
  else
  {
    if (magneticX > 0) compass = 180;
    else compass = 0;
  }
  
  if (compass > 360) compass = compass - 360;
}

void onStatus(int p3, int p2, int p1)
{
  if (p3 == 1) analogWrite(pin3, 255);
  else analogWrite(pin3, 0);

  if (p2 == 1) analogWrite(pin2, 255);
  else analogWrite(pin2, 0);

  if (p1 == 1) analogWrite(pin1, 255);
  else analogWrite(pin1, 0);
}

//to check whether the data sent is received by remote Xbee or not
//invert dataReceive to 1 if data is received by remote Xbee

void checkData()
{
  if (xbee.readPacket(1000))
  {
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE)
    {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      if (txStatus.getDeliveryStatus() == SUCCESS) 
      {
        dataReceive = 1;        
      } 
      else
      {

      }
    }      
  } 
  else 
  {

  }

//  delay(100);
}

void saveMemory()
{
  myFile = SD.open("data.csv", FILE_WRITE);
  
  if (myFile)
  {
    write();
    
    myFile.close();
  } 
}

//write data to memory card. Temperature, Altitude and Humidity are all multiplied by 100. Later in excel must be divided by 100;
void write()
{
  // make a string for assembling the data to log:
  String dataString = "";
  int temp, b11, b12, b21, b22;
  
  temp = humidity * 100; // later must divide by 100
  b11 = bat11 * 1000; //in mVolt
  b12 = bat12 * 1000;
  b21 = bat21 * 1000;
  b22 = bat22 * 1000;
  
  dataString = dataString + String(temperatureGet);
  dataString = dataString + ",";
  dataString = dataString + String(pressure);
  dataString = dataString + ",";
  dataString = dataString + String(altitudeGet);
  dataString = dataString + ",";
  dataString = dataString + String(temp);
  dataString = dataString + ",";
  dataString = dataString + String(magneticX);
  dataString = dataString + ",";
  dataString = dataString + String(magneticY);
  dataString = dataString + ",";
  dataString = dataString + String(magneticZ);
  dataString = dataString + ",";
  dataString = dataString + String(b11);
  dataString = dataString + ",";
  dataString = dataString + String(b12);
  dataString = dataString + ",";
  dataString = dataString + String(b21);
  dataString = dataString + ",";
  dataString = dataString + String(b22);

  myFile.println(dataString);  
}


void readJoystick()
{
  select = analogRead(pinS) * 5 / 1024.0;

  horizontal = analogRead(pinH);
  //  horizontal = analogRead(pinH) * 5 / 1024.0;

  //  if ((horizontal >= 2.45) && (horizontal <= 2.48)) horizontal = 0;
  //  else 	horizontal = 2.45 - horizontal;
  //
  //  horizontal = horizontal * 2.5 // to increase the rotating speed

  vertical = analogRead(pinV);
  //  vertical = analogRead(pinV) * 5 / 1024.0;

  //  if ((vertical >= 2.50) && (vertical <= 2.53))     vertical = 0;
  //  else 	vertical = 2.53 - vertical;
  //
  //  vertical = vertical * 2.5; //set the increasing speed
  
  
}

