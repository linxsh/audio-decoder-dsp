#include "os_timer.h"
#ifdef CONFIG_DSP32
#include "../dsp32/dsp32_timer.h"
#endif
#ifdef CONFIG_X86
#include "../x86/x86_timer.h"
#endif

static unsigned int sFreq = 0;

void os_timer_init(unsigned int freq)
{
	sFreq = freq;
#ifdef CONFIG_DSP32
	dsp32_timer_init();
#endif
#ifdef CONFIG_X86
	x86_timer_init();
#endif
}

void os_timer_start(unsigned int num)
{
#ifdef CONFIG_DSP32
	dsp32_timer_start(num);
#endif
#ifdef CONFIG_X86
	x86_timer_start(num);
#endif
}

void os_timer_stop(unsigned int num)
{
#ifdef CONFIG_DSP32
	dsp32_timer_stop(num);
#endif
#ifdef CONFIG_X86
	x86_timer_stop(num);
#endif
}

void os_timer_clear(unsigned int num)
{
#ifdef CONFIG_DSP32
	dsp32_timer_clear(num);
#endif
#ifdef CONFIG_X86
	x86_timer_clear(num);
#endif
}

int os_timer_read_ms(unsigned int num)
{
#ifdef CONFIG_DSP32
	unsigned int cnt = 0;
	cnt = dsp32_timer_read(num);

	return (sFreq == 0) ? 0 : (cnt / (sFreq / 1000));
#endif
#ifdef CONFIG_X86
	return x86_timer_read_ms(num);
#endif
}

void os_timer_wait_ms(unsigned int num, unsigned int timeMs)
{
	os_timer_stop (num);
	os_timer_clear(num);
	os_timer_start(num);
	while (os_timer_read_ms(num) < timeMs);
	os_timer_stop (num);
}
