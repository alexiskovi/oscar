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
#include "modules/supervisor/submodules/proto/gnss_conf.pb.h"
#include "modules/supervisor/submodules/proto/sv_gnss_msg.pb.h"
#include "modules/drivers/gnss/proto/gnss_best_pose.pb.h"



namespace apollo {
namespace supervisor {

class GNSSSupervisor : public SupervisorRunner {
 public:
  GNSSSupervisor();
  void RunOnce(const double current_time) override;
  void GetStatus(std::string* submodule_name, int* status, std::string* debug_msg);
 private:
  int status_;
  std::string debug_msg_;
  std::shared_ptr<apollo::cyber::Node> sv_gnss_node_;
  std::shared_ptr<apollo::cyber::Reader<apollo::drivers::gnss::GnssBestPose>> best_pose_reader_;
  std::shared_ptr<cyber::Writer<sv_gnss_msg>> gnss_status_writer_;
  sv_gnss_conf gnss_conf_;
};

}
}
