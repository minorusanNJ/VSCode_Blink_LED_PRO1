#include "system.h"
#include "bsp.h"
#include "GPIO/gpio.h"

void _init(void) {
    // 何もしない
}
int main(void)
{
    system_init();
    bsp_init();
    while(1)
    {
        bsp_led_toggle();
		system_delay(800000);
    }
}