#include "driver/gpio.h"
#include "freertos/projdefs.h"

void blink_task(void){
    int count = 0;
    gpio_pad_select_gpio(2);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT_OUTPUT);
    //gpio_set_level(GPIO_NUM_2, 1);
    while(1){
    gpio_set_level(GPIO_NUM_2, !gpio_get_level(GPIO_NUM_2));
    vTaskDelay(500/portTICK_PERIOD_MS);
    }
}
