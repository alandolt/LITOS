#ifndef INIT_MATRIX
#define INIT_MATRIX

#include <SmartMatrix3.h>

// known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24
extern const uint8_t kMatrixWidth;   // known working: 32, 64, 96, 128
extern const uint8_t kMatrixHeight;  // known working: 16, 32, 48, 64
extern const uint8_t kRefreshDepth;  // known working: 24, 36, 48
extern const uint8_t kDmaBufferRows; // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
extern const uint8_t kPanelType;     // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
extern const uint8_t kMatrixOptions; // see http://docs.pixelmatix.com/SmartMatrix for options
extern const uint8_t kBackgroundLayerOptions;

extern const int defaultBrightness; // (10%) brightness

extern void init_matrix();
extern SMLayerBackground<rgb24, 0u> &ref_backgroundLayer();
extern void matrix_on();
extern void matrix_off();

#endif