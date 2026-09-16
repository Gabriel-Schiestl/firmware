#include <stdint.h>
#include "esp_rom_sys.h"   // to use esp_rom_delay_us

// ESP32 GPIO register addresses (from the Technical Reference Manual)
#define GPIO_OUT_W1TS_REG (*(volatile uint32_t*)0x3FF44008) // "write 1 to set" -> turns pin on
#define GPIO_OUT_W1TC_REG (*(volatile uint32_t*)0x3FF4400C) // "write 1 to clear" -> turns pin off
#define GPIO_ENABLE_W1TS_REG (*(volatile uint32_t*)0x3FF44024) // enables pin as output

#define LED_PIN 2   // GPIO2, onboard LED on many ESP32 DevKit boards

typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH = 1,
} gpio_level_t;

void my_gpio_write(unsigned int pin, gpio_level_t level) {
    if (level == GPIO_LOW) {
        GPIO_OUT_W1TC_REG = (1 << pin);
    } else {
        GPIO_OUT_W1TS_REG = (1 << pin);
    }
}

void app_main(void) {
    // Step 1: configure pin 2 as OUTPUT
    GPIO_ENABLE_W1TS_REG = (1 << LED_PIN);

    while (1) {
        my_gpio_write(LED_PIN, GPIO_HIGH); // turn the LED on
        esp_rom_delay_us(500000);           // wait 500ms

        my_gpio_write(LED_PIN, GPIO_LOW); // turn the LED off
        esp_rom_delay_us(500000);
    }
}