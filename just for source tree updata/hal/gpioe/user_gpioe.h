#ifndef __USER_GPIOE_H__
#define __USER_GPIOE_H__

#include "stdint.h"
#include "nrf_gpio.h"

#define LED_1          17
#define INTPIN1 13 //SW1
#define INTPIN2 14 //SW2
#define INTPIN3 15 //SW3
#define INTPIN4 16 //SW4

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50) 

/**@brief Button event handler type. */
typedef void (*app_extint_handler_t)(uint8_t pin_no, uint8_t button_action);

/**@brief Button configuration structure. */
typedef struct
{
    uint8_t              pin_no;           /**< Pin to be used as a button. */
    uint8_t              active_state;     /**< APP_BUTTON_ACTIVE_HIGH or APP_BUTTON_ACTIVE_LOW. */
    nrf_gpio_pin_pull_t  pull_cfg;         /**< Pull-up or -down configuration. */
    app_extint_handler_t extint_handler;   /**< Handler to be called when button is pushed. */
} app_extint_cfg_t;

uint32_t user_gpioe_init(void);

#endif




