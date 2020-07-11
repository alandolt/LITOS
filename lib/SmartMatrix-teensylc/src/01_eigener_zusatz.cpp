#include "01_eigener_zusatz.h"
#include "esp32_i2s_parallel.h"
int sig_data_base_stored;
int sig_clk_stored;
i2s_parallel_config_t cfg_stored;

void sleep_matrix()
{
    for (int x = 0; x < cfg_stored.bits; x++)
    {
        gpio_setup_out(cfg_stored.gpio_bus[x], 0x100);
    }
    gpio_setup_out(cfg_stored.gpio_clk, 0x100);
}

void reset_matrix_after_sleep()
{
    for (int x = 0; x < cfg_stored.bits; x++)
    {
        gpio_setup_out(cfg_stored.gpio_bus[x], sig_data_base_stored + x);
    }
    gpio_setup_out_invert(cfg_stored.gpio_clk, sig_clk_stored);
}
