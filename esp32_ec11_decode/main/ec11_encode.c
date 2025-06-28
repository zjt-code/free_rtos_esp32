
#include<stdio.h>
#include<driver/gpio.h>
#include<driver/gpio_filter.h>
#include "ec11_encode.h"
#include <esp_heap_caps.h>
#include <esp_log.h>
static const char* TAG = "ec11_encode";

static bool b_ec11_changed=false;
static bool b_ec11_key_changed =false;
static t_key_State iState_ec11 =K_STATE_NULL;
static t_ec11_clock iEcll_Clock_value=EC11_ROT_NONE;
static t_key_State ecll_state_buff[4]={K_STATE_NULL,K_STATE_NULL,K_STATE_NULL,K_STATE_NULL};
static uint8_t ecll_buff_cnt=0;

/***************static functin */
static void IRAM_ATTR ec11_gpio_isr( void *arg)
{

    uint32_t gpio_num = (uint32_t) arg;
    // 获取中断状态
    uint32_t level = gpio_get_level(gpio_num);

    if((gpio_num ==PIN_EC11_A)||(gpio_num ==PIN_EC11_B))
    {
        // 
        if(iState_ec11 == K_STATE_NULL)
        {
            if((gpio_num == PIN_EC11_A)&&(level==0))
            {
                iEcll_Clock_value = EC11_CLOCKWISE;
                iState_ec11=K_STATE_A_DOWN;

            }

             if((gpio_num == PIN_EC11_B)&&(level==0))
            {
                iEcll_Clock_value = EC11_COUNTERCLOKCWISE;
                iState_ec11=k_STATE_B_DOWN;
            }


        }
        else{

            if(iEcll_Clock_value ==EC11_CLOCKWISE)
            {
                switch(iState_ec11)
                {
                    case K_STATE_A_DOWN:
                    if((gpio_num == PIN_EC11_B)&&(level==0)) // B down
                    {
                        iState_ec11 = k_STATE_B_DOWN;
                    }
                    break;
                    case k_STATE_B_DOWN:
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
                    }
                    break;
                    case k_STATE_B_UP:                   
                    default:
                    break;
                }

            }
            else if (iEcll_Clock_value ==EC11_COUNTERCLOKCWISE)
            {
                switch(iState_ec11)
                {
                    case k_STATE_B_DOWN:
                    if((gpio_num == PIN_EC11_A)&&(level==0)) // B down
                    {
                        iState_ec11 = K_STATE_A_DOWN;
                    }
                    break;
                    case K_STATE_A_DOWN:
                    if((gpio_num == PIN_EC11_B)&&(level==1)) // B UP
                    {
                        iState_ec11 = k_STATE_B_UP;
                    }
                    break;
                    case k_STATE_B_UP:
                     if((gpio_num == PIN_EC11_A)&&(level==1)) //A UP
                    {
                        iState_ec11 = K_STATE_NULL;
                        b_ec11_changed =true;
                    }
                    break;
                    case K_STATE_A_UP:                   
                    default:
                    break;
                }
            }
            

            }

        }
       
    

    if(gpio_num ==PIN_EC11_KEY)
    {

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

    
    esp_err_t err_t= gpio_install_isr_service(ESP_INTR_FLAG_IRAM) ;

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