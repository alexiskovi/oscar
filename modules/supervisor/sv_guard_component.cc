#include "modules/supervisor/sv_guard_component.h"

namespace apollo {
namespace supervisor {

using apollo::control::ControlCommand;

bool SupervisorGuard::Init() {
  std::shared_ptr<apollo::cyber::Node> supervisor_guard_node_(apollo::cyber::CreateNode("supervisor_guard"));
  control_cmd_reader_ = node_->CreateReader<ControlCommand>(
    "/apollo/control",
    [this](const std::shared_ptr<ControlCommand>& cmd) {
      std::lock_guard<std::mutex> lock(mutex_);
      control_cmd_.CopyFrom(*cmd);
    });
  sv_guard_writer_ = node_->CreateWriter<GuardianCommand>("/apollo/guardian");
  return true;
}

bool SupervisorGuard::Proc(const std::shared_ptr<apollo::supervisor::sv_decision>& sv_decision_msg) {
  if (sv_decision_msg -> status() == apollo::supervisor::sv_decision::ERROR) {
    SupervisorGuard::SoftStop();
  }
  else if (sv_decision_msg -> status() == apollo::supervisor::sv_decision::FATAL) {
    SupervisorGuard::EStop();
  }
  else {
    SupervisorGuard::PassThroughControlCommand();
    common::util::FillHeader(node_->Name(), &sv_guard_cmd_);
    sv_guard_writer_->Write(sv_guard_cmd_);
  }
  return true;
}

void SupervisorGuard::PassThroughControlCmd() {
  std::lock_guard<std::mutex> lock(mutex_);
  sv_guard_cmd_.mutable_control_command()->CopyFrom(control_cmd_);
}

void SupervisorGuard::SoftStop() {
  std::lock_guard<std::mutex> lock(mutex_);

  sv_guard_cmd_.mutable_control_command()->set_throttle(0.0);
  sv_guard_cmd_.mutable_control_command()->set_steering_target(0.0);
  sv_guard_cmd_.mutable_control_command()->set_steering_rate(25.0);
  sv_guard_cmd_.mutable_control_command()->set_brake(10.0);

}

void SupervisorGuard::EStop() {
  std::lock_guard<std::mutex> lock(mutex_);
  
  sv_guard_cmd_.mutable_control_command()->set_throttle(0.0);
  sv_guard_cmd_.mutable_control_command()->set_steering_target(0.0);
  sv_guard_cmd_.mutable_control_command()->set_steering_rate(25.0);
  sv_guard_cmd_.mutable_control_command()->set_brake(30.0);


}

}
}