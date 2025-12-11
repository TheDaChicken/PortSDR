//
// Created by TheDaChicken on 12/11/2025.
//

#ifndef PORTSDR_ERROR_H
#define PORTSDR_ERROR_H

enum class ErrorCode : int {
    OK = 0,
    INVALID_ARGUMENT = -1,
    DEVICE_NOT_FOUND = -2,
    FAILED_TO_INITIALIZE = -3,
    HOST_UNAVAILABLE = -4,
    LIBUSB_ERROR = -5,
    UNINITIALIZED = -6,
    UNKNOWN = -100
};

#endif //PORTSDR_ERROR_H