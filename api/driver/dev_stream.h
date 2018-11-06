#ifndef __DEV_STREAM_H__
#define __DEV_STREAM_H__

typedef struct {
	GeneralReg   *reg;
	unsigned int  ext_reg_size;
	unsigned int  ext_reg_num;
	void         *ext_reg_phys_addr;
	void         *ext_reg_virt_addr;
	unsigned int  run_freq;
	void         *run_space_phys_addr;
	void         *run_space_vrit_addr;
	unsigned int  run_space_size;
	void         *firmware_addr;
	void         *firmware_size;
	unsigned int  run_task0_id;
	unsigned int  run_task1_id;
} DevStream;

typedef struct {
} SubDevStatus;

typedef struct {
	DevStream    *dev;
	ExtReadReg   *r_reg;
	void         *r_phys_reg;
	ExtWriteReg  *w_reg;
	void         *w_phys_reg;
	ExtDebugReg  *d_reg;
	void         *d_phys_reg;
	ExtBufferReg *b_reg;
	void         *b_phys_reg;
	unsigned int  task_id;
	unsigned int  task_active;
} SubDevStream;

extern int  stream_init  (void);
extern void stream_uninit(void);
extern int  stream_isr   (void);

#endif
