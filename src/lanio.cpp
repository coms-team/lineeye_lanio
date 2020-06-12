#include <pybind11/pybind11.h>
#include <boost/format.hpp>

namespace py = pybind11;
#include "lanio.h"

namespace lineeye {

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

}  // namespace lineeye

// clang-format off
PYBIND11_MODULE(lineeye_lanio, m) {
    py::class_<lineeye::LanIO>(m, "LanIO")
        .def(py::init<>())
        .def("get_num_devices", &lineeye::LanIO::get_num_devices)
        .def("get_ip_address", &lineeye::LanIO::get_ip_address);
    py::register_exception<lineeye::LanIOException>(m, "LanIOException");
}
// clang-format on
