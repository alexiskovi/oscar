#pragma once

#include <string>

namespace apollo {
namespace supervisor {

class SupervisorRunner {
 public:
  SupervisorRunner(const std::string &name, const double interval);
  virtual ~SupervisorRunner() = default;

  void Tick(const double current_time);

  virtual void RunOnce(const double current_time) = 0;

  virtual void GetStatus(int* status, std::string* debug_msg) = 0;

 protected:
  std::string name_;
  unsigned int round_count_ = 0;

 private:
  double interval_;
  double next_round_ = 0;
};

}
}
