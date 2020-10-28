//reference
//https://electrosome.com/switch-arduino-uno/#:~:text=In%20this%20example%2C%20a%20push,DOWN%20resistors%20while%20interfacing%20switch.
/*
 1 rev= 2048 steps
 2 ms = 1 step ...1sec= 512 steps
 1 rev=2048 steps= 4 sec
 no. of rotations per mins= 15 rotations(speed)

 */

#include <Stepper.h>
const int stepsPerRevolution = 200;   //2048 max
const int buttonPin = 4; 
int buttonState = 0;
const int ledPin = 2; //LED Connected to PIN 2
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
void setup() {
 
  myStepper.setSpeed(60);    //number of rotations/min
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
}
void loop() {
  buttonState = digitalRead(buttonPin);// if button is HIGH Led will glow
 if (buttonState == HIGH)
  {digitalWrite(ledPin, HIGH); //LED ON
        delay(3000); //3 Second Delay
        digitalWrite(ledPin, LOW); //LED OFF
        }
 if (buttonState == LOW) {     //if button is LOW stepper motor will rotate
     
  Serial.println("counterclockwise");
  myStepper.step(-200);
 }
  }
