#include <FastLED.h>
#include <Math.h>

/*
   Animation Configuration Structs
*/
// sine_wave = {phase, loop_delay, brightness, hue, rotation, saturation, frequency, cutoff, bg_color, bg_brightness, velocity, backwards}
typedef struct sine_wave {
  int phase;                // value of phase change
  int loop_delay;           // animation loop delay
  uint8_t brightness;       // light intensity, 0-255
  uint8_t hue;              // color (based on color wheel, 0-255)
  uint8_t saturation;       // color saturation
  uint8_t rotation;         // rate of color rotation
  uint8_t frequency;        // width of wave
  uint8_t cutoff;           // width cutoff (lower == longer wave)
  uint8_t bg_color;         // background hue
  uint8_t bg_brightness;    // background brightness
  int8_t velocity;          // wave animation speed
  bool backwards;           // change animation direction
} SineWave;



// rainbow = {palette, blending, brightness, steps, velocity}
typedef struct rainbow {
  CRGBPalette16 palette; //color palette
  TBlendType blending; //color blending profile
  uint8_t brightness, steps; // light intensity
  int8_t velocity;    // animation speed
} Rainbow;


typedef struct ripple {
  int8_t center, hue_step; //0, -1
  uint8_t min_hue, max_hue, hue, brightness, saturation, r_delay, max_steps;
} Ripple;


// color = {hue, saturation, brightness}
typedef struct color {
  uint8_t hue, saturation, brightness; //MAX, MAX, 0
} SolidColor;


typedef struct stripe {
  uint8_t index, width, hue, saturation, brightness, hue_delta, velocity;
} Stripe;


/*
   Environment Constants
*/
#define CLOCK_PIN 3       // Clock Pin
#define LED_PIN 4         // Data Pin
#define BUTTON_PIN 5      // Mode selection buttion
#define NUM_LEDS 28       // Change to reflect the number of LEDs you have
#define COLOR_ORDER BGR   // GBR, RGB, BRG
#define NUM_MODES 3       // Number of different mode programs
#define MAX 255           // Max value for many animation parameters
#define MIN 0             // Min value for many animation parameters
#define MAX_STEPS 16

CRGB leds[NUM_LEDS];      //naming our LED array
int ledMode = 0;          //Starting animation mode
TBlendType current_blending; //color blending profile


/*
 *  ***Initialize animation model structs ****

*/
// color = {hue, saturation, brightness}
SolidColor color_0 = {150, 200, 150};
SolidColor color_off = {0, 0, 0};

// sine_wave = {phase, loop_delay, brightness, hue, rotation, saturation, frequency, cutoff, bg_color, bg_brightness, velocity, backwards}
SineWave sine_wave_1 = {0, 20, 150, 100, 200, 10, 200, 192, 130, 100, 4, 0};
SineWave sine_wave_2 = {5, 20, 150, 100, 200, 10, 200, 192, 130, 100, 4, 0};
SineWave sine_wave_3 = {10, 20, 150, 100, 200, 10, 200, 192, 130, 100, 4, 0};

Rainbow rainbow_ripple      = {(CRGBPalette16)RainbowColors_p, LINEARBLEND, 150, 16, 10};
Ripple fast_purple_ripple   = {0, 1, 100, 220, 0, 150, 200, 30, 5};
Ripple slow_warm_ripple     = {0, 1, 0, 90, 100, 150, 200, 60, 3};
Stripe rainbow_stripe       = {0, 8, 50, 200, 100, 5, 40};
/*
    END MODELS
*/



void setup() {
  delay(2000); // power-up safety delay
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  FastLED.addLeds<DOTSTAR, LED_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  current_blending = LINEARBLEND;

}

/*
   Run The Animation Programs
*/
void loop() {
  // If button pressed (signal LOW), prepare to change selected program
  if (digitalRead(BUTTON_PIN) == LOW) {
    //FastLED.clearData();
    // Waiting for button release
    while (digitalRead(BUTTON_PIN) == LOW) {
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      delay(100);
    }

    ledMode++;  // Rotate to next LED animation mode (wraps back to 0)
    if (ledMode > NUM_MODES) {
      ledMode = 0;
    }
  }

  switch (ledMode) {
    case 0:
      animate_sine_wave(&sine_wave_1);
      break;
    case 1:
      animate_sine_wave(&sine_wave_2);
      break;
    case 2:
      animate_sine_wave(&sine_wave_3);
      break;
  }
}


