
// This example demonstrates continuous conversion mode using the
// DRDY pin to check for conversion completion.

#include <Adafruit_MAX31856.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

// milliseconds to delay after each main loop iteration
#define LOOP_DELAY 1000
// set up thermocouple definitions
#define TC_DRDY 14
#define TC_CS 10

// define relay control pin
#define RELAY 7

// define hall effect sensor pin for measuring furnace circuit
#define FURNACE_CIRCUIT A1

// set up tft definitions
// This is related to the screen
#define TFT_CS        9
#define TFT_DC        8
#define TFT_RST       -1 // if -1, short tft reset to artuino reset pin
#define TFT_W         160
#define TFT_H         128
// TFT_W and TFT_H should be flipped if screen orientation is changed

#define TEXT_BUF_SIZE 8 // 4 digits + 1 decimal point + 1 decimal number + 1 spot for a null character
char tft_out_buf[TEXT_BUF_SIZE];

#define BG_COLOR ST77XX_BLACK
#define TXT_COLOR ST77XX_WHITE

// use hardware SPI, just pass in the CS pin
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(TC_CS);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// String object to print to the screen
String tft_text("NA");

void setup_tft() {
  Serial.println(F("Beginning ST7735 TFT initialization"));
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(3);
  tft.fillScreen(BG_COLOR);
  tft.setTextSize(2);
  Serial.println(F("TFT initialized."));
}

void setup_thermocouple() {
  Serial.println("Beginning MAX31856 thermocouple initialization");
  pinMode(TC_DRDY, INPUT);
  while (!maxthermo.begin()) {
    Serial.print(".");
    delay(10);
  }
  maxthermo.setThermocoupleType(MAX31856_TCTYPE_K);
  maxthermo.setConversionMode(MAX31856_CONTINUOUS);
  Serial.println("Thermocouple initialized.");
}

void setup_relay() {
  Serial.println("Beginning relay initialization");
  pinMode(RELAY, OUTPUT);
  Serial.println(relay_on());
  delay(1000);
  Serial.println(relay_off());
  Serial.println("Relay initialized.");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Beginning initialization.");
  while (!Serial) {
    delay(10);
  }
  setup_relay();
  setup_tft();
  setup_thermocouple();
  Serial.println("Setup initialized.");
}

int relay_off() {
  digitalWrite(RELAY, LOW);
  Serial.println("relay turned off");
  return(digitalRead(RELAY));
}

int relay_on() {
  digitalWrite(RELAY, HIGH);
  Serial.println("relay turned on");
  return(digitalRead(RELAY));
}

void display_furnace_status(uint16_t color) {
  int furn=analogRead(FURNACE_CIRCUIT);
  Serial.print("Furnace: ");
  Serial.println(furn);
  
  // clear the old text from the screen (overwrite with bg color)
  tft.setCursor(0, TFT_H/2-7);
  tft.setTextColor(BG_COLOR, BG_COLOR);
  tft.print(tft_out_buf);
  
  // now write the new value
  
  dtostrf(furn, TEXT_BUF_SIZE-1, 1, tft_out_buf);
  tft.setCursor(0, TFT_H/2-7);
  tft.setTextColor(color, BG_COLOR);
  tft.print(tft_out_buf);
}

void display_temperature(uint16_t color) {
  // clear the old text from the screen (overwrite with bg color)
  tft.setCursor(0, TFT_H/2-7);
  tft.setTextColor(BG_COLOR, BG_COLOR);
  tft.print(tft_out_buf);
  
  // put value from thermocouple into TFT output buffer
  dtostrf(maxthermo.readThermocoupleTemperature(), TEXT_BUF_SIZE-1, 1, tft_out_buf);

  // now write the new value
  tft.setCursor(0, TFT_H/2-7);
  tft.setTextColor(color, BG_COLOR);
  tft.print(tft_out_buf);
  Serial.println(tft_out_buf);
}

void loop() {
  // The DRDY output goes low when a new conversion result is available
  int count = 0;
  while (digitalRead(TC_DRDY)) {
    if (count++ > 200) {
      Serial.print(".");
      count = 0;
    }
  }
  display_temperature(TXT_COLOR);
  delay(LOOP_DELAY);
  display_furnace_status(TXT_COLOR);
  delay(LOOP_DELAY);
}
