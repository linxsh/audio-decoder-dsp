#ifndef __REG_H__
#define __REG_H__

typedef enum {
	CODEC_ID_MPEG12     = 1,
	CODEC_ID_OPUS       = 2,
	CODEC_ID_VORBIS     = 3,
	CODEC_ID_DOLBY      = 4,
	CODEC_ID_DOLBY_PLUS = 5,
	CODEC_ID_UNKOWN,
} CodecID;

typedef struct {
	CodecID      CODEC_ID;     //enc data format
	unsigned int BITS;         //PCM data format(0: auto)
	unsigned int CHANNELS;     //PCM data format(0: auto)
	unsigned int SAMPLE_RATE;  //PCM data format(0: auto)
	unsigned int ENDIAN;       //PCM data format(0: auto; 1: big endian; 2: little endian)
	unsigned int INTERLACE;
} DecoderReadCtrl;

typedef struct {
	unsigned int ERROR_CODE;
	unsigned int SAMPLE_RATE;  //PCM data format(0: auto)
	unsigned int CHANNELS;     //PCM data format(0: auto)
	unsigned int BITS;         //PCM data format(0: auto)
	unsigned int ENDIAN;       //PCM data format(0: auto; 1: big endian; 2: little endian)
	unsigned int INTERLACE;
} DecoderWriteCtrl;

typedef struct {
} DecoderDebugCtrl;

typedef struct {
	unsigned int SAMPLE_RATE;  //PCM data format(0: auto)
	unsigned int CHANNELS;     //PCM data format(0: auto)
	unsigned int BITS;         //PCM data format(0: auto)
	unsigned int ENDIAN;       //PCM data format(0: auto; 1: big endian; 2: little endian)
	unsigned int INTERLACE;
	unsigned int FRAME_MS;
	unsigned int BIT_RATE;
	unsigned int CBR;
	unsigned int CVBR;
	unsigned int COMPLEXITY;
	unsigned int EXPECT_LOSS;
	CodecID      CODEC_ID;     //enc data format
} EncoderReadCtrl;

typedef struct {
	unsigned int ERROR_CODE;
	unsigned int DATA_LESS_NUM;
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
	DecoderReadCtrl DECODER;
	EncoderReadCtrl ENCODER;
	DecryptReadCtrl DECRYPT;
	EncryptReadCtrl ENCRYPT;
	FilterReadCtrl   FILTER;
} ExtReadReg;

typedef union {
	DecoderWriteCtrl DECODER;
	EncoderWriteCtrl ENCODER;
	DecryptWriteCtrl DECRYPT;
	EncryptWriteCtrl ENCRYPT;
	FilterWriteCtrl   FILTER;
} ExtWriteReg;

typedef union {
	DecoderDebugCtrl DECODER;
	EncoderDebugCtrl ENCODER;
	DecryptDebugCtrl DECRYPT;
	EncryptDebugCtrl ENCRYPT;
	FilterDebugCtrl   FILTER;
} ExtDebugReg;

typedef struct {
	unsigned int I_EXT;
	unsigned int I_S_ADDR;     /* 绝对起始地址 */
	unsigned int I_R_ADDR;     /* 相对地址 */
	unsigned int I_W_ADDR;     /* 相对地址 */
	unsigned int I_CHANNEL;    /* 多通道个数，例如用于存储非交织数据 */
	unsigned int I_SIZE;       /* 每个通道的大小 */
	unsigned int I_LOOP;       /* 通道填满, W_ADDR == R_ADDR */
	unsigned int I_EOF;        /* 通道写数据时，结束标记*/
	unsigned int O_EXT;
	unsigned int O_S_ADDR;
	unsigned int O_R_ADDR;
	unsigned int O_W_ADDR;     //?? dsp -> cpu
	unsigned int O_CHANNEL;
	unsigned int O_SIZE;
	unsigned int O_LOOP;
} ExtBufferReg;

/*************************************************************************
 * INTEN/INT      :    0  ~ 3    task0 interrupt
 *                     ......    ........
 *                     28 ~ 31   task7 interrupt
 *
 * RUN_ENABLE     :    0         run/stop startup() function
 *                     1         run      startup() flags    //??? use start interrupt
 *                     2         stop     startup() flags    //??? use start interrupt
 *
 * TASK_CTRL      :    0  ~ 2    select task0 ~ task7
 *                     3         run/stop
 *                     31        assign one task
 *
 * TASK_ID        :    0  ~ 3    task0 magic
 *                     ......    .......
 *                     28 ~ 31   task7 magic
 *
 * TASK_DO        :    0  ~ 3    task0 do
 *                     ......    .......
 *                     28 ~ 31   task1 do
 *
 * TASK_QUEQUE    :    0  ~ 3    store task0 ~ task7
 *                     ......    .......
 *                     28 ~ 31   store task0 ~ task7
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
	unsigned int RUN_ENABLE;                 /* 0x11c     位置不能改变*/
	unsigned int TASK_CTRL;                  /* 0x120     位置不能改变*/
	unsigned int TASK_ID;
	unsigned int TASK_DO;
	unsigned int RES2[5];                    /* audio key 位置不能改变*/
	unsigned int EXT0;                       /* 0x140 */
	unsigned int RUN_SPACE_ADDR;
	unsigned int RUN_SPACE_SIZE;
	unsigned int EXT1;
	unsigned int EXT_R_TO_TASK0;             /* 0x150 */
	unsigned int EXT_W_TO_TASK0;
	unsigned int EXT_D_TO_TASK0;
	unsigned int EXT_B_TO_TASK0;
	unsigned int EXT2;                       /* 0x160 */
	unsigned int EXT_R_TO_TASK1;
	unsigned int EXT_W_TO_TASK1;
	unsigned int EXT_D_TO_TASK1;
	unsigned int EXT_B_TO_TASK1;             /* 0x170 */
	unsigned int RES3[7];
	unsigned int RES4[7];                    /* 0x190 R */
	unsigned int TASK_QUEQUE;                /* 0x1C8 R 位置不能改变*/
	unsigned int TASK_WAIT_TIME;             /* 0x1CC */
} GeneralReg;

