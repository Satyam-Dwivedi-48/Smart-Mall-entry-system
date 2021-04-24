#include <LiquidCrystal.h>// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 13, 4, 3, 2);
#define TEMPERATURE A0//this is amalog port in lower right half
#define MOTOR 10
#include <Servo.h> //includes the servo library
//An analog signal can take on any number of values. A digital signal, on the other hand, has only two values: HIGH and LOW
Servo myservo1;//declaring the servo motor object for controlling the attribute
const int cold = 102;//human temp parameter,used in loop2()
int ir_s1 = 8;//used in setup later
int ir_s2 = 7;//used in setup later in pinmode commands
int flag;//serves for primary testing and screening of temperature 

int Total = 50;//total number of people allowed in mall
int Space;//later used to decrease and increase the live count of occupants of the mall 



void setup()
{
  	pinMode(ir_s1, INPUT);//defining which is input and which is output
  	pinMode(ir_s2, INPUT);//defines these ports as input ports
  	pinMode(MOTOR, OUTPUT);//refer line 4,port 10
  	pinMode(A2, INPUT); //sensor input refer line 3
  	pinMode(6, OUTPUT); //green led
  	pinMode(5, OUTPUT); //red led output
	myservo1.attach(9);//atach the motor to a pon number 9(9&10 are the allowed ones)
	myservo1.write(100);//angle 100 degree 
	lcd.begin(16, 2);//Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display.
	//needs to be called before any other LCD library commands.
	//all greeting are carried out here with delays of 2s
	lcd.setCursor (0,0);//basic displaying of greeting
	lcd.print(" Welcome ");
	lcd.setCursor (0,1);
	lcd.print(" To The Mall ");
	delay (2000);
	lcd.setCursor (0,0);
	lcd.print("  Mall Entry ");
	lcd.setCursor (0,1);
	lcd.print("     System     ");
	delay (2000);
	lcd.clear();
	Space = Total;//assign the variable to max space available
} 
int speed_decider(int temp)
{
 	if(temp<20)
    	return 0;//no action needed to be taken,in control temperature
  	else if(temp>40)
    	return 255;//if temp is greater than 40,we crank fan to maximum speed
  	else
	//Re-maps a number from one range to another. That is,
	//a value of fromLow would get mapped to toLow, a value of fromHigh to toHigh
	//used to get appropiate speed by mapping on a relative 0-255 scale
    	return map(temp, 20, 40, 0, 255);//between 20 and 40 need to be rescaled
}


void loop()
{ 
 	int temperature = analogRead(TEMPERATURE);//pin is 10 and reads the value
	 //This means that it will map input voltages between 0 and the operating voltage(5V or 3.3V) into integer values between 0 and 1023.
  	temperature = map(temperature, 20, 358, -40, 125);//re-scales 
  	Serial.println(speed_decider(temperature));//gets the speed value from the func and 
  	analogWrite(MOTOR, speed_decider(temperature));//analogWrite values from 0 to 255
  	loop2();//this is called anf sets flag variable and turns on the LED to display the
  	if(flag==1)//this flag variable is a check of tempearture first,if temp is valid we let the entry happen
	{
		if(digitalRead (ir_s1) == HIGH)//If someone wants to cross over and enter,s1 is entry
		{
			if(Space>0)//checks for avalilibilty of space
			{
          		myservo1.write(0);//if space is there we tuen from 100 to 0
          		delay(1000);//1 second delay
          		Space = Space-1;//decrese to depict ingress
          		myservo1.write(100);//close the gate again to previous position
          		delay(1000);//wait for another 1s
			}
			else//incase space is less
			{
				//basic input output formatting 
				lcd.setCursor (0,0);
				lcd.print(" Sorry no Space ");
				lcd.setCursor (0,1);
				lcd.print("    Available    "); 
				delay (1000);
				lcd.clear();
			}
		}
    	lcd.setCursor (0,0);//return to our old screen ehich shows the total occupants and free space
		lcd.print("Total Space: ");
		lcd.print(Total);
		lcd.setCursor (0,1);
		lcd.print("Have  Space: ");
		lcd.print(Space);
    	delay(2000);
  	}
  	else
  	{
		//flag !=1,nobody has the permission to enter as they failed the temperature screening,it won't show anything but it won't open
    	digitalWrite(8,LOW);//recalling ir_s1 = 8,the output is made low as a precautionary step to prevent opening of gate
		lcd.setCursor (0,0);
		lcd.print("Total Space: ");
		lcd.print(Total);
		lcd.setCursor (0,1);
		lcd.print("Have  Space: ");
		lcd.print(Space);
  	}
	//exit is triggered by sensor 2 and that allows the exit and increases the vacancy controlled by space variable
  	if(digitalRead (ir_s2) == HIGH)
    {
     	 if(Space<Total)//increase only if value is less than total or it will cross the max and give false output
         {
            delay(1000); 
          	Space = Space+1;//increment to depict egress 
            delay(1000);
         }
    }
}
void loop2() //responsible for temperature screening of people before entering
{
	int sensor = analogRead(A2);//reads from the temp sensor of screening 0-1023
  	float voltage = (sensor / 1024.0) * 5.0;//the sensor will report a value of 0 to 1023. 
	//If you divide that by 1024 we will get a percentage that you can multiply to give you an output of 0 to 5 volts.
	//It is just converting the sensor output to a value that you can work with to control things.
  	float tempC = (voltage - .5) * 100;//in degree celcius
  	float tempF = (tempC * 1.8) + 32;//in degree fahrenhiet
  	Serial.print("temp: ");//Prints data to the serial port as human-readable ASCII text
  	Serial.print(tempF);//prints ultimately "temp: x"
	//6 is GREEN LED port
	//5 is RED LED port
   	if (tempF < cold)//refer line 8 for declaration
	{ 
    	digitalWrite(6, HIGH);//if in range of normal allowed temp,make green led glow
    	digitalWrite(5, LOW);//make red turn off
    	flag=1;//this tells other function that it is safe to open the gate
  	}
  	else
  	{	 
   		digitalWrite(6, LOW);//green is turned off
    	digitalWrite(5, HIGH);//red is turned on trelling it is danger,and does not allow to enter 
    	flag=0;//tells that not safe to open the gate
  	}
  	delay(10);
}
