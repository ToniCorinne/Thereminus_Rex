#include <FastLED.h>


#define LED_PIN 34              // which pin are LEDS connected to?
#define NUM_LEDS 78
#define COLOR_ORDER GRB
#define LED_TYPE WS2812B

/* 
 set your desired minimum and maxium brigtness settings here.
 Valid values are 0 - 255
 With 0 being fully dim, or not lit, and 255 being fully on.
 Therefore half power, or 50%, would be 128
 */

#define MAX_BRIGHTNESS 110     // 
#define MIN_BRIGHTNESS 5       // 

const int brightnessInPin = A14;  // The Analog input pin that the brightness control potentiometer is attached to.
const int speedInPin = A12;       // Analog input pin that the speed control potentiometer is attached to.
const int patternInPin = A13;      // The Analog input pin that the colour control potentiometer is attached to.

int delayValue;
int mappedValue;
int mappedHue;
int lastHue = 255;

CRGB leds[NUM_LEDS];


CRGBPalette16 currentPalette;
TBlendType    currentBlending;


extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(MAX_BRIGHTNESS);
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
}

void loop() {
    mappedValue = map(analogRead(brightnessInPin), 0, 1023, 0, 255);
    FastLED.setBrightness(constrain(mappedValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS));
    ChangePalette();
    
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors( startIndex);
    
    FastLED.show();
    delayValue = map(analogRead(speedInPin), 0, 1023, 1, 250);
    FastLED.delay(1000 / delayValue);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void ChangePalette()
{
    mappedHue = map(analogRead(patternInPin), 0, 1023, 0, 255);

    if( lastHue != mappedHue) {
        lastHue = mappedHue;
        if(0 <= mappedHue && mappedHue< 23)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if(23 <= mappedHue && mappedHue < 46)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if(46 <= mappedHue && mappedHue < 70)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if(70 <= mappedHue && mappedHue < 93)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if(93 <= mappedHue && mappedHue < 117)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if(117 <= mappedHue && mappedHue < 140)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if(140 <= mappedHue && mappedHue < 163)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if(163 <= mappedHue && mappedHue < 187)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if(187 <= mappedHue && mappedHue < 210)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if(210 <= mappedHue && mappedHue < 233)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if(233 <= mappedHue && mappedHue <= 255)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

