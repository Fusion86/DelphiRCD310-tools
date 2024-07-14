#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
// #include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "driver/uart.h"

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#include "esp_hidd.h"
#include "esp_hid_gap.h"

#include "blehid.h"
#include "rcd.h"

#define EX_UART_NUM UART_NUM_1
#define PATTERN_CHR_NUM (1)

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

static const char *TAG = "RCDBT";

static QueueHandle_t uart0_queue;
static bool is_volume_on = false;
static int current_audio_source = 0;

typedef struct
{
    TaskHandle_t task_hdl;
    esp_hidd_dev_t *hid_dev;
    uint8_t protocol_mode;
    uint8_t *buffer;
} local_param_t;

static local_param_t s_ble_hid_param = {0};

const unsigned char mediaReportMap[] = {
    0x05, 0x0C, // Usage Page (Consumer)
    0x09, 0x01, // Usage (Consumer Control)
    0xA1, 0x01, // Collection (Application)
    0x85, 0x03, //   Report ID (3)
    0x09, 0x02, //   Usage (Numeric Key Pad)
    0xA1, 0x02, //   Collection (Logical)
    0x05, 0x09, //     Usage Page (Button)
    0x19, 0x01, //     Usage Minimum (0x01)
    0x29, 0x0A, //     Usage Maximum (0x0A)
    0x15, 0x01, //     Logical Minimum (1)
    0x25, 0x0A, //     Logical Maximum (10)
    0x75, 0x04, //     Report Size (4)
    0x95, 0x01, //     Report Count (1)
    0x81, 0x00, //     Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,       //   End Collection
    0x05, 0x0C, //   Usage Page (Consumer)
    0x09, 0x86, //   Usage (Channel)
    0x15, 0xFF, //   Logical Minimum (-1)
    0x25, 0x01, //   Logical Maximum (1)
    0x75, 0x02, //   Report Size (2)
    0x95, 0x01, //   Report Count (1)
    0x81, 0x46, //   Input (Data,Var,Rel,No Wrap,Linear,Preferred State,Null State)
    0x09, 0xE9, //   Usage (Volume Increment)
    0x09, 0xEA, //   Usage (Volume Decrement)
    0x15, 0x00, //   Logical Minimum (0)
    0x75, 0x01, //   Report Size (1)
    0x95, 0x02, //   Report Count (2)
    0x81, 0x02, //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xE2, //   Usage (Mute)
    0x09, 0x30, //   Usage (Power)
    0x09, 0x83, //   Usage (Recall Last)
    0x09, 0x81, //   Usage (Assign Selection)
    0x09, 0xB0, //   Usage (Play)
    0x09, 0xB1, //   Usage (Pause)
    0x09, 0xB2, //   Usage (Record)
    0x09, 0xB3, //   Usage (Fast Forward)
    0x09, 0xB4, //   Usage (Rewind)
    0x09, 0xB5, //   Usage (Scan Next Track)
    0x09, 0xB6, //   Usage (Scan Previous Track)
    0x09, 0xB7, //   Usage (Stop)
    0x15, 0x01, //   Logical Minimum (1)
    0x25, 0x0C, //   Logical Maximum (12)
    0x75, 0x04, //   Report Size (4)
    0x95, 0x01, //   Report Count (1)
    0x81, 0x00, //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x80, //   Usage (Selection)
    0xA1, 0x02, //   Collection (Logical)
    0x05, 0x09, //     Usage Page (Button)
    0x19, 0x01, //     Usage Minimum (0x01)
    0x29, 0x03, //     Usage Maximum (0x03)
    0x15, 0x01, //     Logical Minimum (1)
    0x25, 0x03, //     Logical Maximum (3)
    0x75, 0x02, //     Report Size (2)
    0x81, 0x00, //     Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,       //   End Collection
    0x81, 0x03, //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,       // End Collection
};

static esp_hid_raw_report_map_t ble_report_maps[] = {
    /* This block is compiled for bluedroid as well */
    {
        .data = mediaReportMap,
        .len = sizeof(mediaReportMap)}};

