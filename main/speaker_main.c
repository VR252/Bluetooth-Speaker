#include <string.h>
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "driver/i2s.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_err.h"

static const char *TAG = "BT_AUDIO";

#define I2S_NUM              I2S_NUM_0
#define I2S_SAMPLE_RATE      44100
#define I2S_BITS_PER_SAMPLE  I2S_BITS_PER_SAMPLE_16BIT


/*
VCC
GND
LCK -> GPIO25
DIN -> GPIO26
BCK -> GPIO22
*/


// I2S pins — adjust if needed
#define I2S_BCK_PIN   26
#define I2S_WS_PIN    25
#define I2S_DATA_PIN  22

// Forward declarations
static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);
static void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len);

// Configure I2S peripheral for audio output
static void i2s_init(void) {
    i2s_config_t i2s_cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .intr_alloc_flags = 0
    };
    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM, &i2s_cfg, 0, NULL));

    i2s_pin_config_t pin_cfg = {
        .bck_io_num = I2S_BCK_PIN,
        .ws_io_num  = I2S_WS_PIN,
        .data_out_num = I2S_DATA_PIN,
        .data_in_num  = I2S_PIN_NO_CHANGE
    };
    ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM, &pin_cfg));

    ESP_LOGI(TAG, "I2S driver initialized");
}

// A2DP data callback — write PCM to I2S
static void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len) {
    size_t bytes_written = 0;
    i2s_write(I2S_NUM, data, len, &bytes_written, portMAX_DELAY);
}

// A2DP event callback: connection, start, stop
static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param) {
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            ESP_LOGI(TAG, "A2DP connection state changed");
            break;
        case ESP_A2D_AUDIO_STATE_EVT:
            ESP_LOGI(TAG, "A2DP audio state changed");
            break;
        case ESP_A2D_AUDIO_CFG_EVT:
            ESP_LOGI(TAG, "A2DP audio config received");
            break;
        default:
            break;
    }
}

// GAP callback to set device name
static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    switch (event) {
        case ESP_BT_GAP_DISC_RES_EVT:
            break;
        default:
            break;
    }
}

void bt_audio_init(void) {
    /* Release BLE memory */
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    /* Init controller */
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));

    /* Enable CLASSIC BT */
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));

    /* Init Bluedroid */
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    /* Continue with A2DP */
    i2s_init();

    ESP_ERROR_CHECK(esp_a2d_register_callback(bt_app_a2d_cb));
    ESP_ERROR_CHECK(esp_a2d_sink_register_data_callback(bt_app_a2d_data_cb));
    ESP_ERROR_CHECK(esp_a2d_sink_init());

    ESP_ERROR_CHECK(
        esp_bt_gap_set_scan_mode(
            ESP_BT_CONNECTABLE,
            ESP_BT_GENERAL_DISCOVERABLE
        )
    );
}



void app_main(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    
    bt_audio_init();


    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    

}
