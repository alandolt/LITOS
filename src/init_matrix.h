#ifndef INIT_MATRIX
#define INIT_MATRIX

#include <SmartMatrix3.h>
#include "wellplate.h"

#define GPIOPINOUT ESP32_FORUM_PINOUT
#define COLOR_DEPTH 24                                        // known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24
const uint8_t kMatrixWidth = 64;                              // known working: 32, 64, 96, 128
const uint8_t kMatrixHeight = 32;                             // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 24;                             // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 4;                             // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN; // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);    // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

const int defaultBrightness = (25 * 255) / 100; // (10%) brightness

void wellplate::well_col(int x, int y, uint8_t r, uint8_t g, uint8_t b) // here defined as can not access to backgroundlayer in class (extern not working, as type of backgroundLayer is runtime defined by SMARTMATRIX_ALLOCATE_BUFFERS)

{
    backgroundLayer.fillRectangle(x, y, x + size_of_illumination, y + size_of_illumination, rgb24{r, g, b});
}

#endif