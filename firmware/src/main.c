/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#if CONFIG_BT_NIMBLE_ENABLED
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#else
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#endif

#if CONFIG_BT_NIMBLE_ENABLED
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#define ESP_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define ESP_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
#else
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#endif

#include "esp_hidh.h"
#include "esp_hid_gap.h"

static const char *TAG = "REMOTE_TEST";
static EventGroupHandle_t remote_events;
static QueueHandle_t control_queue;
#define REMOTE_CONNECTED BIT0
static const uint8_t remote_address[6] = {0xff, 0xff, 0x50, 0x00, 0x27, 0xda};
static bool button_down;
typedef enum {
    LIGHT_DISCONNECTED,
    LIGHT_RED,
    LIGHT_RED_AND_YELLOW,
    LIGHT_GREEN,
    LIGHT_YELLOW,
} light_state_t;
static const char *const light_states[] = {
    "FLASHING_YELLOW", "RED", "RED_AND_YELLOW", "GREEN", "YELLOW"
};
typedef enum {
    CONTROL_CONNECTED,
    CONTROL_DISCONNECTED,
    CONTROL_BUTTON_PRESS,
} control_event_t;

#define DISCONNECTED_BLINK_MS 500
#define RED_TO_GREEN_DELAY_MS 1000
#define GREEN_TO_RED_DELAY_MS 1500

static TickType_t phase_start;
static light_state_t phase = LIGHT_DISCONNECTED;
static bool yellow_blink_on = true;

#define RED_PIN GPIO_NUM_25
#define YELLOW_PIN GPIO_NUM_26
#define GREEN_PIN GPIO_NUM_27

static void show_light_state(light_state_t state, bool blink_on)
{
    // Switch all lamps off before enabling the lamps in the new state.
    ESP_ERROR_CHECK(gpio_set_level(RED_PIN, 0));
    ESP_ERROR_CHECK(gpio_set_level(YELLOW_PIN, 0));
    ESP_ERROR_CHECK(gpio_set_level(GREEN_PIN, 0));
    ESP_ERROR_CHECK(gpio_set_level(RED_PIN, state == LIGHT_RED || state == LIGHT_RED_AND_YELLOW));
    ESP_ERROR_CHECK(gpio_set_level(YELLOW_PIN, state == LIGHT_RED_AND_YELLOW ||
                                                state == LIGHT_YELLOW ||
                                                (state == LIGHT_DISCONNECTED && blink_on)));
    ESP_ERROR_CHECK(gpio_set_level(GREEN_PIN, state == LIGHT_GREEN));
    ESP_LOGI(TAG, "LIGHTS -> %s%s", light_states[state],
             state == LIGHT_DISCONNECTED ? (blink_on ? " ON" : " OFF") : "");
}

static void init_light_pins(void)
{
    gpio_config_t outputs = {
        .pin_bit_mask = (1ULL << RED_PIN) | (1ULL << YELLOW_PIN) | (1ULL << GREEN_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&outputs));
    show_light_state(LIGHT_DISCONNECTED, yellow_blink_on);
}

static void traffic_light_task(void *arg)
{
    phase_start = xTaskGetTickCount();
    while (true) {
        TickType_t duration = portMAX_DELAY;
        if (phase == LIGHT_DISCONNECTED) {
            duration = pdMS_TO_TICKS(DISCONNECTED_BLINK_MS);
        } else if (phase == LIGHT_RED_AND_YELLOW) {
            duration = pdMS_TO_TICKS(RED_TO_GREEN_DELAY_MS);
        } else if (phase == LIGHT_YELLOW) {
            duration = pdMS_TO_TICKS(GREEN_TO_RED_DELAY_MS);
        }

        TickType_t timeout = portMAX_DELAY;
        if (duration != portMAX_DELAY) {
            TickType_t elapsed = xTaskGetTickCount() - phase_start;
            timeout = elapsed >= duration ? 0 : duration - elapsed;
        }

        control_event_t event;
        if (xQueueReceive(control_queue, &event, timeout) == pdTRUE) {
            if (event == CONTROL_CONNECTED) {
                phase = LIGHT_RED;
                show_light_state(phase, false);
                phase_start = xTaskGetTickCount();
            } else if (event == CONTROL_DISCONNECTED) {
                phase = LIGHT_DISCONNECTED;
                yellow_blink_on = true;
                show_light_state(phase, yellow_blink_on);
                phase_start = xTaskGetTickCount();
            } else if (event == CONTROL_BUTTON_PRESS) {
                if (phase == LIGHT_RED) {
                    phase = LIGHT_RED_AND_YELLOW;
                    show_light_state(phase, false);
                    phase_start = xTaskGetTickCount();
                } else if (phase == LIGHT_GREEN) {
                    phase = LIGHT_YELLOW;
                    show_light_state(phase, false);
                    phase_start = xTaskGetTickCount();
                }
            }
        } else if (phase == LIGHT_DISCONNECTED) {
            yellow_blink_on = !yellow_blink_on;
            show_light_state(phase, yellow_blink_on);
            phase_start = xTaskGetTickCount();
        } else if (phase == LIGHT_RED_AND_YELLOW) {
            phase = LIGHT_GREEN;
            show_light_state(phase, false);
        } else if (phase == LIGHT_YELLOW) {
            phase = LIGHT_RED;
            show_light_state(phase, false);
        }
    }
}

