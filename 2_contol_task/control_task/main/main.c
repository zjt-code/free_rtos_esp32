#include <stdio.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include "lcd.h"
// #include "soc/rtc.h"
#include "esp_system.h"
// #include "esp_rom_sys.h"



volatile static bool b_lcd_busy =false;

SemaphoreHandle_t xSemaphore_lcd = NULL;

static TaskHandle_t xMain = NULL, xTask01 = NULL, xTask02=NULL;

void my_Task01(void *pvParameter)
{
    static u32  task01_cnt=0;
    u8   buff[16];
    while (1) 
    {
        /* code */
      
        /* Block to wait for prvTask1() to notify this task. */     
        
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );  
        {  
            task01_cnt++;
            
            // if(!b_lcd_busy)
            if(xSemaphoreTake(xSemaphore_lcd,1000/portTICK_PERIOD_MS)==pdTRUE)
            {    b_lcd_busy = true;  
                 Show_Str(0,80,(u8*)"Task01_loop:\r\n",16,1);
                 sprintf((char *)buff,"%u\r\n",task01_cnt);
                 Show_Str(0,100,buff,16,0);
                 b_lcd_busy = false;

                 xSemaphoreGive(xSemaphore_lcd);
             }

            printf("task01_n = %d\r\n",task01_cnt);
        }       
         
        
    }
    
}

void my_Task02(void *pvParameters)
{

    TickType_t xLaskWakeUptime = xTaskGetTickCount();
    static uint32_t t1_no=0,t2_no=0, t2_cnt=0;

    while(1)
    {

        //vTaskDelayUntil(&xLaskWakeUptime,1000/portTICK_PERIOD_MS);
        vTaskDelayUntil(&xLaskWakeUptime,500/portTICK_PERIOD_MS);
        t2_cnt++;

        printf(" Task02 count=%lu , t2_no= %lu\r\n",t1_no,t2_no);

        if(t2_cnt%2 ==0)
        {
            xTaskNotifyGive(xTask01);
            t1_no++;
        }        
        xTaskNotifyGive(xMain);
        t2_no++;
    
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
   // Show_Str(0,0,(u8 *)"@jtNight14",16,1);

    u16 lcd_id = LCD_Read_ID(0x04);
    u8  buf[16] = {0};

    printf("Chip ID=0X%x\r\n",lcd_id);
    sprintf((char *)buf,"ID=0X%x\r\n",lcd_id);

    Show_Str(0,16,buf,16,1);


    printf("idf_version =%s\r\n",esp_get_idf_version());


    Show_Str(0,32,esp_get_idf_version(),16,1);


    TickType_t xLaskWakeUptime = xTaskGetTickCount();

    const TickType_t delay = 100/portTICK_PERIOD_MS;
    
    xTaskCreate(my_Task01,"my_Task01",4096,(void*)1,tskIDLE_PRIORITY+1,&xTask01);
    configASSERT( xTask01 );
    xTaskCreate(my_Task02,"my_Task02",4096,(void*)1,tskIDLE_PRIORITY+1,&xTask02);
    configASSERT( xTask02 );

    // get app_main task handle  and save to global static   
    xMain = xTaskGetCurrentTaskHandle();

    xSemaphore_lcd = xSemaphoreCreateMutex();

    u8 my_cnt_str[16];
    static u32 main_cnt=0;

    while (1)
    { 

      /* Block to wait for prvTask2() to notify this task. */  
          ulTaskNotifyTakeIndexed( 0, pdTRUE, portMAX_DELAY );
          main_cnt++;
        //if(!b_lcd_busy)
        if(xSemaphoreTake(xSemaphore_lcd,1000/portTICK_PERIOD_MS)==pdTRUE)
        {
            b_lcd_busy =true;    
            Show_Str(0,48,(u8*)"Main_Task_Loop:\r\n",16,1);
            sprintf((char *)my_cnt_str,"%d\r\n",main_cnt);
            Show_Str(0,64,my_cnt_str,16,0);
            b_lcd_busy = false;

            xSemaphoreGive(xSemaphore_lcd);
    
        }  

         printf(" Main task count=%u\r\n",main_cnt);      
    }
    

}
