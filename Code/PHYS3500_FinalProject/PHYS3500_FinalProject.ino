/**
 * @author Tyler Conley
 * @date 2.20.2020
 * This is an all-inclusive piece of code made to run a three-axis machine
 * The x- and y- axes are run via step motors (included in basic Arduino kit)
 * The z-axis runs using a simple DC Motor
 * 
 * A joystick is used to control the movement in all directions
 * 
 * Each axis has a maximum of 6 inches in each direction,
 * and these maximums are defined using the DCMAX, XMAX, and YMAX constants
 */

#include <Stepper.h>

const int STEPSPERREV = 32;

const int JOYX = A1;
const int JOYY = A0;
const int JOYZ = 0;


const int DC0 = 3;
const int DC1 = 4;
int dcCount;
const int DCMAX = 50;


const int STX0 = 6;
const int STX1 = 7;
const int STX2 = 8;
const int STX3 = 9;
int xStepsCurr;
const int XMAX = 64;


const int STY0 = 10;
const int STY1 = 11;
const int STY2 = 12;
const int STY3 = 13;
int yStepsCurr;
const int YMAX = 64;


Stepper xStepper(STEPSPERREV, STX0, STX1, STX2, STX3);
Stepper yStepper(STEPSPERREV, STY0, STY1, STY2, STY3);


void setup() {

  pinMode( DC0, OUTPUT );
  pinMode( DC1, OUTPUT );

  pinMode( JOYZ, INPUT_PULLUP );

  digitalWrite( DC0, LOW );
  digitalWrite( DC1, LOW );

  xStepper.setSpeed(200);
  xStepsCurr = 0;
  
  yStepper.setSpeed(200);
  yStepsCurr = 0;
  
  Serial.begin(9600);
}

void loop() {
  int x, y, z;
  
  x = analogRead( JOYX );
  y = analogRead( JOYY );
  z = digitalRead( JOYZ );
  

  // check speed of motor to see how fast it gets the rocket from top to bottom, and decide on a max value
  if( z == 0 && dcCount <= DCMAX ) {
    digitalWrite( DC0, HIGH );
    digitalWrite( DC1, LOW );
    dcCount++;
  }
 
  if( z != 0 && dcCount >=0 ) {
    digitalWrite( DC0, LOW );
    digitalWrite( DC1, HIGH );
    dcCount--;
  }

  Serial.print(x);
  Serial.print(" , ");
  Serial.print(y);
  Serial.print(" , ");
  Serial.println(z);

  
  x-=502; // 0 for x is at 527, this number is so that the motor doesn't move when the joystick doesn't
  y-=527; // 0 for y is at 502, see above
  
  
  
  // logic for x

  if( x - 10 > 0 ) {
    if( !( xStepsCurr + 1 > XMAX ) ){
      xStepper.step( 1 );
      xStepsCurr ++;
    }
  }

  if ( x + 10 < 0 ) {
    if( !( xStepsCurr -1 < 0 ) ) {
      xStepper.step( -1 );
      xStepsCurr --;
    }
  }

  //logic for y

  if ( y - 10 > 0 ) {
    if( !( yStepsCurr + 1 > YMAX ) ) {
      yStepper.step( 1 );
      yStepsCurr ++;
    }
  }

  if ( y + 10 < 0 ) {
    if( !( yStepsCurr - 1 < 0 ) ) {
    yStepper.step( -1 );
    yStepsCurr --;
    }
  }
  Serial.print("\n\nStep Speeds: ");
  Serial.print( xStepsCurr );
  Serial.print( " , " );
  Serial.println( yStepsCurr );
  Serial.println("\n\n");  
}
