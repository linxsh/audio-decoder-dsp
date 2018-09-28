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
	unsigned int UART_FREQUENCY;             /* 0x110 */
	unsigned int INTEN;
	unsigned int INT; //???多路如何共享, 或者是否可以复用
	unsigned int EXTEND_H_ADDR;              //64bits system
	unsigned int RUN_CTRL;                   /* 0x120 */
	unsigned int RUN_SAPCE_S_ADDR;
	unsigned int RUN_SAPCE_SIZE;
	unsigned int IN_CTRL;
	unsigned int OUT_CTRL;                   /* 0x130 */
	unsigned int RES1[2];
	unsigned int IN_BUFFER_S_ADDR;
	unsigned int IN_BUFFER_SIZE;             /* 0x140 */
	unsigned int IN_BUFFER_R_ADDR; //相对地址
	unsigned int IN_BUFFER_W_ADDR; //相对地址/* 0x148 */
	unsigned int RES2[4];
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
	unsigned int TASK_WAIT_TIME;             /* 0x1CC */
} GeneralReg;

typedef enum {
	REG_BUF_INPUT = 0,
	REG_BUF_OUTPUT,
} RegBufType;

typedef enum {
	REG_INT_FINISH    = (0x1<<0),
	REG_INT_NEED_DATA = (0x1<<1),
	REG_INT_OVER,
} RegIntType;

extern unsigned int   reg_get_uart_frequency(void);
extern unsigned char* reg_get_run_space_addr(void);
extern unsigned int   reg_get_run_space_size(void);
extern unsigned char* reg_get_buffer_addr   (RegBufType type);
extern unsigned int   reg_get_buffer_size   (RegBufType type);
extern unsigned int   reg_get_buffer_r_addr (RegBufType type);
extern unsigned int   reg_get_buffer_w_addr (RegBufType type);
extern unsigned int   reg_get_buffer_count  (RegBufType type);
extern unsigned int   reg_get_codec_type    (void);

extern unsigned int   reg_set_interrupt_type(RegIntType type);
extern unsigned int   reg_set_buffer_r_addr (RegBufType type, unsigned int addr);
extern unsigned int   reg_set_buffer_w_addr (RegBufType type, unsigned int addr);
#endif
