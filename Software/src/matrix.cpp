/**
 * @file matrix.cpp
 * @author Alex Landolt 
 * @brief C file containing functions needed for controlling the LED matrix and the MOSFET
 * @version 0.3
 * @date 2020-05-25
 */
#include "matrix.h"

RGB64x32MatrixPanel_I2S_DMA backgroundLayer;

#define R1 GPIO_NUM_22
#define G1 GPIO_NUM_32
#define BL1 GPIO_NUM_21
#define R2 GPIO_NUM_19
#define G2 GPIO_NUM_33
#define BL2 GPIO_NUM_17

#define CH_A GPIO_NUM_16
#define CH_B GPIO_NUM_25
#define CH_C GPIO_NUM_27
#define CH_D GPIO_NUM_26
#define CH_E -1

#define LAT GPIO_NUM_14
#define OE GPIO_NUM_13
#define CLK GPIO_NUM_12

int sig_data_base = I2S1O_DATA_OUT8_IDX;
int sig_clk = I2S1O_WS_OUT_IDX;
gpio_num_t gpio_pins_def[16] = {R1, G1, BL1, R2, G2, BL2, LAT, OE, CH_A, CH_B, CH_C, CH_D};

const byte led_matrix_mosfet = 4; /// pin where the MOSFET is located
static bool is_matrix_on;

/**
 * @brief Used to generate reference to LED matrix, as so we can control it in the wellplate part of LITOS code
 * 
 * @return RGB64x32MatrixPanel_I2S_DMA&, that is nothing more than a reference to the LED matrix
 */
RGB64x32MatrixPanel_I2S_DMA &ref_backgroundLayer()
{
    return backgroundLayer;
}

/**
 * @brief called in void setup to initialize matrix
 * 
 */
void init_matrix()
{
    //matrix_on();
    digitalWrite(led_matrix_mosfet, HIGH);
    reset_panel();
    digitalWrite(led_matrix_mosfet, LOW);

    backgroundLayer.setPanelBrightness(60);
    backgroundLayer.begin(R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK);
    digitalWrite(led_matrix_mosfet, HIGH);
    matrix_off(); /// helper function to cut off data lines
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
        backgroundLayer.fillScreen(0x0000);
        digitalWrite(led_matrix_mosfet, LOW);
        delay(20);
        sleep_matrix(gpio_pins_def);
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
        wake_up_matrix(gpio_pins_def);
        digitalWrite(led_matrix_mosfet, HIGH);
        is_matrix_on = true;
    }
}

void gpio_setup_out(gpio_num_t gpio, int sig)
{
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio], PIN_FUNC_GPIO);
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
    gpio_matrix_out(gpio, sig, false, false);
}

void sleep_matrix(gpio_num_t *gpio_pins)
{
    for (int x = 0; x < 12; x++)
    {
        gpio_setup_out(gpio_pins[x], 0x100);
    }
    gpio_setup_out(CLK, 0x100);
}

void wake_up_matrix(gpio_num_t *gpio_pins)
{
    for (int x = 0; x < 12; x++)
    {
        gpio_setup_out(gpio_pins[x], sig_data_base + x);
    }
    gpio_setup_out(CLK, sig_clk);
}

void reset_panel()
{
    int MaxLed = 64;

    int C12[16] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int C13[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};

    pinMode(R1, OUTPUT);
    pinMode(G1, OUTPUT);
    pinMode(BL1, OUTPUT);
    pinMode(R2, OUTPUT);
    pinMode(G2, OUTPUT);
    pinMode(BL2, OUTPUT);
    pinMode(CH_A, OUTPUT);
    pinMode(CH_B, OUTPUT);
    pinMode(CH_C, OUTPUT);
    pinMode(CH_D, OUTPUT);
    pinMode(CH_E, OUTPUT);
    pinMode(CLK, OUTPUT);
    pinMode(LAT, OUTPUT);
    pinMode(OE, OUTPUT);

    // Send Data to control register 11
    digitalWrite(OE, HIGH); // Display reset
    digitalWrite(LAT, LOW);
    digitalWrite(CLK, LOW);
    for (int l = 0; l < MaxLed; l++)
    {
        int y = l % 16;
        digitalWrite(R1, LOW);
        digitalWrite(G1, LOW);
        digitalWrite(BL1, LOW);
        digitalWrite(R2, LOW);
        digitalWrite(G2, LOW);
        digitalWrite(BL2, LOW);
        if (C12[y] == 1)
        {
            digitalWrite(R1, HIGH);
            digitalWrite(G1, HIGH);
            digitalWrite(BL1, HIGH);
            digitalWrite(R2, HIGH);
            digitalWrite(G2, HIGH);
            digitalWrite(BL2, HIGH);
        }
        if (l > MaxLed - 12)
        {
            digitalWrite(LAT, HIGH);
        }
        else
        {
            digitalWrite(LAT, LOW);
        }
        digitalWrite(CLK, HIGH);
        digitalWrite(CLK, LOW);
    }
    digitalWrite(LAT, LOW);
    digitalWrite(CLK, LOW);
    // Send Data to control register 12
    for (int l = 0; l < MaxLed; l++)
    {
        int y = l % 16;
        digitalWrite(R1, LOW);
        digitalWrite(G1, LOW);
        digitalWrite(BL1, LOW);
        digitalWrite(R2, LOW);
        digitalWrite(G2, LOW);
        digitalWrite(BL2, LOW);
        if (C13[y] == 1)
        {
            digitalWrite(R1, HIGH);
            digitalWrite(G1, HIGH);
            digitalWrite(BL1, HIGH);
            digitalWrite(R2, HIGH);
            digitalWrite(G2, HIGH);
            digitalWrite(BL2, HIGH);
        }
        if (l > MaxLed - 13)
        {
            digitalWrite(LAT, HIGH);
        }
        else
        {
            digitalWrite(LAT, LOW);
        }
        digitalWrite(CLK, HIGH);
        digitalWrite(CLK, LOW);
    }
    digitalWrite(LAT, LOW);
    digitalWrite(CLK, LOW);
}