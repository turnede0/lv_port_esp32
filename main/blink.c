#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_ota_ops.h"
#include "driver/gpio.h"
#include "blink.h"

#define BLINK_GPIO GPIO_NUM_2
int LED_value =0; 
void blink_task(void *pvParameter) {
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    esp_app_desc_t running_app_info;
    const esp_partition_t *running = esp_ota_get_running_partition();
    if (esp_ota_get_partition_description(running, &running_app_info) != ESP_OK) {
        vTaskDelete(NULL);
    }
    const int version = atoi(running_app_info.version);
    while(1) {
        // blink the version number
        for (int i=0; i<version; ++i) {
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            LED_value = 1 ;
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            LED_value = -1 ;
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        
    }
}