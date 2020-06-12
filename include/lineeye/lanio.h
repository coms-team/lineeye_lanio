#pragma once

#include <stdexcept>
extern "C" {
#include "LELanio.h"
}
#include <sstream>
#include <memory>

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

class LanIODevice {
  public:
    LanIODevice(const std::string& ipaddress);
    virtual ~LanIODevice();
    void set_output_port(int port, bool t);
    void set_output_ports(int bit);
    bool get_input_port(int port);
    int get_input_ports();

  protected:
    int handle_;
};
class LanIO {
  public:
    LanIO();
    virtual ~LanIO();
    int get_num_devices();
    std::string get_ip_address(int lanio_num);
    std::unique_ptr<LanIODevice> get_device(const std::string& ipaddress);
};

}  // namespace lineeye
