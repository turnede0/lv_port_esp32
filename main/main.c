#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "https_ota.h"
#include "blink.h"
#include "adc_event.h"
#include "gui.h"
//#include "testpage_ui.h"




void app_main() {

    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the gui_task to core 0 */
    xTaskCreatePinnedToCore(&gui_task, "gui_task", 4096*2, NULL, 0, NULL, 1);
    
    //xTaskCreatePinnedToCore(&ota_task, "ota_task", 4 * 1024 * 8, NULL, 4, NULL, 0);
    xTaskCreatePinnedToCore(&blink_task, "blink_task", 1024 * 8, NULL, 0, NULL, 1);

    xTaskCreatePinnedToCore(&adc_event_task, "adc_event_task", 1024 * 8, NULL, 0, NULL, 1);
    
   // xTaskCreatePinnedToCore(&number_update_task, "number_update_task", 1024 * 8, NULL, 0, NULL, 0);
    //my_refr_func();
    vTaskDelay(2000 / portTICK_PERIOD_MS);

   

}