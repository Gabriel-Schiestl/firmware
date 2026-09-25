#include <stdio.h>
#include <stdint.h>
#include "esp_rom_sys.h"
#include <stdbool.h>

#define GPIO_IN_REG (*(volatile uint32_t*)0x3FF4403C) // IN register that we can read state from input pins
#define GPIO_OUT_W1TC_REG (*(volatile uint32_t*)0x3FF4400C) //  set 1 to a bit clears its function
#define IO_MUX_GPIO4_REG (*(volatile uint32_t*)0x3FF49048) // IO_MUX pin 4 register
#define GPIO_OUT_W1TS_REG (*(volatile uint32_t*)0x3FF44008) // turns output pin as HIGH
#define GPIO_ENABLE_W1TS_REG (*(volatile uint32_t*)0x3FF44024) // enable pin as output
#define GPIO_ENABLE_W1TC_REG (*(volatile uint32_t*)0x3FF44028) // disable pin as output
#define GPIO_PIN4_REG (*(volatile uint32_t*)0x3FF44098) // configuration for GPIO pin 4
#define GPIO_STATUS_INT_W1TC (*(volatile uint32_t*)0x3FF4404C) // clear interrupt status
#define DPORT_APP_GPIO_INTERRUPT_MAP_REG (*(volatile uint32_t*)0x3FF00270) // interrupt matrix register for GPIO interrupts

#define BUTTON_PIN 4
#define LED_PIN 2

#define FUN_WPU (1 << 8)
#define FUN_IE  (1 << 9)

bool button_is_pressed() {
    uint32_t value = GPIO_IN_REG & (1 << BUTTON_PIN);
    return value == 0;
}

typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH = 1,
} gpio_level_t;

void gpio_write(unsigned int pin, gpio_level_t level) {
    if (level == GPIO_LOW) {
        GPIO_OUT_W1TC_REG = (1 << pin);
    } else {
        GPIO_OUT_W1TS_REG = (1 << pin);
    }
}

bool led_state = false;

volatile bool button_event = false;

static void IRAM_ATTR gpio_isr(void *arg)
{
    button_event = true;
}

void app_main(void)
{
    // ------ I/O CONFIG
    GPIO_ENABLE_W1TC_REG = (1 << BUTTON_PIN); // disable output driver

    GPIO_ENABLE_W1TS_REG = (1 << LED_PIN);

    gpio_write(LED_PIN, GPIO_LOW); // set led pin as LOW

    IO_MUX_GPIO4_REG = (IO_MUX_GPIO4_REG & ~(0x7 << 12)) | (2 << 12); // clear bits 12-14 and sets 2(010) as GPIO function. "preserve all other bits and substitute those which I want"

    IO_MUX_GPIO4_REG |= FUN_WPU | FUN_IE; // enable FUN_IE and FUN_WPU

    // ------- INTERRUPT CONFIG
    GPIO_PIN4_REG = (GPIO_PIN4_REG & ~(0x7 << 7)) | (2 << 7); // clear bits 7-9 and sets 2(010) for failing edge trigger

    GPIO_PIN4_REG = (GPIO_PIN4_REG & ~(0x1F << 13)) | (1 << 13); // clear bits 13-17 and sets bit 0 as 1: APP CPU interrupt enable

    DPORT_APP_GPIO_INTERRUPT_MAP_REG = 10; // set CPU interrupt. 10: Peripheral, Edge-Triggered, priority 1(low)

    gpio_install_isr_service(0);

    gpio_isr_handler_add(
        GPIO_NUM_4,
        gpio_isr,
        NULL
    );

    while (1) {
        if (button_event) {
            button_event = false;

            led_state = !led_state;

            gpio_write(LED_PIN, led_state ? GPIO_HIGH : GPIO_LOW);
        }
    }
}
