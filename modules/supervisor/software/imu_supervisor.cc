#include "modules/supervisor/software/imu_supervisor.h"

namespace apollo {
namespace supervisor {

IMUSupervisor::IMUSupervisor() 
    : SupervisorRunner("IMUSupervisor", 1) {}

void IMUSupervisor::RunOnce(const double current_time) {
  status_ = 25;
  debug_msg_ = "Check your calibration!";
}

void IMUSupervisor::GetStatus(int* status, std::string* debug_msg) {
  *status = status_;
  *debug_msg = debug_msg_;
}

}
}