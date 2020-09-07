/**
 * @file matrix.h
 * @author Alex Landolt 
 * @brief Header file for functions involved in LED matrix
 * @version 0.3
 * @date 2020-05-25
 */
#ifndef MATRIX_H
#define MATRIX_H

#include <SmartMatrix3.h>

extern void init_matrix(); /// initialize LED matrix
extern void init_matrix_power();
extern SMLayerBackground<rgb24, 0u> &ref_backgroundLayer(); /// get a reference of background layer of LED matrix to manipulate it in other parts of LITOS code
extern void matrix_on();                                    /// supply matrix with current
extern void matrix_off();                                   /// switches matrix off
extern void clear_matrix();
#endif