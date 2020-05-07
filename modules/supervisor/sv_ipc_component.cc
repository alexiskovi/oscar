#include "modules/supervisor/sv_ipc_component.h"
#include "modules/supervisor/software/imu_supervisor.h"
#include "modules/supervisor/software/gnss_supervisor.h"

using apollo::cyber::Time;

namespace apollo {
namespace supervisor {

bool IPCSupervisor::Init() {
  supervisors_.emplace_back(new IMUSupervisor());
  supervisors_.emplace_back(new GNSSSupervisor());

  std::shared_ptr<apollo::cyber::Node> supervisor_node_(
      apollo::cyber::CreateNode("supervisor_ipc"));
  writer_ = supervisor_node_->CreateWriter<apollo::supervisor::SV_info>("/supervisor/general");
  return true;
}

bool IPCSupervisor::Proc() {
  const double current_time = apollo::common::time::Clock::NowInSeconds();

  int worst_status = 0;
  apollo::supervisor::SV_info_Status overall_status = apollo::supervisor::SV_info::OK;
  std::string debug = "";
  for (auto& sv : supervisors_) {
    int status;
    std::string debug_msg;
    sv->Tick(current_time);
    sv->GetStatus(&status, &debug_msg);
    if(status > worst_status){
      if((status >= 10)&&(status < 20)) overall_status = apollo::supervisor::SV_info::WARN;
      if((status >= 20)&&(status < 30)) overall_status = apollo::supervisor::SV_info::ERROR;
      if((status >= 30)&&(status < 40)) overall_status = apollo::supervisor::SV_info::FATAL;
      worst_status = status;
      debug = debug_msg;
    }
  }

  SV_info msg;
  msg.mutable_header()->set_timestamp_sec(Time::Now().ToSecond());
  msg.set_status(overall_status);
  msg.set_message(debug);
  writer_->Write(msg);

  return true;
}

}
}