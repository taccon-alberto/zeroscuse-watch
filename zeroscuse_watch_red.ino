#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// PIN CODECELL C3
#define TFT_CS    3
#define TFT_DC    2
#define TFT_RST   1

#define TFT_MOSI  7
#define TFT_SCLK  6

Adafruit_ST7789 tft = Adafruit_ST7789(
  TFT_CS,
  TFT_DC,
  TFT_RST
);

void setup() {

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  tft.init(240, 280);

  tft.setRotation(0);

  tft.fillScreen(ST77XX_BLACK);

  // centro display
  int x = 120;
  int y = 90;

  // =========================
  // FACCIA
  // =========================



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

  tft.setCursor(textX, textY);
  tft.println(testo);
}

void loop() {

}
