// This AccelerationLamp code by Greg Mayer is licensed under a Creative Commons Attribution 4.0 International License.
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
#include <Wire.h>     // 
#include <LSM303.h>   // for the LSM
#include <Adafruit_NeoPixel.h> // for the Neo Pixel strip
LSM303 lsm;     // something for the LSM
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
#define PIN 6
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// GLOBAL VARIABLES
int NPixels = 30; // Number of pixels in neo pixel strip (some have more than 30)
int MagInt1 = 0;
int MagInt2 = 0;
int MagInt3 = 0;
float MagFloat = 0.0;
char report[80];
int NElements = 300;
float MagArray[300]; // float arrays for x-mag
int On = 1;
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// ACCEL DATA NORMALIZATIONS
float AThresh = 1.4;  // Acceleration threshold for switching lights on/off
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
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void loop() {
  if (On == 0){
    for(int i=0; i<strip.numPixels(); i++) {    
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }  
    strip.show(); 
  }
  AN = Acceleration();
  if (AN >= AThresh) {
    if (On == 0) {
      On = 1;
      rainbowcompass(0,0,0);     
      delay(2000);
    }
    else {
      On = 0;
      delay(2000);
    }
  }

  if (On == 1) {

    MagFloat = 0.0;
    lsm.read(); // update LSM reading
    for (int N=0; N < (NElements-1); N++) {
      MagArray[N+1]=MagArray[N]; // Shuffle down one entry
    } 
    NMX = (lsm.m.x - MXMean)/MXLeng; // normalized x-component of mag;
    NMY = (lsm.m.y - MYMean)/MYLeng; // normalized x-component of mag;
    MagArray[0] = atan2(NMY,NMX);
    for (int N=0; N < NElements; N++) {
      MagFloat=MagFloat+MagArray[N]; // Sum for average
    } 
    MagFloat = MagFloat/NElements;  // Divide for taking average
    MagFloat = 25*MagFloat;  // Scale for smoothing out noise and controlling sensitivity
    MagInt1 = (int) MagFloat; // Cast to int for setting colour
//    MagFloat = 0.85*MagFloat;  // Divide for creating a set of slower lights
//    MagInt2 = (int) MagFloat; // Cast to int for setting colour
//    MagFloat = 0.75*MagFloat;  // Divide for creating a set of slower lights
//    MagInt3 = (int) MagFloat; // Cast to int for setting colour    

    rainbowcompass(MagInt1,MagInt1,MagInt1);     
  }
  delay(10);
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// COLOUR MODE FUNCTIONS
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void rainbowcompass(uint8_t C1, uint8_t C2, uint8_t C3) {
  uint16_t i;
  uint16_t j;
  uint16_t k;

  for(i=0; i<strip.numPixels(); i=i+3) {
    strip.setPixelColor(i, Wheel((C1) & 255)); 
  }
  for(j=1; j<strip.numPixels(); j=j+3) {
    strip.setPixelColor(j, Wheel((C2) & 255)); 
  }
  for(k=2; k<strip.numPixels(); k=k+3) {
    strip.setPixelColor(k, Wheel((C3) & 255)); 
  }
  strip.show();
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, 255 - WheelPos * 3, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color( WheelPos * 3, 0, 255 - WheelPos * 3);
  }
}

















