// ******** fixed.h ************** 
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/3/2018
// Description: Header file for fixed.c
// Lab Number: Lab01
// TA: Zee Lv
// Date of last revision: 9/10/2018
// Hardware Configuration: N/A

#include <stdint.h>

/**
 * Output signed fixed point value to the display with a resolution of 0.01.
 * param: value: Value to be printed to display.
 * limitation: 
 *   The value param must be between -10,000 and 10,000 (exclusive).
 *   Otherwise, "***.**" will be printed to the display instead of 
 *   the correct result.
 */
void ST7735_sDecOut2(int32_t value);


/**
 * Output unsigned integer value to the display with a resolution of 1/64.
 * param: value: Value to be printed to display.
 * limitation: 
 *   The value param must be less than 64,000, otherwise, "***.**" will
 *   be printed to the display instead of the correct result.
 */
void ST7735_uBinOut6(uint32_t value); 

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
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY);

/**
 * Initialize an XY plot with the given dimensions by first clearing the screen.
 * param: title: Title of plot to be printed to the top of the display.
 * param: minX: Minimum x coordinate of the plot.
 * param: maxX: Maximum x coordinate of the plot.
 * param: minY: Minimum y coordinate of the plot.
 * param: maxY: Maximum y coordinate of the plot.
 */
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]);



