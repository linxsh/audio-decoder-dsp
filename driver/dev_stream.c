#include "kernelcall.h"
#include "register.h"
#include "stream_types.h"
#include "dev_stream.h"
#include "dev_list.h"
#include "log_format.h"

#define RUN_SPACE_SIZE (2 * 1024 * 1024)
static unsigned int devInit = 0;

static CodecID _switch_codec_id(StreamCodec codec)
{
	CodecID codec_id = CODEC_ID_UNKOWN;

	switch (codec) {
	case CODEC_MPEG12A:
		codec_id = CODEC_ID_MPEG12;
		break;
	case CODEC_OPUS:
		codec_id = CODEC_ID_OPUS;
		break;
	case CODEC_VORBIS:
		codec_id = CODEC_ID_VORBIS;
		break;
	default:
		break;
	}

	return codec_id;
}

static int _config_buffer(SubDevStream *subDev, StreamConfig *config)
{
	subDev->i_buf.virt_s_addr = gx_page_malloc(config->i_buf.size);
	if (NULL == subDev->i_buf.virt_s_addr) {
		log_format(DRIVER, ERRO,
				"%s %d: in buffer page malloc(size: %x) fail\n",
				__FUNCTION__, __LINE__, config->i_buf.size);
		goto BUFFER_ERROR;
	}
	subDev->i_buf.phys_s_addr = gx_virt_to_phys(subDev->i_buf.virt_s_addr);
	subDev->i_buf.size        = config->i_buf.size;
	subDev->i_buf.loop        = 0;
	subDev->i_buf.r_addr      = 0;
	subDev->i_buf.w_addr      = 0;

	subDev->o_buf.virt_s_addr = gx_page_malloc(config->o_buf.size);
	if (NULL == subDev->o_buf.virt_s_addr) {
		log_format(DRIVER, ERRO,
				"%s %d: out buffer page malloc(size: %x) fail\n",
				__FUNCTION__, __LINE__, config->o_buf.size);
		goto BUFFER_ERROR;
	}
	subDev->o_buf.phys_s_addr = gx_virt_to_phys(subDev->o_buf.virt_s_addr);
	subDev->o_buf.size        = config->o_buf.size;
	subDev->o_buf.loop        = 0;
	subDev->o_buf.r_addr      = 0;
	subDev->o_buf.w_addr      = 0;

	ext_reg_set_buf_addr  (subDev->ext.b_reg, BUFFER_I, subDev->i_buf.phys_s_addr);
	ext_reg_set_buf_size  (subDev->ext.b_reg, BUFFER_I, subDev->i_buf.size);
	ext_reg_set_buf_r_addr(subDev->ext.b_reg, BUFFER_I, 0);
	ext_reg_set_buf_w_addr(subDev->ext.b_reg, BUFFER_I, 0);
	ext_reg_set_buf_loop  (subDev->ext.b_reg, BUFFER_I, 0);
	ext_reg_set_buf_eof   (subDev->ext.b_reg, BUFFER_I, 0);
	ext_reg_set_buf_addr  (subDev->ext.b_reg, BUFFER_O, subDev->o_buf.phys_s_addr);
	ext_reg_set_buf_size  (subDev->ext.b_reg, BUFFER_O, subDev->o_buf.size);
	ext_reg_set_buf_r_addr(subDev->ext.b_reg, BUFFER_O, 0);
	ext_reg_set_buf_w_addr(subDev->ext.b_reg, BUFFER_O, 0);
	ext_reg_set_buf_loop  (subDev->ext.b_reg, BUFFER_O, 0);

	return 0;

BUFFER_ERROR:
	if (subDev->i_buf.virt_s_addr) {
		gx_page_free(subDev->i_buf.virt_s_addr, subDev->i_buf.size);
		subDev->i_buf.virt_s_addr = NULL;
	}

	if (subDev->o_buf.virt_s_addr) {
		gx_page_free(subDev->o_buf.virt_s_addr, subDev->o_buf.size);
		subDev->o_buf.virt_s_addr = NULL;
	}

	return -1;
}

