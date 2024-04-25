#include "main.h"
#include "../scomlib_extra/scomlib_extra.h"
#include "serial.h"
#include <stdio.h>
#include <termios.h> // for baud rate constant
#include <unistd.h>  // Include the header file for usleep

// Function to read a parameter from a device at a specific address
read_param_result_t read_param(int addr, int parameter)
{
    read_param_result_t result;
    scomx_enc_result_t encresult;
    scomx_header_dec_result_t dechdr;
    scomx_dec_result_t decres;
    size_t bytecounter;
    char readbuf[128];
    float outval = 0.0;

    // Encode the read user info value command
    encresult = scomx_encode_read_user_info_value(addr, parameter);

    /*
    // Debug Print encresult in HEX form
    for (size_t i = 0; i < encresult.length; i++) {
        printf("%02X", encresult.data[i]);
    }
    printf("\n");
    */

    // Write the encoded command to the serial port
    bytecounter = serial_write(encresult.data, encresult.length);
    if (bytecounter != encresult.length) {
        result.error = -1;
        return result;
    }

    // Read the frame header from the serial port
    bytecounter = serial_read(readbuf, SCOM_FRAME_HEADER_SIZE);
    if (bytecounter != SCOM_FRAME_HEADER_SIZE) {
        result.error = -1;
        return result;
    }

    // Decode the frame header
    dechdr = scomx_decode_frame_header(readbuf, SCOM_FRAME_HEADER_SIZE);
    if (dechdr.error != SCOM_ERROR_NO_ERROR) {
        result.error = -1;
        return result;
    }

    // Read the frame data from the serial port
    bytecounter = serial_read(readbuf, dechdr.length_to_read);
    if (bytecounter != dechdr.length_to_read) {
        result.error = -1;
        return result;
    }

    // Decode the frame data
    decres = scomx_decode_frame(readbuf, dechdr.length_to_read);
    if (decres.error != SCOM_ERROR_NO_ERROR) {
        result.error = -1;
        return result;
    }

    // Extract the float value from the decoded frame
    result.value = scomx_result_float(decres);
    result.error = 0; // no error

    return result;
}

int main(int argc, const char *argv[])
{
    // default serial port if no argument provided
    const char *port = "/dev/serial/by-path/platform-xhci-hcd.1.auto-usb-0:1.1.1:1.0-port0";

    // Check if a port is provided as a command line argument
    if (argc > 1) {
        port = argv[1];
    }

    printf("Studer serial comm test on port %s\n", port);

    // Initialize the serial port
    if (serial_init(port, B115200, PARITY_EVEN, 1) != 0) {
        return 1;
    }

    while (1) {
        /*
        // Read parameters for addresses 100 to 104
        for (int addr = 100; addr < 105; addr++) {
            read_param_result_t result = read_param(addr, 3136);
            if (result.error == 0) {
                printf("xtender %d output power = %.3f kW\n", addr, -result.value);
            } else {
                printf("xtender %d output power = read failed\n", addr);
            }
        }
        */
        // Iterate over the requested_parameters array
        for (int i = 0; i < sizeof(requested_parameters) / sizeof(parameter_t); i++) {
            // Get the current parameter
            parameter_t current_param = requested_parameters[i];

            // Read the parameter
            read_param_result_t result = read_param(current_param.address, current_param.parameter);

            // Check if the read was successful
            if (result.error == 0) {
                // Print the parameter name and value
                printf("%s = %.3f %s\n", current_param.name, result.value * current_param.sign, current_param.unit);
            } else {
                // Print an error message
                printf("%s = read failed\n", current_param.name);
            }
        }

        printf("---------------------------------------------------------\n");

        usleep(500000); // sleep for 500 milliseconds
    }

    return 0;
}
