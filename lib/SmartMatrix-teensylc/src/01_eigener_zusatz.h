#ifndef EIGENER_ZUSATZ_H
#define EIGENER_ZUSATZ_H
#include "esp32_i2s_parallel.h"

extern int sig_data_base_stored;
extern int sig_clk_stored;
extern i2s_parallel_config_t cfg_stored;
void reset_matrix_after_sleep();
void sleep_matrix();
#endif