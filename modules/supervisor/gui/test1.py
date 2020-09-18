
import sys

from supervisor_api import SupervisorPreferences

supervisor = SupervisorPreferences()
print(supervisor.get_gnss_status())
