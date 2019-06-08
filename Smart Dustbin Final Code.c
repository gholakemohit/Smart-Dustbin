// for wifi module
#include <SoftwareSerial.h>

// for servo motor
#include <Servo.h>

// for esp8266 wifi module
#define RX 10
#define TX 11

// connect to wifi
String AP = "mohit";				// CHANGE ME
String PASS = "12345679"; 			// CHANGE ME
String API = "DSD7N40M7K35EFTW";    // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "Garbage Level";
String field2 = "Dust Level";

// for us 1 garbage level
const int trigPin1 = 2;
const int echoPin1 = 3;
long duration1;
long distance1;

// for us 2 lid opening
const int trigPin2 = 6;
const int echoPin2 = 7;
long duration2;
long distance2;

// for wifi module
SoftwareSerial esp8266(RX,TX); 
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

// for us 1 garbage level
int valSensor1 = 1;

// for dust sensor
int valSensor2 = 2;

// for us 2 lid opening
int valSensor3 = 0;

// for servo motor
Servo myservo; 
int ang = 0;

// time delay for dust sensor
unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

// for dust sensor
int measurePin = A5;
int ledPower = 12;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;


// setup function
void setup() 
{
	
	Serial.begin(9600);
  	
  	// connect to wifi
  	esp8266.begin(115200);
  	sendCommand("AT", 5, "OK");
  	sendCommand("AT+CWMODE=1", 5, "OK");
  	sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  
  	// for us 1 garbage level
  	pinMode(trigPin1, OUTPUT); 
  	pinMode(echoPin1, INPUT); 
  
  	// for us 2 lid opening
  	pinMode(trigPin2, OUTPUT); 
  	pinMode(echoPin2, INPUT); 
  	
  	// for servo motor
  	myservo.attach(8);
  	
  	// for dust sensor
  	pinMode(ledPower,OUTPUT);

}

// loop function
void loop() 
{

	// getting data from us 1 grabage level
	valSensor1 = getSensorData1();

	// getting data from dust sensor
	valSensor2 = getSensorData2();

	// sending data to thingspeak cloud
 	String getData = "GET /update?api_key=" + API + "&" + field1 + "=" + String(valSensor1) + "&" + field2 + "=" + String(valSensor2);
	sendCommand("AT+CIPMUX=1", 5, "OK");
 	sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST + "\"," + PORT, 15, "OK");
 	sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 4, ">");
 	esp8266.println(getData);
 	delay(1500);
 	countTrueCommand++;
 	sendCommand("AT+CIPCLOSE=0",5,"OK");

 	
 	// getting data from us 2 lid opening
 	digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPi2, LOW);
	duration2 = pulseIn(echoPin2, HIGH);
	distance2 = duration2 / 58;
    
    if(distance2 < 20)
    {
        for (ang = 0; ang <= 120; ang += 120)
        { 
        	myservo.write(ang);              
        }

    	for (ang = 120; ang >= 0; ang -= 120) 
        { 
          	myservo.write(ang); 
        }
		delay(5000);
    }    

}

// for us 1 garbage level
int getSensorData1()
{
	digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
	
	// Sets the trigPin1 on HIGH state for 10 micro seconds
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    
    digitalWrite(trigPin1, LOW);
	
	// Reads the echoPin1, returns the sound wave travel time in microseconds
	duration1 = pulseIn(echoPin1, HIGH);
	
	// Calculating the distance in cm
	distance1 = duration1 / 58;

	return distance1;

}

// for dust sensor
int getSensorData2()
{

	digitalWrite(ledPower, LOW);
  	delayMicroseconds(samplingTime);

	voMeasured = analogRead(measurePin);
	delayMicroseconds(deltaTime);
  	
  	digitalWrite(ledPower,HIGH);
  	delayMicroseconds(sleepTime);

	calcVoltage = voMeasured * ( 5.0 / 1024 );
	 
	dustDensity = 0.17 * calcVoltage - 0.1;

  	if ( dustDensity < 0)
  	{
    	dustDensity = 0.00;
  	}

  	delay(1000);
  
    return dustDensity;

}

// used while connecting to wifi
void sendCommand(String command, int maxTime, char readReplay[]) 
{

	Serial.print(countTrueCommand);
  	Serial.print(". at command => ");
  	Serial.print(command);
  	Serial.print(" ");
  	
  	while(countTimeCommand < (maxTime * 1))
  	{
    	esp8266.println(command);		//at+cipsend
    	if(esp8266.find(readReplay))	//ok
    	{
      		found = true;
      		break;
    	}
  
    	countTimeCommand++;
  	}
  
  	if(found == true)
  	{
    	Serial.println("OYI");
    	countTrueCommand++;
    	countTimeCommand = 0;
  	}
  
  	if(found == false)
  	{
    	Serial.println("Fail");
    	countTrueCommand = 0;
    	countTimeCommand = 0;
  	}
  
  	found = false;

 }