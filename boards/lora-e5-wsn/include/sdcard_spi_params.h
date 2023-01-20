#ifndef SDCARD_SPI_PARAMS_H
#define SDCARD_SPI_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Card detect pin
 */
 #define CARD_DETECT_PIN              (GPIO_PIN(PORT_B, 10))

/**
 * @brief   sdcard_spi configuration
 */
static const  sdcard_spi_params_t sdcard_spi_params[] = {
    {
        .spi_dev        = SPI_DEV(1),
        .cs             = GPIO_PIN(PORT_B, 9),
        .clk            = GPIO_PIN(PORT_B, 13),
        .mosi           = GPIO_PIN(PORT_A, 10),
        .miso           = GPIO_PIN(PORT_B, 14),
        .power          = GPIO_UNDEF,
        .power_act_high = true
    },
};

#ifdef __cplusplus
}
#endif

#endif /* SDCARD_SPI_PARAMS_H */
/** @} */
