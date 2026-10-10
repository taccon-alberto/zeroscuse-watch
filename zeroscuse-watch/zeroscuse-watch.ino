
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

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

int x = 120;
int y = 90;

float offsetADC = 0;

// SOGLIE PROVVISORIE - DA CALIBRARE
const float SOGLIA_ARANCIONE = 3.0;
const float SOGLIA_ROSSA = 6.0;

// Sensibilita teorica - da verificare
const float PPM_PER_ADC = 0.0806;

// Colore arancione RGB565
const uint16_t COLOR_ORANGE = 0xFD20;

// Gestione dello stato
enum ScreenState {
  STATE_NONE,
  STATE_GREEN,
  STATE_ORANGE,
  STATE_RED
};

ScreenState currentState = STATE_NONE;
float lastPpmDisplayed = -1.0;

// ---------------------
// LETTURA ADC FILTRATA
// ---------------------

float readADC()
{
  long sum = 0;

  for (int i = 0; i < 64; i++)
  {
    sum += analogRead(SENSOR_PIN);
    delay(2);
  }

  return (float)sum / 64.0;
}

// ---------------------
// TESTO CENTRATO
// ---------------------

void drawCenteredText(const char* testo, uint16_t color)
{
  tft.setTextColor(color, ST77XX_BLACK);
  tft.setTextSize(3);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds(testo, 0, 0, &x1, &y1, &w, &h);

  int textX = (240 - w) / 2 - x1;
  int textY = 185;

  tft.setCursor(textX, textY);
  tft.print(testo);
}

// ---------------------
// FACCINA ROSSA
// ---------------------

void showRed(float ppm)
{
  // Pulisce l'area della faccia
  tft.fillCircle(x, y, 60, ST77XX_BLACK);

  // Bocca triste
  for (int i = 0; i < 5; i++) {
    tft.drawCircle(x, y + 40, 20 + i, ST77XX_RED);
  }

  // Copre la meta inferiore della bocca
  tft.fillRect(x - 40, y + 38, 80, 30, ST77XX_BLACK);

  // Occhi
  tft.fillCircle(x - 18, y - 15, 6, ST77XX_RED);
  tft.fillCircle(x + 18, y - 15, 6, ST77XX_RED);

  // Contorno della faccia
  for (int i = 0; i < 5; i++) {
    tft.drawCircle(x, y, 54 + i, ST77XX_RED);
  }

  drawCenteredText("NON PUOI GUIDARE", ST77XX_RED);
}

// ---------------------
// FACCINA ARANCIONE
// ---------------------

void showOrange(float ppm)
{
  // Pulisce l'area della faccia
  tft.fillCircle(x, y, 60, ST77XX_BLACK);

  // Bocca neutra - linea orizzontale
  for (int i = 0; i < 5; i++) {
    tft.drawFastHLine(x - 22, y + 28 + i, 44, COLOR_ORANGE);
  }

  // Occhi
  tft.fillCircle(x - 18, y - 15, 6, COLOR_ORANGE);
  tft.fillCircle(x + 18, y - 15, 6, COLOR_ORANGE);

  // Contorno della faccia
  for (int i = 0; i < 5; i++) {
    tft.drawCircle(x, y, 54 + i, COLOR_ORANGE);
  }

  drawCenteredText("ATTENZIONE", COLOR_ORANGE);
}

// ---------------------
// FACCINA VERDE
// ---------------------

void showGreen(float ppm)
{
  // Pulisce l'area della faccia
  tft.fillCircle(x, y, 60, ST77XX_BLACK);

  // Bocca sorridente
  for (int i = 0; i < 5; i++) {
    tft.drawCircle(x, y, 30 + i, ST77XX_GREEN);
  }

  // Copre la meta superiore della bocca
  tft.fillRect(x - 40, y - 35, 80, 35, ST77XX_BLACK);

  // Occhi
  tft.fillCircle(x - 18, y - 15, 6, ST77XX_GREEN);
  tft.fillCircle(x + 18, y - 15, 6, ST77XX_GREEN);

  // Contorno della faccia
  for (int i = 0; i < 5; i++) {
    tft.drawCircle(x, y, 54 + i, ST77XX_GREEN);
  }

  drawCenteredText("PUOI GUIDARE", ST77XX_GREEN);
}

// ---------------------
// AGGIORNAMENTO PPM
// ---------------------

void updatePPM(float ppm, uint16_t color)
{
  tft.fillRect(0, 220, 240, 30, ST77XX_BLACK);

  String valStr = String(ppm, 1) + " PPM";

  tft.setTextSize(2);
  tft.setTextColor(color, ST77XX_BLACK);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds(valStr, 0, 0, &x1, &y1, &w, &h);

  int valX = (240 - w) / 2 - x1;

  tft.setCursor(valX, 220);
  tft.print(valStr);
}

// ---------------------
// SETUP
// ---------------------

void setup()
{
  Serial.begin(115200);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  analogReadResolution(12);

  tft.init(240, 280);
  tft.enableDisplay(true);
  tft.writeCommand(ST77XX_SLPOUT);

  delay(120);

  tft.writeCommand(ST77XX_DISPON);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  // Calibrazione offset iniziale
  // Eseguire solo con sensore a riposo
  long sum = 0;

  for (int i = 0; i < 500; i++)
  {
    sum += analogRead(SENSOR_PIN);
    delay(5);
  }

  offsetADC = sum / 500.0;

  Serial.print("Offset ADC = ");
  Serial.println(offsetADC, 2);
}

// ---------------------
// LOOP PRINCIPALE
// ---------------------

void loop()
{
  float adc = readADC();

  float adcNet = adc - offsetADC;

  if (adcNet < 0) {
    adcNet = 0;
  }

  float ppm = adcNet * PPM_PER_ADC;

  // Monitor seriale
  Serial.print("ADC = ");
  Serial.print(adc, 2);

  Serial.print("   PPM = ");
  Serial.println(ppm, 2);

  ScreenState newState;
  uint16_t currentColor;

  // Selezione dello stato
  if (ppm < SOGLIA_ARANCIONE)
  {
    newState = STATE_GREEN;
    currentColor = ST77XX_GREEN;
  }
  else if (ppm < SOGLIA_ROSSA)
  {
    newState = STATE_ORANGE;
    currentColor = COLOR_ORANGE;
  }
  else
  {
    newState = STATE_RED;
    currentColor = ST77XX_RED;
  }

  // Aggiorna tutta l'interfaccia
  // soltanto quando cambia lo stato
  if (newState != currentState)
  {
    currentState = newState;

    // Pulisce completamente faccia e testo
    tft.fillCircle(x, y, 60, ST77XX_BLACK);
    tft.fillRect(0, 175, 240, 80, ST77XX_BLACK);

    if (currentState == STATE_GREEN)
    {
      showGreen(ppm);
    }
    else if (currentState == STATE_ORANGE)
    {
      showOrange(ppm);
    }
    else
    {
      showRed(ppm);
    }

    updatePPM(ppm, currentColor);
    lastPpmDisplayed = ppm;
  }

  // Aggiorna solo il numero se cambia
  else if (fabsf(ppm - lastPpmDisplayed) >= 0.1f)
  {
    updatePPM(ppm, currentColor);
    lastPpmDisplayed = ppm;
  }

  delay(300);
}
