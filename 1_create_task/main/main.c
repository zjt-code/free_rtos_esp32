#include <stdio.h>
#include <FreeRTOSConfig.h>
#include <esp_task.h>

#include<driver/gpio.h>


#define CONFIG_GPIO_OUTPUT_0    14
#define CONFIG_GPIO_OUTPUT_1    15

#define GPIO_OUTPUT_IO_0    CONFIG_GPIO_OUTPUT_0
#define GPIO_OUTPUT_IO_1    CONFIG_GPIO_OUTPUT_1
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))

static uint32_t lop=0;

void myTask_01(void * pvParameters)
{
    uint32_t task_01_lop=0;

     configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    while(1)
    {

       vTaskDelay(500/portTICK_PERIOD_MS);
       printf("my01---%lu\r\n",task_01_lop++);
       // fflush(stdout);
      // task_01_lop++;

       if(task_01_lop%2)
       {
        gpio_set_level(CONFIG_GPIO_OUTPUT_0,1);
        gpio_set_level(CONFIG_GPIO_OUTPUT_1,1);

       }
       else
       {
         gpio_set_level(CONFIG_GPIO_OUTPUT_0,0);
        gpio_set_level(CONFIG_GPIO_OUTPUT_1,0);
       }
        
    }

}

void app_main(void)
{

   static uint32_t main_loop=0;

    TaskHandle_t * pH_task01=NULL;

    // must enable external psram
    xTaskCreate(myTask_01,"myTask_01", 60,(void *)1,tskIDLE_PRIORITY + 1,pH_task01);


      //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);


    while (1)
    {
        /* code */
        vTaskDelay(500/portTICK_PERIOD_MS);
       printf("hello---%lu\r\n", main_loop++);
       // fflush(stdout);

      

    //    if(task_01_lop%2)
    //    {
    //     gpio_set_level(CONFIG_GPIO_OUTPUT_0,1);
    //     gpio_set_level(CONFIG_GPIO_OUTPUT_1,1);

    //    }
    //    else
    //    {
    //      gpio_set_level(CONFIG_GPIO_OUTPUT_0,0);
    //     gpio_set_level(CONFIG_GPIO_OUTPUT_1,0);
    //    }
    }
    
}
