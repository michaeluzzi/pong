#include <SPI.h>
#include <Ethernet.h>
#include <PinChangeInt.h> // necessary otherwise we get undefined reference errors.
#include <AdaEncoder.h>

// define rotary encoder pins
#define a_PINA 2
#define a_PINB 3
#define b_PINA 4
#define b_PINB 5

// initialize rotary encoder variables
int8_t clicks=0;
char id=0;

//mike
//byte mac[] = { 0x90, 0xa2, 0xda, 0x0d, 0x2a, 0x3d };
//IPAddress ip(128,122,151,149);

// mark
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x8E, 0x06};
IPAddress ip(128,122,151,192);

// Enter the IP address of the computer on which
// you'll run the pong server:
IPAddress server(128,122,151,164);

const int connectButton = 9;  // the pushbutton for connecting/disconnecting
const int connectionLED = 6;  // this LED indicates whether you're connected
const int sendInterval = 20;  // minimum time between messages to the server
const int debounceInterval = 15;  // used to smooth out pushbutton readings

EthernetClient client;               // instance of the Client class
int lastButtonState = 0;     // previous state of the pushbutton
long lastTimeSent = 0;       // timestamp of the last server message

boolean nameSent = false;

void setup()
{
  // initialize serial and Ethernet ports:
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  // establish contact with Processing
  establishContact();
  
  // initialize rotary encoders
  AdaEncoder::addEncoder('a', a_PINA, a_PINB);
  AdaEncoder::addEncoder('b', b_PINA, b_PINB);
 
  // initialize digital inputs and outputs:
  pinMode(connectButton, INPUT);
  pinMode(connectionLED, OUTPUT);
  
  delay(1000);      // give the Ethernet shield time to set up
  Serial.println("Starting");
}// end setup

void loop()
{
  // note the current time in milliseconds:
  long currentTime = millis();
  // check to see if the pushbutton's pressed:
  boolean buttonPushed = buttonRead(connectButton);
  
  // if the button's just pressed:
  if (buttonPushed)
  {
    // if the client's connected, disconnect:
    if (client.connected())
    {
      Serial.println("disconnecting");
      client.print("x");
      client.stop();
      nameSent = false;
    } // if the client's disconnected, try to connect:
    else
    {
      Serial.println("connecting");
      client.connect(server, 8080);
    }
  }
  
  // rotary encoder stuff, not sure exactly how this works
  encoder *thisEncoder;
  thisEncoder=AdaEncoder::genie(&clicks, &id);
  
  // if the client's connected, and the send interval has elapased:
  if (client.connected() && (currentTime - lastTimeSent > sendInterval))
  { 
    if (nameSent == false)
    {
      client.print("netch-a-sketch\n");
      nameSent = true;  
    } 
    // if an encoder changes value, send appropriate message to the server and/or Processing
    if (thisEncoder != NULL)
    {
      if (id == 'a')
      {
        if (clicks > 0)
        {
          client.print("r");
          if (Serial.available() > 0)
          {
             Serial.println("r");
          }
        }
        if (clicks < 0)
        {
          client.print("l");
          if (Serial.available() > 0)
          {
            Serial.println("l"); 
          }
        }
      }
      else if (id == 'b')
      {
        if (clicks > 0)
        {
          client.print("u");
          if (Serial.available() > 0)
          {
            Serial.println("u"); 
          }
        }
        if (clicks < 0)
        {
          client.print("d");
          if (Serial.available() > 0)
          {
            Serial.println("d"); 
          }
        }
      }
  }
    
    //save this moment as last time you sent a message:
    lastTimeSent = currentTime; 
  }

  // set the connection LED based on the connection state:
  digitalWrite(connectionLED, client.connected());
  
}// end loop

boolean buttonRead(int thisButton)
{
  boolean result = false;          
  // temporary state of the button:
  int currentState = digitalRead(thisButton);
  // final state of the button: 
  int buttonState = lastButtonState; 
  // get the current time to time the debounce interval:  
  long lastDebounceTime = millis();  

  while ((millis() - lastDebounceTime) < debounceInterval)
  {
    // read the state of the switch into a local variable:
    currentState = digitalRead(thisButton);

    // If the pushbutton changed due to noise:
    if (currentState != buttonState)
    {
      // reset the debouncing timer
      lastDebounceTime = millis();
    } 

    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState = currentState;
  }
  // if the button's changed and it's high:
  if(buttonState != lastButtonState && buttonState == HIGH)
  {
    result = true;
  }

  // save the current state for next time:
  lastButtonState = buttonState; 
  return result;
}// end buttonRead

// establishes serial connection with Processing
void establishContact()
{
  while (Serial.available() <= 0)
  {
    Serial.println("hello");   // send an initial string
    delay(300);
  }
}
