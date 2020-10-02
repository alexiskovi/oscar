#pragma once

#include <memory>
#include <vector>
#include "modules/common/time/time.h"
#include "cyber/component/timer_component.h"
#include "cyber/cyber.h"
#include "boost/bind.hpp"

#include "modules/guardian/proto/guardian.pb.h"
#include "modules/control/proto/control_cmd.pb.h"
#include "modules/supervisor/proto/sv_decision.pb.h"


namespace apollo {
namespace supervisor {

class SupervisorGuard : public apollo::cyber::Component<apollo::supervisor::sv_decision> {
 public:
  bool Init();
  bool Proc(const std::shared_ptr<apollo::supervisor::sv_decision>& sv_decision_msg);
 private:
  void PassThroughControlCmd();
  void SoftStop();
  void EStop();
  apollo::control::ControlCommand control_cmd_;
  apollo::guardian::GuardianCommand sv_guard_cmd_;
  std::shared_ptr<apollo::cyber::Node> supervisor_guard_node_;
  std::shared_ptr<apollo::cyber::Reader<apollo::control::ControlCommand>>
      control_cmd_reader_;
  std::shared_ptr<apollo::cyber::Writer<apollo::guardian::GuardianCommand>>
      sv_guard_writer_;
  std::mutex mutex_;
};

CYBER_REGISTER_COMPONENT(SupervisorGuard)

}
}
