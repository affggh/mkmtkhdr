/* tools/mkmtkhdr/mkmtkhdr.c
**
** Based on: android_device_oppo_r819/mkmtkbootimg/mkbootimg.c
**
** Copyright 2007 The Android Open Source Project
** Copyright 2013 OmniROM
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <libgen.h>

#include "mtkimg.h"

static void *load_file(const char *fn, unsigned *_sz, enum  mtk_type type)
{
    char *data;
    int sz;
    int datasz;
    int fd;
    int offset = 0;
    int needs_mtkheader = 0;

    data = 0;
    fd = open(fn, O_RDONLY);
    if(fd < 0) return 0;

    sz = datasz = lseek(fd, 0, SEEK_END);
    if(sz < 0) goto oops;

    if(lseek(fd, 0, SEEK_SET) != 0) goto oops;
    if(type != MTK_NONE)
    {
        unsigned int magic;
        if(read(fd, &magic, 4) == 4 && magic != MTK_MAGIC)
        {
           printf("No MTK header, making one...\n");
           printf("Type is %d (%s)\n", type, mtk_names[type]);
           needs_mtkheader=1;
           sz += sizeof(mtk_header);
           offset = sizeof(mtk_header);
           printf("old sz=%d, new sz=%d, offset=%d\n", datasz, sz, offset);
        } else {
            printf("MTK header already exists!");
            goto oops;
        }
        if(lseek(fd, 0, SEEK_SET) != 0) goto oops;
    }

    data = (char*) malloc(sz);
    if(data == 0) goto oops;

    if(read(fd, &data[offset], datasz) != datasz) goto oops;
    close(fd);

    if(needs_mtkheader)
    {
        printf("Generating MTK header...\n");
        mtk_header* hdr = (mtk_header*)data;
        memset(hdr->padding, 0xFF, sizeof(mtk_header));
        hdr->info.magic = MTK_MAGIC;
        hdr->info.size = datasz;
        memset(hdr->info.name, 0, sizeof(hdr->info.name));
        strcpy(hdr->info.name, mtk_names[type]);
    }
    if(_sz) *_sz = sz;
    return data;

oops:
    close(fd);
    if(data != 0) free(data);
    return 0;
}

int write_file(const char *fn, void *data, int sz)
{
    int fd;

    fd = open(fn, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(fd < 0) {
        fprintf(stderr,"error: could not create '%s'\n", fn);
        return 1;
    }
    if(write(fd, data, sz) != (ssize_t) sz) {
        unlink(fn);
        close(fd);
        fprintf(stderr,"error: failed writing '%s': %s\n", fn,
                strerror(errno));
        return 1;
    }

    return 0;
}

int usage(void)
{
    fprintf(stderr,"usage: mkmtkhdr\n"
            "       [ --kernel <zImage filename> ]\n"
            "       [ --rootfs|--recovery <ramdisk filename> ]\n"
            );
    return 1;
}

int main(int argc, char **argv)
{
    mtk_img_hdr hdr;

    char *kernel_fn = NULL;
    void *kernel_data = NULL;
    char *rootfs_fn = NULL;
    void *rootfs_data = NULL;
    char *recovery_fn = NULL;
    void *recovery_data = NULL;

    char out_fn[PATH_MAX];
    int fd;

    argc--;
    argv++;

    if(argc < 2) return usage();
    while(argc > 0){
        char *arg = argv[0];
        char *val = argv[1];
        argc -= 2;
        argv += 2;
        if(!strcmp(arg, "--kernel")) {
            kernel_fn = val;
        } else if(!strcmp(arg, "--rootfs")) {
            rootfs_fn = val;
        } else if(!strcmp(arg, "--recovery")) {
            recovery_fn = val;
        } else {
            return usage();
        }
    }

    if (kernel_fn) {
        kernel_data = load_file(kernel_fn, &hdr.kernel_size, MTK_KERNEL);
        if(kernel_data == 0) {
            fprintf(stderr,"error: could not load kernel zImage '%s'\n", kernel_fn);
            return 1;
        } else {
            sprintf(out_fn, "%s-mtk", basename(kernel_fn));
            if(write_file(out_fn, kernel_data, hdr.kernel_size) == 1) return 1;
        }
    }

    if (rootfs_fn) {
        rootfs_data = load_file(rootfs_fn, &hdr.rootfs_size, MTK_ROOTFS);
        if(rootfs_data == 0) {
            fprintf(stderr,"error: could not load rootfs ramdisk '%s'\n", rootfs_fn);
            return 1;
        } else {
            sprintf(out_fn, "%s-mtk", basename(rootfs_fn));
            if(write_file(out_fn, rootfs_data, hdr.rootfs_size) == 1) return 1;
        }
    }

    if (recovery_fn) {
        recovery_data = load_file(recovery_fn, &hdr.recovery_size, MTK_RECOVERY);
        if(recovery_data == 0) {
            fprintf(stderr,"error: could not load recovery ramdisk '%s'\n", recovery_fn);
            return 1;
        } else {
            sprintf(out_fn, "%s-mtk", basename(recovery_fn));
            if(write_file(out_fn, recovery_data, hdr.recovery_size) == 1) return 1;
        }
    }

    return 0;
}
