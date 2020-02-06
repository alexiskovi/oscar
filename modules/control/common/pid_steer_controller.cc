/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "modules/control/common/pid_steer_controller.h"

#include <cmath>

#include "cyber/common/log.h"

namespace apollo {
namespace control {

double PIDSteerController::Control(const double angle, const double goal, const double dt) {
  if (dt <= 0) {
    AWARN << "dt <= 0, will use the last output, dt: " << dt;
    return previous_output_;
  }
  double diff = 0;
  double output = 0;

  if (first_hit_) {
    first_hit_ = false;
  } else {
    diff = (angle - last_angle_) / dt;
  }
  const double error = goal - angle;
  // integral hold
  if (!integrator_enabled_) {
    integral_ = 0;
  } else if (!integrator_hold_) {
    integral_ += error * dt * ki_;
    // apply Ki before integrating to avoid steps when change Ki at steady state
    if (integral_ > integrator_saturation_high_) {
      integral_ = integrator_saturation_high_;
      integrator_saturation_status_ = 1;
    } else if (integral_ < integrator_saturation_low_) {
      integral_ = integrator_saturation_low_;
      integrator_saturation_status_ = -1;
    } else {
      integrator_saturation_status_ = 0;
    }
  }
  last_angle_ = angle;
  output = error * kp_ + integral_ + diff * kd_;  // Ki already applied
  proportional_ = error * kp_;
  differential_ = diff * kd_;
  previous_output_ = output;
  return output;
}

void PIDSteerController::Reset() {
  last_angle_ = 0.0;
  previous_output_ = 0.0;
  integral_ = 0.0;
  first_hit_ = true;
  integrator_saturation_status_ = 0;
  output_saturation_status_ = 0;
}

void PIDSteerController::Init(const PidSteerConf &pid_steer_conf) {
  last_angle_ = 0.0;
  previous_output_ = 0.0;
  integral_ = 0.0;
  first_hit_ = true;
  integrator_enabled_ = pid_steer_conf.integrator_enable();
  integrator_saturation_high_ =
      std::fabs(pid_steer_conf.integrator_saturation_level());
  integrator_saturation_low_ =
      -std::fabs(pid_steer_conf.integrator_saturation_level());
  integrator_saturation_status_ = 0;
  integrator_hold_ = false;
  output_saturation_high_ = std::fabs(pid_steer_conf.output_saturation_level());
  output_saturation_low_ = -std::fabs(pid_steer_conf.output_saturation_level());
  output_saturation_status_ = 0;
  SetPID(pid_steer_conf);
}

void PIDSteerController::SetPID(const PidSteerConf &pid_steer_conf) {
  kp_ = pid_steer_conf.kp();
  ki_ = pid_steer_conf.ki();
  kd_ = pid_steer_conf.kd();
  kaw_ = pid_steer_conf.kaw();
}

int PIDSteerController::IntegratorSaturationStatus() const {
  return integrator_saturation_status_;
}

bool PIDSteerController::IntegratorHold() const { return integrator_hold_; }

void PIDSteerController::SetIntegratorHold(bool hold) { integrator_hold_ = hold; }

double PIDSteerController::getIntegral() {
   return integral_;
}

double PIDSteerController::getProportional() {
   return proportional_;
}

double PIDSteerController::getDifferential() {
   return differential_;
}

}  // namespace control
}  // namespace apollo
