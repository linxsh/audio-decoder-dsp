#ifndef __REG_H__
#define __REG_H__

typedef struct {
} DecoderReadCtrl;

typedef struct {
	unsigned int sample_rate;
	unsigned int channels;
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

/*************************************************************************
 * INTEN/INT       :
 *
 * RUN_CTRL        :   0  ~ 3    task id
 *                     4  ~ 7    task type
 *                     8  ~ 15   codec id
 *                     16 ~ 19   frame num
 *                     24        all task have stopped, and cpu quest reset
 *                     25        start or stop task
 *                     30        run endian
 *                     31        task enable
 *
 * IN_CTRL/OUT_CTRL:   0        buffer one or more channel
 *                     1 ~ 3    buffer channel count
 *                     4 ~ 5    buffer store mode
 *                     6        buffer data encrypt
 *                     7        buffer data empty
*************************************************************************/

typedef struct {
	unsigned int RES0[64];
	unsigned int RES1[1];                    /* 0x100 */
	unsigned int EEAR;
	unsigned int EPCR;
	unsigned int EXPC;
	unsigned int RUN_FREQUENCY;              /* 0x110 */
	unsigned int INTEN;
	unsigned int INT;
	unsigned int EXT0;
	unsigned int RUN_CTRL;                   /* 0x120     位置不能改变*/
	unsigned int RES2[7];                    /* audio key 位置不能改变*/
	unsigned int RUN_SAPCE_S_ADDR;           /* 0x140 */
	unsigned int RUN_SAPCE_SIZE;
	unsigned int IN_CTRL;
	unsigned int OUT_CTRL;
	unsigned int EXT1;                       /* 0x150 */
	unsigned int IN_BUFFER_S_ADDR;
	unsigned int IN_BUFFER_SIZE;
	unsigned int IN_BUFFER_R_ADDR; //相对地址
	unsigned int IN_BUFFER_W_ADDR; //相对地址/* 0x160 */
	unsigned int EXT2;
	unsigned int OUT_BUFFER_S_ADDR;
	unsigned int OUT_BUFFER_SIZE;
	unsigned int OUT_BUFFER_R_ADDR;          /* 0x170 */
	unsigned int OUT_BUFFER_W_ADDR;
	unsigned int EXTRA_R_TO_SDRAM;
	unsigned int EXTRA_W_TO_SDRAM;
	unsigned int EXTRA_D_TO_SDRAM;           /* 0x180 */
	unsigned int BUFADDR_TO_SDRAM;
	unsigned int RES3[2];
	unsigned int CODEC_TYPE;                 /* 0x190 R */
	unsigned int ERROR_CODE;
	unsigned int NEED_DATA_NUM;
	unsigned int RES4[4];
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
	REG_INT_INIT_OK   = (0x1<<4),
	REG_INT_RESET_OK  = (0x1<<5),
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
extern unsigned int   reg_get_codec_id       (void);
extern unsigned int   reg_get_task_enable    (void);
extern unsigned int   reg_get_task_id        (void);
extern unsigned int   reg_get_task_do        (void);
extern unsigned int   reg_get_task_queue     (RegTaskQueue queue);
extern unsigned int   reg_get_request_reset  (void);

extern void reg_set_interrupt_type(RegIntType type);
extern void reg_set_buffer_r_addr (RegBufType type, unsigned int addr);
extern void reg_set_buffer_w_addr (RegBufType type, unsigned int addr);
extern void reg_set_task_enable   (unsigned int enable);
extern void reg_set_task_queue    (RegTaskQueue queue, unsigned int value);
extern void reg_set_codec_type    (unsigned int codecType);
extern void reg_set_error_code    (unsigned int errorCode);

extern ExtraReadReg*  reg_get_extra_read_addr (void);
extern ExtraWriteReg* reg_get_extra_write_addr(void);
extern ExtraDebugReg* reg_get_extra_debug_addr(void);

extern void         extra_reg_set(unsigned int *reg, unsigned int value);
extern unsigned int extra_reg_get(unsigned int *reg);
#endif
