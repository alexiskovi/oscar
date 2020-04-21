#include "modules/common/time/time.h"
#include "modules/supervisor/sv_guardian_component.h"

using apollo::cyber::Time;
using apollo::cyber::Duration;

std::mutex mu;

namespace apollo {
namespace supervisor {

GSupervisor::~GSupervisor() {
  if (signal_thread_.joinable()) {
    signal_thread_.join();
  }
}

void GSupervisor::ErrorSignal() {
  signal_active_ = true;
  system("play -nq -t alsa synth 0.1 sine 300");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  system("play -nq -t alsa synth 0.1 sine 300");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  system("play -nq -t alsa synth 0.1 sine 300");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  system("play -nq -t alsa synth 0.1 sine 300");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  signal_active_ = false;
}

void GSupervisor::WarningSignal() {
  signal_active_ = true;
  system("play -nq -t alsa synth 0.1 sine 400");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  system("play -nq -t alsa synth 0.1 sine 400");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  signal_active_ = false;
}

bool GSupervisor::Init() {
  auto sv_guardian_node = apollo::cyber::CreateNode("sv_guardian");
  status_reader_ = sv_guardian_node->CreateReader<apollo::supervisor::SV_info>(
          "supervisor/general", nullptr);
  ACHECK(status_reader_ != nullptr);
  last_call_ = Time::Now();
  signal_active_ = false;
  return true;
}

bool GSupervisor::Proc() {
  status_reader_->Observe();
  const auto &status_msg = status_reader_->GetLatestObserved();
  if(status_msg == nullptr) {
    AERROR << "status msg is not ready";
    return false;
  }

  if(Time::Now().ToSecond() - status_msg->header().timestamp_sec() > 0.5) {
    AERROR << "Got timeout in " << Time::Now().ToSecond() - status_msg->header().timestamp_sec() << " sec";
    if (!signal_active_){
      if (signal_thread_.joinable()) {
        signal_thread_.join();
      }
      signal_thread_ = std::thread(&GSupervisor::ErrorSignal, this);
    }
  }

  if(status_msg->status() == 2) {
    AWARN << "Got warning";
    if (!signal_active_){
      if (signal_thread_.joinable()) {
        signal_thread_.join();
      }
      signal_thread_ = std::thread(&GSupervisor::WarningSignal, this);
    }
  }

  if(status_msg->status() == 3) {
    AWARN << "Got error";
    if (!signal_active_){
      if (signal_thread_.joinable()) {
        signal_thread_.join();
      }
      signal_thread_ = std::thread(&GSupervisor::ErrorSignal, this);
    }
  }

  return true;
}

}
}