static int _config_task(SubDevStream *subDev, StreamConfig *config)
{
	subDev->task = config->task;
	if (TASK_DEOCDE == subDev->task) {
		CodecID codec_id = _switch_codec_id(config->decode.i_codec);

		ext_reg_set(&(subDev->ext.r_reg->DECODER.CODEC_ID),    codec_id);
		ext_reg_set(&(subDev->ext.r_reg->DECODER.BITS),        config->decode.o_PCM.bits);
		ext_reg_set(&(subDev->ext.r_reg->DECODER.ENDIAN),      config->decode.o_PCM.endian);
		ext_reg_set(&(subDev->ext.r_reg->DECODER.SAMPLE_RATE), config->decode.o_PCM.sample_rate);
		ext_reg_set(&(subDev->ext.r_reg->DECODER.CHANNELS),    config->decode.o_PCM.channels);
	} else if (TASK_ENCODE == subDev->task) {
	} else if (TASK_RESAMPLE == subDev->task) {
	}

	return 0;
}

static int _start_task(SubDevStream *subDev)
{
	if (subDev->i_empty || subDev->o_full)
		return 0;

	ext_reg_set_buf_r_addr(subDev->ext.b_reg, BUFFER_I, subDev->i_buf.r_addr);
	ext_reg_set_buf_w_addr(subDev->ext.b_reg, BUFFER_I, subDev->i_buf.w_addr);
	ext_reg_set_buf_loop  (subDev->ext.b_reg, BUFFER_I, subDev->i_buf.loop);
	ext_reg_set_buf_r_addr(subDev->ext.b_reg, BUFFER_O, subDev->o_buf.r_addr);
	ext_reg_set_buf_w_addr(subDev->ext.b_reg, BUFFER_O, subDev->o_buf.w_addr);
	ext_reg_set_buf_loop  (subDev->ext.b_reg, BUFFER_O, subDev->o_buf.loop);
	gx_dcache_clean_range(subDev->ext.s_addr, subDev->ext.e_addr);
	
	return 0;
}

int stream_init(void)
{
	if (devInit == 0) {
		DevStream *dev = create_dev();

		if (dev == NULL) {
			log_format(DRIVER, ERRO,
					"%s %d: dev error\n",
					__FUNCTION__, __LINE__);
			return -1;
		}

		dev->ext_reg_size   = sizeof(ExtReadReg) + sizeof(ExtWriteReg) + sizeof(ExtDebugReg) + sizeof(ExtBufferReg);
		dev->ext_reg_num    = get_sub_dev_num();
		dev->run_space_size = RUN_SPACE_SIZE;

		dev->run_space_vrit_addr = gx_page_malloc (dev->run_space_size + (dev->ext_reg_size * dev->ext_reg_num));
		dev->run_space_phys_addr = gx_virt_to_phys(dev->run_space_vrit_addr);
		dev->ext_reg_virt_addr   = dev->run_space_vrit_addr + dev->run_space_size;
		dev->ext_reg_phys_addr   = dev->run_space_phys_addr + dev->run_space_size;

#ifdef CONFIG_DSP32
		gx_firmware_fetch(&dev->firmware_addr, &dev->firmware_size);
		if (dev->firmware_addr && (dev->firmware_size != 0)) {
			gx_memcpy            (dev->run_space_vrit_addr, dev->firmware_addr, dev->firmware_size);
			gx_dcache_clean_range(dev->run_space_vrit_addr, dev->run_space_size);
		}

		gx_request_mem_region();
		dev->reg = (GeneralReg *)gx_ioremap();
#else
		dev->reg = (GeneralReg *)gx_mallocz(sizeof(GeneralReg));
		register_init(dev->reg, stream_isr);
#endif
		reg_set_run_space_addr(dev->run_space_phys_addr);
		reg_set_run_space_size(dev->run_space_size);
		reg_set_run_frequency (dev->run_freq);

		reg_clr_task_isr_en(~0x0);
		reg_enable_run();
		while (!(reg_get_task_isr() & ISR_ENABLE_OK)) {
			log_format(DRIVER, DEBU,
					"%s %d: wait enable\n",
					__FUNCTION__, __LINE__);
			gx_mdelay(10);
		}
	}

	devInit++;

	return 0;
}

void stream_uninit(void)
{
	devInit--;

	if (devInit == 0) {
		DevStream *dev = search_dev();

		reg_clr_task_isr_en(~0x0);
		reg_disable_run();
		while (!(reg_get_task_isr() & ISR_DISABLE_OK)) {
			log_format(DRIVER, DEBU,
					"%s %d: wait disable\n",
					__FUNCTION__, __LINE__);
			gx_mdelay(10);
		}

#ifdef CONFIG_DSP32
#else
		register_destroy();
		gx_free(dev->reg);
		gx_page_free(dev->run_space_vrit_addr,
				dev->run_space_size + (dev->ext_reg_size * dev->ext_reg_num));
		dev->run_space_vrit_addr = 0;
		dev->run_space_phys_addr = 0;
		dev->run_space_size      = 0;
		dev->ext_reg_phys_addr   = 0;
		dev->ext_reg_virt_addr   = 0;
		dev->ext_reg_size        = 0;
		dev->ext_reg_num         = 0;

		destroy_dev();
#endif
	}

	return;
}

