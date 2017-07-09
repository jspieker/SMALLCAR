#include "FastLED.h"
#include <VirtualWire.h>

//Lichtdisplay initialisieren
#define DATA_PIN 4
#define CLOCK_PIN 5
#define COLOR_ORDER BGR
#define LED_TYPE APA102         //DotStar
#define ALL_LEDS 114
#define LINKE_SEITE_HINTEN 0
#define LINKE_SEITE_MITTE 11
#define LINKE_SEITE_VORNE 22
#define ARMATUR_LINKS 22
#define ARMATURENBRETT_LINKS 22
#define ARMATURENBRETT_RECHTS 26
#define ARMATUR_RECHTS 38
#define RECHTE_SEITE_VORNE 39
#define RECHTE_SEITE_MITTE 51
#define RECHTE_SEITE_HINTEN 60
#define RUECKSCHEIBE_RECHTS 60
#define RUECKSCHEIBE_LINKS 72
#define RECHTS_OBEN_VORNE 72
#define RECHTS_OBEN_HINTEN 93
#define LINKS_OBEN_HINTEN 93
#define LINKS_OBEN_VORNE 113
#define FULL_LENGTH 999
uint8_t max_bright = 64;
struct CRGB leds[ALL_LEDS];   //LED-Array initialisieren
uint8_t thisdelay = 5;        //Intervall
int led;

//RF - Receiver initialisieren
#define rfReceivePin A0
char distanceCharArray[4];  // RF Übertragungscontainer
int currentDistance;

//Nutzerkonfiguration
CHSV myColorHSV(160, 128, 255);
CRGB red(255, 0, 0);
CRGB green(0, 255, 0);
CRGB blue(0, 0, 255);
CRGB yellow(255, 255, 0);
CRGB white = CRGB::White;

void setup() {
  //FastLED setup
  LEDS.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, ALL_LEDS);
  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 500);
  FastLED.clear();

  //RF-Receiver
  vw_set_ptt_inverted(true);
  vw_set_rx_pin(6);
  vw_setup(4000);
  vw_rx_start();             // RF-Empfaenger starten
  pinMode(13, OUTPUT);       // Onboard-LED als Statusanzeige
} // setup()


void loop () {
  dynamicLightPattern(2, 10); //Zum verwenden der Sensorbox. Attribute: Wieviele cm pro LED, Offset

  bremsMuster();
} // loop()

void bremsMuster() {
  dynamicBrightness(0, 25);
  displayColor(ARMATUR_LINKS, ARMATUR_RECHTS, red);
  blinkLeds(ARMATUR_LINKS, ARMATUR_RECHTS, 5, 50, red); //Von wo, nach wo, Distanz ab der Blinken einsetzen soll, Blinkintervall in ms, Farbe
} //bremsMuster()

void ueberholMuster() {
  //dynamicLightPattern(2, 10); //Attribut: Wieviele cm pro LED, Offset

  dynamicLine(ARMATUR_LINKS, ARMATUR_RECHTS - 5, 5, white, yellow);
  dynamicLine(ARMATUR_RECHTS, ARMATUR_LINKS + 5, 5, white, yellow);
} //ueberholMuster()

void displayColor(int from, int to, CRGB color1) {
  fill_gradient_RGB (leds, from, color1, to, color1);
  FastLED.show();
} //displayColor()

void blinkLeds(int from, int to, int startDistance, int delayTime, CRGB color) {
  startDistance -= 10;
  startDistance = startDistance / 2;

  if (currentDistance <= startDistance) {
    for (int i = from; i <= to; i++) {
      leds[i] = false;
    }
    FastLED.delay(delayTime);
    for (int i = from; i <= to; i++) {
      leds[i] = color;
    }
    FastLED.delay(delayTime);
    FastLED.show();
  }
} //blinkLeds()

void dynamicBrightness(int lowerBound, int higherBound) {
  int mapped = map(currentDistance, lowerBound, higherBound, 50, 0);

  if (currentDistance <= higherBound) {
    FastLED.setBrightness(mapped);
  } else {
    FastLED.setBrightness(0);
  }
  FastLED.show();
} //dynamicBrightness();

void dynamicLine(int from, int to, int lengthOfLine, CRGB color1, CRGB color2) {
  if (from < to) {
    int newFrom = from + currentDistance;
    int newTo = to + currentDistance; //wird für variable Lichtmusterlänge benötigt
    if (led != newFrom) { //Wenn eingehende und bereits leuchtende LED dieselbe sind, nicht erneut updaten (verhindert flackern)
      for (int i = from; i < to; i++) {

        if (i <= (to - currentDistance)) {
          fill_gradient_RGB (leds, from, color1, i, color2);
          //leds[i] = color;

          if (lengthOfLine != FULL_LENGTH) { //Länge des zu leuchtenden Streifens limitieren
            for (int j = (i - lengthOfLine); j >= from; j--) {
              leds[j] = false;
            }
          }
        } else {
          leds[i] = false;
        }
      }
    }
  } else {
    from--;
    int newTo = to + currentDistance - 1;

    for (int i = from; i >= to; i--) {
      if (i < newTo) {
        leds[i] = false;
      } else {
        //leds[i] = color;
        fill_gradient_RGB (leds, from, color1, i, color2);

        if (lengthOfLine != FULL_LENGTH) { //Länge des zu leuchtenden Streifens limitieren
          for (int j = (i + lengthOfLine); j <= from; j++) {
            leds[j] = false;
          }
        }
      }
    }
  }

  show_at_max_brightness_for_power();
} //dynamicLine()


void dynamicDot() {
  if (led != currentDistance) { //Wenn eingehende und bereits leuchtende LED dieselbe sind, nicht erneut updaten (verhindert flackern)
    leds[led] = CRGB::Black;
    FastLED.show();
    led = currentDistance; //Entsprechende LED soll leuchten

    if (led <= LINKS_OBEN_VORNE) {
      //fill_gradient (leds, 0, CHSV(100, 187, 255), buf[0], CHSV(200, 187, 255), SHORTEST_HUES);
      leds[led] = CRGB::Blue;
    }
  }
  show_at_max_brightness_for_power();
} //dynamicDot()

void led_test() {
  static uint8_t hue = 0;
  FastLED.showColor(CHSV(hue++, 255, 255));
} // led_test()


void dynamicLightPattern(int divisor, int offset) {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  EVERY_N_MILLISECONDS(thisdelay) { // FastLED based non-blocking routine to update/display the sequence.
    if (vw_get_message(buf, &buflen)) { // Non-blocking
      digitalWrite(13, true); //Onboard-LED an, wenn Nachricht empfangen

      for (int i = 0; i < buflen; i++) { //Nachricht mit richtiger Checksum
        distanceCharArray[i] = char(buf[i]);
      }

      distanceCharArray[buflen] = '\0'; //Array nullterminieren, da Probleme auftreten, wenn die neue Distanz mehr Ziffern als die vorherige hat
      currentDistance = atoi(distanceCharArray); //Zu Integer konvertieren
      currentDistance -= offset;
      currentDistance = currentDistance / divisor;
    }
  }
} //dynamicLightPattern()
