/*
    Lego Powerfunctions car model controlled over bluetooth
    Arduino Uno controls motors and servo
    Remote controlled with android app developed with MIT App Inventor 2

    Circuit:
    Serial communication   (uses Uno pin 0,1)    Bluetooth-module is attached (has to be detached when loading program over USB)
    L293D with motor       (uses Uno digital pins 2,5,9)
    L293D with servo       (uses Uno digital pins 6,8,3)
*/

#include <SPI.h>              // Serial Peripheral Interface Library
#include <string.h>           // contains function strtok: split string into tokens
#include <Thread.h>           // Theads for constant blinking, independent from loop() processing time



// Serial buffer size: calculate based on max input size expected for one command over bluetooth serial interface
#define INPUT_SIZE 30



// Motor control digital output pins defined as global constants (4 wheel drive with 2 Lego motors):
const int controlPin1A = 2;                  // L293D driver input 1A on pin no 2  http://www.ti.com/lit/ds/symlink/l293.pdf connected to Arduino digital output pin 2
const int controlPin2A = 5;                  // L293D driver input 2A on pin no 7 connected to Arduino digital output pin 5
const int motorEnablePin = 9;                // L293D ENable(1,2) input on pin no 1 connected to Arduino digital output pin 9
// Servo control digital output pins defined as global constants (Servo steering with 1 Lego servo):
const int controlPin3A = 6;                  // L293D driver input 3A on pin no 10 connected to Arduino digital output pin 6
const int controlPin4A = 8;                  // L293D driver input 4A on pin no 15 connected to Arduino digital output pin 8
const int servoEnablePin = 3;                // L293D ENable(3,4) input on pin no 9 connected to Arduino digital output pin 3

// Motor control global variables:
int motorSpeed = 0;                          // Motor speed 0..255
int motorDirection = 1;                      // Forward (1) or reverse (0)

// Servo control global variables:
int steering = 0;                            // Servo position 0..255
int steeringDirection = 0;                   // Left (0) and Right (1)



// Lights control digital output pins:
const int controlPinLightData  = 11;
const int controlPinLightStore = 12;
const int controlPinLightClock = 13;
int result = 1;
/*
    const int controlPinLightFlash = 13;
    //const int controlPinLightFlashLeft = 10;
    //const int controlPinLightFlashRight = 4;
    const int controlPinLightHeadTail = 12;
    const int controlPinLightBreak = 11;
*/

// Lights control global variables:
bool lightFlashOn        = false;  // top blue flashing
bool lightFlashSemaphor1 = true;  // top blue flashing (on/off semaphor)
bool lightFlashSemaphor2 = true;  // top blue flashing (on/off semaphor)
bool lightHeadTail       = false;  // front white, red tail
bool lightBreak          = true;  // tail breaklight (on at start)
bool lightBreakDistance  = false;  // tail breaklight if autobreaking
bool lightBlinkBothOn    = false;  // both sides yellow blinking
bool lightBlinkOff       = false;  // top yellow flashing (on/off semaphor)
bool lightBlinkLeftOn    = false;  // left side yellow blinking
bool lightBlinkRightOn   = false;  // right side yellow blinking

// Threads for lights:
Thread lightBlinkThread  = Thread();
Thread lightFlashThread1 = Thread();
Thread lightFlashThread2 = Thread();



void setup()
{
    Serial.begin(9600);     // 9600 initialize serial communication
    Serial.setTimeout(50);  // 1000 ms time out

    // Declare digital output pins:
    pinMode(controlPin1A, OUTPUT);      // 1A
    pinMode(controlPin2A, OUTPUT);      // 2A
    pinMode(motorEnablePin, OUTPUT);    // EN1,2
    pinMode(controlPin3A, OUTPUT);      // 3A
    pinMode(controlPin4A, OUTPUT);      // 4A
    pinMode(servoEnablePin, OUTPUT);    // EN3,4

    pinMode(controlPinLightData, OUTPUT);
    pinMode(controlPinLightStore, OUTPUT);
    pinMode(controlPinLightClock, OUTPUT);

    digitalWrite(motorEnablePin, LOW);  // motor off
    digitalWrite(servoEnablePin, LOW);  // steering centered

    lightBlinkThread.onRun(setLightsBlink);
    lightBlinkThread.setInterval(250);

    lightFlashThread1.onRun(setLightsFlash1);
    lightFlashThread1.setInterval(100);

    lightFlashThread2.onRun(setLightsFlash2);
    lightFlashThread2.setInterval(160);
}