int stream_open(unsigned int id)
{
	SubDevStream *subDev = NULL;
	void *sVirtAddr = NULL;
	void *sPhysAddr = NULL;
	unsigned int offset = 0;

	subDev = create_sub_dev(id);
	if (!subDev) {
		log_format(DRIVER, ERRO,
				"%s %d: sub dev(%d) create error\n",
				__FUNCTION__, __LINE__, id);
		return -1;
	}

	subDev->task_id     = id;
	subDev->task_active = 0;
	subDev->i_empty     = 0;
	subDev->o_full      = 0;

	subDev->dev = search_dev();
	if (!subDev->dev) {
		log_format(DRIVER, ERRO,
				"%s %d: dev not exist\n",
				__FUNCTION__, __LINE__);
		return -1;
	}

	sVirtAddr = subDev->dev->ext_reg_virt_addr;
	sPhysAddr = subDev->dev->ext_reg_phys_addr;
	offset    = id * subDev->dev->ext_reg_size;
	subDev->ext.r_reg      = (ExtReadReg   *)(sVirtAddr + offset);
	subDev->ext.r_phys_reg = (void *)        (sPhysAddr + offset);
	offset   += sizeof(ExtReadReg);
	subDev->ext.w_reg      = (ExtWriteReg  *)(sVirtAddr + offset);
	subDev->ext.w_phys_reg = (void *)        (sPhysAddr + offset);
	offset   += sizeof(ExtWriteReg);
	subDev->ext.d_reg      = (ExtDebugReg  *)(sVirtAddr + offset);
	subDev->ext.d_phys_reg = (void *)        (sPhysAddr + offset);
	offset   += sizeof(ExtDebugReg);
	subDev->ext.b_reg      = (ExtBufferReg *)(sVirtAddr + offset);
	subDev->ext.b_phys_reg = (void *)        (sPhysAddr + offset);
	offset   += sizeof(ExtBufferReg);
	subDev->ext.s_addr = sVirtAddr;
	subDev->ext.e_addr = sVirtAddr + offset;

	return 0;
}

void stream_close(unsigned int id)
{
	SubDevStream *subDev = search_sub_dev(id);

	if (!subDev) {
		log_format(DRIVER, ERRO,
				"%s %d: sub dev(%d) not exist\n",
				__FUNCTION__, __LINE__, id);
		return;
	}

	subDev->ext.r_reg      = NULL;
	subDev->ext.r_phys_reg = NULL;
	subDev->ext.w_reg      = NULL;
	subDev->ext.w_phys_reg = NULL;
	subDev->ext.d_reg      = NULL;
	subDev->ext.d_phys_reg = NULL;
	subDev->ext.b_reg      = NULL;
	subDev->ext.b_phys_reg = NULL;
	subDev->task_id     = 0;
	subDev->task_active = 0;

	destroy_sub_dev(id);

	return;
}

int stream_config(unsigned int id, StreamConfig *config)
{
	SubDevStream *subDev = search_sub_dev(id);

	if (!subDev) {
		log_format(DRIVER, ERRO,
				"%s %d: sub dev(%d) not exist\n",
				__FUNCTION__, __LINE__, id);
		return -1;
	}

	if (_config_buffer(subDev, config) != 0) {
		log_format(DRIVER, ERRO,
				"%s %d: sub dev(%d) config buffer error\n",
				__FUNCTION__, __LINE__, id);
		return -1;
	}

	if (_config_task(subDev, config) != 0) {
		log_format(DRIVER, ERRO,
				"%s %d: sub dev(%d) config task error\n",
				__FUNCTION__, __LINE__, id);
		return -1;
	}

	return 0;
}

int stream_start(unsigned int id)
{
	SubDevStream *subDev = search_sub_dev(id);

	if (!subDev) {
		log_format(DRIVER, ERRO,
				"%s %d: sub dev(%d) not exist\n",
				__FUNCTION__, __LINE__, id);
		return -1;
	}

	_start_task(subDev);

	return 0;
}

int stream_stop(unsigned int id)
{
	return 0;
}

int stream_read(unsigned int id, unsigned char *buf, unsigned int size)
{
	return 0;
}

int stream_write(unsigned int id, unsigned char *buf, unsigned int size)
{
	return 0;
}

int stream_isr(void)
{
	return 0;
}