static esp_hid_device_config_t ble_hid_config = {
    .vendor_id = 0x16C0,
    .product_id = 0x05DF,
    .version = 0x0100,
    .device_name = "RCDcontrol",
    .manufacturer_name = "Espressif",
    .serial_number = "1234567890",
    .report_maps = ble_report_maps,
    .report_maps_len = 1};

void esp_hidd_send_consumer_value(uint8_t key_cmd, bool key_pressed)
{
    uint8_t buffer[HID_CC_IN_RPT_LEN] = {0, 0};
    if (key_pressed)
    {
        switch (key_cmd)
        {
        case HID_CONSUMER_CHANNEL_UP:
            HID_CC_RPT_SET_CHANNEL(buffer, HID_CC_RPT_CHANNEL_UP);
            break;

        case HID_CONSUMER_CHANNEL_DOWN:
            HID_CC_RPT_SET_CHANNEL(buffer, HID_CC_RPT_CHANNEL_DOWN);
            break;

        case HID_CONSUMER_VOLUME_UP:
            HID_CC_RPT_SET_VOLUME_UP(buffer);
            break;

        case HID_CONSUMER_VOLUME_DOWN:
            HID_CC_RPT_SET_VOLUME_DOWN(buffer);
            break;

        case HID_CONSUMER_MUTE:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_MUTE);
            break;

        case HID_CONSUMER_POWER:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_POWER);
            break;

        case HID_CONSUMER_RECALL_LAST:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_LAST);
            break;

        case HID_CONSUMER_ASSIGN_SEL:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_ASSIGN_SEL);
            break;

        case HID_CONSUMER_PLAY:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_PLAY);
            break;

        case HID_CONSUMER_PAUSE:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_PAUSE);
            break;

        case HID_CONSUMER_RECORD:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_RECORD);
            break;

        case HID_CONSUMER_FAST_FORWARD:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_FAST_FWD);
            break;

        case HID_CONSUMER_REWIND:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_REWIND);
            break;

        case HID_CONSUMER_SCAN_NEXT_TRK:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_SCAN_NEXT_TRK);
            break;

        case HID_CONSUMER_SCAN_PREV_TRK:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_SCAN_PREV_TRK);
            break;

        case HID_CONSUMER_STOP:
            HID_CC_RPT_SET_BUTTON(buffer, HID_CC_RPT_STOP);
            break;

        default:
            break;
        }
    }
    esp_hidd_dev_input_set(s_ble_hid_param.hid_dev, 0, HID_RPT_ID_CC_IN, buffer, HID_CC_IN_RPT_LEN);
    return;
}

// void ble_hid_demo_task(void *pvParameters)
// {
//     static bool send_volum_up = false;
//     while (1)
//     {
//         ESP_LOGI(TAG, "Send the volume");
//         if (send_volum_up)
//         {
//             esp_hidd_send_consumer_value(HID_CONSUMER_VOLUME_UP, true);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//             esp_hidd_send_consumer_value(HID_CONSUMER_VOLUME_UP, false);
//         }
//         else
//         {
//             esp_hidd_send_consumer_value(HID_CONSUMER_VOLUME_DOWN, true);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//             esp_hidd_send_consumer_value(HID_CONSUMER_VOLUME_DOWN, false);
//         }
//         send_volum_up = !send_volum_up;
//         vTaskDelay(2000 / portTICK_PERIOD_MS);
//     }
// }

void ble_hid_task_start_up(void)
{
    //     if (s_ble_hid_param.task_hdl)
    //     {
    //         // Task already exists
    //         return;
    //     }
    //     /* Executed for bluedroid */
    //     xTaskCreate(ble_hid_demo_task, "ble_hid_demo_task", 2 * 1024, NULL, configMAX_PRIORITIES - 3,
    //                 &s_ble_hid_param.task_hdl);
}

void ble_hid_task_shut_down(void)
{
    //     if (s_ble_hid_param.task_hdl)
    //     {
    //         vTaskDelete(s_ble_hid_param.task_hdl);
    //         s_ble_hid_param.task_hdl = NULL;
    //     }
}

