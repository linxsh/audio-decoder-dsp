#ifndef __REG_H__
#define __REG_H__

typedef struct {
} DecoderReadCtrl;

typedef struct {
} DecoderWriteCtrl;

typedef struct {
} DecoderDebugCtrl;

typedef struct {
} EncoderReadCtrl;

typedef struct {
} EncoderWriteCtrl;

typedef struct {
} EncoderDebugCtrl;

typedef struct {
} DecryptReadCtrl;

typedef struct {
} DecryptWriteCtrl;

typedef struct {
} DecryptDebugCtrl;

typedef struct {
} EncryptReadCtrl;

typedef struct {
} EncryptWriteCtrl;

typedef struct {
} EncryptDebugCtrl;

typedef struct {
} FilterReadCtrl;

typedef struct {
} FilterWriteCtrl;

typedef struct {
} FilterDebugCtrl;

typedef union {
	DecoderReadCtrl DECODER_R_REG;
	EncoderReadCtrl ENCODER_R_REG;
	DecryptReadCtrl DECRYPT_R_REG;
	EncryptReadCtrl ENCRYPT_R_REG;
	FilterReadCtrl   FILTER_R_REG;
} ExtraReadReg;

typedef union {
	DecoderWriteCtrl DECODER_W_REG;
	EncoderWriteCtrl ENCODER_W_REG;
	DecryptWriteCtrl DECRYPT_W_REG;
	EncryptWriteCtrl ENCRYPT_W_REG;
	FilterWriteCtrl   FILTER_W_REG;
} ExtraWriteReg;

typedef union {
	DecoderDebugCtrl DECODER_D_REG;
	EncoderDebugCtrl ENCODER_D_REG;
	DecryptDebugCtrl DECRYPT_D_REG;
	EncryptDebugCtrl ENCRYPT_D_REG;
	FilterDebugCtrl   FILTER_D_REG;
} ExtraDebugReg;

typedef struct {
	unsigned int RES0[1];                    /* 0x100 */
	unsigned int EEAR;
	unsigned int EPCR;
	unsigned int EXPC;
	unsigned int RUN_FREQUENCY;              /* 0x110 */
	unsigned int INTEN;
	unsigned int INT;
	unsigned int EXT0;
	unsigned int RUN_CTRL;                   /* 0x120  位置不能改变*/
	unsigned int RUN_SAPCE_S_ADDR;
	unsigned int RUN_SAPCE_SIZE;
	unsigned int IN_CTRL;
	unsigned int OUT_CTRL;                   /* 0x130 */
	unsigned int RES1[1];
	unsigned int EXT1;
	unsigned int IN_BUFFER_S_ADDR;
	unsigned int IN_BUFFER_SIZE;             /* 0x140 */
	unsigned int IN_BUFFER_R_ADDR; //相对地址
	unsigned int IN_BUFFER_W_ADDR; //相对地址/* 0x148 */
	unsigned int RES2[3];
	unsigned int EXT2;
	unsigned int OUT_BUFFER_S_ADDR;
	unsigned int OUT_BUFFER_SIZE;            /* 0x160 */
	unsigned int OUT_BUFFER_R_ADDR;
	unsigned int OUT_BUFFER_W_ADDR;          /* 0x168 */
	unsigned int RES3[4];
	unsigned int EXTRA_R_TO_SDRAM;
	unsigned int EXTRA_W_TO_SDRAM;           /* 0x180 */
	unsigned int EXTRA_D_TO_SDRAM;
	unsigned int RES4[2];
	unsigned int CODEC_TYPE;                 /* 0x190 R */
	unsigned int RES8[6];
	unsigned int TASK_QUEQUE;                /* 0x1C8 R 位置不能改变*/
	unsigned int TASK_WAIT_TIME;             /* 0x1CC */
} GeneralReg;

typedef enum {
	REG_BUF_INPUT = 0,
	REG_BUF_OUTPUT,
} RegBufType;

typedef enum {
	REG_INT_FINISH    = (0x1<<0),
	REG_INT_NEED_DATA = (0x1<<1),
	REG_INT_OVER      = (0x1<<2),
	REG_RESET_OK      = (0x1<<4),
} RegIntType;

typedef enum {
	REG_TASK_QUEUE0 = (0x1<<0),
	REG_TASK_QUEUE1 = (0x1<<1)
} RegTaskQueue;

extern unsigned int   reg_get_run_frequency (void);
extern unsigned char* reg_get_run_space_addr (void);
extern unsigned int   reg_get_run_space_size (void);
extern unsigned char* reg_get_buffer_addr    (RegBufType type);
extern unsigned int   reg_get_buffer_size    (RegBufType type);
extern unsigned int   reg_get_buffer_r_addr  (RegBufType type);
extern unsigned int   reg_get_buffer_w_addr  (RegBufType type);
extern unsigned int   reg_get_buffer_count   (RegBufType type);
extern unsigned int   reg_get_codec_type     (void);

extern void reg_set_interrupt_type(RegIntType type);
extern void reg_set_buffer_r_addr (RegBufType type, unsigned int addr);
extern void reg_set_buffer_w_addr (RegBufType type, unsigned int addr);
extern void reg_set_task_work (unsigned int enable);
extern void reg_set_task_queue(RegTaskQueue queue, unsigned int value);

extern ExtraReadReg*  reg_get_extra_read_addr (void);
extern ExtraWriteReg* reg_get_extra_write_addr(void);
extern ExtraDebugReg* reg_get_extra_debug_addr(void);

#endif
