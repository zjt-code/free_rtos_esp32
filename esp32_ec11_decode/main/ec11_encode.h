#ifndef _EC11_ENCODE_H

#include <driver/gpio.h>
typedef enum {
    K_STATE_NULL,
    K_STATE_A_DOWN,
    K_STATE_A_UP,
    K_STATE_B_DOWN,
    K_STATE_B_UP,
    K_STATE_PRESS_DOWN,
    K_STATE_PRESS_UP,
}t_key_State;


typedef enum
{
 EC11_ROT_NONE,
 EC11_CLOCKWISE,
 EC11_COUNTERCLOKCWISE,

}t_ec11_clock;

#define PIN_EC11_A      GPIO_NUM_11
#define PIN_EC11_B      GPIO_NUM_12
#define PIN_EC11_KEY    GPIO_NUM_10

void ec11_encode_init(void);
uint8_t ecll_encode_rot(void);
bool ec11_encode_get_changed(void);

#endif