/*
   Solid Color Animation
*/
void animate_color(color* c) {
  fill_solid(leds, NUM_LEDS, CHSV(c->hue, c->saturation, c->brightness));
  FastLED.setBrightness(c->brightness);
  FastLED.show();
}


/*
   Sine Wave Animation
*/
void animate_sine_wave(sine_wave* w) {
  fill_sine_leds(w);
  show_at_max_brightness_for_power();
  delay_at_max_brightness_for_power(w->loop_delay * 2.5);
}

void fill_sine_leds(sine_wave* w) {
  if (!w->backwards) {
    w->phase += w->velocity;
  } else {
    w->phase -= w->velocity;
  }

  w->hue += w->rotation;

  for (int i = 0; i < NUM_LEDS; i++) {
    w->brightness = cubicwave8((i * w->frequency) + w->phase) > w->cutoff ? w->brightness : 0;

    leds[i] = CHSV(w->bg_color, w->saturation, w->bg_brightness);
    leds[i] += CHSV(w->hue, w->saturation, w->brightness);                               // Assigning hues and brightness to the led array.
  }
  w->bg_color++;
}


/*
   Rainbow Animation
*/
void animate_rainbow(rainbow* r) {
  static uint8_t i = 0;
  i++;

  FastLED.setBrightness(r->brightness);
  FillLEDsFromPaletteColors(r, i);
  FastLED.show();
  FastLED.delay(10);
}

/*
   Generate Rainbow LED Pattern
*/
void FillLEDsFromPaletteColors(rainbow* r, uint8_t color_idx) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(r->palette, color_idx, r->brightness, r->blending);
    color_idx += r->steps;
  }
}

void animate_ripple(ripple* r) {
  fill_ripple(r);
  show_at_max_brightness_for_power();
  delay_at_max_brightness_for_power(r->r_delay);
}

void fill_ripple(ripple* r) {
  if (r->hue > r->max_hue || r->hue < r->min_hue) {
    r->hue = random16(r->min_hue, r->max_hue);
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(r->hue + r->hue_step, r->saturation, r->brightness);  // Rotate background color.
    FastLED.show();
    delay(r->r_delay);

    switch (r->hue_step) {
      case -1:
        r->center = random(NUM_LEDS);
        r->hue = random16(r->min_hue, r->max_hue);
        r->hue_step = 1;
        break;
      case 0:
        leds[r->center] = CHSV(r->hue++, r->saturation, r->brightness);              // Display the first pixel of the ripple.
        r->hue_step++;
        break;
      case MAX_STEPS:
        r->hue_step = -1;
        break;
      default:
        leds[wrap(r->center + r->hue_step)] += CHSV(r->hue++, r->saturation, r->brightness); // Display the next pixels in the range for one side.
        leds[wrap(r->center - r->hue_step)] += CHSV(r->hue++, r->saturation, r->brightness); // Display the next pixels in the range for the other side.
        r->hue_step++;
        break;
    }
  }
  delay(r->r_delay);
}


int wrap(int step) {
  if (step < 0) {
    return NUM_LEDS + step;
  } else if (step > NUM_LEDS - 1) {
    return step - NUM_LEDS;
  }
  return step;
}

void animate_stripe(stripe* s) {
  fill_stripes(s);
}

void color_change_stripe(stripe* s) {
  if (s->width >= NUM_LEDS) {
    s->width = NUM_LEDS - 1;
  }

  fill_stripes(s);
}

void fill_stripes(stripe* s) {
  int fill_pos, clear_pos;

  for (int i = s->index; i < NUM_LEDS; i++) {
    fill_pos = i;
    s->hue += s->hue_delta;

    if (fill_pos > NUM_LEDS) {
      fill_pos -= NUM_LEDS;
    }

    clear_pos = fill_pos - s->width;

    leds[fill_pos] = CHSV(s->hue, s->saturation, s->brightness);
    FastLED.show();

    if (clear_pos >= NUM_LEDS) {
      clear_pos -= NUM_LEDS;
    } else if (clear_pos < 0) {
      clear_pos += NUM_LEDS;
    }

    leds[clear_pos] = 0;
    delay(s->velocity);
  }
}

void clear_stripe(stripe* s) {
  int clear_pos = s->index - s->width;

  for (int j = 0; j < s->width; j++) {
    if (clear_pos >= NUM_LEDS) {
      clear_pos -= NUM_LEDS;
    } else if (clear_pos < 0) {
      clear_pos += NUM_LEDS;
    }

    leds[clear_pos] = CRGB::Black;
    FastLED.show();
    clear_pos++;
    delay(s->velocity);
  }

}

