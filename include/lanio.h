#pragma once

#include <stdexcept>
extern "C" {
#include "LELanio.h"
}
#include <sstream>

namespace lineeye {

class LanIOException : public std::runtime_error {
    public:
    LanIOException(const std::string& msg) : std::runtime_error(msg) {
        std::stringstream ss;
        ss << msg << " - ";
        int code = LELanioGetLastError();
        switch (code) {
            case LANIOERROR::LELANIOERR_NOINIT:
                ss << "library is not initialized";
                break;
            case LELANIOERR_NOLANIO:
                ss << "no lanio device is found";
            case LELANIOERR_NOTEXIST:
                ss << "more devices are found rather than we searched previously";
            case LELANIOERR_NOTFOUND:
                ss << "No such a device";
            case LELANIOERR_NOCONNECTED:
                ss << "not connected yet";
            case LELANIOERR_INVALIDOPERATION:
                ss << "invalid operation";
            case LELANIOERR_ALREADY_CONNECTED:
                ss << "it is already connected";
            case LELANIOERR_OVERCONNECTMAX:
                ss << "too many connections";
            case LELANIOERR_INVALIDRETURNVALUE:
                ss << "invalid return value";
            default:
                ss << "unknown error";
        }
        // call constructor again
        std::runtime_error(ss.str().c_str());
    }
};

class LanIO {
  public:
    LanIO();
    virtual ~LanIO();
    int get_num_devices();
    std::string get_ip_address(int lanio_num);
};
}  // namespace lineeye
