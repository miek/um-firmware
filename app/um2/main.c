
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_delay.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "app_scheduler.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "led.h"
#include "bleConfig.h"
#include "boards.h"

#include "simple_ble.h"
#include "simple_adv.h"
#include "simple_timer.h"


/*******************************************************************************
 *   State and Configuration
 ******************************************************************************/

// Randomly generated UUID for this service
simple_ble_service_t service_handle = {
    .uuid128 = {{0x83, 0xbe, 0xd0, 0x58, 0xf1, 0xaf, 0x42, 0x62,
	             0x9c, 0xf8, 0x4f, 0x73, 0xe9, 0xdb, 0x9c, 0xe4}}
};
simple_ble_char_t num_handle = {.uuid16 = CUSTOM_SERVICE_CHAR_NUMBER_SHORT_UUID};

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    .platform_id       = 0x40,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = DEVICE_NAME,       // used in advertisements if there is room
    .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(8, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(10, UNIT_1_25_MS),
};

// State for this CuSToM service application
static ble_cstm_t cstm;


/*******************************************************************************
 *   Timer Callback
 ******************************************************************************/

static void timer_handler (void* p_context) {
	cstm.num_value++;
	led_toggle(LED_0);
}


/*******************************************************************************
 *   INIT FUNCTIONS
 ******************************************************************************/

static void timers_init(void) {
	simple_timer_start(1000, timer_handler);
}

// Init services
void services_init (void) {

    // Add custom service
    simple_ble_add_service(&service_handle);

    // Add the characteristic
    simple_ble_add_characteristic(1, 0, 0, 0, // read, write, notify, vlen
                                  1, (uint8_t*) &cstm.num_value,
                                  &service_handle,
                                  &num_handle);
}

int main(void) {

	// Initialize
	led_init(LED_0);

	// Setup Simple BLE
	simple_ble_init(&ble_config);

	// Setup our advertisement
	simple_adv_service(&service_handle.uuid_handle);

	// You must initialize and use app timers to initialize conn params
	timers_init();

	// Enter main loop
	while(1) {
		power_manage();
	}
}

