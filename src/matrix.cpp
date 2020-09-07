/**
 * @file matrix.cpp
 * @author Alex Landolt 
 * @brief C file containing functions needed for controlling the LED matrix and the MOSFET
 * @version 0.3
 * @date 2020-05-25
 */
#include "matrix.h"
#define COLOR_DEPTH 24

#define GPIOPINOUT ESP32_FORUM_PINOUT

const int defaultBrightness = (100 * 255) / 100;                                     /// at this part of the program global brightness levels can be adjusted
const byte led_matrix_pins[] = {12, 13, 14, 16, 17, 19, 21, 22, 25, 26, 27, 32, 33}; /// ESP32 pins needed for the LED matrix
const byte led_matrix_mosfet = 4;                                                    /// pin where the MOSFET is located
static bool is_matrix_on;

/// specific settings concerning smartmatrix
const uint8_t kMatrixWidth = 64;                              // known working: 32, 64, 96, 128
const uint8_t kMatrixHeight = 32;                             // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 36;                             // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 2;                             // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN; // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);    // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions); /// allocate space for buffer of LED matrix
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);    /// allocate space for background layer

/**
 * @brief Used to generate reference to LED matrix, as so we can control it in the wellplate part of LITOS code
 * 
 * @return SMLayerBackground<rgb24, 0u>&, that is nothing more than a reference to the LED matrix
 */
SMLayerBackground<rgb24, 0u> &ref_backgroundLayer()
{
    return backgroundLayer;
}

/**
 * @brief called in void setup to initialize matrix
 * 
 */
void init_matrix()
{
    matrix.setBrightness(defaultBrightness);
    matrix.setRefreshRate(80);
    matrix.addLayer(&backgroundLayer); /// add allocated background layer to matrix construct
    matrix.begin(15000);               /// start LED matrix (allocate all the needed memory), however leaves 15K bytes free in Heap
    matrix_on();
    backgroundLayer.fillScreen({0, 0, 0}); /// fill matrix buffer black
    backgroundLayer.swapBuffers();         /// swap buffer with background, so that the change is seen
    matrix_off();                          /// helper function to cut off data lines
}

void init_matrix_power()
{
    pinMode(led_matrix_mosfet, OUTPUT); /// initialize MOSFET pin
    is_matrix_on = true;
    matrix_off();
}

/**
 * @brief helper function to cut off data lines and power through MOSFET and internal pull down resistors 
 * 
 */
void matrix_off()
{
    if (is_matrix_on)
    {
        backgroundLayer.fillScreen(rgb24{0, 0, 0});
        digitalWrite(led_matrix_mosfet, LOW);
        delay(20);
        sleep_matrix();
        is_matrix_on = false;
    }
}
/**
 * @brief activate matrix by supplying power through internal MOSFET 
 * 
 */
void matrix_on()
{
    if (!is_matrix_on)
    {
        reset_matrix_after_sleep();
        digitalWrite(led_matrix_mosfet, HIGH);
        is_matrix_on = true;
    }
}

void clear_matrix()
{
    backgroundLayer.fillScreen({0, 0, 0});
}