typedef enum {
	TASK_NO_0 = (0x1 << 0),
	TASK_NO_1 = (0x2 << 0)
} TaskNo;

typedef enum {
	TASK_START = (0x1 << 2),
	TASK_STOP  = (0x2 << 2),
} TaskAction;

typedef enum {
	TASK_NONE    = 0,
	TASK_DECODER,
	TASK_ENCODER,
	TASK_DECRYPT,
	TASK_ENCRYPT,
	TASK_FILTER
} TaskDo;

typedef enum {
	TASK_QUEQUE_0 = (0x1 << 0),
	TASK_QUEQUE_1 = (0x1 << 1),
} TaskQueue;

typedef enum {
	BUFFER_I = 0,
	BUFFER_O,
} BufferIO;

typedef enum {
	ISR_TASK_NO_0_FINISH    = (0x1 << 0),
	ISR_TASK_NO_0_STOP      = (0x1 << 1),
	ISR_TASK_NO_0_DATA_LESS = (0x1 << 2),
	ISR_TASK_NO_0_DATA_EOF  = (0x1 << 3),

	ISR_TASK_NO_1_FINISH    = (0x1 << 8),
	ISR_TASK_NO_1_STOP      = (0x1 << 9),
	ISR_TASK_NO_1_DATA_LESS = (0x1 << 10),
	ISR_TASK_NO_1_DATA_EOF  = (0x1 << 11),

	ISR_ENABLE_OK    = (0x1<<23),
	ISR_DISABLE_OK   = (0x1<<24),
} IsrType;

typedef enum {
	DECODER_ERROR = -88,
} ErrorCode;

typedef int (*IsrFunc)(void);

extern void register_init(GeneralReg *reg, IsrFunc func);
extern void register_destroy(void);

extern void           reg_enable_run  (void);
extern void           reg_disable_run (void);
extern unsigned int   reg_disable_get (void);
extern void           reg_enable_task (void);
extern void           reg_clear_task  (TaskQueue queue);

extern void           reg_set_task_isr(IsrType isr);
extern void           reg_clr_task_isr(IsrType isr);
extern void           reg_clr_all_task_isr(void);
extern unsigned int   reg_get_task_isr(void);
extern void           reg_set_task_isr_en(IsrType isr);
extern void           reg_clr_task_isr_en(IsrType isr);
extern void           reg_clr_all_task_isr_en(void);
extern unsigned int   reg_get_task_isr_en(void);

extern unsigned int   reg_get_run_frequency (void);
extern unsigned char* reg_get_run_space_addr(void);
extern unsigned int   reg_get_run_space_size(void);
extern TaskNo         reg_get_task_no       (void);
extern TaskAction     reg_get_task_action   (void);
extern unsigned int   reg_get_task_id       (TaskNo  no);
extern TaskDo         reg_get_task_do       (TaskNo  no);

extern void reg_set_run_frequency (unsigned int   freq);
extern void reg_set_run_space_addr(unsigned char *addr);
extern void reg_set_run_space_size(unsigned int   size);

extern ExtReadReg*    reg_get_ext_read_reg (TaskNo no);
extern ExtWriteReg*   reg_get_ext_write_reg(TaskNo no);
extern ExtDebugReg*   reg_get_ext_debug_reg(TaskNo no);
extern void           ext_reg_set(unsigned int *reg, unsigned int value);
extern unsigned int   ext_reg_get(unsigned int *reg);

extern ExtBufferReg*  reg_get_ext_buffer_reg (TaskNo no);
extern unsigned char* ext_reg_get_buf_addr   (ExtBufferReg *reg, BufferIO io);
extern unsigned int   ext_reg_get_buf_size   (ExtBufferReg *reg, BufferIO io);
extern unsigned int   ext_reg_get_buf_r_addr (ExtBufferReg *reg, BufferIO io);
extern unsigned int   ext_reg_get_buf_w_addr (ExtBufferReg *reg, BufferIO io);
extern unsigned int   ext_reg_get_buf_channel(ExtBufferReg *reg, BufferIO io);
extern unsigned int   ext_reg_get_buf_loop   (ExtBufferReg *reg, BufferIO io);
extern unsigned int   ext_reg_get_buf_eof    (ExtBufferReg *reg, BufferIO io);
extern void           ext_reg_set_buf_addr   (ExtBufferReg *reg, BufferIO io, unsigned char *addr);
extern void           ext_reg_set_buf_size   (ExtBufferReg *reg, BufferIO io, unsigned int   size);
extern void           ext_reg_set_buf_r_addr (ExtBufferReg *reg, BufferIO io, unsigned int addr);
extern void           ext_reg_set_buf_w_addr (ExtBufferReg *reg, BufferIO io, unsigned int addr);
extern void           ext_reg_set_buf_loop   (ExtBufferReg *reg, BufferIO io, unsigned int loop);
extern void           ext_reg_set_buf_eof    (ExtBufferReg *reg, BufferIO io, unsigned int eof);

#endif
