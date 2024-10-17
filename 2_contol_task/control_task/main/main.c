#include <stdio.h>
#include<stdio.h>
#include<freertos/FreeRTOS.h>


void my_Task01(void *pvParameter)
{
    TickType_t task01_laskwaktu = xTaskGetTickCount();


    while (1)    {
        /* code */
        vTaskDelayUntil(&task01_laskwaktu,1000/portTICK_PERIOD_MS);
        printf("my_Task01 %s\r\n", __TIMESTAMP__);
    }
    
}

void app_main(void)
{

    TickType_t xLaskWakeUptime = xTaskGetTickCount();

    const TickType_t delay = 500/portTICK_PERIOD_MS;
    
    xTaskCreate(my_Task01,"my_Task01",40,(void*)1,tskIDLE_PRIORITY,NULL);
    
    while (1)
    {
        /* code */
        vTaskDelayUntil(&xLaskWakeUptime, delay);
        printf("Main Task\r\n");
       
    }
    

}
