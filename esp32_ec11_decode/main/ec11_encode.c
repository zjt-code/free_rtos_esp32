
#include<stdio.h>
#include<driver/gpio.h>
#include<driver/gpio_filter.h>
#include "ec11_encode.h"
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio_filter.h"

static const char* TAG = "ec11_encode";

static bool b_ec11_changed=false;
static bool b_ec11_key_changed =false;
static t_key_State iState_ec11 =K_STATE_NULL;
static t_ec11_clock iEcll_Clock_value=EC11_ROT_NONE;
static t_key_State ecll_state_buff[4]={K_STATE_NULL,K_STATE_NULL,K_STATE_NULL,K_STATE_NULL};
#if 0
static const t_key_State ecll_Clockwise[4]={K_STATE_A_DOWN,K_STATE_B_DOWN,K_STATE_A_UP,K_STATE_B_UP};
static const t_key_State ecll_CounterClockwise[4]={K_STATE_B_DOWN,K_STATE_A_DOWN,K_STATE_B_UP,K_STATE_A_UP};
#endif

// 创建队列用于从ISR向任务传递事件
QueueHandle_t gpio_evt_queue = NULL;

// 中断事件结构体
typedef struct {
    uint32_t gpio_num;
    uint32_t level;
    t_ec11_clock iEcll_Clock_value;
    bool b_ec11_changed;
    bool b_ec11_key_changed;
} gpio_event_t;




static uint8_t ecll_buff_cnt=0;

/***************static functin */
static void IRAM_ATTR ec11_gpio_isr( void *arg)
{
    static gpio_event_t evt;
    uint32_t gpio_num = (uint32_t) arg;
    // 获取中断状态
      uint32_t level = gpio_get_level(gpio_num);  

    
    evt.gpio_num = gpio_num;
    evt.level=level;

      // 发送事件到队列
  //  xQueueSendFromISR(gpio_evt_queue, &evt, NULL);

#if 0
    if(level == 0){
         if(gpio_num == PIN_EC11_A){
            ecll_state_buff[ecll_buff_cnt++]=K_STATE_A_DOWN;
         }

         if(gpio_num == PIN_EC11_B){
             ecll_state_buff[ecll_buff_cnt++]=K_STATE_B_DOWN;
         }

     }

     if(level ==1) {
         if(gpio_num == PIN_EC11_A){
            ecll_state_buff[ecll_buff_cnt++]=K_STATE_A_UP;
         }

         if(gpio_num == PIN_EC11_B){
            ecll_state_buff[ecll_buff_cnt++]=K_STATE_B_UP;
         }
     }

      if(ecll_buff_cnt >=4){    

        if(!memcmp(ecll_state_buff,ecll_Clockwise,sizeof(ecll_Clockwise))){
        
            b_ec11_changed = true;
            iEcll_Clock_value = EC11_CLOCKWISE;

        }else if (!memcmp(ecll_state_buff,ecll_CounterClockwise,sizeof(ecll_CounterClockwise))){
            
            b_ec11_changed = true;
            iEcll_Clock_value = EC11_COUNTERCLOKCWISE;        
        }
        ecll_buff_cnt=0; 
        memset(ecll_state_buff,K_STATE_NULL,sizeof(ecll_state_buff));
    }
  #endif
  
  #if 1
      
    if((gpio_num ==PIN_EC11_A)||(gpio_num ==PIN_EC11_B)){
        // 
        if(iState_ec11 == K_STATE_NULL) {
            if((gpio_num == PIN_EC11_A)&&(level==0)) {
                iEcll_Clock_value = EC11_CLOCKWISE;
                iState_ec11=K_STATE_A_DOWN;
            }

             if((gpio_num == PIN_EC11_B)&&(level==0)){
                iEcll_Clock_value = EC11_COUNTERCLOKCWISE;
                iState_ec11=K_STATE_B_DOWN;
            }


        }
        else{

            if(iEcll_Clock_value ==EC11_CLOCKWISE){
                switch(iState_ec11)
                {
                    case K_STATE_A_DOWN:
                    if((gpio_num == PIN_EC11_B)&&(level==0)) // B down
                    {
                        iState_ec11 = K_STATE_B_DOWN;
                    }
                    break;
                    case K_STATE_B_DOWN:
                    if((gpio_num == PIN_EC11_A)&&(level==1)) // A UP
                    {
                        iState_ec11 = K_STATE_A_UP;
                    }
                    break;
                    case K_STATE_A_UP:
                     if((gpio_num == PIN_EC11_B)&&(level==1)) //B UP
                    {
                        iState_ec11 = K_STATE_NULL;
                        b_ec11_changed =true;
                        evt.b_ec11_changed =true;
                        evt.iEcll_Clock_value = EC11_CLOCKWISE;
                              // 发送事件到队列                       
                        xQueueSendFromISR(gpio_evt_queue, &evt, NULL);
                    }
                    break;
                    case K_STATE_B_UP:                   
                    default:
                    break;
                }

            }
            else if (iEcll_Clock_value ==EC11_COUNTERCLOKCWISE)
            {
                switch(iState_ec11)
                {
                    case K_STATE_B_DOWN:
                    if((gpio_num == PIN_EC11_A)&&(level==0)) // B down
                    {
                        iState_ec11 = K_STATE_A_DOWN;
                    }
                    break;
                    case K_STATE_A_DOWN:
                    if((gpio_num == PIN_EC11_B)&&(level==1)) // B UP
                    {
                        iState_ec11 = K_STATE_B_UP;
                    }
                    break;
                    case K_STATE_B_UP:
                     if((gpio_num == PIN_EC11_A)&&(level==1)) //A UP
                    {
                        iState_ec11 = K_STATE_NULL;
                        b_ec11_changed =true;
                        evt.b_ec11_changed =true;
                        evt.iEcll_Clock_value = EC11_COUNTERCLOKCWISE;
                              // 发送事件到队列                       
                        xQueueSendFromISR(gpio_evt_queue, &evt, NULL);
                    }
                    break;
                    case K_STATE_A_UP:                   
                    default:
                    break;
                }
            }
            

            }

        }
       
  #endif  

    if(gpio_num ==PIN_EC11_KEY)
    {

    }

}