#if CONFIG_BT_HID_HOST_ENABLED
static const char * remote_device_name = CONFIG_EXAMPLE_PEER_DEVICE_NAME;
#endif // CONFIG_BT_HID_HOST_ENABLED

#if !CONFIG_BT_NIMBLE_ENABLED
static char *bda2str(uint8_t *bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}
#endif

void hidh_callback(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    esp_hidh_event_t event = (esp_hidh_event_t)id;
    esp_hidh_event_data_t *param = (esp_hidh_event_data_t *)event_data;

    switch (event) {
    case ESP_HIDH_OPEN_EVENT: {
        if (param->open.status == ESP_OK) {
            xEventGroupSetBits(remote_events, REMOTE_CONNECTED);
            button_down = false;
            control_event_t control = CONTROL_CONNECTED;
            xQueueSend(control_queue, &control, 0);
            const uint8_t *bda = esp_hidh_dev_bda_get(param->open.dev);
            if (bda) {
                ESP_LOGI(TAG, ESP_BD_ADDR_STR " OPEN: %s", ESP_BD_ADDR_HEX(bda), esp_hidh_dev_name_get(param->open.dev));
                esp_hidh_dev_dump(param->open.dev, stdout);
            }
        } else {
            ESP_LOGE(TAG, " OPEN failed!");
        }
        break;
    }
    case ESP_HIDH_BATTERY_EVENT: {
        const uint8_t *bda = esp_hidh_dev_bda_get(param->battery.dev);
        if (bda) {
            ESP_LOGI(TAG, ESP_BD_ADDR_STR " BATTERY: %d%%", ESP_BD_ADDR_HEX(bda), param->battery.level);
        }
        break;
    }
    case ESP_HIDH_INPUT_EVENT: {
        const uint8_t *bda = esp_hidh_dev_bda_get(param->input.dev);
        if (bda) {
            ESP_LOGI(TAG, ESP_BD_ADDR_STR " INPUT: %8s, MAP: %2u, ID: %3u, Len: %d, Data:", ESP_BD_ADDR_HEX(bda), esp_hid_usage_str(param->input.usage), param->input.map_index, param->input.report_id, param->input.length);
            ESP_LOG_BUFFER_HEX(TAG, param->input.data, param->input.length);
        }
        if (param->input.usage == ESP_HID_USAGE_CCONTROL &&
            param->input.report_id == 2 && param->input.length == 2) {
            bool down = param->input.data[0] == 0x01 && param->input.data[1] == 0x00;
            if (down && !button_down) {
                control_event_t control = CONTROL_BUTTON_PRESS;
                xQueueSend(control_queue, &control, 0);
            }
            button_down = down;
        }
        break;
    }
    case ESP_HIDH_FEATURE_EVENT: {
        const uint8_t *bda = esp_hidh_dev_bda_get(param->feature.dev);
        if (bda) {
            ESP_LOGI(TAG, ESP_BD_ADDR_STR " FEATURE: %8s, MAP: %2u, ID: %3u, Len: %d", ESP_BD_ADDR_HEX(bda),
                    esp_hid_usage_str(param->feature.usage), param->feature.map_index, param->feature.report_id,
                    param->feature.length);
            ESP_LOG_BUFFER_HEX(TAG, param->feature.data, param->feature.length);
        }
        break;
    }
    case ESP_HIDH_CLOSE_EVENT: {
        xEventGroupClearBits(remote_events, REMOTE_CONNECTED);
        button_down = false;
        control_event_t control = CONTROL_DISCONNECTED;
        xQueueSend(control_queue, &control, 0);
        const uint8_t *bda = esp_hidh_dev_bda_get(param->close.dev);
        if (bda) {
            ESP_LOGI(TAG, ESP_BD_ADDR_STR " CLOSE: %s", ESP_BD_ADDR_HEX(bda), esp_hidh_dev_name_get(param->close.dev));
        }
        break;
    }
    default:
        ESP_LOGI(TAG, "EVENT: %d", event);
        break;
    }
}

