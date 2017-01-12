//END COM4 DEFAULT: TRANSMIT
//Balloon Satellite
//Edward Elson Kosasih (A0112046X)
//EG1310

#include <XBee.h>
#include <LCDi2cR.h> 
#include <Wire.h>
#include <Math.h>
#include <SD.h>
#include <Servo.h>

#define pi 22/7;

/////////MEMORY CARD CONFIGURATION
File myFile;
int cardCancel = 0;

/////////BATTERY CONTROL CONFIGURATION
int pinBat11 = A3;
int pinBat12 = A4;
int pinBat21 = A5;
int pinBat22 = A6;
int bat11;
int bat12;
int bat21;
int bat22;

/////////// Servo Configuration
Servo servoUp, servoDown;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int posUp = 90;    // variable to store the servo position
int posDown = 125;    // variable to store the servo position
int pinServoUp = 9;
int pinServoDown = 10;

////////HUMIDITY CONFIGURATION
int pinHumid = A7;
int humidity;

////////MAGNETOMETER CONFIGURATION
#define addressHMC 0x1E //0011110b, I2C 7bit address of HMC5883
int magneticX;
int magneticY;
int magneticZ;

/////////PRESSURE CONFIGURATION
#define BMP085_ADDRESS 0x77  // I2C address of BMP085
const unsigned char OSS = 0;  // Oversampling Setting

// Calibration values
int ac1;
int ac2; 
int ac3; 
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1; 
int b2;
int mb;
int mc;
int md;

// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5; 

double temperature;
unsigned long pressure;
double altitude;
int altitudeSend;
int temperatureSend;

///////// XBEE CONFIGURATION
XBee xbee = XBee();

uint8_t payload[] = { 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x408D4B8D); // end_2, XB24-B, 1347

ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
XBeeResponse response   = XBeeResponse();
ZBRxResponse        rx  = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

int dataReceive;
int out;
int error;
int refresh;

//////////RECEIVED DATA CONFIGURATION
int verticalGet;
int horizontalGet;
int keyState;
double vertical;
double horizontal;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  //XBEE CONFIGURATION
  Serial2.begin(9600);
  xbee.setSerial(Serial2);

  //PRESSURE CONFIGURATION
  bmp085Calibration();
  
  ////MAGNETOMETER CONFIGURATION
  Wire.begin();
  Wire.beginTransmission(addressHMC); //open communication with HMC5883
  Wire.write((byte)0x02); //select mode register
  Wire.write((byte)0x00); //continuous measurement mode
  Wire.endTransmission();
  
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

  ////SERVO CONFIGURATION
  
  servoUp.attach(pinServoUp);
  servoDown.attach(pinServoDown);
  
  servoUp.write(posUp);
  servoDown.write(posDown);
	
}

void loop()
{  
  readPressure();
  readMagnetometer();
  readHumidity();
  readBattery();
  
  //*****SAVE TO MEMORY****//
  if (cardCancel == 0)
	saveMemory();

//  //the altitude and temperature obtained from readPressure() is of type double
//  //XBee can only send integer, thus, to preserve the 2 decimal numbers, multiply altitude and temperature by 100 to make it integer.
//  //store this in altitudeSend and temperatureSend
//  //Later in the remoteXBee program, the data received will be divided by 100 before it is printed on LCD
//
  altitudeSend = altitude*100;
  temperatureSend = temperature*100;

  dataReceive = 0;
  refresh = 0;

  //keep on transmitting data until remote Xbee receive them, after 5 count refresh reading data
  while ((dataReceive == 0) & (refresh < 10))
  {
    payload[0] = temperatureSend >> 8 & 0xff;
    payload[1] = temperatureSend & 0xff;
    payload[2] = pressure >> 24 & 0xff;
    payload[3] = pressure >> 16 & 0xff; 
    payload[4] = pressure >> 8 & 0xff;
    payload[5] = pressure & 0xff;  
    payload[6] = altitudeSend >> 8 & 0xff;
    payload[7] = altitudeSend & 0xff;
    
    payload[8] = magneticX >> 8 & 0xff;
    payload[9] = magneticX & 0xff;
    payload[10] = magneticY >> 8 & 0xff;
    payload[11] = magneticY & 0xff;
    payload[12] = magneticZ >> 8 & 0xff;
    payload[13] = magneticZ & 0xff;
    
    payload[14] = humidity >> 8 & 0xff;
    payload[15] = humidity & 0xff;  
    payload[16] = bat11 >> 8 & 0xff;
    payload[17] = bat11 & 0xff;
    payload[18] = bat12 >> 8 & 0xff;
    payload[19] = bat12 & 0xff;
    payload[20] = bat21 >> 8 & 0xff;
    payload[21] = bat21 & 0xff;
    payload[22] = bat22 >> 8 & 0xff;
    payload[23] = bat22 & 0xff;

    xbee.send(zbTx);

    checkData();
    refresh++;
  }

  if (refresh != 10)
	receiver();

//delay(1000);
}

// system is in state of receiving data from remote XBee
void receiver()
{
  out = 0;
  error = 0;

  while ((out == 0) && (error <= 1000))
  {

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

        //the first byte of data determines the keyState
        keyState = rx.getData(0);
        interpretData(keyState);	
      }

      else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE)
      {
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
      }
    }
    
    error++;
  }
}

