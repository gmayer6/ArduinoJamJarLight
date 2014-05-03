// This AccelerationLamp code by Greg Mayer is licensed under a Creative Commons Attribution 4.0 International License.
// Some portions of this program is based on code made available by Adafruit
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
#include <Wire.h>     // 
#include <Time.h>
#include <LSM303.h>   // for the LSM
#include <Adafruit_NeoPixel.h> // for the Neo Pixel strip
LSM303 lsm;     // something for the LSM
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
#define PIN 6
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// GLOBAL VARIABLES
int NPixels = 30; // Number of pixels in neo pixel strip (some have more than 30)
int MagInt1 = 0;
float MagFloat = 0.0;
char report[80];
int NElements = 300;
float MagArray[300]; // float arrays for x-mag
int LightsOn = 0;
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// ACCEL DATA NORMALIZATIONS
float OffThreshold = 0.075;
float AThresh = 1.65;  // Acceleration threshold for switching lights on/off
float AN; // Normalized acceleration
// Maximum and minimum accelrations recorded by the LSM303. These values may be specific to your device but can be read off from the serial monitor using the LSM303 Test sketch. 
float AXMin = -16000;
float AXMax = 16000;
float AYMin = -15000;
float AYMax = 15000;
float AZMin = -19000;
float AZMax = 19000;
// These are parameters that will be used to normalize meaured accelerations
float AXMean = (AXMax + AXMin)/2;
float AYMean = (AYMax + AYMin)/2;
float AZMean = (AZMax + AZMin)/2;
float AXLeng = (AXMax - AXMin)/2;
float AYLeng = (AYMax - AYMin)/2;
float AZLeng = (AZMax - AZMin)/2;
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// MAG DATA NORMALIZATIONS
// Maximum and minimum magnometer data recorded by the LSM303. These values can be read off from the serial monitor using the LSM303 Test sketch. 
float MXMin = -450;
float MXMax = 450;
float MYMin = -450;
float MYMax = 450;
// These are parameters that will be used to normalize meaured mag data
float MXMean = (MXMax + MXMin)/2;
float MYMean = (MYMax + MYMin)/2;
float MXLeng = (MXMax - MXMin)/2;
float MYLeng = (MYMax - MYMin)/2;
float NMX;
float NMY;
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NPixels, PIN, NEO_GRB + NEO_KHZ800);
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void setup() {
  Serial.begin(9600);
  Wire.begin();
  // NEOPIXEL STRIP SETUP
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  // LSM SETUP
  lsm.init();
  lsm.enableDefault();  
  // Set initial mags to zero
  for (int N=0; N < NElements; N++) {
    MagArray[N]=0.0;
  }
  LightsOn = 0; // initially, all lights are off

}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void loop() {

  if (LightsOn == 0){ // set all lights to off
    for(int i=0; i<strip.numPixels(); i++) {    
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }  
    strip.show(); // set lights to off
  }

  LightsOn = LightSwitch(LightsOn);

  if (LightsOn == 1) {
    MagneticRainbow(200);
  }
  delay(10);
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FUNCTIONS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ACCELERATION FUNCTION
float Acceleration() {   // Measure acceleration
  lsm.read();
  float XN = (lsm.a.x - AXMean)/AXLeng; // normalized x-component of acceleration
  float YN = (lsm.a.y - AYMean)/AYLeng; // normalized y-component of acceleration
  float ZN = (lsm.a.z - AZMean)/AZLeng; // normalized z-component of acceleration
  float AN = sqrt(pow(XN,2)+pow(YN,2)+pow(ZN,2));  // normalized magnitude of acceleration vector, should be close to 1 if chip is stationary
  return AN;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// RAINBOW COMPASS
void rainbowcompass(uint8_t C1, uint8_t C2, uint8_t C3) {
  uint16_t i;
  uint16_t j;
  uint16_t k;

  for(i=0; i<strip.numPixels(); i=i+1) {
    strip.setPixelColor(i, Wheel((C1) & 255, 0)); 
  }
  strip.show();
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// COLOUR WHEEL
// This is a modified version of Adafruit's Colour Wheel function
// Input a value 0 to 255 and a brightness scaling factor to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos,float B) {
  if(WheelPos < 85) {
    return strip.Color(B*WheelPos * 3, B*(255 - WheelPos * 3), 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(B*(255 - WheelPos * 3), 0, B*WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, B* WheelPos * 3, B*(255 - WheelPos * 3));
  }
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MAGNETIC RAINBOW
void MagneticRainbow(uint8_t wait) {
  uint16_t i, C;
  float B; // brightness

  for(C=0; C<256; C++) {

    MagFloat = 0.0;
    lsm.read(); // update LSM reading
    // ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  
    // SMOOTHING OPERATION
    // Magnetometer readings have some noise, so we can average over NElements measurements
    for (int N=0; N < (NElements-1); N++) {  
      MagArray[N+1]=MagArray[N]; // Shuffle down one entry
    } 
    // ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  
    // UPDATE DIRECTION
    NMX = (lsm.m.x - MXMean)/MXLeng; // normalized x-component of mag;
    NMY = (lsm.m.y - MYMean)/MYLeng; // normalized x-component of mag;
    MagArray[0] = atan2(NMY,NMX);
    // ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  
    // SMOOTHING OPERATION (a moving average filter)
    for (int N=0; N < NElements; N++) {
      MagFloat=MagFloat+MagArray[N]; // Sum for average
    } 
    MagFloat = MagFloat/NElements;  // Divide by number of measurements, finds average direction
    // ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~      
    // NORMALIZE THE DIRECTION NUMBER
    // We want to scale our direction number to the interval [0,1] to control brightness
    // The inverse tangent produces a number between -π and +π
    // So we can divide approximately by pi to get a number on [-1,+1]
    // Adding 1 and dividing by 2 "should" give us a number on the interval [0,1]
    // It is possible that numbers will go outside of [0,1] because MXMin, MXMax, etc, may be incorrect
    // I've divided by something a bit bigger than 2 to be sure that we don't go outside the interval [0,1]
    MagFloat = (MagFloat/3.14159 + 1)/2.01; 
    if (MagFloat<OffThreshold) {
      MagFloat = 0;
    } 
    Serial.println(MagFloat);  

    LightsOn = LightSwitch(LightsOn); // check the acceleration to see if lights need to be switched on/off

    if (LightsOn=1){  // if the lights should be on, change their colours
      for(i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel((i+C) & 255,MagFloat));
      }
      strip.show();
      delay(wait);
    }
  }
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LIGHT SWITCH
int LightSwitch(int LightsOnLocal) {
  AN = Acceleration(); // measure acceleration

    if (AN >= AThresh) { // if acceleration above threshold, then ...
    if (LightsOnLocal == 0) { // if lights are off, turn the lights on
      LightsOnLocal = 1;  // set switch to "on"
      delay(500);
    }
    else {  // if the lights are on, turn the lights off
      LightsOnLocal = 0;   // set switch to "off"
      rainbowcompass(0,0,0);     // set lights to off
      delay(500);
    }
  }
  return LightsOnLocal;
}