#define SCAN_DURATION_SECONDS 5

void hid_demo_task(void *pvParameters)
{
    while (true) {
        if (xEventGroupGetBits(remote_events) & REMOTE_CONNECTED) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        size_t count = 0;
        esp_hid_scan_result_t *results = NULL;
        ESP_LOGI(TAG, "Looking for remote controller. Disconnect it from any existing connection and turn it on.");
        esp_err_t error = esp_hid_scan(SCAN_DURATION_SECONDS, &count, &results);
        if (error == ESP_OK) {
            for (esp_hid_scan_result_t *r = results; r; r = r->next) {
                if (r->transport == ESP_HID_TRANSPORT_BLE &&
                    memcmp(r->bda, remote_address, sizeof(remote_address)) == 0) {
                    ESP_LOGI(TAG, "Found your remote; connecting...");
                    esp_hidh_dev_open(r->bda, r->transport, r->ble.addr_type);
                    break;
                }
            }
        } else {
            ESP_LOGW(TAG, "Scan failed: %s", esp_err_to_name(error));
        }
        esp_hid_scan_results_free(results);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
#if CONFIG_BT_NIMBLE_ENABLED
void ble_hid_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}
void ble_store_config_init(void);
#endif
void app_main(void)
{
    esp_err_t ret;
    remote_events = xEventGroupCreate();
    configASSERT(remote_events);
    control_queue = xQueueCreate(8, sizeof(control_event_t));
    configASSERT(control_queue);
    init_light_pins();
    BaseType_t task_created = xTaskCreate(traffic_light_task, "traffic_lights", 2048, NULL, 2, NULL);
    configASSERT(task_created == pdPASS);
#if HID_HOST_MODE == HIDH_IDLE_MODE
    ESP_LOGE(TAG, "Please turn on BT HID host or BLE!");
    return;
#endif
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ESP_LOGI(TAG, "setting hid gap, mode:%d", HID_HOST_MODE);
    ESP_ERROR_CHECK( esp_hid_gap_init(HID_HOST_MODE) );
#if CONFIG_BT_BLE_ENABLED
    ESP_ERROR_CHECK( esp_ble_gattc_register_callback(esp_hidh_gattc_event_handler) );
#endif /* CONFIG_BT_BLE_ENABLED */
    esp_hidh_config_t config = {
        .callback = hidh_callback,
        .event_stack_size = 4096,
        .callback_arg = NULL,
    };
    ESP_ERROR_CHECK( esp_hidh_init(&config) );

#if !CONFIG_BT_NIMBLE_ENABLED
    char bda_str[18] = {0};
    ESP_LOGI(TAG, "Own address:[%s]", bda2str((uint8_t *)esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
#endif


#if CONFIG_BT_NIMBLE_ENABLED
    /* XXX Need to have template for store */
    ble_store_config_init();

    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
	/* Starting nimble task after gatts is initialized*/
    ret = esp_nimble_enable(ble_hid_host_task);
    if (ret) {
        ESP_LOGE(TAG, "esp_nimble_enable failed: %d", ret);
    }

    vTaskDelay(200);

    uint8_t own_addr_type = 0;
    int rc;
    uint8_t addr_val[6] = {0};

    rc = ble_hs_id_copy_addr(BLE_ADDR_PUBLIC, NULL, NULL);

    rc = ble_hs_id_infer_auto(0, &own_addr_type);

    if (rc != 0) {
        ESP_LOGI(TAG, "error determining address type; rc=%d\n", rc);
        return;
    }

    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);

    ESP_LOGI(TAG, "Device Address: ");
    ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x \n", addr_val[5], addr_val[4], addr_val[3],
		                                      addr_val[2], addr_val[1], addr_val[0]);

#endif
    xTaskCreate(&hid_demo_task, "hid_task", 6 * 1024, NULL, 2, NULL);
}
