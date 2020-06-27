#include "init_matrix.h"
#define DEBUG

//#define GPIOPINOUT ESP32_FORUM_PINOUT
#define COLOR_DEPTH 24

#define CLKS_DURING_LATCH 0
#define MATRIX_I2S_MODE I2S_PARALLEL_BITS_16
#define MATRIX_DATA_STORAGE_TYPE uint16_t

#define R1_PIN 16
#define G1_PIN 27
#define B1_PIN 17
#define R2_PIN 19
#define G2_PIN 26
#define B2_PIN 21

#define A_PIN 22
#define B_PIN 25
#define C_PIN 13
#define D_PIN 33
#define E_PIN -1
#define LAT_PIN 32
#define OE_PIN 14
#define CLK_PIN 12

#define GPIO_PWM0A_OUT GPIO_NUM_32
#define GPIO_SYNC0_IN GPIO_NUM_34

const int defaultBrightness = (10 * 255) / 100; // (10%) brightness
const byte led_matrix_pins[] = {12, 13, 14, 16, 17, 19, 21, 22, 25, 26, 27, 32, 33};
const byte led_matrix_mosfet = 4;

const uint8_t kMatrixWidth = 64;                              // known working: 32, 64, 96, 128
const uint8_t kMatrixHeight = 32;                             // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 24;                             // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 4;                             // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN; // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);    // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

SMLayerBackground<rgb24, 0u> &ref_backgroundLayer()
{
    return backgroundLayer;
}

void init_matrix()
{
    pinMode(led_matrix_mosfet, OUTPUT);
    digitalWrite(led_matrix_mosfet, HIGH);
    matrix.addLayer(&backgroundLayer);
    matrix.begin();
    matrix.setBrightness(defaultBrightness);
    backgroundLayer.fillScreen({0, 0, 0});
    backgroundLayer.swapBuffers();

    digitalWrite(led_matrix_mosfet, LOW);
}

void matrix_off()
{
    //Thomas verwendet input, ich glaube pull down wäre besser, muss getestet werden
    /*for (byte i = 0; i < 13; i++)
    {
        pinMode(led_matrix_pins[i], INPUT);
    }*/
    digitalWrite(led_matrix_mosfet, LOW);
}

void matrix_on()
{
    //Thomas verwendet input, ich glaube pull down wäre besser, muss getestet werden
    /*for (byte i = 0; i < 13; i++)
    {
        pinMode(led_matrix_pins[i], INPUT);
    }*/
    digitalWrite(led_matrix_mosfet, HIGH);
#ifdef DEBUG
    backgroundLayer.fillScreen({255, 0, 0});
#endif
}