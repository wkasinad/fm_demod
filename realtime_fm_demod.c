/*
 * Program where I will itereratively develop a real time FM demodulator using
 * librtlsdr
 */

#include <stdio.h>
#include <stdlib.h>
#include "rtl-sdr.h"
#include "convenience.h"

#define DEFAULT_SAMPLE_RATE		2048000
#define DEFAULT_BUF_LENGTH		16384
#define MINIMAL_BUF_LENGTH		512
#define MAXIMAL_BUF_LENGTH		(256 * 16384)

struct sdr_data {
    uint8_t *buffer;
    int buf_len;
    int buf_max;
};

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

void init_sdr(struct sdr_data *sdr_data) {
    struct sdr_data *sdr = sdr_data;
    sdr->buf_max = DEFAULT_BUF_LENGTH * sizeof(*sdr->buffer);
    sdr->buffer = malloc(sdr->buf_max);
}

void destroy_sdr(struct sdr_data *sdr_data) {
    struct sdr_data *sdr = sdr_data;
    free(sdr->buffer);
    sdr->buffer = NULL;
}

/*
 * Writes a predefined buffer into a dongle state structure
 * Used to test the read function
 */
void write_buffer(struct sdr_data *sdr_data, uint32_t *source, uint32_t source_len) {
    struct sdr_data *sdr = sdr_data;
    int i;
    for(i = 0; i < source_len; i++) {
        sdr->buffer[i] = source[i];
    }
    sdr->buf_len = source_len;
}

/*
 * Prints out the state of the dongle state structure's buf
 */
void print_buffer(struct sdr_data *sdr_data) {
    struct sdr_data *sdr = sdr_data;
    int i;
    for(i = 0; i < sdr->buf_len; i++) {
        printf("%x,", sdr->buffer[i]);
    }
    printf("\n");
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

    // 4 for all debug, 3 for info, 0 for nothing
    uint32_t debug = 0;
    rtlsdr_dev_t *dev = NULL;


    // open connection to the rtlsdr device
    int r;
    r = rtlsdr_open(&dev, (uint32_t)dev_index, debug);
    if (r < 0) {
        fprintf(stderr, "failed to open device %d\n", dev_index);
    }
    printf("opened device\n");

    //create the buffer to operate on
    struct sdr_data sdr;
    init_sdr(&sdr);

    // print the state of the dongle buffer before modification
    // always going to be empty
    printf("dongle buf is ");
    print_buffer(&sdr);

    // test writing a buffer
    // uint32_t test[] = {1, 2, 18, 42};
    // uint32_t test_size = sizeof(test)/sizeof(test[0]);
    // write_buffer(&sdr, test, test_size);

    // reads from the rtlsdr dongle
    verbose_reset_buffer(dev); // necessary if you want to read
    r = rtlsdr_read_sync(dev, (void *)sdr.buffer, sdr.buf_max, &sdr.buf_len);
    if (r < 0) {
        fprintf(stderr, "WARNING: sync read failed with error code %d.\n", r);
    }
    printf("nread is %d\n", sdr.buf_len);

    // print the state of the dongle buffer after modification
    printf("dongle buf after read is ");
    print_buffer(&sdr);

    destroy_sdr(&sdr);

    // close connection to the rtlsdr device
    rtlsdr_close(dev);
    printf("closed device\n");

    return 0;
}
