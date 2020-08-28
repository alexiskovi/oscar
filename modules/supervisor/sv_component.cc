#include "modules/supervisor/sv_component.h"

#include "modules/supervisor/submodules/gnss/sv_gnss_submodule.h"
#include "modules/supervisor/submodules/imu/sv_imu_submodule.h"

using apollo::cyber::Time;

DEFINE_bool(sound_auto_only, false,
            "Make sound signals only in autonomous mode");  

DEFINE_string(sv_preferences_file, "/apollo/modules/supervisor/conf/preferences.yaml",
            "Path to supervisor submodules preferences file");  


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

void Supervisor::GetModuleParameters(std::string module_name) {
  submodule_parameters callback_msg;
  callback_msg.set_sound_on(sv_preferences_[module_name]["sound_on"].as<bool>());
  callback_msg.set_debug_mode(sv_preferences_[module_name]["debug_mode"].as<bool>());
  callback_writer_->Write(callback_msg);
}

bool Supervisor::SetParameter(std::string module, std::string config, std::string value) {
  try {
    sv_preferences_[module][config] = value;
    return true;
  }
  catch(const std::exception& e) {
    return false;
  }
}

void Supervisor::PreferencesCallback(const std::shared_ptr<apollo::supervisor::sv_set_get>& msg) {
  if(msg->cmd() == "get_parameters") {
    Supervisor::GetModuleParameters(msg->submodule().module_name());
    return void();
  }
  if(msg->cmd() == "change_parameters") {
    Supervisor::SetParameter(msg->submodule().module_name(), msg->submodule().config_name(), msg->submodule().new_value());
    return void();
  }
  if(msg->cmd() == "save_parameters") {
    Supervisor::SaveCurrentConfig();
    return void();
  }
  AERROR << "Unknown cmd parameter: " << msg->cmd();
}

bool Supervisor::Init() {
  // Collecting sub-supervisors
  supervisors_.emplace_back(new GNSSSupervisor());
  supervisors_.emplace_back(new IMUSupervisor());
  
  std::shared_ptr<apollo::cyber::Node> supervisor_node_(apollo::cyber::CreateNode("supervisor"));
  decision_writer_ = supervisor_node_->CreateWriter<apollo::supervisor::sv_decision>("/supervisor/decision");
  callback_writer_ = supervisor_node_->CreateWriter<apollo::supervisor::submodule_parameters>("/supervisor/callback");
  preferences_reader_ = supervisor_node_->CreateReader<apollo::supervisor::sv_set_get>
    ("/supervisor/preferences", boost::bind(&Supervisor::PreferencesCallback, this, _1));
  
  sv_preferences_ = YAML::LoadFile("/apollo/modules/supervisor/conf/preferences.yaml");

  //Creating chassis_detail reader to get driving mode status
  chassis_detail_reader_ = supervisor_node_->CreateReader<apollo::canbus::ChassisDetail>("/apollo/canbus/chassis_detail", nullptr);

  // If sound switched on default, check sound
  if(sv_preferences_["sv"]["sound_on"].as<bool>()) {
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
  std::string bad_submodule_name = "";

  for (auto& sv : supervisors_) {
    int status;
    std::string debug_msg;
    std::string submodule_name;
    sv->Tick(current_time);
    sv->GetStatus(&submodule_name, &status, &debug_msg);
    if(status > worst_status){
      if((status >= 10)&&(status < 20)) overall_status = apollo::supervisor::sv_decision::WARN;
      if((status >= 20)&&(status < 30)) overall_status = apollo::supervisor::sv_decision::ERROR;
      if((status >= 30)&&(status < 40)) overall_status = apollo::supervisor::sv_decision::FATAL;
      worst_status = status;
      bad_submodule_name = submodule_name;
      debug = debug_msg;
    }
  }

  // Sound information
  if((sv_preferences_["sv"]["sound_on"].as<bool>())&&(sv_preferences_[bad_submodule_name]["sound_on"].as<bool>())) {
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
  }

  sv_decision msg;
  msg.mutable_header()->set_timestamp_sec(Time::Now().ToSecond());
  msg.set_status(overall_status);
  msg.set_message(debug);
  decision_writer_->Write(msg);

  return true;
}

void Supervisor::SaveCurrentConfig() {
  std::ofstream fout(FLAGS_sv_preferences_file);
  fout << sv_preferences_;
  fout.close();
}

Supervisor::~Supervisor() {
  if (signal_thread_.joinable()) {
    signal_thread_.join();
  }
}

}
}