#include <stdio.h>
#include <termios.h> // for baud rate constant

#include "../scomlib_extra/scomlib_extra.h"
#include "serial.h"
#include <unistd.h> // Include the header file for usleep

static void hex_dump(const void *inmem, size_t len)
{
    const unsigned char *mem = (const unsigned char *)inmem;

    for (unsigned row = 0; row <= len / 16; row++) {
        printf("0x%04X:  ", row * 16);

        // print hexa
        for (unsigned tb = row * 16; tb < row * 16 + 16; tb += 2) {
            if (tb == len) {
                for (unsigned dummy = tb; dummy < row * 16 + 16; dummy += 2)
                    printf("     ");
                break;
            } else if (tb + 1 == len) {
                printf("%02X   ", mem[tb]);
                for (unsigned dummy = tb + 2; dummy < row * 16 + 16; dummy += 2)
                    printf("     ");
                break;
            } else
                printf("%02X%02X ", mem[tb], mem[tb + 1]);
        }

        printf(" ");

        // print ascii
        for (unsigned a = row * 16; a < row * 16 + 16; a++) {
            if (a == len)
                break;

            unsigned char chr = mem[a];
            if (chr < 32 || chr > 126)
                chr = '.';

            printf("%c", chr);
        }

        printf("\n");
    }
}

int test()
{
    scomx_enc_result_t encresult;
    scomx_header_dec_result_t dechdr;
    scomx_dec_result_t decres;
    size_t bytecounter;
    char readbuf[128];
    float outval = 0.0;

    encresult = scomx_encode_read_user_info_value(SCOMX_DEST_XTM(3), SCOMX_INFO_XTENDER_OUT_AC_POWER);

    bytecounter = serial_write(encresult.data, encresult.length);
    if (bytecounter != encresult.length) {
        return 10;
    }

    bytecounter = serial_read(readbuf, SCOM_FRAME_HEADER_SIZE);
    if (bytecounter != SCOM_FRAME_HEADER_SIZE) {
        return 1;
    }

    dechdr = scomx_decode_frame_header(readbuf, SCOM_FRAME_HEADER_SIZE);
    if (dechdr.error != SCOM_ERROR_NO_ERROR) {
        return 2;
    }

    bytecounter = serial_read(readbuf, dechdr.length_to_read);
    if (bytecounter != dechdr.length_to_read) {
        return 3;
    }

    decres = scomx_decode_frame(readbuf, dechdr.length_to_read);
    if (decres.error != SCOM_ERROR_NO_ERROR) {
        return 4;
    }

    outval = scomx_result_float(decres);

    printf("XTENDER_OUT_AC_POWER: %.3f\n", outval);

    return 0;
}

int main(int argc, const char *argv[])
{
    const char *port = "/dev/serial/by-path/platform-xhci-hcd.1.auto-usb-0:1.1.1:1.0-port0";

    if (argc > 1) {
        port = argv[1];
    }

    printf("Studer serial comm test on port %s\n", port);
    if (serial_init(port, B115200, PARITY_EVEN, 1) != 0) {
        return 1;
    }
    while (1) {
        int r = test();
        if (r != 0) {
            printf("Test failed with error code %d\n", r);
        }
        usleep(500000); // sleep for 500 milliseconds
    }

    return 0;
}
