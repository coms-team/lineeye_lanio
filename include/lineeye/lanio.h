#pragma once

#include <memory>
#include <sstream>
#include <stdexcept>

namespace lineeye {

class LanIOException : public std::runtime_error {
public:
  LanIOException(const std::string& msg);
};

class LanIODevice {
public:
  LanIODevice(int id);
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
  std::unique_ptr<LanIODevice> get_device(int id);
};

}  // namespace lineeye
