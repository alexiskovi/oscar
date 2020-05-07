#include "modules/supervisor/software/imu_supervisor.h"

DEFINE_double(imu_check_period, 0.2,
            "Interval in seconds.");
DEFINE_double(imu_timeout_sec, 2.0,
            "Timeout in seconds.");            
DEFINE_string(sv_imu_conf_file, "/apollo/modules/supervisor/conf/imu_conf.pb.txt",
            "Path to IMU conf file");


namespace apollo {
namespace supervisor {

IMUSupervisor::IMUSupervisor() 
    : SupervisorRunner("IMUSupervisor", FLAGS_imu_check_period) {
  sv_sub_node_ = apollo::cyber::CreateNode("sv_sub_node");
  imu_reader_ = sv_sub_node_->CreateReader<apollo::drivers::gnss::InsStat>("/apollo/sensor/gnss/ins_stat", nullptr);
  ACHECK(imu_reader_ != nullptr);
  ACHECK(
    cyber::common::GetProtoFromFile(FLAGS_sv_imu_conf_file, &imu_conf_))
    << "Unable to load imu conf file: " + FLAGS_sv_imu_conf_file;
}

void IMUSupervisor::RunOnce(const double current_time) {
  status_ = 0;
  imu_reader_->Observe();
  const auto &imu_msg = imu_reader_->GetLatestObserved();
  if(imu_msg == nullptr) {
    AERROR << "IMU message is not ready";
    return void();
  }
  // Check timeout
  if(current_time - imu_msg->header().timestamp_sec() > FLAGS_imu_timeout_sec){
    status_ = imu_conf_.timeout_priority();
    debug_msg_ = "IMU message timeout";
  }
  // Check calibration statuss
  if(imu_msg->ins_status() != 3){
    status_ = imu_conf_.bad_calibration_priority();
    debug_msg_ = "Bad IMU calibration status";
  }
}

void IMUSupervisor::GetStatus(int* status, std::string* debug_msg) {
  *status = status_;
  *debug_msg = debug_msg_;
}

}
}