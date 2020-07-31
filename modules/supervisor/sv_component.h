#pragma once

#include <memory>
#include <vector>

#include "modules/common/time/time.h"
#include "cyber/component/timer_component.h"
#include "cyber/cyber.h"
#include "modules/supervisor/common/supervisor_runner.h"
#include "modules/supervisor/proto/sv_decision.pb.h"
#include "modules/supervisor/proto/parameter_server.pb.h"
#include "modules/common/adapters/adapter_gflags.h"
#include "modules/canbus/proto/chassis_detail.pb.h"


namespace apollo {
namespace supervisor {

class Supervisor : public apollo::cyber::TimerComponent {
 public:
  bool Init() override;
  bool Proc() override;
  void ErrorSignal();
  void WarningSignal();
  bool LaunchParameterService();
  void GetCurrentMode(bool* status);
  ~Supervisor();
 private:
  std::shared_ptr<apollo::cyber::Node> supervisor_node_;
  std::vector<std::shared_ptr<SupervisorRunner>> supervisors_;
  std::shared_ptr<apollo::cyber::Writer<apollo::supervisor::sv_decision>> writer_;
  std::thread signal_thread_;
  bool signal_active_flag_;
  bool auto_driving_mode_;
  parameters sv_preferences_;
  std::shared_ptr<cyber::Reader<apollo::canbus::ChassisDetail>>
      chassis_detail_reader_;
};

CYBER_REGISTER_COMPONENT(Supervisor)

}
}
