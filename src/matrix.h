#ifndef MATRIX_H
#define MATRIX_H

#include <SmartMatrix3.h>

extern void init_matrix();
extern SMLayerBackground<rgb24, 0u> &ref_backgroundLayer();
extern void matrix_on();
extern void matrix_off();

#endif