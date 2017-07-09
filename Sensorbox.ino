#include <Arduino.h>
#include <TM1637Display.h>
#include <VirtualWire.h>
#include <NewPing.h>

//Distanzsensor
#define trigPin A0
#define echoPin A1
#define MAX_DISTANCE 200
NewPing sonar(trigPin, echoPin, MAX_DISTANCE);

//Display
#define CLK A3
#define DIO A4
TM1637Display display(CLK, DIO);

//RF-Transmitter
char Sensor1CharMsg[4];
long distance;
static unsigned long timer = millis();

void setup() {
  //Distanzsensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //RF-Transmitter
  vw_set_ptt_inverted(false);
  vw_set_tx_pin(A2);
  vw_setup(4000); // speed of data transfer Kbps

  //Display
  display.setBrightness(0x0f);
}

void loop() {
  if (millis() >= timer) {
    distance = sonar.ping_cm();
    itoa(distance, Sensor1CharMsg, 10);
    display.showNumberDec(distance, false, 4, 0);
    vw_send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));
    vw_wait_tx();
    timer += 50;
  }
}
