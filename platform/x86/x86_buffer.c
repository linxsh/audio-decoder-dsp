#include "os_define.h"
#include "x86_buffer.h"

int x86_buffer_read(unsigned char *dstAddr, unsigned char *srcAddr,
		unsigned int sAddr, unsigned int eAddr, unsigned int rAddr, unsigned int len)
{
	if ((rAddr + len) > eAddr) {
		unsigned int tmpLen = eAddr - rAddr;

		os_memcpy(dstAddr,          srcAddr + rAddr, tmpLen);
		os_memcpy(dstAddr + tmpLen, srcAddr + sAddr, len - tmpLen);
	} else
		os_memcpy(dstAddr, srcAddr + rAddr, len);

	return 0;
}

int x86_buffer_write(unsigned char *srcAddr, unsigned char *dstAddr,
		unsigned int sAddr, unsigned int eAddr, unsigned int wAddr, unsigned int len)
{
	if ((wAddr + len) > eAddr) {
		unsigned int tmpLen = eAddr - wAddr;

		os_memcpy(dstAddr + wAddr, srcAddr,          tmpLen);
		os_memcpy(dstAddr + sAddr, srcAddr + tmpLen, len - tmpLen);
	} else
		os_memcpy(dstAddr + wAddr, srcAddr, len);

	return 0;
}
