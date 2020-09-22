from supervisor_api import SupervisorPreferences
import time

TEST_DURATION_S = 5

sv_test = SupervisorPreferences()


print("Initializating...")
time.sleep(3.0)

params = sv_test.get_gnss_parameters()
print("GNSS parameters:")
print(params)
params = sv_test.get_imu_parameters()
print("IMU parameters:")
print(params)
time.sleep(2.0)

print(sv_test.get_gnss_status())
sv_test.define_gnss_sound_state(False)
sv_test.save_current_parameters()

print("Terminated")