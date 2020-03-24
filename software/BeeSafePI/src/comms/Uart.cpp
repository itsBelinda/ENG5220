#include "Uart.h"

#include <stdexcept>
#include <sys/ioctl.h>

Uart::Uart()
{
    device = -1;
    conf();
}

/**
 * Destructor is used to close the comms i.e. the Uart
 * serial interface.
 */
Uart::~Uart()
{
    if (device != -1) {
        close(device);
    }
}

int Uart::conf()
{

    // Open the device.
    device = open(DEVICE_PATH, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (device == -1) {
        fprintf(stderr, "Failed to open device\r\n");
        fprintf(stderr, "Error: %s\n",strerror(errno));
        return -1;
    }

    // Get the current device configuration.
    struct termios configuration = {0};
    if (tcgetattr(device, &configuration) != 0) {
        goto err;
    }

    // Reconfigure the device.
    configuration.c_cflag &= ~CSTOPB;
    configuration.c_cflag |= CLOCAL;
    configuration.c_cflag |= CREAD;
    configuration.c_cc[VTIME] = 0;
    configuration.c_cc[VMIN] = 0;
    configuration.c_iflag = 0;
    configuration.c_oflag = 0;
    configuration.c_lflag = 0;

    // Set the speed.
    if (cfsetspeed(&configuration, DEVICE_BAUD_RATE)) {
        goto err;
    }

    // TODO: Sets the terminal?
    cfmakeraw(&configuration);

    // Set the parameters associated with the terminal.
    if (tcsetattr(device, TCSANOW, &configuration) < 0
        || tcsetattr(device, TCSAFLUSH, &configuration) < 0) {
        goto err;
    }

    // Device successfully configured, return.
    return 0;

    // Handle any errors during the configuration process.
err:

    // Display the errors.
    fprintf(stderr, "Failed to configure the device\r\n");
    fprintf(stderr, "Error: %s\n",strerror(errno));

    // Handle clean-up.
    close(device);
    device = -1;

    return -1;
}

/**
 * Check whether or not the device has been opened and configured.
 * 'device' will always be -1 if the latter is not achieved.
 *
 * @return True if the device is open (!=-1), false otherwise.
 */
bool Uart::isOpen()
{
    return device != -1;
}

int Uart::getDevice()
{
    return device;
}

/**
 * Peeking algorithm for reading bytes from the serial buffer.
 * Note, this method is capable of receiving larger blocks; using
 * peeking + general timeout, issues pertaining to indefinite timeouts
 * are resolved.
 *
 * @param buffer The char array into which the result will be written.
 * @param bytesExpected The number of bytes that are to be returned.
 * @param timeoutMs How long to wait for data before returning.
 * @return The number of bytes that have successfully been read, -1
 *      otherwise i.e. errors.
 */
ssize_t Uart::readBuffer(char * const buffer, size_t bytesExpected,
                         const int timeoutMs)
{

    // Check that the device has been established.
    if (device == -1) {
        return -1;
    }

    // Check that the buffer is big enough.
    if (sizeof(buffer) < bytesExpected) {
        return -1;
    }

    // The number of bytes peeked; the last number of bytes peeked.
    size_t bytesPeeked = 0;
    size_t lastBytesPeeked = 0;

    // Timing related variables.
    struct timespec pause = {0};
    pause.tv_sec = 0;
    pause.tv_nsec = timeoutMs * 1000;

    // Keep peeking at the buffer until a timeout.
    while (true) {

        // Update the last number of bytes peeked; break if block is met.
        lastBytesPeeked = bytesPeeked;
        if (bytesPeeked >= bytesExpected) {
            break;
        }

        // Sleep the thread until an interrupt.
        nanosleep(&pause, nullptr);
        ioctl(device, FIONREAD, &bytesPeeked);

        // Check if the read has timed out.
        if (bytesPeeked == lastBytesPeeked) {
            break;
        }
    }

    // Set the buffer bytes to null terminators.
    memset(buffer, '\0', bytesExpected);

    // Read the block regardless of what's within.
    if (bytesPeeked >= bytesExpected) {
        bytesPeeked = bytesExpected;
    }

    // Read whatever bytes are present from the buffer.
    return read(device, buffer, bytesPeeked);
}

/**
 * Write a string to the device via the UArt
 * serial interface. Note, the string is converted
 * to a character array and only then written.
 *
 * @param cmd The string command sent via the serial interface.
 * @return The number of chars (bytes) that have been successfully
 *      written to the device, -1 otherwise i.e. error.
 */
ssize_t Uart::writeBuffer(std::string &cmd)
{

    // Convert the string into a char buffer.
    char cmdBuffer[cmd.size() + 1];
    strcpy(cmdBuffer, cmd.c_str());

    // Attempt to write the converted command to the device.
    return writeBuffer(cmdBuffer);
}

/**
 * Write a character array (C-string) to the device via the Uart
 * serial interface.
 *
 * @param cmdBuffer The C-string command that is to be written to the
 *      device.
 * @return The number of chars (bytes) that have been successfully
 *      written to the device, -1 otherwise i.e. error.
 */
ssize_t Uart::writeBuffer(const char * const cmdBuffer)
{

    // If the device is present, write a command.
    if (device != -1) {
        return write(device, cmdBuffer, strlen(cmdBuffer) + 1);
    }

    // Failed to write to serial.
    return -1;
}
