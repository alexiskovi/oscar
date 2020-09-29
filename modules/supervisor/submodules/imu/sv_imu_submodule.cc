#include "modules/supervisor/submodules/imu/sv_imu_submodule.h"

DEFINE_double(imu_check_period, 0.3,
            "Interval in seconds.");            
DEFINE_string(sv_imu_conf_file, "/apollo/modules/supervisor/submodules/conf/imu_conf.pb.txt",
            "Path to IMU conf file");

namespace apollo {
namespace supervisor {

IMUSupervisor::IMUSupervisor() 
    : SupervisorRunner("IMUSupervisor", FLAGS_imu_check_period) {
  sv_imu_node_ = apollo::cyber::CreateNode("sv_imu_node");
  ins_stat_reader_ = sv_imu_node_->CreateReader<apollo::drivers::gnss::InsStat>("/apollo/sensor/gnss/ins_stat", nullptr);
  ACHECK(ins_stat_reader_ != nullptr);
  ACHECK(
    cyber::common::GetProtoFromFile(FLAGS_sv_imu_conf_file, &imu_conf_))
    << "Unable to load IMU conf file: " + FLAGS_sv_imu_conf_file;
  imu_status_writer_ = sv_imu_node_->CreateWriter<sv_imu_msg>("/supervisor/imu/status");
}

void IMUSupervisor::RunOnce(const double current_time) {
  status_ = 0;
  debug_msg_ = "";
  ins_stat_reader_->Observe();
  const auto &ins_stat_msg = ins_stat_reader_->GetLatestObserved();

  if(ins_stat_msg == nullptr) {
    AERROR << "INS stat message is not ready";
    status_ = (int)imu_conf_.ins_stat_not_ready_priority();
    debug_msg_ = "INS stat is not ready. Check that IMU module is on.";
    return void();
  }
  // Check INS stat timeout
  if(current_time - ins_stat_msg->header().timestamp_sec() > imu_conf_.max_timeout_s()){
    if(status_ < (int)imu_conf_.timeout_priority()){
      status_ = (int)imu_conf_.timeout_priority();
      debug_msg_ = "IMU message timeout";
    }
  }
  // Check calibration status
  if(ins_stat_msg->ins_status() != 3){  // 3 is state for GOOD INS calibration
    if(status_ < (int)imu_conf_.bad_calibration_priority()){
      status_ = (int)imu_conf_.bad_calibration_priority();
      debug_msg_ = "IMU invalid calibration status";
    }
  }

  sv_imu_msg msg;
  msg.set_calibration_status(ins_stat_msg->ins_status());
  if(status_ < 10) msg.set_overall_status("OK");
  if((status_ >= 10)&&(status_ < 20)) msg.set_overall_status("WARNING");
  if((status_ >= 20)&&(status_ < 30)) msg.set_overall_status("ERROR");
  if((status_ >= 30)&&(status_ < 40)) msg.set_overall_status("FATAL");
  msg.set_debug_message(debug_msg_);
  imu_status_writer_->Write(msg);

}

void IMUSupervisor::GetStatus(std::string* submodule_name, int* status, std::string* debug_msg) {
  *submodule_name = "imu";
  *status = status_;
  *debug_msg = debug_msg_;
}

}
}