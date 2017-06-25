

#include <Adafruit_GFX.h>
#include <fast_SSD1306.h>

#include <FastLED.h>
FASTLED_USING_NAMESPACE

// Bunch of XBM based icons
#include "swicons.h"
// Galactic Basic Font
#include "aurebesh5.h"
#define GALACTICBASIC &Aurebesh_Condensed5pt7b

// SSD1306
fast_SSD1306 display(0);

// ws2812
#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    8
#define MAX_BRIGHTNESS 60
CRGB leds[NUM_LEDS];

const char filler1[] PROGMEM = "Lorem ipsum dolor sit amet";
const char filler2[] PROGMEM = "Conse adipisci elit";
const char filler3[] PROGMEM = "Sed do eiusmod tempor incididunt ut labore et dolore";
const char filler4[] PROGMEM = "Ut enim ad minim veniam, quis nostrud exercit ullamco";
const char filler5[] PROGMEM = "Duis aute dolor in reprehend in voluptate velit esse";
const char filler6[] PROGMEM = "Excepteur sint occaecat cupidatat non proident, sunt";

// Random text for contract sheets
const char* const filler[] PROGMEM = { filler1, filler2, filler3, filler4, filler5, filler6 };

// Buffer to hold data pulled out of PROGMEM
char txt[60];

// timers
unsigned long lastLed = 0;
unsigned long lastDisp = 0;


// globals
uint8_t gHue = 0;
uint8_t gCurrentPatternNumber = 0;
uint8_t gCurrentDispPatternNumber = 0;

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
typedef uint8_t (*SimpleDispPatternList[])();

void red_rain();
void bpm();
void sinelonGlitter();
//void twobytwo();

uint8_t dummy();
uint8_t hWipeDown();
uint8_t hWipeUp();
uint8_t about();
uint8_t crest();
uint8_t yarin();
uint8_t bounty();
uint8_t radar();
uint8_t target();


SimplePatternList gPatterns = { red_rain, bpm, sinelonGlitter};
SimpleDispPatternList gDispPatterns = { target, hWipeDown, dummy, crest, hWipeUp, yarin, hWipeDown,
                                        bounty, hWipeDown, dummy, radar, hWipeUp, bounty, hWipeUp, dummy, hWipeDown, bounty,
                                        hWipeDown, bounty, dummy, target, hWipeDown, bounty, hWipeDown, dummy,
                                        bounty, hWipeDown, bounty, dummy, radar, hWipeUp, dummy
                                      };

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  display.drawXBitmap(0, 0, jaing_head_49_64_bits, 49, 64, 1);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  about();
  delay(1000);
}

#define LEDMILLIS 1000/120
#define DISPMILLIS 1000/24

void loop() {
  unsigned long curMillis = millis();

  unsigned long sinceLed = curMillis - lastLed;
  unsigned long sinceDisp = curMillis - lastDisp;
  unsigned long wait_time = LEDMILLIS;

  if ( sinceLed > LEDMILLIS ) {
    // do LED frame
    gPatterns[gCurrentPatternNumber]();
    FastLED.show();
    lastLed = curMillis;
  } else {
    wait_time = min(LEDMILLIS - sinceLed, wait_time);
  }

  if ( sinceDisp > DISPMILLIS ) {
    // do display frame
    if ( display.is_done() ) {
      if (gDispPatterns[gCurrentDispPatternNumber]()) {
        nextDispPattern();
      }
      lastDisp = curMillis;
      digitalWrite(13, 0);
    } else {
      digitalWrite(13, 1);
    }
  } else {
    wait_time = min(wait_time, DISPMILLIS - sinceDisp);
  }

  // do some periodic updates
  EVERY_N_MILLISECONDS( 30 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
  EVERY_N_SECONDS( 30 ) {
    nextPattern();  // change patterns periodically
  }
  delay( min(wait_time - 1, 0) );
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))



