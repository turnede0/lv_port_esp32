#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "adc_event.h"
#include "driver/adc.h"


int adc_val_1 =0; 



void adc_event_task(void *pvParameter) {
    
    while(1) {
        // blink the version number
        for (int i=0; i<5; ++i) {

            adc1_config_width(ADC_WIDTH_BIT_12);
            adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_0);
            adc_val_1 = adc1_get_raw(ADC1_CHANNEL_6);


        }


        
    }
}