void loop()
{
    // Get next command from serial bluetooth (add 1 byte for final 0)

    // array of type char (C-string) with null-termination (https://www.arduino.cc/en/Reference/String)
    char input[INPUT_SIZE + 1];

    //read Serial until new line or buffer full or time out
    byte commandSendSize = Serial.readBytesUntil('\n', input, INPUT_SIZE);

    // Add the final 0 to end the C-string
    input[commandSendSize] = 0;

    if ( commandSendSize > 0 )
    {
        char* command    = strtok(input, ",");        // RC

        int RCsteering   = atoi(strtok(NULL, ","));   // -100 (left).. 0 .. 100 (right)
        int RCspeed      = atoi(strtok(NULL, ","));   // -100 (full speed reverse) .. 0 (stop) .. 100 (full speed forward)

        int RClightWarn  = atoi(strtok(NULL, ","));   // both sides yellow blinking
        int RClightHead  = atoi(strtok(NULL, ","));   // front white, red tail
        int RClightFlash = atoi(strtok(NULL, ","));   // top yellow flashing

        int RCSensors    = atoi(strtok(NULL, ","));   // enable/disable distance sensors

        // Set motor and servo control variables
        if (RCsteering > 0)
            steeringDirection = 1;                      // turn right if RCsteering = 1..100
        else
            steeringDirection = 0;                      // turn left  if RCsteering = -100..0
        steering = int(2.55 * abs(RCsteering));      // Servo position 0..255 based on command 0..100

        if (RCspeed > 0)
            motorDirection = 1;                         // Set car in forward if RCspeed = 1..100
        else
            motorDirection = 0;                         // Set car in reverse if RCspeed = -100..0
        motorSpeed = int(2.55 * abs(RCspeed));       // Set speed 0..255 based on command 0..100

        if (RClightWarn > 0)
            lightBlinkBothOn = true;
        else
            lightBlinkBothOn = false;

        if (RClightHead > 0)
            lightHeadTail = true;
        else
            lightHeadTail = false;

        if (RClightFlash > 0)
            lightFlashOn = true;
        else
            lightFlashOn = false;

        if ( RCspeed >= -15 && RCspeed <= 15 )
            lightBreak = true;
        else
            lightBreak = false;


        if (RCsteering < -30)
            lightBlinkLeftOn = true;
        else
            lightBlinkLeftOn = false;

        if (RCsteering > 30)
            lightBlinkRightOn = true;
        else
            lightBlinkRightOn = false;
    }

    if ( lightBlinkThread.shouldRun() )
        lightBlinkThread.run();

    if ( lightFlashThread1.shouldRun() )
        lightFlashThread1.run();

    if ( lightFlashThread2.shouldRun() )
        lightFlashThread2.run();

    drawLights();
    SetMotorControl(); // adjust motor direction and speed
}



void setLightsBlink()
{
    if (lightBlinkOff == false)
    {
        lightBlinkOff = true;
    }
    else
    {
        lightBlinkOff = false;
    }
}



void setLightsFlash1()
{
    if (lightFlashSemaphor1 == false)
    {
        lightFlashSemaphor1 = true;
    }
    else
    {
        lightFlashSemaphor1 = false;
    }
}



void setLightsFlash2()
{
    if (lightFlashSemaphor2 == false)
    {
        lightFlashSemaphor2 = true;
    }
    else
    {
        lightFlashSemaphor2 = false;
    }
}


void drawLights()
{
    uint8_t result = ((lightFlashOn ? lightFlashSemaphor1 : false) << 7) | ((lightBlinkBothOn ? lightBlinkOff : (lightBlinkRightOn ? lightBlinkOff : false)) << 6) | ((lightBlinkBothOn ? lightBlinkOff : (lightBlinkLeftOn ? lightBlinkOff : false)) << 5) | (lightHeadTail  << 4) | (lightHeadTail << 3) | (lightHeadTail << 2) | ((lightBreak ? lightBreak : (lightBreakDistance ? lightBreakDistance : (motorSpeed <= 25 ? true : false))) << 1) | ((lightFlashOn ? lightFlashSemaphor2 : false) << 0);

    digitalWrite(controlPinLightStore, LOW);
    shiftOut(controlPinLightData, controlPinLightClock, MSBFIRST, result);
    //shiftOut(controlPinLightData, controlPinLightClock, LSBFIRST, result); // 2,12,16,32,64
    digitalWrite(controlPinLightStore, HIGH);
}



void SetMotorControl()
{
    /*
        L293 logic:    EN1,2   1A    2A
                       H       H     L    Motor turns left  (Forward; motorDirection == 1)
                       H       L     H    Motor turns right (Reverse; motorDirection == 0)

                       EN3,4   3A    4A
                       H       H     L    Servo turns left  (steeringDirection == 0)
                       H       L     H    Servo turns right (steeringDirection == 1)

        Motor speed:   PWM signal on EN1,2 (490 Hz; digital output value 0..255 for motorSpeed)
        Servo position:PWM signal on EN3,4 (490 Hz; digital output value 0..255 for position; 0 is straight ahead)
    */

    if (motorDirection == 1)
    {
        // set motor forward
        digitalWrite(controlPin1A, HIGH);
        digitalWrite(controlPin2A, LOW);
    }
    else
    {
        // set motor reverse
        digitalWrite(controlPin1A, LOW);
        digitalWrite(controlPin2A, HIGH);
    }
    // set motor speed
    analogWrite(motorEnablePin, motorSpeed);


    if (steeringDirection == 0)
    {
        // set servo left
        digitalWrite(controlPin3A, HIGH);
        digitalWrite(controlPin4A, LOW);
    }
    else
    {
        // set servo right
        digitalWrite(controlPin3A, LOW);
        digitalWrite(controlPin4A, HIGH);
    }
    // set servo position
    analogWrite(servoEnablePin, steering);
}