static void ble_hidd_event_callback(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    esp_hidd_event_t event = (esp_hidd_event_t)id;
    esp_hidd_event_data_t *param = (esp_hidd_event_data_t *)event_data;
    static const char *TAG = "HID_DEV_BLE";

    switch (event)
    {
    case ESP_HIDD_START_EVENT:
    {
        ESP_LOGI(TAG, "START");
        esp_hid_ble_gap_adv_start();
        break;
    }
    case ESP_HIDD_CONNECT_EVENT:
    {
        ESP_LOGI(TAG, "CONNECT");
        break;
    }
    case ESP_HIDD_PROTOCOL_MODE_EVENT:
    {
        ESP_LOGI(TAG, "PROTOCOL MODE[%u]: %s", param->protocol_mode.map_index, param->protocol_mode.protocol_mode ? "REPORT" : "BOOT");
        break;
    }
    case ESP_HIDD_CONTROL_EVENT:
    {
        ESP_LOGI(TAG, "CONTROL[%u]: %sSUSPEND", param->control.map_index, param->control.control ? "EXIT_" : "");
        if (param->control.control)
        {
            // exit suspend
            // ble_hid_task_start_up();
        }
        else
        {
            // suspend
            // ble_hid_task_shut_down();
        }
        break;
    }
    case ESP_HIDD_OUTPUT_EVENT:
    {
        ESP_LOGI(TAG, "OUTPUT[%u]: %8s ID: %2u, Len: %d, Data:", param->output.map_index, esp_hid_usage_str(param->output.usage), param->output.report_id, param->output.length);
        ESP_LOG_BUFFER_HEX(TAG, param->output.data, param->output.length);
        break;
    }
    case ESP_HIDD_FEATURE_EVENT:
    {
        ESP_LOGI(TAG, "FEATURE[%u]: %8s ID: %2u, Len: %d, Data:", param->feature.map_index, esp_hid_usage_str(param->feature.usage), param->feature.report_id, param->feature.length);
        ESP_LOG_BUFFER_HEX(TAG, param->feature.data, param->feature.length);
        break;
    }
    case ESP_HIDD_DISCONNECT_EVENT:
    {
        ESP_LOGI(TAG, "DISCONNECT: %s", esp_hid_disconnect_reason_str(esp_hidd_dev_transport_get(param->disconnect.dev), param->disconnect.reason));
        // ble_hid_task_shut_down();
        esp_hid_ble_gap_adv_start();
        break;
    }
    case ESP_HIDD_STOP_EVENT:
    {
        ESP_LOGI(TAG, "STOP");
        break;
    }
    default:
        break;
    }
    return;
}

