/*
 * Program where I will itereratively develop a real time FM demodulator using
 * librtlsdr
 */

#include <stdio.h>
#include <stdlib.h>
#include "rtl-sdr.h"
#include "convenience.h"

#define DEFAULT_SAMPLE_RATE		24000
#define DEFAULT_BUF_LENGTH		(1 * 16384)
#define MAXIMUM_OVERSAMPLE		16
#define MAXIMUM_BUF_LENGTH		(MAXIMUM_OVERSAMPLE * DEFAULT_BUF_LENGTH)

struct dongle_state {
    uint16_t buf16[MAXIMUM_BUF_LENGTH];
    uint32_t buf_len;
    rtlsdr_dev_t *dev;
};

struct dongle_state dongle;

static void fm_callback(unsigned char *buf, uint32_t len, void *ctx) {
	int i;
	struct dongle_state *dongle = ctx;
	for (i=0; i<(int)len; i++) {
		dongle->buf16[i] = (int16_t)buf[i] - 127;
    }
    dongle->buf_len = len;
}

int write_buf(struct dongle_state * s, uint16_t * buf, int buf_len) {
    struct dongle_state * dongle = s;
    int i;
    for(i = 0; i < buf_len; i++) {
        dongle->buf16[i] = buf[i];
    }
    dongle->buf_len = buf_len;
    return 0;
}

int read_buf(struct dongle_state * s) {
    struct dongle_state * dongle = s;
    int i;
    for(i = 0; i < (dongle->buf_len / sizeof(dongle->buf16[0])); i++) {
        printf("%x,", dongle->buf16[i]);
    }
    printf("\n");
    return 0;
}

int main(int argc, char **argv) {
    int r;
    int dev_index = 0;

    dev_index = verbose_device_search("0");
    if (dev_index < 0) {
        fprintf(stderr, "failed to find sdr device\n");
        exit(1);
    }

    r = rtlsdr_open(&dongle.dev, (uint32_t)dev_index);
    if (r < 0) {
        fprintf(stderr, "failed to open device %d\n", dev_index);
    }
    printf("opened device\n");

    printf("dongle buf is ");
    read_buf(&dongle);

    rtlsdr_read_async(dongle.dev, fm_callback, (void *)&dongle, 0, dongle.buf_len);

    printf("dongle buf after read is ");
    read_buf(&dongle);

    rtlsdr_close(dongle.dev);
    printf("closed device\n");

    return 0;
}
