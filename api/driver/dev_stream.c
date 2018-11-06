#include "kernelcall.h"
#include "register.h"
#include "dev_stream.h"
#include "dev_list.h"
#include "log_format.h"

#define RUN_SPACE_SIZE (2 * 1024 * 1024)
static unsigned int devInit = 0;

int stream_init(void)
{
	if (devInit == 0) {
		DevStream *dev = create_dev();

		if (dev == NULL) {
			log_format(DRIVER, ERRO, "%s %d: dev error\n", __FUNCTION__, __LINE__);
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
			log_format(DRIVER, DEBU, "%s %d\n", __FUNCTION__, __LINE__);
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
			log_format(DRIVER, DEBU, "%s %d\n", __FUNCTION__, __LINE__);
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
		log_format(DRIVER, ERRO, "%s %d: sub dev(%d) error\n", __FUNCTION__, __LINE__, id);
		return -1;
	}

	subDev->task_id     = id;
	subDev->task_active = 0;

	subDev->dev = search_dev();
	if (!subDev->dev) {
		log_format(DRIVER, ERRO, "%s %d: dev error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	sVirtAddr = subDev->dev->ext_reg_virt_addr;
	sPhysAddr = subDev->dev->ext_reg_phys_addr;
	offset    = id * subDev->dev->ext_reg_size;
	subDev->r_reg      = (ExtReadReg *)  (sVirtAddr + offset);
	subDev->r_phys_reg = (void *)        (sPhysAddr + offset);
	offset   += sizeof(ExtReadReg);
	subDev->w_reg      = (ExtWriteReg *) (sVirtAddr + offset);
	subDev->w_phys_reg = (void *)        (sPhysAddr + offset);
	offset   += sizeof(ExtWriteReg);
	subDev->d_reg      = (ExtDebugReg *) (sVirtAddr + offset);
	subDev->d_phys_reg = (void *)        (sPhysAddr + offset);
	offset   += sizeof(ExtDebugReg);
	subDev->b_reg      = (ExtBufferReg *)(sVirtAddr + offset);
	subDev->b_phys_reg = (void *)        (sPhysAddr + offset);

	return 0;
}

void stream_close(unsigned int id)
{
	return;
}

int stream_config(unsigned int id)
{
	return 0;
}

int stream_start(unsigned int id)
{
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
