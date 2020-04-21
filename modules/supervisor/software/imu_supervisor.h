#pragma once

#include "modules/supervisor/common/supervisor_runner.h"
#include "cyber/common/log.h"

namespace apollo {
namespace supervisor {

class IMUSupervisor : public SupervisorRunner {
 public:
  IMUSupervisor();
  void RunOnce(const double current_time) override;
  void GetStatus(int* status, std::string* debug_msg);
 private:
  int status_;
  std::string debug_msg_;
};

}
}