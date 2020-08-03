from supervisor_api import SupervisorPreferences
import time

TEST_DURATION_S = 5

sv_test = SupervisorPreferences()

sv_test.create_preferences_publisher()
sv_test.create_decision_subscriber()

print("Sending get_parameters...")
time.sleep(3.0)
sv_test.send_config_change("get_parameters")
time.sleep(3.0)

for _ in range(TEST_DURATION_S):
    time.sleep(1.0)
    print('=' * 100)
    print("Status: ", sv_test.CURRENT_GLOBAL_STATUS)    
    print("Message: ", sv_test.DEBUG_MESSAGE)
    print('=' * 100)
    print("Sending config changing...")
    sv_test.send_config_change("change_parameters", "control", "sound", 0)
    print('=' * 100)

print("Terminated")