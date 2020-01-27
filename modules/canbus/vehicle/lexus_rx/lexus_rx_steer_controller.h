#include "modules/canbus/vehicle/lexus_rx/protocol/speed_b4.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f0.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_2e4.h"

namespace apollo {
    namespace canbus {
        namespace lexus_rx {

            enum SteeringSource {
                STERING_UNKNOWN = 0,
                STEERING_ALTER = 1,
                STEERING_LKA = 2
            };

            const double alter_speed_min = 0.0;
            //const double alter_speed_min = -0.01;
            const double alter_speed_max = 5.55;
            const double lka_speed_min = 4.7;
            //const double alter_speed_max = -0.01;
            //const double lka_speed_min = -0.01;
            const double lka_speed_max = 11.0;

	    class SteeringController {
            public:
                SteeringController();
		/* */
                void set_enable();
                /* */
                void set_disable();
                /* */
                void set_speed( const double speed );

                /* */
                void set_current_steering_angle( const double angle );
                /* */
                void set_goal_steering_angle( const double angle );

                void set_lka_steer( Steering2E4 * steer_2e4 ) { steering_2e4_ = steer_2e4; }
                void set_alter_steer( Steering7F0 * steer_7f0 ) { steering_7f0_ = steer_7f0; }

            private:
                Steering2E4 *steering_2e4_;
                Steering7F0 *steering_7f0_;
                double speed_ = 0.0;
                bool is_enabled_ = false;
                double angle_ = 0;
                SteeringSource source_ = STEERING_LKA;
            };
        }
    }
}
