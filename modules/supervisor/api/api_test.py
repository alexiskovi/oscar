from supervisor_api import SupervisorPreferences
import time

TEST_DURATION_S = 5

sv_test = SupervisorPreferences()


print("Initializating...")
time.sleep(3.0)

params = sv_test.GetGNSSParameters()
print(params)

sv_test.DefineSVSoundState(False)

sv_test.SaveCurrentParameters()

print("Terminated")