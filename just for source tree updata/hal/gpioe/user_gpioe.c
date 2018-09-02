#include "sdk_common.h"
#include "app_error.h"
#include "nrf_drv_gpiote.h"
#include "nrf_assert.h"
#include "sdk_macros.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "app_timer.h"

#include "user_gpioe.h"

APP_TIMER_DEF(m_detection_delay_timer_id);  /**< Polling timer id. */

/**@brief Function for handling the timeout that delays reporting buttons as pushed.
 *
 * @details    The detection_delay_timeout_handler(...) is a call-back issued from the app_timer
 *             module. It is called with the p_context parameter. The p_context parameter is
 *             provided to the app_timer module when a timer is started, using the call
 *             @ref app_timer_start. On @ref app_timer_start the p_context will be holding the
 *             currently pressed buttons.
 *
 * @param[in]  p_context   Pointer used for passing information app_start_timer() was called.
 *                         In the app_button module the p_context holds information on pressed
 *                         buttons.
 */
static void detection_delay_timeout_handler(void * p_context)
{
        NRF_LOG_INFO("detection_delay_timeout_handler" );
//    NRF_LOG_FLUSH();

 
}


static void gpiote_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    uint32_t err_code;
    nrf_drv_gpiote_out_toggle(LED_1);
    
    // Start detection timer. If timer is already running, the detection period is restarted.
    // NOTE: Using the p_context parameter of app_timer_start() to transfer the pin states to the
    //       timeout handler (by casting event_pins_mask into the equally sized void * p_context
    //       parameter).
    err_code = app_timer_stop(m_detection_delay_timer_id);
    if (err_code != NRF_SUCCESS)
    {
        // The impact in app_button of the app_timer queue running full is losing a button press.
        // The current implementation ensures that the system will continue working as normal.
        return;
    }
    
        NRF_LOG_INFO("gpiote_event_handler:pin=%d,action=%d.",pin,action);
//    NRF_LOG_FLUSH();

    switch (pin)
    {
        case INTPIN1:
             err_code = app_timer_start(m_detection_delay_timer_id, BUTTON_DETECTION_DELAY, NULL);
            if (err_code != NRF_SUCCESS)
            {
                // The impact in app_button of the app_timer queue running full is losing a button press.
                // The current implementation ensures that the system will continue working as normal.
                APP_ERROR_HANDLER(err_code);
            }
            
            break;

        default:
            APP_ERROR_HANDLER(pin);
            break;
    }
}

/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static void extint_event_handler(uint8_t pin_no, uint8_t button_action)
{
     NRF_LOG_INFO("extint_event_handler:pin=%d,action=%d.",pin_no,button_action);
    NRF_LOG_FLUSH();

    switch (pin_no)
    {
        case INTPIN1:
 
            break;

        default:
            APP_ERROR_HANDLER(pin_no);
            break;
    }
}


uint32_t user_gpioe_init(void)
{
    uint32_t err_code;
    uint32_t extint_count;
    
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

    err_code = nrf_drv_gpiote_out_init(LED_1, &out_config);
    APP_ERROR_CHECK(err_code);
    
    //The array must be static because a pointer to it will be saved in the button handler module.
    static app_extint_cfg_t extints[] =
    {
        {INTPIN1 , false, NRF_GPIO_PIN_PULLUP, extint_event_handler}
    };
    
   extint_count = ARRAY_SIZE(extints);
    
    if (!nrf_drv_gpiote_is_init())
    {
        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }
    
     while ( extint_count--)
    {
        app_extint_cfg_t const * p_btn = &extints[extint_count];


        nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
        config.pull = p_btn->pull_cfg;

        err_code = nrf_drv_gpiote_in_init(p_btn->pin_no, &config, gpiote_event_handler);
        APP_ERROR_CHECK(err_code);
        
        nrf_drv_gpiote_in_event_enable(extints[extint_count].pin_no, true);
    }
    
        // Create polling timer.
    app_timer_create(&m_detection_delay_timer_id,
                            APP_TIMER_MODE_SINGLE_SHOT,
                            detection_delay_timeout_handler);
//    
//    app_timer_start(m_detection_delay_timer_id, BUTTON_DETECTION_DELAY, NULL);
    
    NRF_LOG_INFO("GPIOE started.");
//    NRF_LOG_FLUSH();
    
    return NRF_SUCCESS;
}
    


