//CNC router hardware revision 2.1
//
//Code to run on the command computer see:
//commands-CNC.c
//
//Source & info: www.HomoFaciens.de


/*
Thanks (HomoFaciens) for sharing this code

This code is modified by Mariwan Jalal mariwan.jalal@gmail.com  
The original code uses the A1543. Since I have many L298N 
I needed to use them rather than A1543. 
Be aware that the Pins of Arduino is not 
compatible with the A1543 version. 
and I needed to use the Analog pins as digital to cover
all signals. Please let me know if you find any bugs or 
suggestions. 
*/
#include <Stepper.h>
#include <avr/io.h>
#include <util/delay.h>

//Change values below to adapt your motor
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

#define X_1        2       //input IN1_L298
#define X_2        3       //input IN2_L298
#define X_3        4       //input IN3_L298
#define X_4        5       //input IN4_L298

#define Y_1        6       //input IN1_L298
#define Y_2        7       //input IN2_L298
#define Y_3        8       //input IN3_L298
#define Y_4        9       //input IN4_L298

#define Z_1        10       //input IN1_L298
#define Z_2        11       //input IN2_L298
#define Z_3        12       //input IN3_L298
#define Z_4        13       //input IN4_L298

#define DISABLE    14       //DISABLE L2981
#define RELAY      15       //Router relay

#define READ_LENGTH 10

int readySend;
byte readByte;

long XSetPoint;
long XIsPoint;

long YSetPoint;
long YIsPoint;

long ZSetPoint;
long ZIsPoint;

unsigned long stepTime;

unsigned long microsStart;
unsigned long microsTime;
unsigned long stepPause;


// initialize the stepper library on pins 8 through 11:
Stepper xStepper(stepsPerRevolution, X_1, X_2, X_3, X_4);
Stepper yStepper(stepsPerRevolution, Y_1, Y_2, Y_3, Y_4);
Stepper zStepper(stepsPerRevolution, Z_1, Z_2, Z_3, Z_4);

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('X');   // send a capital X
    delay(300);
  }
}

void setup(){
 // set the speed at 60 rpm:
  xStepper.setSpeed(60);
  yStepper.setSpeed(60);
  zStepper.setSpeed(60);
   
  // initialize the serial port:
  Serial.begin(9600);
  
  pinMode(DISABLE, OUTPUT);
  digitalWrite(DISABLE, 1);

  readByte = 0;
  readySend = 0;

  XSetPoint = 0;
  XIsPoint  = 0;

  YSetPoint = 0;
  YIsPoint  = 0;

  ZSetPoint = 0;
  ZIsPoint  = 0;

  stepPause = 1000;

  // start serial port at 115200 bps:
  Serial.begin(115200);
  establishContact();  // send a byte to establish contact until receiver responds

  digitalWrite(DISABLE, 0);


}
void loop(){

  if(XSetPoint > XIsPoint){
  XIsPoint++;
  xStepper.step(1);
  }
  if(XSetPoint < XIsPoint){
  XIsPoint--;
  xStepper.step(-1);
  }

  if(YSetPoint > YIsPoint){
   YIsPoint++;
   yStepper.step(1);
  }
  if(YSetPoint < YIsPoint){
  yStepper.step(-1);
  YIsPoint--;
  }

  if(ZSetPoint > ZIsPoint){
  zStepper.step(1);
    ZIsPoint++;
  }
  if(ZSetPoint < ZIsPoint){
  yStepper.step(-1);
    ZIsPoint--;
  }
/*
 * I am not sure if this needed. I just comment them. 
  delayMicroseconds(20);
  microsTime = microsStart;
  while(microsTime - microsStart < stepPause){
    microsTime = micros();
    if(microsTime < microsStart){
      microsStart = micros();
    }
  }
  microsStart = micros();
  
  */
  readByte = 0;
  if (Serial.available() > 0){//if we get a valid byte, read from serial:
    //get incoming byte:
    readByte = Serial.read();
    readySend++;
    if(readySend == READ_LENGTH){
      Serial.print('r');   // send a 'r' to initiate next data from computer
      readySend = 0;
    }
  }

  if(readByte > 0){
    if(readByte == 255){
      digitalWrite(RELAY, 1);
    }
    if(readByte == 254){
      digitalWrite(RELAY, 0);
    }

    if(readByte == 128){
      stepPause += 100;
    }

    if(readByte == 64){
      stepPause -= 100;
    }

    if(readByte == 32){
      ZSetPoint--;
    }
    if(readByte == 16){
      ZSetPoint++;
    }

    if(readByte == 8){
      YSetPoint-=1;
    }
    if(readByte == 4){
      YSetPoint+=1;
    }

    if(readByte == 2){
      XSetPoint-=1;
    }
    if(readByte == 1){
      XSetPoint+=1;
    }
  }
}


