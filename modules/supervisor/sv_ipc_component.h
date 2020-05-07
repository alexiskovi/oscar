#pragma once

#include <memory>
#include <vector>

#include "modules/common/time/time.h"
#include "cyber/component/timer_component.h"
#include "cyber/cyber.h"
#include "modules/supervisor/common/supervisor_runner.h"
#include "modules/supervisor/proto/general_msg.pb.h"

namespace apollo {
namespace supervisor {

class IPCSupervisor : public apollo::cyber::TimerComponent {
 public:
  bool Init() override;
  bool Proc() override;
 private:
  std::shared_ptr<apollo::cyber::Node> supervisor_node_;
  std::vector<std::shared_ptr<SupervisorRunner>> supervisors_;
  std::shared_ptr<apollo::cyber::Writer<apollo::supervisor::SV_info>> writer_;

};

CYBER_REGISTER_COMPONENT(IPCSupervisor)

}
}
