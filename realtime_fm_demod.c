/*
 * Program where I will itereratively develop a real time FM demodulator using
 * librtlsdr
 */

#include <stdio.h>
#include <stdlib.h>
#include "rtl-sdr.h"
#include "convenience.h"

static rtlsdr_dev_t *dev = NULL;

int main(int argc, char **argv) {
    int r;
    int dev_index = 0;

    dev_index = verbose_device_search("0");
    if (dev_index < 0) {
        fprintf(stderr, "failed to find sdr device\n");
        exit(1);
    }
    r = rtlsdr_open(&dev, (uint32_t)dev_index);
    if (r < 0) {
        fprintf(stderr, "failed to open device %d\n", dev_index);
    }

    printf("probably opened connection to sdr.\n");

    rtlsdr_close(dev);

    printf("closed connection to sdr. exiting\n");

    return 0;
}
