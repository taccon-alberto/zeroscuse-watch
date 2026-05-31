#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// PIN CODECELL C3
#define SENSOR_PIN 3

#define TFT_CS    6
#define TFT_DC    5
#define TFT_RST   7

#define TFT_MOSI  8
#define TFT_SCLK  9

Adafruit_ST7789 tft = Adafruit_ST7789(
  TFT_CS,
  TFT_DC,
  TFT_RST
);

int x = 120;
  int y = 90;

float offsetADC = 0;

// DA TARARE CON MISURE REALI
const float SOGLIA_PPM = 6.0;

// Sensibilità teorica calcolata
const float PPM_PER_ADC = 0.0806;

// ---------------------
// LETTURA ADC FILTRATA
// ---------------------

float readADC()
{
  long sum = 0;

  for(int i = 0; i < 64; i++)
  {
    sum += analogRead(SENSOR_PIN);
    delay(2);
  }

  return (float)sum / 64.0;
}


void showDanger(float ppm)
{
   // faccia piena
  tft.fillCircle(x, y, 52, ST77XX_BLACK);
 
  // occhi
  tft.fillCircle(x - 18, y - 15, 6, ST77XX_ORANGE);
  tft.fillCircle(x + 18, y - 15, 6, ST77XX_ORANGE);



  // guance
  // tft.fillCircle(x - 30, y + 5, 4, ST77XX_BLUE);
 //  tft.fillCircle(x + 30, y + 5, 4, ST77XX_BLUE);
  // outline
  for (int i = 0; i<5; i++)
  tft.drawCircle(x, y, 54+i, ST77XX_ORANGE);
// BOCCA
tft.fillRect(
  x - 30,
  y + 20 ,
  60,
  5,
  ST77XX_ORANGE
);

 
  // =========================
  // TESTO CENTRATO
  // =========================

  tft.setTextColor(ST77XX_ORANGE);
  tft.setTextSize(3);

  String testo = "ATTENZIONE";
 

  int16_t x1, y1;
  uint16_t w, h;

  // calcola dimensione testo
  tft.getTextBounds(testo, 0, 0, &x1, &y1, &w, &h);

  // centra orizzontalmente
  int textX = (240 - w) / 2;

  // posizione verticale
  int textY = 210;

  tft.fillRect(0, 200, 240, 80, ST77XX_BLACK);

  tft.setCursor(textX, textY);
  tft.println(testo);
 
  tft.println(ppm,1);
}

void showRed(float ppm) {
 // faccia piena
  tft.fillCircle(x, y, 52, ST77XX_BLACK);

  // occhi
  tft.fillCircle(x - 18, y - 15, 6, ST77XX_RED);
  tft.fillCircle(x + 18, y - 15, 6, ST77XX_RED);

  // bocca triste più morbida
// =========================
// BOCCA TRISTE TONDA
// =========================

// arco spesso
for (int i = 0; i < 5; i++) {

  tft.drawCircle(
    x,
    y + 40,     // centro più in basso
    20 + i,
    ST77XX_RED
  );
}

// nasconde metà inferiore del cerchio
tft.fillRect(
  x - 50,
  y + 35,
  100,
  50,
  ST77XX_BLACK
);

  // guance
  // tft.fillCircle(x - 30, y + 5, 4, ST77XX_BLUE);
 //  tft.fillCircle(x + 30, y + 5, 4, ST77XX_BLUE);
  // outline
  for (int i = 0; i<5; i++)
  tft.drawCircle(x, y, 54+i, ST77XX_RED);
  // =========================
  // TESTO CENTRATO
  // =========================

  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(3);

  String testo = "NON GUIDARE";

  int16_t x1, y1;
  uint16_t w, h;

  // calcola dimensione testo
  tft.getTextBounds(testo, 0, 0, &x1, &y1, &w, &h);

  // centra orizzontalmente
  int textX = (240 - w) / 2;

  // posizione verticale
  int textY = 210;

  tft.fillRect(0, 200, 240, 80, ST77XX_BLACK);

  tft.setCursor(textX, textY);
  tft.println(testo);
  tft.println(ppm,1);
}

void showGreen(float ppm) {
 
   // faccia piena
  tft.fillCircle(x, y, 52, ST77XX_BLACK);
  // bocca triste più morbida
// =========================
// BOCCA SORRIDENTE
// =========================

// spessore bocca
for (int i = 0; i < 5; i++) {

  tft.drawCircle(
    x,
    y ,   // centro cerchio
    30 + i,   // raggio
    ST77XX_GREEN
  );
}

// copre metà superiore
tft.fillRect(
  x - 50,
  y - 35,
  100,
  55,
  ST77XX_BLACK
);
  // occhi
  tft.fillCircle(x - 18, y - 15, 6, ST77XX_GREEN);
  tft.fillCircle(x + 18, y - 15, 6, ST77XX_GREEN);



  // guance
  // tft.fillCircle(x - 30, y + 5, 4, ST77XX_BLUE);
 //  tft.fillCircle(x + 30, y + 5, 4, ST77XX_BLUE);
  // outline
  for (int i = 0; i<5; i++)
  tft.drawCircle(x, y, 54+i, ST77XX_GREEN);
  // =========================
  // TESTO CENTRATO
  // =========================

  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);

  String testo = "PUOI GUIDARE";

  int16_t x1, y1;
  uint16_t w, h;

  // calcola dimensione testo
  tft.getTextBounds(testo, 0, 0, &x1, &y1, &w, &h);

  // centra orizzontalmente
  int textX = (240 - w) / 2;

  // posizione verticale
  int textY = 210;

  tft.fillRect(0, 200, 240, 80, ST77XX_BLACK);

  tft.setCursor(textX, textY);
  tft.println(testo);
  tft.println(ppm);
}


void setup() {

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  analogReadResolution(12);
  tft.init(240, 280);
  Serial.begin(115200);
  tft.enableDisplay(true);
  tft.writeCommand(ST77XX_SLPOUT);
  delay(120);
  tft.writeCommand(ST77XX_DISPON);

  tft.setRotation(0);

  tft.fillScreen(ST77XX_BLACK);

  // centro display
 
  // =========================
  // FACCIA
  // =========================

 
  long sum = 0;

  for(int i = 0; i < 500; i++)
  {
    sum += analogRead(SENSOR_PIN);
    delay(5);
  }

  offsetADC = sum / 500.0;

  Serial.print("Offset ADC = ");
  Serial.println(offsetADC);
}

void loop()
{
  float adc = readADC();

  float adcNet = adc - offsetADC;

  if(adcNet < 0)
    adcNet = 0;

  float ppm = adcNet * PPM_PER_ADC;

  Serial.print("ADC = ");
  Serial.print(adc);
  Serial.print("   PPM = ");
  Serial.println(ppm,2);
  if(ppm < SOGLIA_PPM)
  {
    showGreen(ppm);
  }
  else
  {
    showRed(ppm);
  }
  delay(1000);
}
