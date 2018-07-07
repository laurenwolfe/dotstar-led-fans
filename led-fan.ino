#include <FastLED.h>
#include "ledfan.h"

/* 
 *  ***Initialize animation model structs ****
 *  
 */
// sine_wave = {phase, loop_delay, brightness, hue, rotation, saturation, frequency, cutoff, bg_color, bg_brightness, velocity, backwards}
SineWave sine_wave_1 = {0, 4, 255, 30, 1, 255, 25, 192, 0, 55, 4, 0};

// rainbow = {palette, blending, brightness, steps, velocity}
Rainbow rainbow_0 = {(CRGBPalette16)RainbowColors_p, LINEARBLEND, 150, 70, 150};
Rainbow rainbow_1 = {(CRGBPalette16)RainbowColors_p, LINEARBLEND, 150, 16, 30};
Rainbow rainbow_2 = {(CRGBPalette16)RainbowColors_p, LINEARBLEND, 150, 40, 100};

// ripple = {0, -1, hue, saturation, bg_color_rotation, brightness, velocity, max_steps}
Ripple ripple_1 = {0, -1, 140, 255, 150, 150, 16, 10};

// color = {hue, saturation, brightness}
SolidColor color_1 = {150, 200, 150};


void setup() {
  delay(2000); // power-up safety delay
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  FastLED.addLeds<DOTSTAR, LED_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  current_blending = LINEARBLEND;

}

/*
 * Run The Animation Programs
 */
void loop() {
  // If button pressed (signal LOW), prepare to change selected program
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Waiting for button release
    while (digitalRead(BUTTON_PIN) == LOW) {
      //FastLED.setBrightness(MIN);
      //FastLED.show();
      delay(100);
    }

    ledMode++;  // Rotate to next LED animation mode (wraps back to 0)
    if(ledMode > NUM_MODES) {
      ledMode = 0;
    }
  }

  switch (ledMode) {
    case 0:
//      FastLED.setBrightness(sine_wave_1.brightness);
//      FastLED.show();
//      animate_sine_wave(&sine_wave_1);
      animate_rainbow(&rainbow_0);

      break;
    case 1:
//      FastLED.setBrightness(ripple_1.brightness);
//      FastLED.show();
      animate_ripple(&ripple_1);
      break;
    case 2:
//      FastLED.setBrightness(color_1.brightness);
//      FastLED.show();
      animate_rainbow(&rainbow_1);
      break;
    case 3:
//      FastLED.setBrightness(rainbow_1.brightness);
//      FastLED.show();
      animate_rainbow(&rainbow_2);
      break;
  }
}


/*
 * Solid Color Animation
 */
void animate_color(color* c) {
  fill_solid(leds, NUM_LEDS, CHSV(c->hue, c->saturation, c->brightness));
  FastLED.setBrightness(c->brightness);
  FastLED.show();
}


/*
 * Sine Wave Animation
 */
void animate_sine_wave(sine_wave* w) {
  fill_sine_leds(w);
  show_at_max_brightness_for_power();
  delay_at_max_brightness_for_power(w->loop_delay * 2.5);
}

void fill_sine_leds(sine_wave* w) {
  if(!w->backwards) {
    w->phase += w->velocity;
  } else {
    w->phase -= w->velocity;
  }

  w->hue += w->rotation;

  for (int i = 0; i < NUM_LEDS; i++) {
    w->brightness = cubicwave8((i * w->frequency) + w->phase) > w->cutoff ? w->brightness : 0;

    leds[i] = CHSV(w->bg_color, MAX, w->bg_brightness);
    leds[i] += CHSV(w->hue, w->saturation, w->brightness);                               // Assigning hues and brightness to the led array.
  }
    w->bg_color++;
}


/*
 * Rainbow Animation
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
 * Generate Rainbow LED Pattern
 */
void FillLEDsFromPaletteColors(rainbow* r, uint8_t color_idx) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(r->palette, color_idx, r->brightness, r->blending);
    color_idx += r->steps;
  }
}


/*
 * Ripple Animation
 */
 /* HERE IS PROBLEM */
void animate_ripple(ripple* r) {
  r->hue = 140; 
  r->hue++;
  if(r->hue > 220) {
    r->hue = 140;
  }
   
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(r->hue++, r->saturation, r->bg_color_rotation);  // Rotate background color.
  }

  switch(r->hue_step) {
    case -1:
      r->center = random(NUM_LEDS);
      r->hue = random16(MIN, MAX + 1);
      r->hue_step = 0;
      break;
    case 0:
      leds[r->center] = CHSV(r->hue, r->saturation, r->bg_color_rotation);              // Display the first pixel of the ripple.    
      r->hue_step++;
      break;
    case MAX_STEPS:
      r->hue_step = -1;
      break;
    default:
      leds[wrap(r->center + r->hue_step)] += CHSV(r->hue, r->saturation, r->bg_color_rotation / r->hue_step * 2); // Display the next pixels in the range for one side.
      leds[wrap(r->center - r->hue_step)] += CHSV(r->hue, r->saturation, r->bg_color_rotation / r->hue_step * 2); // Display the next pixels in the range for the other side.
      r->hue_step++;
      break;
  }
  show_at_max_brightness_for_power();
  delay_at_max_brightness_for_power(10 * 2.5);  
}

int wrap(int step) {
  if(step < 0) {
    return NUM_LEDS + step;
  } else if(step > NUM_LEDS - 1) {
    return step - NUM_LEDS;
  }
  return step;
}

