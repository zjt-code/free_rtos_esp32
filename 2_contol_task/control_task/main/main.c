#include <stdio.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include "lcd.h"
// #include "soc/rtc.h"
#include "esp_system.h"
// #include "esp_rom_sys.h"


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

    LCD_Init();
    LCD_Display_Dir(USE_LCM_DIR);
    LCD_Clear(YELLOW);		//清屏	

    
   // esp_rom_delay_us(10);

    LCD_DisplayOn();
    LCD_Clear(YELLOW);

   // LCD_ShowChar(0,16,'A',16,1);
    Show_Str(0,0,(u8 *)"@jtNitht14",16,1);

    u16 lcd_id = LCD_Read_ID(0x04);
    u8  buf[16] = {0};

    printf("Chip ID=0X%x\r\n",lcd_id);
    sprintf((char *)buf,"ID=0X%x\r\n",lcd_id);

    Show_Str(0,16,buf,16,1);


   // printf()

    TickType_t xLaskWakeUptime = xTaskGetTickCount();

    const TickType_t delay = 500/portTICK_PERIOD_MS;
    
    xTaskCreate(my_Task01,"my_Task01",40,(void*)1,tskIDLE_PRIORITY,NULL);
    
    while (1)
    {
        /* code */
        vTaskDelayUntil(&xLaskWakeUptime, delay);
        //lcd_rst_set(0);

        printf("Main Task\r\n");
        printf("U16 LEN =%d\r\n",sizeof(unsigned short));

        vTaskDelayUntil(&xLaskWakeUptime, delay);

       // lcd_rst_set(1);
       
    }
    

}