long hWipeDownLine = -1;
uint8_t hWipeDown(void) {
  if ( hWipeDownLine == -1 ) {
    hWipeDownLine = 0;
  }
  long i = hWipeDownLine - 3;
  if (i >= 0) display.drawFastHLine(0, i, 128, BLACK);
  i++;
  if (i >= 0) display.drawFastHLine(0, i, 128, BLACK);
  i++;
  if (i >= 0) display.drawFastHLine(0, i, 128, BLACK);
  i++;
  if (i < 64) display.drawFastHLine(0, i, 128, WHITE);
  i++;
  if (i < 64) display.drawFastHLine(0, i, 128, WHITE);
  i++;
  if (i < 64) display.drawFastHLine(0, i, 128, WHITE);
  i++;
  display.display();
  hWipeDownLine = i;
  if ( hWipeDownLine > 64 ) {
    //end condition
    hWipeDownLine = -1;
    return 1;
  }
  return 0;
}

long hWipeUpLine = -1;
uint8_t hWipeUp(void) {
  if ( hWipeDownLine == 1000 ) {
    hWipeDownLine = 63;
  }
  long i = hWipeDownLine - 3;
  if (i >= 0) display.drawFastHLine(0, i, 128, WHITE);
  i++;
  if (i >= 0) display.drawFastHLine(0, i, 128, WHITE);
  i++;
  if (i >= 0) display.drawFastHLine(0, i, 128, WHITE);
  i++;
  if (i < 64) display.drawFastHLine(0, i, 128, BLACK);
  i++;
  if (i < 64) display.drawFastHLine(0, i, 128, BLACK);
  i++;
  if (i < 64) display.drawFastHLine(0, i, 128, BLACK);
  i++;
  display.display();
  hWipeDownLine -= 3;
  if ( hWipeDownLine < 0 ) {
    //end condition
    hWipeDownLine = 1000;
    return 1;
  }
  return 0;
}

unsigned long frame_count = 0;

uint8_t dummy() {
  display.clearDisplay();
  uint8_t *buffer = display.get_buffer();
  uint8_t rand1;
  for ( long x = 0; x < 1024; ) {
    rand1 = random8();
    buffer[x++] = rand1;
    buffer[x++] = rand1;
    buffer[x++] = rand1;
    buffer[x++] = rand1;
  }
  display.display();
  return (frame_count++ > 12);
}

uint8_t about(void) {
  if ( frame_count == 0 ) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.clearDisplay();
    display.println("Animation Demo");
    display.println(" by Eric Warnke");
    display.println(" ericew@gmail.com");
    display.display();
  }
  return (frame_count++ > 24 * 3);
}

