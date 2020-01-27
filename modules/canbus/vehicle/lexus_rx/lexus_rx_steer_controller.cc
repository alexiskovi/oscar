#include "modules/canbus/vehicle/lexus_rx/lexus_rx_steer_controller.h"

using namespace apollo::canbus::lexus_rx;

SteeringController::SteeringController() {}

void SteeringController::set_enable() {
    is_enabled_ = true;
    if ( speed_ >= alter_speed_min && speed_ <= lka_speed_min ) {
        steering_7f0_->set_enable();
        steering_2e4_->set_disable();
        source_ = STEERING_ALTER;
        printf("Speed %f. Source ALTER\n", speed_);
    }
    else if ( speed_ > lka_speed_min && speed_ <= alter_speed_max ) {
        if ( source_ == STEERING_ALTER ) {
            steering_7f0_->set_enable();
            steering_2e4_->set_disable();
            printf("Speed %f. Source ALTER\n", speed_);
        } else {
            steering_7f0_->set_disable();
            steering_2e4_->set_enable();
            printf("Speed %f. Source LKA\n", speed_);
        }
    }
    else if ( speed_ > alter_speed_max && speed_ <= lka_speed_max ) {
        steering_7f0_->set_disable();
        steering_2e4_->set_enable();
        source_ = STEERING_LKA;
        printf("Speed %f. Source LKA\n", speed_);
    }
}

void SteeringController::set_disable() {
    printf("Disable steering controller\n");
    is_enabled_ = false;
    steering_2e4_->set_disable();
    steering_7f0_->set_disable();
}

void SteeringController::set_speed( const double speed ) {
    speed_ = speed;
    steering_2e4_->set_speed( speed );
    steering_7f0_->set_speed( speed );
}

void SteeringController::set_current_steering_angle( const double angle ) {
    angle_ = angle;
    steering_2e4_->set_current_steering_angle( angle );
    steering_7f0_->set_current_steering_angle( angle );
}

void SteeringController::set_goal_steering_angle( const double angle ) {
    steering_2e4_->set_goal_steering_angle( angle );
    steering_7f0_->set_goal_steering_angle( angle );
}
