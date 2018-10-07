#ifndef __OS_TIMER_H__
#define __OS_TIMER_H__

extern void os_timer_init   (unsigned int freq);
extern void os_timer_start  (unsigned int num);
extern void os_timer_stop   (unsigned int num);
extern void os_timer_clear  (unsigned int num);
extern int  os_timer_read_ms(unsigned int num);

#endif
