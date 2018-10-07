#ifndef __X86_TIMER_H__
#define __X86_TIMER_H__

extern void x86_timer_init   (void);
extern void x86_timer_start  (int num);
extern void x86_timer_stop   (int num);
extern void x86_timer_clear  (int num);
extern int  x86_timer_read_ms(int num);

#endif
