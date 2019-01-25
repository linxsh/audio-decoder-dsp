#ifndef __DEV_LIST_H__
#define __DEV_LIST_H__

extern MajorDev* create_dev (void);
extern MajorDev* search_dev (void);
extern void      destroy_dev(void);

extern unsigned int  get_sub_dev_num(void);
extern SubDev*       create_sub_dev (unsigned int id);
extern SubDev*       search_sub_dev (unsigned int id);
extern void          destroy_sub_dev(unsigned int id);
#endif
