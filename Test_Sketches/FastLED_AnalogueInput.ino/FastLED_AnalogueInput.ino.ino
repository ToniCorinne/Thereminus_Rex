//FastLED_AnalogueInput.ino

/*
   Using a potentiometer to control colour, brightness and speed.
 Wire up as per http://arduino.cc/en/Tutorial/AnalogInput
 You can connect the wiper, to any analogue input pin, and 
 adjust the settings below.
 You will need three 10k potentiometers.
 */


#include <FastLED.h>


#define LED_PIN 34              // which pin are LEDS connected to?
#define NUM_LEDS 78
#define COLOR_ORDER RGB
#define LED_TYPE WS2812        // i'm using WS2811s, FastLED supports lots of different types.

/* 
 set your desired minimum and maxium brigtness settings here.
 Valid values are 0 - 255
 With 0 being fully dim, or not lit, and 255 being fully on.
 Therefore half power, or 50%, would be 128
 */

#define MAX_BRIGHTNESS 120     // Thats full on, watch the power!
#define MIN_BRIGHTNESS 10     // set to a minimum of 25%

const int brightnessInPin = A14;  // The Analog input pin that the brightness control potentiometer is attached to.
const int speedInPin = A12;       // Analog input pin that the speed control potentiometer is attached to.
const int colourInPin = A13;      // The Analog input pin that the colour control potentiometer is attached to.

struct CRGB leds[NUM_LEDS];

void setup() {
  delay(3000); // in case we do something stupid. We dont want to get locked out.

  LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(MAX_BRIGHTNESS);
}

void loop() {
  // read the analog brightness value:
  //int brightValue = analogRead(brightnessInPin);            
  // map it to the range of the FastLED brightness:
  int mappedValue = map(analogRead(brightnessInPin), 0, 1023, 0, 255);

  /* 
   At this point, brightness could be full off (mappedValue == 0)
   or it could be fully on (mappedValue == 255).
   if you are ruuning from a battery pack, or in a dark room, you 
   may not want full brightness.
   Or if you are in daylight, you may not want the pixels to go out.
   the following code, checks if mappedValue is above or below our defined
   brightness settings above.
   It works like this.
   
   we get mappedValue: if mappedValue is between MIN_BRIGHTNESS and MAX_BRIGHTNESS.
   we get MIN_BRIGHTNESS: if mappedValue is less than our defined MIN_BRIGHTNESS.
   we get MAX_BRIGHTNESS: if mappedValue is greater than our defined MAX_BRIGHTNESS
   
   so, it limits range of brightness values.
   
   */

  //int outputValue = constrain(mappedValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS);

  // now we set the brightness of the strip
  FastLED.setBrightness(constrain(mappedValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS));

  // read the analog speed value:          
  // map it to a value used in delay();
  int delayValue = map(analogRead(speedInPin), 0, 1023, 0, 50);  

  int mappedHue;
  // read the analog brightness value:
  //int hueValue = analogRead(colourInPin);            
  // map it to the range of the FastLED brightness:



  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    mappedHue = map(analogRead(colourInPin), 0, 1023, 0, 255);
    // Set the i'th led to the chosen colour
    leds[i] = CHSV(mappedHue, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Black;
    // Wait a little bit before we loop around and do it again
    delay(delayValue);  
  }


  // Now go in the other direction.  
  for(int i = NUM_LEDS-1; i >= 0; i--) {
    mappedHue = map(analogRead(colourInPin), 0, 1023, 0, 255);
    // Set the i'th led to the chosen colour 
    leds[i] = CHSV(mappedHue, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Black;
    // Wait a little bit before we loop around and do it again
    delay(delayValue);  
  }



}