// 任务处理GPIO事件
static void gpio_task_example(void* arg) {
    gpio_event_t event;
    uint32_t a=0,b=0;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &event, portMAX_DELAY)) {          
              
                if(event.b_ec11_changed)
                {
                    event.b_ec11_changed = false;
                    if(event.iEcll_Clock_value == EC11_CLOCKWISE){
                        a++;
                    }
                    if(event.iEcll_Clock_value == EC11_COUNTERCLOKCWISE){
                        b++;
                    }
                    printf("EC11_CLOCKWISE = %lu, EC11_COUNTERCLOKCWISE= %lu \r\n",a,b);
                }     
        }
      
    }
}


void ec11_encode_init(void)
{

    gpio_config_t ec11_GPIOConfig=
    {
        .pin_bit_mask = (1<<PIN_EC11_A)|(1<<PIN_EC11_B)|(1<<PIN_EC11_KEY),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en =false,
        .pull_up_en = true,
        .intr_type = GPIO_INTR_ANYEDGE,
    };

    gpio_config(&ec11_GPIOConfig);

    gpio_glitch_filter_handle_t  pin_glith_handle_a=NULL,pin_glith_handle_b=NULL,pin_glith_handle_key=NULL;

    gpio_pin_glitch_filter_config_t pin_glitch_a={},pin_glitch_b={},pin_glitch_key={};
    pin_glitch_a.clk_src =GLITCH_FILTER_CLK_SRC_DEFAULT;
    pin_glitch_a.gpio_num = PIN_EC11_A;


    pin_glitch_b.clk_src =GLITCH_FILTER_CLK_SRC_DEFAULT;
    pin_glitch_b.gpio_num = PIN_EC11_B;


    pin_glitch_key.clk_src =GLITCH_FILTER_CLK_SRC_DEFAULT;
    pin_glitch_key.gpio_num = PIN_EC11_KEY;

     gpio_new_pin_glitch_filter(&pin_glitch_a,&pin_glith_handle_a);
     gpio_new_pin_glitch_filter(&pin_glitch_b,&pin_glith_handle_b);
     gpio_new_pin_glitch_filter(&pin_glitch_key,&pin_glith_handle_key);

    gpio_glitch_filter_enable(pin_glith_handle_a);
    gpio_glitch_filter_enable(pin_glith_handle_b);
    gpio_glitch_filter_enable(pin_glith_handle_key);

    
    esp_err_t err_t= gpio_install_isr_service(0) ;

    if(err_t !=ESP_OK)
    {
        printf("install isr service fail \\r\\r");
    }
    //  gpio_num_t num_a =PIN_EC11_A,num_b =PIN_EC11_A,num_press =PIN_EC11_A;
    err_t=gpio_isr_handler_add(PIN_EC11_A,ec11_gpio_isr, (void *)PIN_EC11_A);
     if(err_t !=ESP_OK)
    {
        printf("install isr service fail \\r\\r");
    }
    err_t= gpio_isr_handler_add(PIN_EC11_B,ec11_gpio_isr, (void *)PIN_EC11_B);
      if(err_t !=ESP_OK)
    {
        printf("install isr service fail \\r\\r");
    }
      err_t=gpio_isr_handler_add(PIN_EC11_KEY,ec11_gpio_isr, (void *)PIN_EC11_KEY);
    if(err_t !=ESP_OK)
    {
        printf("install isr service fail \\r\\r");
    }

    // gpio_isr_register
    printf(" encc_encode init......\r\n");

    // create
    gpio_evt_queue = xQueueCreate(10,sizeof(gpio_event_t));

    if(gpio_evt_queue ==NULL)
    {
        printf("create gpio evt queue fail\r\n ");
    }
       // 创建任务处理GPIO事件
    xTaskCreate(gpio_task_example, "gpio_task", 4096, NULL, 10, NULL);

}


bool ec11_encode_get_changed(void)
{
    if(b_ec11_changed)
    {
        b_ec11_changed = false;
        return true;
    }
    else{return false;}
}

uint8_t ecll_encode_rot(void)
{
    return iEcll_Clock_value;
}