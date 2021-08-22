/**
  * @author Tyler
  * 
  * This is a single-file program for running the 3-Axis Motion System.
  * 
  * The 3-Axis Motion System was originally a school project of mine, which I
  * decided to completely refactor to make it more workable and to use some
  * of my newer experience and skills in microcontrollers and C++.
  * 
  * The output of the program is directed toward 3 NEMA-17 stepper motors, driven by
  * TB6600 stepper driver motors, and the input of the program is pulled from a simple joystick.  
  * The movement of the joystick controls the X- and Y-axes, while the button on it controls
  * whether the Z-axis moves up or down.
  * 
  * In addition to this, the endstops are reed switches, which will go HIGH if a
  * magnet comes close to them.  Magnets will be affixed to the sliders on each axis,
  * which will give the motion system the ability to find the (0, 0, 0) coordinate 
  * of the system.
  * 
  * All global definitions are done up front, including all pins, as well as a handful
  * of constants and a global variable storing the coordinates of the system. The setup()
  * and loop() functions follow, defining pinouts in the setup() function. The functions called
  * by the setup() and loop() functions come after, until the end of the program
  */


/*
 * x, y, and z axes represented as integers for extra readability
 * 
 * { x: 0, y: 1, z: 2 }
 */
#define x 0
#define y 1
#define z 2


/*
 * Pin Setup for the control joystick (JOY), step and direction pins for each stepper motor (STEP_PINS
 * and DIR_PINS), and reed switch endstops (ENDSTOPS)
 */
const int JOY[3] = { 38, 39, 0 };     // pins 38 and 39 on the Teensy++ 2.0 are analog pins
const int PUL_PINS[3] = { 1, 3, 5 };
const int DIR_PINS[3] = { 2, 4, 6 };
const int ENDSTOPS[3] = { 7, 8, 9 };


/*
 * Global variable to keep current coordinates (defined in the home_axes() function, 
 * which is called in the setup() function).  Constant values also defined for the maximum
 * number of steps for each axis in MAX.
 */
int coordinates[3];
const int MAX[3] = { 382, 382, 382 };


/*
 * Setup Function
 * 
 * Defines pin modes for each digital pin except for the ones already defined in the
 * stepper motor definitions above. Also sets the speed of each stepper motor and homes
 * each axis to 0.
 */
void setup() {
    pinMode( PUL_PINS[0], OUTPUT );
    pinMode( PUL_PINS[1], OUTPUT );
    pinMode( PUL_PINS[2], OUTPUT );

    pinMode( DIR_PINS[0], OUTPUT );
    pinMode( DIR_PINS[1], OUTPUT );
    pinMode( DIR_PINS[2], OUTPUT );
  
    pinMode( ENDSTOPS[0], INPUT_PULLUP );
    pinMode( ENDSTOPS[1], INPUT_PULLUP );
    pinMode( ENDSTOPS[2], INPUT_PULLUP );
    
    pinMode( JOY[2], INPUT_PULLUP );

    home_axes();
    coordinates[0] = 0;
    coordinates[1] = 0;
    coordinates[2] = 0;
}


/*
 * Loop Function
 * 
 * Handles movement for each axis repeatedly on loop, using the handle() function
 */
void loop() {
    handle( x ); // handles movement in x direction
    handle( y ); // handles movement in y direction 
    handle( z ); // handles movement in z direction
}


/*
 * Homes each axis to 0, using the reed switch endstops
 */
void home_axes(){
    while( digitalRead( ENDSTOPS[x] ) == 0 || 
        digitalRead( ENDSTOPS[y] ) == 0 || 
        digitalRead( ENDSTOPS[z] ) == 0 ){
                  
        if( digitalRead( ENDSTOPS[x] ) == 0 ){
             step_motor( x, -1 );
        }
        if( digitalRead( ENDSTOPS[y] ) == 0 ){
            step_motor( y, -1 );
        }   
        if( digitalRead( ENDSTOPS[z] ) == 0 ){
            step_motor( z, -1 );
        }
    }
}


/*
 * Returns a value depending on if the proposed move is valid within the coordinate system
 */
int validate( int axis, int value ){
    if ( coordinates[axis] + value > MAX[axis] ){
        return MAX[axis] - coordinates[axis];
    }
    if ( coordinates[axis] + value < 0 ) {
        return -coordinates[axis];
    }
    return value;
}


/*
 * Handle function for each axis (x: 0, y: 1, z: 2)
 * 
 * X and Y give a number between 0 and 5 steps depending on how far the joystick was pushed,
 * which gets a validation check from the validate() function
 * 
 * Z gets either 5 or -5 steps depending on if the joystick button is pressed or not. Also gets checked
 * by the validate() function
 */
void handle( int axis ){
    int dir;
    switch( axis ){
      case x:
        	dir = analogRead( JOY[x] );
  	      step_motor( x, validate( x, dir/100 ) );
          break;
      case y:
          dir = analogRead( JOY[y] );
  	      step_motor( y, validate( y, dir/100 ) );
          break;
      case z:
          dir = digitalRead( JOY[z] );
  	      if ( dir == 0 ){
  	    	    step_motor( z, validate( z, -5 ) );
  	      } 
  	      else {
  	    	    step_motor( z, validate( z, 5 ) );
  	      }
          break;
    }
}


/*
 * Function controlling the stepping of motors.
 * 
 * Relays instructions to the step and direction
 * pins of the targeted stepper motors. The direction
 * is defined by whether the number of steps given
 * to the function is positive or negative.
 * 
 * The motor will receive a step signal every 2 milliseconds,
 * as time is delayed for 1000 microseconds between switching
 * the pin on and off
 */
void step_motor( int axis, int steps ){
    coordinates[axis] += steps;
    if ( steps > 0 ) {
        digitalWrite( DIR_PINS[axis], HIGH );
    }
    if ( steps < 0 ) {
        digitalWrite( DIR_PINS[axis], LOW );
        steps = -steps;
    }

    for ( int i=0; i<steps; i++ ) {
        digitalWrite( PUL_PINS[axis], HIGH );
        delayMicroseconds( 1000 );
        digitalWrite( PUL_PINS[axis], LOW );
        delayMicroseconds( 1000 );
    }
}
