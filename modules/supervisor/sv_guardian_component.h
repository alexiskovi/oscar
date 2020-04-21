#pragma once

#include <memory>
#include <vector>
#include <cstdlib>
#include <thread>

#include "cyber/component/timer_component.h"
#include "cyber/cyber.h"
#include "modules/supervisor/proto/general_msg.pb.h"
#include "cyber/time/time.h"
#include "cyber/time/duration.h"

namespace apollo {
namespace supervisor {

cyber::Time last_call_;

class GSupervisor : public apollo::cyber::TimerComponent {
 public:
  bool Init() override;
  bool Proc() override;
  void ErrorSignal();
  void WarningSignal();
  ~GSupervisor();
 private:
  std::shared_ptr<cyber::Reader<apollo::supervisor::SV_info>>
      status_reader_;
  std::thread signal_thread_;
  bool signal_active_;
};

CYBER_REGISTER_COMPONENT(GSupervisor)

}
}
