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
        printf("Type is %d\n", type);
        unsigned int magic;
        if(read(fd, &magic, 4) == 4 && magic != MTK_MAGIC)
        {
           printf("No MTK Header, making one...\n");
           needs_mtkheader=1;
           sz += sizeof(mtk_header);
           offset = sizeof(mtk_header);
           printf("old sz=%d, new sz=%d, offset=%d\n", datasz, sz, offset);
        }
        if(lseek(fd, 0, SEEK_SET) != 0) goto oops;
    }

    data = (char*) malloc(sz);
    if(data == 0) goto oops;

    if(read(fd, &data[offset], datasz) != datasz) goto oops;
    close(fd);

    if(needs_mtkheader)
    {
        printf("Generating mtk header\n");
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
}

int usage(void)
{
    fprintf(stderr,"usage: mkmtkhdr\n"
            "       --kernel <filename>\n"
            "       --ramdisk <filename>\n"
            );
    return 1;
}

int main(int argc, char **argv)
{
    boot_img_hdr hdr;

    char *kernel_fn = NULL;
    void *kernel_data = NULL;
    char *ramdisk_fn = NULL;
    void *ramdisk_data = NULL;

    char out_fn[PATH_MAX];
    int fd;

    argc--;
    argv++;

    if(argc < 2) {
        return usage();
    }
    while(argc > 0){
        char *arg = argv[0];
        char *val = argv[1];
        argc -= 2;
        argv += 2;
        if(!strcmp(arg, "--kernel")) {
            kernel_fn = val;
        } else if(!strcmp(arg, "--ramdisk")) {
            ramdisk_fn = val;
        } else {
            return usage();
        }
    }

    if (kernel_fn) {
        kernel_data = load_file(kernel_fn, &hdr.kernel_size, MTK_KERNEL);
        if(kernel_data == 0) {
            fprintf(stderr,"error: could not load kernel '%s'\n", kernel_fn);
            return 1;
        } else {
            sprintf(out_fn, "%s-mtk", basename(kernel_fn));
            write_file(out_fn, kernel_data, hdr.kernel_size);
        }
    }

    if (ramdisk_fn) {
        ramdisk_data = load_file(ramdisk_fn, &hdr.ramdisk_size, MTK_ROOTFS);
        if(ramdisk_data == 0) {
            fprintf(stderr,"error: could not load ramdisk '%s'\n", ramdisk_fn);
            return 1;
        } else {
            sprintf(out_fn, "%s-mtk", basename(ramdisk_fn));
            write_file(out_fn, ramdisk_data, hdr.ramdisk_size);
        }
    }

    return 0;
}
