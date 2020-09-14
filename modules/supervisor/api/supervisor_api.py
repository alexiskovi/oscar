import time

from cyber_py3 import cyber
from modules.supervisor.proto.parameter_server_pb2 import sv_set_get
from modules.supervisor.proto.parameter_server_pb2 import submodule_parameters
from modules.supervisor.proto.sv_decision_pb2 import sv_decision

GET_PARAMETERS = "get_parameters"
CHANGE_PARAMETER = "change_parameters"
SAVE_PARAMETERS = "save_parameters"
SUPERVISOR_MODULE = "sv"
GNSS_MODULE = "gnss"
IMU_MODULE = "imu"
DEBUG_MODE = "debug_mode_on"
SOUND_MODE = "sound_on"
WAIT_FOR_PARAMETER = 0.025

class SupervisorPreferences:

    def __init__(self):
        #
        # Creating Cyber RT Node, publishers and subscribers
        #
        cyber.init()
        self.node = cyber.Node("api_node")
        self.CURRENT_GLOBAL_STATUS = 0
        self.DEBUG_MESSAGE = "no msg recieved"
        self.params = {
            "sound_on" : False,
            "debug_mode" : False,
        }
        self.parameters_flag = False
        self._create_callback_subscriber()
        self._create_decision_subscriber()
        self._create_preferences_publisher()


    def _decision_callback(self, decision_data):
        # Callback function for supervisor global decision
        self.CURRENT_GLOBAL_STATUS = decision_data.status
        self.DEBUG_MESSAGE = decision_data.message

    def _parameters_callback(self, submodule_data):
        # Callback function for getting current parameters
        self.parameters_flag = True
        self.params["sound_on"] = submodule_data.sound_on
        self.params["debug_mode"] = submodule_data.debug_mode

    def _create_preferences_publisher(self):
        # Publisher for preferences set and get interface
        self.preferences_pub = self.node.create_writer("/supervisor/preferences", sv_set_get, 5)

    def _create_decision_subscriber(self):
        # Subscriber for global supervisor decision channel
        self.node.create_reader("/supervisor/decision", sv_decision, self._decision_callback)
    
    def _create_callback_subscriber(self):
        # Subscriber for supervisor module callbacks
        self.node.create_reader("/supervisor/callback", submodule_parameters, self._parameters_callback)

    def define_gnss_sound_state(self, state):
        # Sending new state setting for sound in GNSS module
        # incoming: bool state
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.submodule.module_name = GNSS_MODULE
        msg.submodule.config_name = SOUND_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def define_gnss_debug_state(self, state):
        # Sending new state setting for debug in GNSS module
        # incoming: bool state
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.submodule.module_name = GNSS_MODULE
        msg.submodule.config_name = DEBUG_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def get_gnss_parameters(self):
        # Sending request to get current GNSS parameters
        msg = sv_set_get()
        msg.cmd = GET_PARAMETERS
        msg.submodule.module_name = GNSS_MODULE
        self.preferences_pub.write(msg)
        while not(self.parameters_flag):
            time.sleep(WAIT_FOR_PARAMETER)
        self.parameters_flag = False
        return self.params

    def define_sv_sound_state(self, state):
        # Sending new state setting for sound in Supervisor module
        # incoming: bool state
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.submodule.module_name = SUPERVISOR_MODULE
        msg.submodule.config_name = SOUND_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def define_sv_debug_state(self, state):
        # Sending new state setting for debug in Supervisor module
        # incoming: bool state
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.submodule.module_name = SUPERVISOR_MODULE
        msg.submodule.config_name = DEBUG_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def get_sv_parameters(self):
        # Sending request to get current Supervisor parameters
        msg = sv_set_get()
        msg.cmd = GET_PARAMETERS
        msg.submodule.module_name = SUPERVISOR_MODULE
        self.preferences_pub.write(msg)
        while not(self.parameters_flag):
            time.sleep(WAIT_FOR_PARAMETER)
        self.parameters_flag = False
        return self.params

    def define_imu_sound_state(self, state):
        # Sending new state setting for sound in IMU module
        # incoming: bool state
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.submodule.module_name = IMU_MODULE
        msg.submodule.config_name = SOUND_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def define_imu_debug_state(self, state):
        # Sending new state setting for debug in IMU module
        # incoming: bool state
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.submodule.module_name = IMU_MODULE
        msg.submodule.config_name = DEBUG_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def get_imu_parameters(self):
        # Sending request to get current IMU parameters
        msg = sv_set_get()
        msg.cmd = GET_PARAMETERS
        msg.submodule.module_name = IMU_MODULE
        self.preferences_pub.write(msg)
        while not(self.parameters_flag):
            time.sleep(WAIT_FOR_PARAMETER)
        self.parameters_flag = False
        return self.params

    def save_current_parameters(self):
        # Sending request to save current parameters (rewrites file preferences.yaml)
        msg = sv_set_get()
        msg.cmd = SAVE_PARAMETERS
        self.preferences_pub.write(msg)

    #
    # Crutches for now
    #

    def get_gnss_status_word(self):
        # TO DO
        return "OK"
    
    def get_gnss_status(self):
        # TO DO
        params = {
            "a": 1,
            "b": 2,
            "c": 3,
            "d": 4,
            "e": 5,
            "f": 6,
        }
        return params

    def __exit__(self):
        cyber.shutdown()