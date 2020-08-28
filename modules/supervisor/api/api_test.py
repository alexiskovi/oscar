from supervisor_api import SupervisorPreferences
import time

TEST_DURATION_S = 5

sv_test = SupervisorPreferences()


print("Initializating...")
time.sleep(3.0)

params = sv_test.GetGNSSParameters()
print("GNSS parameters:")
print(params)
params = sv_test.GetIMUParameters()
print("IMU parameters:")
print(params)
time.sleep(2.0)

sv_test.DefineGNSSSoundState(False)

sv_test.SaveCurrentParameters()

print("Terminated")