/* tools/mkmtkhdr/mtkimg.h
**
** Based on: android_device_oppo_r819/mkmtkbootimg/bootimg.h
**
** Copyright 2007 The Android Open Source Project
** Copyright 2013 OmniROM
*/

#ifndef _BOOT_IMAGE_H_
#define _BOOT_IMAGE_H_

typedef union
{
    struct {
        unsigned int magic;
        unsigned int size;
        char name[32];
    } info;
    unsigned char padding[512]; // Pad up to 512bytes
} mtk_header;

#define MTK_MAGIC 0x58881688

enum mtk_type
{
    MTK_NONE=0,
    MTK_KERNEL,
    MTK_ROOTFS
};

const char *mtk_names[] = { "", "KERNEL", "ROOTFS" };


typedef struct boot_img_hdr boot_img_hdr;

struct boot_img_hdr
{
    unsigned kernel_size;  /* size in bytes */
    unsigned ramdisk_size; /* size in bytes */
};

#endif
