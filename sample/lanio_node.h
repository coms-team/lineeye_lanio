#include <lineeye/lanio.h>
#include <lineeye_lanio/Set.h>
#include <lineeye_lanio/SetBitwise.h>
#include <ros/ros.h>
#include <std_msgs/Int16.h>
#include <mutex>

using namespace lineeye;
class LanIONode {
public:
  LanIONode() : nh_("~") {
    int num = io_.get_num_devices();
    if (num < 1) {
      ROS_ERROR("No device found!");
      ros::shutdown();
      return;
    }
    ROS_INFO("%d devices found", num);
    double input_port_check_interval;
    ros::param::param<double>("input_port_check_interval", input_port_check_interval, 1.0);
    // obtain first device ip address
    // std::string ip = io_.get_ip_address(1);

    // ROS_INFO("IP: %s", ip.c_str());
    device_ = io_.get_device(0);
    input_pub_ = nh_.advertise<std_msgs::Int16>("input_port_values", 1, true);
    service_ = nh_.advertiseService("set", &LanIONode::set, this);
    service_bitwise_ = nh_.advertiseService("set_bitwise", &LanIONode::set_bitwise, this);
    timer_ = nh_.createTimer(ros::Duration(input_port_check_interval), &LanIONode::_check_input_ports, this);
  }
  virtual ~LanIONode() {}

  bool set(lineeye_lanio::Set::Request& req, lineeye_lanio::Set::Response& res) {
    std::lock_guard<std::mutex> lock(mtx_);
    try {
      device_->set_output_port(req.port, req.val);
      res.success = true;
    } catch (LanIOException& e) {
      res.success = false;
      res.message = e.what();
    }
    return true;
  }
  bool set_bitwise(lineeye_lanio::SetBitwise::Request& req, lineeye_lanio::SetBitwise::Response& res) {
    std::lock_guard<std::mutex> lock(mtx_);
    try {
      device_->set_output_ports(req.val);
      res.success = true;
    } catch (LanIOException& e) {
      res.success = false;
      res.message = e.what();
    }
    return true;
  }

  void _check_input_ports(const ros::TimerEvent& e) {
    std::lock_guard<std::mutex> lock(mtx_);
    std_msgs::Int16 msg;
    try {
      msg.data = device_->get_input_ports();
    } catch (LanIOException& e) {
      ROS_WARN(e.what());
    }
    input_pub_.publish(msg);
  }

protected:
  LanIO io_;
  std::unique_ptr<LanIODevice> device_;
  ros::NodeHandle nh_;
  ros::Timer timer_;
  ros::ServiceServer service_, service_bitwise_;
  ros::Publisher input_pub_;
  std::mutex mtx_;
};
