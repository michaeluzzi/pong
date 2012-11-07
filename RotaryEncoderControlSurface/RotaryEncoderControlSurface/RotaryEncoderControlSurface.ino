#include <PinChangeInt.h> // necessary otherwise we get undefined reference errors.
#include <AdaEncoder.h>

#define a_PINA 2
#define a_PINB 3


int8_t clicks=0;
char id=0;

void setup()
{
  Serial.begin(9600);
  AdaEncoder::addEncoder('a', a_PINA, a_PINB);
}

void loop()
{
  encoder *thisEncoder;
  thisEncoder=AdaEncoder::genie(&clicks, &id);
  if (thisEncoder != NULL) {
//    Serial.print(id); Serial.print(':');
    if (clicks > 0) {
      Serial.println("GO RIGHT");
    }
    if (clicks < 0) {
       Serial.println("GO LEFT");
    }
  }
}
