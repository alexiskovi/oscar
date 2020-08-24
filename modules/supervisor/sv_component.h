#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "modules/common/time/time.h"
#include "cyber/component/timer_component.h"
#include "cyber/cyber.h"
#include "boost/bind.hpp"

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
  void GetCurrentMode(bool* status);
  void GetModuleParameters(std::string module_name);
  void SaveCurrentConfig();
  bool SetParameter(std::string module, std::string config, std::string value);
  void PreferencesCallback(const std::shared_ptr<apollo::supervisor::sv_set_get>& msg);
  ~Supervisor();
 private:
  std::shared_ptr<apollo::cyber::Node> supervisor_node_;
  std::vector<std::shared_ptr<SupervisorRunner>> supervisors_;
  std::thread signal_thread_;
  bool signal_active_flag_;
  bool auto_driving_mode_;
  std::shared_ptr<cyber::Reader<apollo::canbus::ChassisDetail>>
      chassis_detail_reader_;
  std::shared_ptr<cyber::Reader<apollo::supervisor::sv_set_get>>
      preferences_reader_;
  std::shared_ptr<cyber::Writer<apollo::supervisor::submodule_parameters>>
      callback_writer_;
  std::shared_ptr<cyber::Writer<apollo::supervisor::sv_decision>>
      decision_writer_;
  YAML::Node sv_preferences_;
};

CYBER_REGISTER_COMPONENT(Supervisor)

}
}
