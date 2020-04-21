#include "modules/supervisor/common/supervisor_runner.h"
#include "cyber/common/log.h"

namespace apollo {
namespace supervisor {

SupervisorRunner::SupervisorRunner(const std::string &name, const double interval)
    : name_(name), interval_(interval) {}

void SupervisorRunner::Tick(const double current_time) {
  if (next_round_ <= current_time) {
    ++round_count_;
    AINFO_EVERY(100) << name_ << " is running round #" << round_count_;
    next_round_ = current_time + interval_;
    RunOnce(current_time);
  }
}

}
}
