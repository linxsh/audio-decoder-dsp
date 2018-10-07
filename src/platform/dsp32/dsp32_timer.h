#ifndef __DSP32_TIMER_H__
#define __DSP32_TIMER_H__

extern void dsp32_timer_init   (void);
extern void dsp32_timer_start  (int num);
extern void dsp32_timer_stop   (int num);
extern void dsp32_timer_clear  (int num);
extern int  dsp32_timer_read_ms(int num);

#endif
