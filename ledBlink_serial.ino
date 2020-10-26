const int ledPin =  13;      // the number of the LED pin
 
// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
 
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)
 
void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);    
   Serial.begin(9600);  // initialize the uart interface with 9600 BR
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);  
}
 
void loop()
{
  // here is where you'd put code that needs to be running all the time.
   // check to see if it's time to blink the LED; that is, if the 
  // difference between the current time and last time you blinked 
  // the LED is bigger than the interval at which you want to 
  // blink the LED.
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
 
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
 
    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
  if (stringComplete) {
    //Serial.println(inputString);
    // clear the string:
    interval = inputString.toInt();
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    int inChar = (char)Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inputString += (char)inChar;
    }
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