// in this state, the system receive data from remote XBee
// exit the receiver state only when one of these two cases is fulfilled:
// 1. remote XBee have receive the data
// 2. after certain time (when "error" reaches 1000), if remote XBee don't receive the data, reset this module to the transmitting state.
//	  in this case, we need to press the reset button in remote XBee

//Remote XBee might be in either of these two keyStates:
// 1. sending data read from keypad (keyState 1)
// 		In keystate 1, the system will not invert "out" to 1. So that the system will keep on waiting for data from remote XBee.
//		the system will also reset "error" to 0
// 2. sending data as per normal (keyState 0)
// 		In keystate 0, the system will invert "out" to 0. So that the system will instantly toggle into transmitting state.

void interpretData(int keyState)
{
  if (keyState == 0)
  {
    out = 1;
  }

  else //keystate = 0
  {
    horizontalGet = rx.getData(1) << 8 | rx.getData(2);
    verticalGet = rx.getData(3) << 8 | rx.getData(4);
    
    voltage();

    ////*****Servo Control*****////    
    servoControl();

    error = 0;
  }
}

//invert dataReceive to 1 if remote XBee has receive the data sent
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
}

//calculate the voltage of horizontal and vertical value from joystick
void voltage()
{
  horizontal = horizontalGet * 5 / 1024.0;

  if ((horizontal >= 2.45) && (horizontal <= 2.48)) horizontal = 0;
  else 	horizontal = - 2.45 + horizontal;

  horizontal = horizontal * 2.5; // NEED to increase the rotating speed

  vertical = verticalGet * 5 / 1024.0;

  if ((vertical >= 2.50) && (vertical <= 2.53))     vertical = 0;
  else 	vertical = 2.53 - vertical;

   vertical = vertical * 2.5; //increase the increasing speed
  
}

void servoControl()
{ 
	if ((horizontal >= -0.02) & (horizontal <= 0.02))
	{
                posUp = 90;
		servoUp.write(posUp);
	}
       else
	{
		posUp = posUp + horizontal;
               
                if (posUp > 180) posUp = 180;
                else if (posUp < 0) posUp = 0;
                
		servoUp.write(posUp);              // set the speed of rotation 
                delay(500);
	}
        
      
       	if (vertical != 0)
	{
		posDown = posDown + vertical;
                
                if (posDown > 255) posDown = 255;
                else if (posDown < 0) posDown = 0;
                
		servoDown.write(posDown);              // set the speed of rotation 
	}
    
	
        //Serial.print(posUp);
        //Serial.print(", ");
        //Serial.println(posDown);
        
	delay(100);                  	       // waits 15ms for the servo to reach the position
}

//to read battery voltage
void readBattery()
{
  bat11 = analogRead(pinBat11);
  bat12 = analogRead(pinBat12);
  bat21 = analogRead(pinBat21);
  bat22 = analogRead(pinBat22);
}

//to read the humidity sensors
void readHumidity()
{
  humidity = analogRead(pinHumid); //later must convert
}

// to read the magnetometer sensors
void readMagnetometer()
{
  Wire.beginTransmission(addressHMC);
  Wire.write((byte)0x03); //select register 3, X MSB register
  Wire.endTransmission();

  //Read data from each axis, 2 registers per axis
  Wire.requestFrom(addressHMC, 6);
  if(6 <= Wire.available()){
    magneticX = Wire.read() << 8; //X msb
    magneticX |= Wire.read();     //X lsb
    magneticZ = Wire.read() << 8; //Z msb
    magneticZ |= Wire.read();     //Z lsb
    magneticY = Wire.read() << 8; //Y msb
    magneticY |= Wire.read();     //Y lsb
  }
}

//read data from BMP085 sensor
void readPressure()
{
  temperature = bmp085GetTemperature(bmp085ReadUT());
  pressure = bmp085GetPressure(bmp085ReadUP());
  altitude = 44330.0 * (1.0 - pow((pressure/101325.0), (1 / 5.255)));
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
  
  temp = ((5000.0 / 31.0)*((humidity/1024.0) - 0.16)) * 100; // later must divide by 100

  b11 = (bat11 * 5.0/1024.0) * 1000; //in mVolt
  b12 = (bat12 * 5.0/1024.0) * 1000;
  b21 = (bat21 * 5.0/1024.0) * 1000;
  b22 = (bat22 * 5.0/1024.0) * 1000;
  
  dataString = dataString + String(temperatureSend);
  dataString = dataString + ",";
  dataString = dataString + String(pressure);
  dataString = dataString + ",";
  dataString = dataString + String(altitudeSend);
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

// Calculate temperature given ut.
// Value returned will be in units of 1 deg C
double bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;

  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8) >> 4)*0.1;  
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long bmp085GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;

  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;

  return p;
}

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write((byte)address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available());

  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write((byte)address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2);
  msb = Wire.read();
  lsb = Wire.read();

  return (int) msb<<8 | lsb;
}

// Read the uncompensated temperature value
unsigned int bmp085ReadUT()
{
  unsigned int ut;

  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write((byte)0xF4);
  Wire.write((byte)0x2E);
  Wire.endTransmission();

  // Wait at least 4.5ms
  delay(5);

  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write((byte)0xF4);
  Wire.write((byte)(0x34 + (OSS<<6)));
  Wire.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write((byte)0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);

  // Wait for data to become available
  while(Wire.available() < 3);
  msb  = Wire.read();
  lsb  = Wire.read();
  xlsb = Wire.read();

  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);

  return up;
}

// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void bmp085Calibration()
{
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
}