uint8_t crest(void) {
  if ( frame_count == 0 ) {
    display.clearDisplay();
    display.drawXBitmap(0, 0, jaing_head_49_64_bits, 49, 64, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(64, 28);
    display.print("Vosh");
    display.setCursor(64, 36);
    display.print(" R'ainer");
    display.setCursor(64, 64 - 8);
    display.print("OM #1801");
    display.display();
  }
  return (frame_count++ > 24 * 6);
}

uint8_t yarin(void) {
  if ( frame_count == 0 ) {
    display.clearDisplay();
    display.setFont(&Aurebesh_Condensed5pt7b);
    display.drawXBitmap(0, 18, planets_128_48_bits, 128, 48, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 8);
    display.setTextWrap(0);
    display.print("Sanning System");
    display.display();
    display.setFont();
  }
  return (frame_count++ > 24 * 6);
}

uint8_t quote(void) {
  if ( frame_count == 0 ) {
    display.clearDisplay();
    display.setTextWrap(1);
    //display.setFont(&mandalor7pt7b);
    display.setCursor(4, 4);
    display.println("Ba\'jur, beskar\'gam, ara\'nov, aliit, Mando\'a bal Oya\'la kot. An vencuyan mhi.");
    display.setCursor(4, 64 - 12);
    display.print(" - Commander Rex");
    display.display();
    display.setFont();
  }
  return (frame_count++ > 24 * 6);
}

uint8_t bounty(void) {
  if ( frame_count == 0 ) {
    display.clearDisplay();
    display.setTextWrap(1);
    display.setFont(&Aurebesh_Condensed5pt7b);
    display.drawXBitmap(0, 0, icons[random(6)], 32, 32, 1);
    display.setCursor(38, 10);
    display.print("  Contract");
    display.setCursor(38, 20);
    display.print("  $");
    display.print(random(10000) * 1000);
    display.setCursor(38, 34);
    int f = random(6);
    strcpy_P(txt, (char*)pgm_read_word(&(filler[f])));
    display.println(txt);
    display.display();
    display.setFont();
  }
  return (frame_count++ > 24 * 6);
}

uint8_t tracking[3][4];
uint8_t r_r = 0;

uint8_t radar(void) {
  int x, tx, ty, ts, td;
  if (frame_count == 0) {
    // init points
    r_r = 0;
    for (x = 0; x < 3; x++) {
      tx = random(128);
      ty = random(64);
      ts = random(10);
      td = sqrt16( sq( abs8(64 - tx)) + sq( abs8(64 - ty)));
      tracking[x][0] = tx;
      tracking[x][1] = ty;
      tracking[x][2] = ts;
      tracking[x][3] = td;
    }
    display.setFont(&Aurebesh_Condensed5pt7b);
  }
  display.clearDisplay();
  display.drawCircle(64, 64, r_r, WHITE);
  for (x = 0; x < 3; x++) {
    td = tracking[x][3];
    if ( r_r > td ) {
      tx = tracking[x][0];
      ty = tracking[x][1];
      ts = tracking[x][2];
      display.fillCircle(tx, ty, ts, WHITE);
      display.setCursor(min(tx + ts + 3, 100), max(ty + 2, 8));
      display.print(td);
    }
  }
  display.display();
  r_r += 1;
  if (r_r > 91) {
    r_r = 0;
  }
  return (frame_count++ > 24 * 10);
}

uint8_t target(void) {
  if ( frame_count == 0 ) {
    display.setFont();
    display.setTextSize(1);
    display.setTextColor(WHITE);
  }
  display.clearDisplay();
  display.drawFastVLine(0, 16, 32, WHITE);
  display.drawFastVLine(63, 0, 64, WHITE);
  display.drawFastVLine(127, 0, 64, WHITE);
  display.drawFastHLine(0, 31, 128, WHITE);
  display.drawFastHLine(117, 0, 10, WHITE);
  display.drawFastHLine(117, 63, 10, WHITE);
  display.drawFastHLine(123, 16, 5, WHITE);
  display.drawFastHLine(123, 48, 5, WHITE);
  display.drawCircle(63, 31, 10, WHITE);
  display.drawCircle(63, 31, 20, WHITE);
  display.drawCircle(63, 31, 31, WHITE);
  display.setCursor(0,0);
  display.print("243.5");
  // Left bottom dot
  if ( frame_count % 8 < 2 ) {
    display.fillRect(4, 50, 8, 8, WHITE);
  }
  // Right up and down
  display.fillRect(120,scale8(sin8(frame_count*2),48)+6,6,6,WHITE);
  // Sweep
  display.drawLine(63,31,32+(cos8(frame_count*2)>>2),sin8(frame_count*2)>>2, WHITE);
  display.display();
  return (frame_count++ > 24 * 10);
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void nextDispPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentDispPatternNumber = (gCurrentDispPatternNumber + 1) % ARRAY_SIZE( gDispPatterns);
  frame_count = 0;
  display.setFont();
}

void addGlitterRed( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::Red;
  }
}

void addGlitterBlue( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::Blue;
  }
}

void sinelonGlitter()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
  addGlitterRed(4);
  addGlitterBlue(4);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 40;
  CRGBPalette16 palette = LavaColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i), beat - gHue + (i * 20));
  }
}

uint8_t rain_frame = 0;

void red_rain() {
  // Slow down a lot
  if (rain_frame++ % 8 != 0) {
    return;
  }

  memmove8(&leds[0], &leds[1], (NUM_LEDS - 1)*sizeof( CRGB ));
  if ( random8() < 40 ) {
    leds[NUM_LEDS - 1] += CHSV(160 + random8() / 2, 255, 255);
  } else {
    leds[NUM_LEDS - 1].fadeToBlackBy(beatsin8(55, 50, 255));
  }
}


