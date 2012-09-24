#include <PinChangeInt.h> // necessary otherwise we get undefined reference errors.
#include <AdaEncoder.h>

#define a_PINA 2
#define a_PINB 3
#define b_PINA 11
#define b_PINB 12

int8_t clicks=0;
char id=0;

void setup()
{
  Serial.begin(115200);
  AdaEncoder::addEncoder('a', a_PINA, a_PINB);
  AdaEncoder::addEncoder('b', b_PINA, b_PINB);  
}

void loop()
{
  encoder *thisEncoder;
  thisEncoder=AdaEncoder::genie(&clicks, &id);
  if (thisEncoder != NULL) {
    Serial.print(id); Serial.print(':');
    if (clicks > 0) {
      Serial.println(" CW");
    }
    if (clicks < 0) {
       Serial.println(" CCW");
    }
  }
  else
  {
    //Serial.println("encoder null");  
  }
}

