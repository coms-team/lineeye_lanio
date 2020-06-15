// #include <pybind11/pybind11.h>
// namespace py = pybind11;
#include "lineeye/lanio.h"
extern "C" {
#include "LELanio.h"
}
#include <boost/format.hpp>

namespace lineeye {

LanIOException::LanIOException(const std::string& msg) : std::runtime_error(msg) {
    std::stringstream ss;
    ss << msg << " - ";
    int code = LELanioGetLastError();
    switch (code) {
        case LELANIOERR_NOINIT:
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

LanIO::LanIO() {
    LELanioInit();
}
LanIO::~LanIO() {
    LELanioEnd();
}
int LanIO::get_num_devices() {
    int num = LELanioSearch(1000);
    if (num == -1) {
        throw LanIOException("could not search devices");
    }
    return num;
}

std::string LanIO::get_ip_address(int i) {
    char ipaddress[16];
    int ret = LELanioGetIpAddress(i, ipaddress);
    if (ret != 0) {
        throw LanIOException("could not get macaddress");
    }
    return std::move(std::string(ipaddress));
}

std::unique_ptr<LanIODevice> LanIO::get_device(const std::string& ipaddress) {
    return std::make_unique<LanIODevice>(ipaddress);
}
LanIODevice::LanIODevice(const std::string& ipaddress) {
    handle_ = LELanioConnectByIpAddress(const_cast<char*>(ipaddress.c_str()));
    if (handle_ == -1) {
        throw LanIOException((boost::format("could not connect to %1%") % ipaddress).str());
    }
}
LanIODevice::~LanIODevice() {
    LELanioClose(handle_);
}

void LanIODevice::set_output_port(int port, bool t) {
    int ret = LELanioOutPio(handle_, port, static_cast<int>(t));
    if (ret == 0) {
        throw LanIOException((boost::format("failed to set output (port:%1%)") % port).str());
    }
}
void LanIODevice::set_output_ports(int bit) {
    int ret = LELanioOutPioAll(handle_, bit);
    if (ret == 0) {
        throw LanIOException((boost::format("failed to set output (bit:%0xd)") % bit).str());
    }
}

bool LanIODevice::get_input_port(int port) {
    int val;
    int ret = LELanioInPio(handle_, port, &val);
    if (ret == 0) {
        throw LanIOException((boost::format("failed to get input (port:%1%)") % port).str());
    }
    return val;
}
int LanIODevice::get_input_ports() {
    int bit;
    int ret = LELanioInPioAll(handle_, &bit);
    if (ret == 0) {
        throw LanIOException((boost::format("failed to get inputs (bit:%0xd)") % bit).str());
    }
    return bit;
}

}  // namespace lineeye

// clang-format off
// PYBIND11_MODULE(lineeye_lanio, m) {
//     py::class_<lineeye::LanIO>(m, "LanIO")
//         .def(py::init<>())
//         .def("get_num_devices", &lineeye::LanIO::get_num_devices)
//         .def("get_ip_address", &lineeye::LanIO::get_ip_address);
//     py::register_exception<lineeye::LanIOException>(m, "LanIOException");
// }
// clang-format on
