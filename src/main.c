/**
 * @file main.c
 * @brief Program to connect to Studer XCom-232, get data and push to MQTT
 * 
 * This program connects to a Studer XCom-232 device, retrieves data, and publishes it to an MQTT broker.
 * 
 * @license MIT License
 * 
 * @author kolin
 * 
 * @dependencies
 * This program uses the following library:
 * - [Studer Library](https://github.com/k3a/studer)
 */

#include "main.h"
#include "../scomlib_extra/scomlib_extra.h"
#include "serial.h"
#include <mosquitto.h>
#include <stdio.h>
#include <string.h>  
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

#ifdef DEBUG
    // Debug Print encresult in HEX form
    for (size_t i = 0; i < encresult.length; i++) {
        printf("%02X", encresult.data[i]);
    }
    printf("\n");
#endif

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
    // Default serial port if no argument provided
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

    mosquitto_lib_init();
    struct mosquitto *mqtt_client = mosquitto_new(NULL, true, NULL);
    int rc = mosquitto_connect(mqtt_client, mqtt_server, mqtt_port, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        printf("Connect failed, return code %d\n", rc);
        return rc;
    }

    while (1) {
        // Iterate over the requested_parameters array
        for (int i = 0; i < sizeof(requested_parameters) / sizeof(parameter_t); i++) {
            // Get the current parameter
            parameter_t current_param = requested_parameters[i];

            // Read the parameter
            read_param_result_t result = read_param(current_param.address, current_param.parameter);

            // Current topic
            char topic[256];
            snprintf(topic, sizeof(topic), "%s/%s/%s", mqtt_topic, current_param.mqtt_prefix, current_param.name);

            // Check if the read was successful
            if (result.error == 0) {
                // Print the parameter name and value
                printf("%s = %.3f %s\n", current_param.name, result.value * current_param.sign, current_param.unit);

                // Convert the float value to a string
                char value_str[32];
                snprintf(value_str, sizeof(value_str), "%.3f", result.value * current_param.sign);

                // Publish the value to MQTT
                rc = mosquitto_publish(mqtt_client, NULL, topic, strlen(value_str), value_str, 0, false);
                if (rc != MOSQ_ERR_SUCCESS) {
                    printf("Publish failed, return code %d\n", rc);
                    rc = mosquitto_reconnect(mqtt_client);
                    if (rc != MOSQ_ERR_SUCCESS) {
                        printf("Reconnect failed, return code %d\n", rc);
                        return rc;
                    }
                }
            } else {
                // Print an error message
                printf("%s = read failed\n", current_param.name);
                if (mqtt_client != NULL && topic != NULL) {
                    mosquitto_publish(mqtt_client, NULL, topic, 3, "nAn", 0, false);
                }
            }
        }

        printf("---------------------------------------------------------\n");

        usleep(500000); // Sleep for 500 milliseconds
    }

    // Cleanup for Mosquitto
    mosquitto_disconnect(mqtt_client);
    mosquitto_destroy(mqtt_client);
    mosquitto_lib_cleanup();
    return 0;
}
