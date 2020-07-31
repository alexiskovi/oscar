from supervisor_api import SupervisorPreferences
import time

TEST_DURATION_S = 15

sv_test = SupervisorPreferences()

sv_test.create_preferences_publisher()
sv_test.create_decision_subscriber()

for _ in range(TEST_DURATION_S):
    time.sleep(1.0)
    print('=' * 100)
    print("Status: ", sv_test.CURRENT_GLOBAL_STATUS)    
    print("Message: ", sv_test.DEBUG_MESSAGE)
    print('=' * 100)
    print("Sending config changing...")
    sv_test.send_config_change("control", "sound", 0)
    print('=' * 100)

print("Terminated")