void uart_log_handler(const char *data, int len)
{
    // Looks like we don't have sscanf_s, so a shitty workaround.
    if (len < 32)
    {
        return;
    }

    int ts, id, param;
    int res = sscanf(data, "%d INFO M ID: 0x%x P: 0x%x", &ts, &id, &param);

    // We don't parse the module name, so we just use `is_keyboard_event()` to check whether the values are valid.
    // Not sure if this is faster.
    if (res == 3)
    {
        if (is_keyboard_event(id, param))
        {
            ESP_LOGE(TAG, "Got key! ID: %s, P: %s", rcd_event_to_string(id), keyboard_btn_to_string(param));

            // Only handle events when AUX is the current audio source.
            if (current_audio_source == SRC_AUX && is_volume_on && id == EVT_KEY_DOWN)
            {
                // This could be sent to a queue and then handled in a different thread. But that is probably not needed?
                switch (param)
                {
                case BTN_TRACK_NEXT:
                    ESP_LOGE(TAG, "Sending event: HID_CONSUMER_SCAN_NEXT_TRK");
                    esp_hidd_send_consumer_value(HID_CONSUMER_SCAN_NEXT_TRK, true);
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    esp_hidd_send_consumer_value(HID_CONSUMER_SCAN_NEXT_TRK, false);
                    break;
                case BTN_TRACK_PREV:
                    ESP_LOGE(TAG, "Sending event: HID_CONSUMER_SCAN_PREV_TRK");
                    esp_hidd_send_consumer_value(HID_CONSUMER_SCAN_PREV_TRK, true);
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    esp_hidd_send_consumer_value(HID_CONSUMER_SCAN_PREV_TRK, false);
                    break;
                }
            }
        }
        else if (id == EVT_SWITCH_MEDIA_SRC)
        {
            current_audio_source = param & 0b1111;
            is_volume_on = (param & SRC_SOUND_ON) >> 16;

            ESP_LOGE(TAG, "current_audio_source: %i, is_volume_on: %i", current_audio_source, is_volume_on);
        }
        else
        {
            ESP_LOGW(TAG, "Got event! ID: %s, P: 0x%x", rcd_event_to_string(id), param);
        }
    }
    else
    {
        ESP_LOGW(TAG, "sscanf_result != 3");
    }
}

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE);
    for (;;)
    {
        // Waiting for UART event.
        if (xQueueReceive(uart0_queue, (void *)&event, (TickType_t)portMAX_DELAY))
        {
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
            switch (event.type)
            {
            // Event of UART receving data
            /*We'd better handler data event fast, there would be much more data events than
            other types of events. If we take too much time on data event, the queue might
            be full.*/
            case UART_DATA:
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "[DATA EVT]:");
                uart_write_bytes(EX_UART_NUM, (const char *)dtmp, event.size);
                break;
            // Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(EX_UART_NUM);
                xQueueReset(uart0_queue);
                break;
            // Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                // If buffer full happened, you should consider increasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(EX_UART_NUM);
                xQueueReset(uart0_queue);
                break;
            // Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                break;
            // Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;
            // Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;
            // UART_PATTERN_DET
            case UART_PATTERN_DET:
                uart_get_buffered_data_len(EX_UART_NUM, &buffered_size);
                int pos = uart_pattern_pop_pos(EX_UART_NUM);
                ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                if (pos == -1)
                {
                    // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
                    // record the position. We should set a larger queue size.
                    // As an example, we directly flush the rx buffer here.
                    uart_flush_input(EX_UART_NUM);
                }
                else
                {
                    // uart_read_bytes(EX_UART_NUM, dtmp, pos, 100 / portTICK_PERIOD_MS);
                    // uint8_t pat[PATTERN_CHR_NUM + 1];
                    // memset(pat, 0, sizeof(pat));
                    // uart_read_bytes(EX_UART_NUM, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                    // ESP_LOGI(TAG, "read data: %s", dtmp);
                    // ESP_LOGI(TAG, "read pat : %s", pat);

                    int read_len = uart_read_bytes(EX_UART_NUM, dtmp, pos + 1, 100 / portTICK_PERIOD_MS);

                    // make sure the line is a standard string, though it might not be needed?
                    dtmp[read_len] = '\0';

                    ESP_LOGI(TAG, "read data: %s", dtmp);
                    uart_log_handler((const char *)dtmp, read_len);
                }
                break;
            // Others
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_err_t ret;
    // UART setup
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Install UART driver, and get the queue.
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);

    // Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(EX_UART_NUM, 5, 4, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Set uart pattern detect function.
    // The RCD doesn't cleanly flush its contents (or maybe it uses RTS/CTS, idk how that works).
    // This means that the last line is usually not detected. This is usually the EVT_KEY_UP event, so that doesn't matter too much.
    uart_enable_pattern_det_baud_intr(EX_UART_NUM, '\n', PATTERN_CHR_NUM, 9, 0, 0);
    // Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(EX_UART_NUM, 20);

    // Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);

    // Bluetooth setup.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "setting hid gap, mode:%d", HID_DEV_MODE);
    ret = esp_hid_gap_init(HID_DEV_MODE);
    ESP_ERROR_CHECK(ret);

    ret = esp_hid_ble_gap_adv_init(ESP_HID_APPEARANCE_GENERIC, ble_hid_config.device_name);
    ESP_ERROR_CHECK(ret);
    if ((ret = esp_ble_gatts_register_callback(esp_hidd_gatts_event_handler)) != ESP_OK)
    {
        ESP_LOGE(TAG, "GATTS register callback failed: %d", ret);
        return;
    }
    ESP_LOGI(TAG, "setting ble device");
    ESP_ERROR_CHECK(
        esp_hidd_dev_init(&ble_hid_config, ESP_HID_TRANSPORT_BLE, ble_hidd_event_callback, &s_ble_hid_param.hid_dev));
}