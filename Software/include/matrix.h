/**
 * @file matrix.h
 * @author Alex Landolt 
 * @brief Header file for functions involved in LED matrix
 * @version 0.3
 * @date 2020-05-25
 */
#ifndef MATRIX_H
#define MATRIX_H

#include "ESP32-RGB64x32MatrixPanel-I2S-DMA.h"

extern void init_matrix(); /// initialize LED matrix
extern void init_matrix_power();
extern RGB64x32MatrixPanel_I2S_DMA &ref_backgroundLayer(); /// get a reference of background layer of LED matrix to manipulate it in other parts of LITOS code
extern void matrix_on();                                   /// supply matrix with current
extern void matrix_off();                                  /// switches matrix off
extern void reset_panel();
void gpio_setup_out(gpio_num_t gpio, int sig);
void sleep_matrix(gpio_num_t *gpio_pins);
void wake_up_matrix(gpio_num_t *gpio_pins);
#endif