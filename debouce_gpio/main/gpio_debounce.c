#include <stdio.h>
#include <stdint.h>
#include "esp_rom_sys.h"
#include <stdbool.h>

#define GPIO_IN_REG (*(volatile uint32_t*)0x3FF4403C)
#define GPIO_OUT_W1TC_REG (*(volatile uint32_t*)0x3FF4400C) 
#define BUTTON_PIN 4

bool button_is_pressed() {
    uint32_t value = GPIO_IN_REG & (1 << BUTTON_PIN);
    return value == 0;
}

void app_main(void)
{
    GPIO_OUT_W1TC_REG = (1 << BUTTON_PIN);

    while (1) {
        if (button_is_pressed()) {
            esp_rom_delay_us(20000);

            if (button_is_pressed()) {
                // do some logic
            }
        }
    }
}
