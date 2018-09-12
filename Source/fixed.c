// ******** fixed.h ************** 
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/3/2018
// Description: File to handle fixed point numbers
// and their applications with the ST7735
// Lab Number: Lab01
// TA: Zee Lv
// Date of last revision: 9/10/2018
// Hardware Configuration: N/A

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fixed.h"
#include "ST7735.h"

#define LCD_PLOT_MIN_X 0
#define LCD_PLOT_MAX_X 127
#define LCD_PLOT_MIN_Y 32
#define LCD_PLOT_MAX_Y 159
#define LCD_PLOT_HEIGHT (LCD_PLOT_MAX_Y - LCD_PLOT_MIN_Y)
#define LCD_PLOT_WIDTH (LCD_PLOT_MAX_X - LCD_PLOT_MIN_X)

int32_t xMin = 0;
int32_t xMax = 0;
int32_t yMin = 0;
int32_t yMax = 0;

/**
 * Convert a number between 0 and 9 to its ASCII representation
 * param: digit: Number to convert to ASCII
 * return: ASCII Representation of digit param.
 */
static inline __attribute__((always_inline)) char digitToASCII(uint8_t digit) {
  return digit + 0x30;
}


/**
 * Output signed fixed point value to the display with a resolution of 0.01.
 * param: value: Value to be printed to display.
 * limitation: 
 *   The value param must be between -10,000 and 10,000 (exclusive).
 *   Otherwise, "***.**" will be printed to the display instead of 
 *   the correct result.
 */
void ST7735_sDecOut2(int32_t value) {
  char buf[6];
  buf[5] = NULL;

  if (value <= -10000) {
    printf("-**.**");
    return;
  }
  if (value >= 10000) {
    printf(" **.**");
    return;
  }

  int32_t originalValue = value;
  if (value < 0)
    value *= -1;
  float decimal = (float) value / 100;
  decimal = (decimal - value) * 100;

  if (originalValue < 0 && originalValue > -1000)
    printf(" ");
  if (originalValue >= 0 && originalValue < 1000)
    printf(" ");
  if (originalValue <= -1000 && originalValue > -10000)
    printf("-");
  if (originalValue >= 1000 && originalValue < 10000)
    printf(" ");

  for (int i = 4; i >= 0; i--) {
    if (i == 2)
      buf[i] = '.';
    else {
      buf[i] = digitToASCII(value % 10);
      value = value / 10;
    }
  }
	
	if (buf[0] == '0')
		buf[0] = originalValue >= 0 ? ' ' : '-';
	
  printf("%s", buf);

}

/**
 * Output unsigned integer value to the display with a resolution of 1/64.
 * param: value: Value to be printed to display.
 * limitation: 
 *   The value param must be less than 64,000, otherwise, "***.**" will
 *   be printed to the display instead of the correct result.
 */
void ST7735_uBinOut6(uint32_t value) {
  uint8_t outSize = 6;
  uint8_t decimalPlaces = 2;

  if (value >= 64000) {
    printf("***.**");
    return;
  }

  char buf[6];
  float decimalValue = (float) value / 64;
  int intPart = decimalValue;
  float decPart = decimalValue - intPart;
  sprintf(buf, "%d.", intPart);
  uint8_t len = strlen(buf);
  for (int i = 0; i < decimalPlaces; ++i) {
    decPart *= 10;
    if (i == decimalPlaces - 1)
      decPart = round(decPart);
    sprintf(&buf[len + i], "%d", (uint32_t) decPart % 10);
  }
  uint8_t padding = outSize - strlen(buf);
  for (int i = 0; i < padding; ++i)
    printf(" ");
  printf("%s", buf);
}

/**
 * Initialize an XY plot with the given dimensions by first clearing the screen.
 * param: title: Title of plot to be printed to the top of the display.
 * param: minX: Minimum x coordinate of the plot.
 * param: maxX: Maximum x coordinate of the plot.
 * param: minY: Minimum y coordinate of the plot.
 * param: maxY: Maximum y coordinate of the plot.
 * limitation:
 *   Assumes that minX < maxX and minY is less than maxY.
 */
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY) {
  ST7735_FillScreen(0);
  ST7735_SetCursor(0, 0);
  ST7735_OutString(title);
  xMin = minX;
  xMax = maxX;
  yMin = minY;
  yMax = maxY;
}

/**
 * Initialize an XY plot with the given dimensions by first clearing the screen.
 * param: title: Title of plot to be printed to the top of the display.
 * param: minX: Minimum x coordinate of the plot.
 * param: maxX: Maximum x coordinate of the plot.
 * param: minY: Minimum y coordinate of the plot.
 * param: maxY: Maximum y coordinate of the plot.
 */
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]) {
  for (int i = 0; i < num; i++) {
    if (bufX[i] >= xMin && bufX[i] <= xMax && bufY[i] >= yMin && bufY[i] <= yMax) {
			int xCoordinate = (LCD_PLOT_WIDTH * (bufX[i] - xMin)) / (xMax - xMin);
			int yCoordinate = ((LCD_PLOT_HEIGHT * (yMax - bufY[i])) / (yMax - yMin)) + LCD_PLOT_MIN_Y;
      ST7735_DrawPixel(xCoordinate, yCoordinate, ST7735_WHITE);
		}
  }
}
