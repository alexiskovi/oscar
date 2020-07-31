#include "modules/supervisor/sv_component.h"

#include "modules/supervisor/submodules/gnss/sv_gnss_submodule.h"

using apollo::cyber::Time;

DEFINE_bool(sound_auto_only, true,
            "Make sound signals only in autonomous mode");  

namespace apollo {
namespace supervisor {

void Supervisor::WarningSignal() {
  signal_active_flag_ = true;
  system("play -nq -t alsa synth 0.1 sine 400");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  system("play -nq -t alsa synth 0.1 sine 400");
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  signal_active_flag_ = false;
}

void Supervisor::ErrorSignal() {
  signal_active_flag_ = true;
  system("play -nq -t alsa synth 0.1 sine 300");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  system("play -nq -t alsa synth 0.1 sine 300");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  system("play -nq -t alsa synth 0.1 sine 300");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  system("play -nq -t alsa synth 0.1 sine 300");
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  signal_active_flag_ = false;
}

void Supervisor::GetCurrentMode(bool* status) {
  //get last message from chassis_detail
  chassis_detail_reader_->Observe();
  const auto &status_msg = chassis_detail_reader_->GetLatestObserved();
  if(status_msg == nullptr) *status = false;
  else {
    *status = status_msg->basic().acc_on_off_button();
  }
}

bool Supervisor::Init() {
  // Collecting sub-supervisors
  supervisors_.emplace_back(new GNSSSupervisor());

  std::shared_ptr<apollo::cyber::Node> supervisor_node_(apollo::cyber::CreateNode("supervisor"));
  writer_ = supervisor_node_->CreateWriter<apollo::supervisor::sv_decision>("/supervisor/decision");

  // Loading preferences
  ACHECK(
    cyber::common::GetProtoFromFile("/apollo/modules/supervisor/conf/preferences.pb.txt", &sv_preferences_))
    << "Unable to load supervisor conf file: /apollo/modules/supervisor/conf/preferences.pb.txt";

  //Creating chassis_detail reader to get driving mode status
  chassis_detail_reader_ = supervisor_node_->CreateReader<apollo::canbus::ChassisDetail>("/apollo/canbus/chassis_detail", nullptr);

  // If sound switched on default, check sound
  if(sv_preferences_.sv_parameters().sound_on()){
    system("play -nq -t alsa synth 0.1 sine 300");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    system("play -nq -t alsa synth 0.1 sine 300");
  }
  return true;
}

bool Supervisor::Proc() {
  const double current_time = apollo::common::time::Clock::NowInSeconds();
  GetCurrentMode(&auto_driving_mode_);
  int worst_status = -1;
  apollo::supervisor::sv_decision_Status overall_status = apollo::supervisor::sv_decision::OK;
  std::string debug = "";

  for (auto& sv : supervisors_) {
    int status;
    std::string debug_msg;
    sv->Tick(current_time);
    sv->GetStatus(&status, &debug_msg);
    if(status > worst_status){
      if((status >= 10)&&(status < 20)) overall_status = apollo::supervisor::sv_decision::WARN;
      if((status >= 20)&&(status < 30)) overall_status = apollo::supervisor::sv_decision::ERROR;
      if((status >= 30)&&(status < 40)) overall_status = apollo::supervisor::sv_decision::FATAL;
      worst_status = status;
      debug = debug_msg;
    }
  }

  switch (overall_status) {
    case apollo::supervisor::sv_decision::OK:
      // all ok
    break;
    case apollo::supervisor::sv_decision::WARN:
      if((!FLAGS_sound_auto_only)||(auto_driving_mode_)){
        if(!signal_active_flag_) {
          if(signal_thread_.joinable()) {
            signal_thread_.join();
          }
          signal_thread_ = std::thread(&Supervisor::WarningSignal, this);
        }
      }
    break;
    case apollo::supervisor::sv_decision::ERROR:
      if((!FLAGS_sound_auto_only)||(auto_driving_mode_)){
        if(!signal_active_flag_) {
          if(signal_thread_.joinable()) {
            signal_thread_.join();
          }
          signal_thread_ = std::thread(&Supervisor::ErrorSignal, this);
        }
      }
    break;
    case apollo::supervisor::sv_decision::FATAL:
      // fatal signal thread
    break;
  }

  sv_decision msg;
  msg.mutable_header()->set_timestamp_sec(Time::Now().ToSecond());
  msg.set_status(overall_status);
  msg.set_message(debug);
  writer_->Write(msg);

  return true;
}

Supervisor::~Supervisor() {
  if (signal_thread_.joinable()) {
    signal_thread_.join();
  }
}

}
}