#include "chip.h"
#include "dsp32_timer.h"

#define CNT_EN_REG  (*(volatile unsigned long*)(CONFIG_TIMER_BASE_ADDR))
#define CNT_CLR_REG (*(volatile unsigned long*)(CONFIG_TIMER_BASE_ADDR + 4))
#define COUNTER     (CONFIG_TIMER_BASE_ADDR + 8)

void dsp32_timer_init(void)
{
	dsp32_timer_clear(0);
	dsp32_timer_clear(1);
	dsp32_timer_clear(2);
	dsp32_timer_clear(3);
	dsp32_timer_clear(4);
	dsp32_timer_clear(5);
}

void dsp32_timer_start(int num)
{
	CNT_EN_REG |= (1 << cnt_num);
}

void dsp32_timer_stop(int num)
{
	CNT_EN_REG &= ~(1 << cnt_num);
}

void dsp32_timer_clear(int num)
{
	CNT_CLR_REG = (1 << cnt_num);
	CNT_CLR_REG = 0;
}

int dsp32_timer_read(int num)
{
	volatile unsigned long* temp;
	unsigned long time;
	temp = (volatile unsigned long*)(COUNTER + (cnt_num << 2));
	time = (unsigned long)(*temp);

	return (int)time;
}
