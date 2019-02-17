#ifndef __X86_BUFFER_H__
#define __X86_BUFFER_H__

extern int x86_buffer_read(unsigned char *dstAddr, unsigned char *srcAddr,
		unsigned int sAddr, unsigned int eAddr, unsigned int rAddr, unsigned int len);

extern int x86_buffer_write(unsigned char *dstAddr, unsigned char *srcAddr,
		unsigned int sAddr, unsigned int eAddr, unsigned int wAddr, unsigned int len);
#endif
