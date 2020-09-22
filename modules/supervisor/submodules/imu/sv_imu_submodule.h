#pragma once

#include <memory>
#include <vector>
#include <cstdlib>
#include <thread>

#include "cyber/component/timer_component.h"
#include "cyber/cyber.h"
#include "cyber/time/time.h"
#include "cyber/time/duration.h"
#include "modules/supervisor/common/supervisor_runner.h"
#include "cyber/common/log.h"
#include "modules/common/adapters/adapter_gflags.h"
#include "cyber/component/timer_component.h"
#include "modules/supervisor/submodules/proto/imu_conf.pb.h"
#include "modules/supervisor/submodules/proto/sv_imu_msg.pb.h"
#include "modules/drivers/gnss/proto/ins.pb.h"
#include "modules/drivers/gnss/proto/imu.pb.h"


namespace apollo {
namespace supervisor {

class IMUSupervisor : public SupervisorRunner {
 public:
  IMUSupervisor();
  void RunOnce(const double current_time) override;
  void GetStatus(std::string* submodule_name, int* status, std::string* debug_msg);
 private:
  int status_;
  std::string debug_msg_;
  std::shared_ptr<apollo::cyber::Node> sv_imu_node_;
  std::shared_ptr<apollo::cyber::Reader<apollo::drivers::gnss::InsStat>> ins_stat_reader_;
  std::shared_ptr<cyber::Writer<sv_imu_msg>> imu_status_writer_;
  sv_imu_conf imu_conf_;
};

}
}
