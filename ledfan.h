/*
 * Animation Configuration Structs
 */
 // sine_wave = {phase, loop_delay, brightness, hue, rotation, saturation, frequency, cutoff, bg_color, bg_brightness, velocity, backwards}
typedef struct sine_wave {
  int phase;                // value of phase change
  int loop_delay;           // animation loop delay
  uint8_t brightness;       // light intensity, 0-255
  uint8_t hue;              // color (based on color wheel, 0-255)
  uint8_t rotation;         // rate of color rotation
  uint8_t saturation;       // color saturation
  uint8_t frequency;        // width of wave
  uint8_t cutoff;           // width cutoff (lower == longer wave)
  uint8_t bg_color;         // background hue
  uint8_t bg_brightness;    // background brightness
  int8_t velocity;          // wave animation speed
  bool backwards;         // change animation direction
} SineWave;


// rainbow = {palette, blending, brightness, steps, velocity}
typedef struct rainbow {
  CRGBPalette16 palette; //color palette
  TBlendType blending; //color blending profile
  uint8_t brightness, steps; // light intensity
  int8_t velocity;    // animation speed
} Rainbow;


// ripple = {0, -1, hue, saturation, bg_color_rotation, brightness, velocity, max_steps}
typedef struct ripple {
  int center, hue_step; //0, -1
  uint8_t hue, saturation, bg_color_rotation, brightness, velocity, max_steps; //randomized, MAX, 50, MAX, 10, 16
} Ripple;


// color = {hue, saturation, brightness}
typedef struct color {
  uint8_t hue, saturation, brightness; //MAX, MAX, 0
} SolidColor;


/*
 * Environment Constants
 */
#define CLOCK_PIN 3       // Clock Pin
#define LED_PIN 4         // Data Pin
#define BUTTON_PIN 5      // Mode selection buttion
#define NUM_LEDS 28       // Change to reflect the number of LEDs you have
#define COLOR_ORDER GBR   // Green, Blue, Red
#define NUM_MODES 3       // Number of different mode programs
#define MAX 255           // Max value for many animation parameters
#define MIN 0             // Min value for many animation parameters
#define MAX_STEPS 16

CRGB leds[NUM_LEDS];      //naming our LED array
int ledMode = 0;          //Starting animation mode
TBlendType current_blending; //color blending profile

