/* tools/mkmtkhdr/mtkimg.h
**
** Based on: android_device_oppo_r819/mkmtkbootimg/bootimg.h
**
** Copyright 2007 The Android Open Source Project
** Copyright 2013 OmniROM
*/

#ifndef _MTK_IMAGE_H_
#define _MTK_IMAGE_H_

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
    MTK_ROOTFS,
    MTK_RECOVERY
};

const char *mtk_names[] = { "", "KERNEL", "ROOTFS", "RECOVERY" };


typedef struct mtk_img_hdr mtk_img_hdr;

struct mtk_img_hdr
{
    unsigned kernel_size;  /* size in bytes */
    unsigned rootfs_size; /* size in bytes */
    unsigned recovery_size;  /* size in bytes */
};

#endif
