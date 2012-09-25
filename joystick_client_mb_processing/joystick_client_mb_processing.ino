/*
  Joystick client
 Language:  Arduino
 
 This program enables an Arduino to control one paddle 
 in a networked Pong game. 
 */

#include <SPI.h>
#include <Ethernet.h>

//byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };

//byte mac[] = { 0xc4, 0x2c, 0x03, 0x38, 0xbf, 0x1a };
byte mac[] = { 0x90, 0xa2, 0xda, 0x0d, 0x2a, 0x3d };
//IPAddress ip(192,168,1,20);
//IPAddress ip(172,29,131,90);
//IPAddress ip(172,20,151,218);
//IPAddress ip(172,20,151,203);
//IPAddress ip(128,122,151,79);
IPAddress ip(128,122,151,149);

// Enter the IP address of the computer on which 
// you'll run the pong server:
//IPAddress server(192,168,1,100); 
IPAddress server(128,122,151,164); 

const int connectButton = 9;  // the pushbutton for connecting/disconnecting
const int connectionLED = 6;  // this LED indicates whether you're connected
const int leftLED = 4;        // this LED indicates that you're moving left
const int rightLED = 5;       // this LED indicates that you're moving right
const int sendInterval = 20;  // minimum time between messages to the server
const int debounceInterval = 15;  // used to smooth out pushbutton readings

int sensorValue1 = 0;
int sensorValue2 = 0;

//const int left = 880;         // threshold for the joystick to go left   
//const int right = 891;        // threshold for the joystick to go right
//const int up = 891;
//const int down = 880;

EthernetClient client;               // instance of the Client class for connecting
//Client client;
int lastButtonState = 0;     // previous state of the pushbutton
long lastTimeSent = 0;       // timestamp of the last server message

int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

void setup()
{
  // initialize serial and Ethernet ports:
  Ethernet.begin(mac, ip);
  //Serial.begin(9600);
  Serial.begin(115200);
  establishContact();
  // initialize digital inputs and outputs:
  pinMode(connectButton, INPUT);
  pinMode(connectionLED, OUTPUT);
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
  
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);

  delay(1000);      // give the Ethernet shield time to set up
  Serial.println("Starting");
}

void loop()
{
  // note the current time in milliseconds:
  long currentTime = millis();
  // check to see if the pushbutton's pressed:
  boolean buttonPushed = buttonRead(connectButton);

  // if the button's just pressed:
  if (buttonPushed) {
    // if the client's connected, disconnect:
    if (client.connected()) {
      Serial.println("disconnecting");
      client.print("x");
      client.stop();
    } // if the client's disconnected, try to connect:
    else {
      Serial.println("connecting");
      client.connect(server, 8080);
    }
  }

  // if the client's connected, and the send interval has elapased:
  if (/*client.connected() && */(currentTime - lastTimeSent > sendInterval)) {   
    
    
    //LEFT RIGHT
    int  currentsensor1Value = analogRead(A0);
    //Serial.println(currentsensor1Value);
   
    if (currentsensor1Value < (sensorValue1 - 5)) {    // moving left
      client.print("l");
      digitalWrite(leftLED, HIGH);
      if (Serial.available() > 0)
      {
        Serial.println("l"); 
      }
    }

    if (currentsensor1Value > (sensorValue1 + 5)) {    // moving right
      client.print("r");
      digitalWrite(rightLED, HIGH);
      if (Serial.available() > 0)
      {
        //Serial.println("here is some stuff");
        Serial.println("r"); 
      }
    }
    
    
    //up and down
    int  currentsensor2Value = analogRead(A1);

    if (currentsensor2Value > (sensorValue2 + 5)) //moving up
    {
      client.print("u");
      if (Serial.available() > 0)
      {
        //Serial.println("here is some stuff");
        Serial.println("u"); 
      }
      
       
    }
    if (currentsensor2Value < (sensorValue2 - 5))
    {
      client.print("d");
      if (Serial.available() > 0)
      {
        //Serial.println("here is some stuff");
        Serial.println("d"); 
      }
       
    }
    
    sensorValue1 = currentsensor1Value;
    sensorValue2 = currentsensor2Value;
    
    if (encoderValue < (lastencoderValue)) {    // moving left
      client.print("l");
      digitalWrite(leftLED, HIGH);
      if (Serial.available() > 0)
      {
        //Serial.println("here is some stuff");
        Serial.println("l"); 
      }
    }

    if (encoderValue > (lastencoderValue)) {    // moving right
      client.print("r");
      digitalWrite(rightLED, HIGH);
      if (Serial.available() > 0)
      {
        //Serial.println("here is some stuff");
        Serial.println("r"); 
      }
    }
    
    lastencoderValue = encoderValue;

    //save this moment as last time you sent a message:
    lastTimeSent = currentTime; 
  }

  // set the connection LED based on the connection state:
  digitalWrite(connectionLED, client.connected());
}
/*
 // if there's incoming data from the client, print it serially:
 if (client.available()) {
 char c = client.read();
 Serial.write(c);
 }
 
 */



// this method reads the button to see if it's just changed
// from low to high, and debounces the button in case of
// electrical noise:

boolean buttonRead(int thisButton) {
  boolean result = false;          
  // temporary state of the button:
  int currentState = digitalRead(thisButton);
  // final state of the button: 
  int buttonState = lastButtonState; 
  // get the current time to time the debounce interval:  
  long lastDebounceTime = millis();  

  while ((millis() - lastDebounceTime) < debounceInterval) {
    // read the state of the switch into a local variable:
    currentState = digitalRead(thisButton);

    // If the pushbutton changed due to noise:
    if (currentState != buttonState) {
      // reset the debouncing timer
      lastDebounceTime = millis();
    } 

    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState = currentState;
  }
// if the button's changed and it's high:
  if(buttonState != lastButtonState && buttonState == HIGH) {
    result = true;
  }

  // save the current state for next time:
  lastButtonState = buttonState; 
  return result;
}

void updateEncoder()
{
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}

void establishContact()
{
  while (Serial.available() <= 0)
  {
    Serial.println("hello");   // send an initial string
    delay(300);
  }
}


