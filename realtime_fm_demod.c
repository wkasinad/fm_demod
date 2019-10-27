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

// structure that maintains the dongle state in the demod program
struct dongle_state {
    uint16_t buf16[MAXIMUM_BUF_LENGTH];
    uint32_t buf_len;
    rtlsdr_dev_t *dev;
};

// global variables
struct dongle_state dongle;

/*
 * Callback function for rtlsdr_read_async
 * should be able to fill a buffer with the results of the read function
 */
static void fm_callback(unsigned char *buf, uint32_t len, void *ctx) {
    int i;
    for(i = 0; i < len; i++) {
        printf("%c", buf[i]);
    }
    printf("\n");
    printf("fm callback\n");
}

/*
 * Writes a predefined buffer into a dongle state structure
 * Used to test the read function
 * s : the dongle struct
 * buf : an array of uint16
 * buf_len : lenth of the buffer
 */
int write_buf(struct dongle_state * s, uint16_t * buf, int buf_len) {
    struct dongle_state * dongle = s;
    int i;
    for(i = 0; i < buf_len; i++) {
        dongle->buf16[i] = buf[i];
    }
    dongle->buf_len = buf_len;
    return 0;
}

/*
 * Prints out the state of the dongle state structure's buf16
 * s : dongle state structure from which the buffer is read
 */
int read_buf16(struct dongle_state * s) {
    struct dongle_state * dongle = s;
    int i;
    for(i = 0; i < (dongle->buf_len / sizeof(dongle->buf16[0])); i++) {
        printf("%x,", dongle->buf16[i]);
    }
    printf("\n");
    return 0;
}

/*
 * Main function for running the fm demod
 *
 * Currently capable of openning a connection to an rtlsdr device and closing it
 * at the end
 *
 * Developing system for reading from the rtlsdr device
 */
int main(int argc, char **argv) {
    // find connected rtlsdr device
    int dev_index = 0;
    dev_index = verbose_device_search("0");
    if (dev_index < 0) {
        fprintf(stderr, "failed to find sdr device\n");
        exit(1);
    }

    uint32_t debug = 4; // 4 for all debug, 3 for info, 0 for nothing

    // open connection to the rtlsdr device
    int r;
    r = rtlsdr_open(&dongle.dev, (uint32_t)dev_index, debug);
    if (r < 0) {
        fprintf(stderr, "failed to open device %d\n", dev_index);
    }
    printf("opened device\n");

    // print the state of the dongle buffer before modification
    // always going to be empty
    printf("dongle buf is ");
    read_buf16(&dongle);

    // from librtlsdr:
    // What I think is the primary function in the dongle thread of rtl_fm
    // Should read from the rtlsdr device and write to the buffer structure in
    // the dongle.
    // currently broken
    rtlsdr_read_async(dongle.dev, fm_callback, (void *)&dongle, 0, dongle.buf_len);

    // print the state of the dongle buffer after modification
    printf("dongle buf after read is ");
    read_buf16(&dongle);

    // close connection to the rtlsdr device
    rtlsdr_close(dongle.dev);
    printf("closed device\n");

    return 0;
}
