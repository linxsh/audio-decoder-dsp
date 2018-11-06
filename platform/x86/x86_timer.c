#include <time.h>
#include "os_define.h"
#include "x86_timer.h"
#include "log_printf.h"

#define MAX_X86_TIMER 6

typedef struct {
	clock_t start_time;
	clock_t total_time;
} X86Timer;

static X86Timer x86Timer[MAX_X86_TIMER];

void x86_timer_init(void)
{
	os_memset(x86Timer, 0, sizeof(x86Timer));
}

void x86_timer_start(int num)
{
	if (num >= MAX_X86_TIMER) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: time init\n", __FUNCTION__, __LINE__);
		return;
	}

	x86Timer[num].start_time = clock();
}

void x86_timer_stop(int num)
{
	if (num >= MAX_X86_TIMER) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: time init\n", __FUNCTION__, __LINE__);
		return;
	}

	if (x86Timer[num].start_time != 0) {
		clock_t endTime = clock();

		x86Timer[num].total_time += (endTime - x86Timer[num].start_time);
		x86Timer[num].start_time  = 0;
	}
}

void x86_timer_clear(int num)
{
	if (num >= MAX_X86_TIMER) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: time init\n", __FUNCTION__, __LINE__);
		return;
	}

	os_memset(&x86Timer[num], 0, sizeof(X86Timer));
}

int x86_timer_read_ms(int num)
{
	if (num >= MAX_X86_TIMER) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: time init\n", __FUNCTION__, __LINE__);
		return 0;
	}

	return (int)(x86Timer[num].total_time/(CLOCKS_PER_SEC/1000));
}

