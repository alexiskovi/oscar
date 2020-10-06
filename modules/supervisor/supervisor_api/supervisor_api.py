import time
import sys
import os, signal
from cyber_py3 import cyber
from modules.supervisor.proto.parameter_server_pb2 import sv_set_get
from modules.supervisor.proto.parameter_server_pb2 import submodule_parameters
from modules.supervisor.proto.sv_decision_pb2 import sv_decision
from modules.supervisor.submodules.proto.sv_gnss_msg_pb2 import sv_gnss_msg
from modules.supervisor.submodules.proto.sv_imu_msg_pb2 import sv_imu_msg

GET_PARAMETERS = "get_parameters"
CHANGE_PARAMETER = "change_parameters"
SAVE_PARAMETERS = "save_parameters"
SUPERVISOR_MODULE = "sv"
GNSS_MODULE = "gnss"
IMU_MODULE = "imu"
DEBUG_MODE = "debug_mode"
SOUND_MODE = "sound_on"
WAIT_FOR_PARAMETER = 0.025
SV_TIMEOUT = 2.5

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
        self._create_gnss_status_subscriber()
        self._create_imu_status_subscriber()


    def _wait_for_callback(self):
        time_exceed = 0.0
        while not(self.parameters_flag) and not cyber.is_shutdown():
            time.sleep(WAIT_FOR_PARAMETER)
            time_exceed += WAIT_FOR_PARAMETER
            if time_exceed >= SV_TIMEOUT:
                print("Connection timeout!")
                break
        return



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

    def _create_gnss_status_subscriber(self):
        self.node.create_reader("/supervisor/gnss/status", sv_gnss_msg, self._update_gnss_msg)

    def _create_imu_status_subscriber(self):
        self.node.create_reader("/supervisor/imu/status", sv_gnss_msg, self._update_imu_msg)

    def _update_gnss_msg(self, gnss_status):
        self.last_gnss_msg = gnss_status

    def _update_imu_msg(self, imu_status):
        self.last_imu_msg = imu_status

    def _gnss_msg_to_dict(self):
        params = {
            "Differential age: ": "",
            "Solution status: ": "",
            "Solution type: ": "",
            "Lateral error: ": "",
            "Longitudinal error: ": "",
            "Overall status: ": "",
            "Debug message: ": ""

        }
        try:
            params["Overall status: "] = self.last_gnss_msg.overall_status
            params["Debug message: "] = self.last_gnss_msg.debug_message
            params["Differential age: "] = "%.4f" % self.last_gnss_msg.differential_age
            params["Solution status: "] = self.last_gnss_msg.sol_status
            params["Solution type: "] = self.last_gnss_msg.sol_type
            params["Lateral error: "] = "%.4f" % self.last_gnss_msg.lateral_error
            params["Longitudinal error: "] = "%.4f" % self.last_gnss_msg.longitudinal_error
        except:
            self._fill_zeros(params)
        return params

    def _imu_msg_to_dict(self):
        params = {
            "Calibration status: ": "",
            "Debug message: ": "",
            "Overall status: ": ""
        }
        try:
            params["Overall status: "] = self.last_imu_msg.overall_status
            params["Calibration status: "] = self.last_imu_msg.calibration_status
            params["Debug message: "] = self.last_imu_msg.debug_message
        except:
            self._fill_zeros(params)
        return params

    def _fill_zeros(self, params):
        for key in params.keys():
            params[key] = "No data yet"



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
        self._wait_for_callback()
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
        self._wait_for_callback()
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
        self._wait_for_callback()
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
        return self._gnss_msg_to_dict()["Overall status: "]

    def get_gnss_status(self):
        params = self._gnss_msg_to_dict()
        return params

    def get_imu_status_word(self):
        return self._imu_msg_to_dict()["Overall status: "]

    def get_imu_status(self):
        params = self._imu_msg_to_dict()
        return params

    def define_canbus_sound_state(self, state):
        # TO DO
        pass

    def define_canbus_debug_state(self, state):
        # TO DO
        pass

    def get_canbus_parameters(self):
        # TO DO
        return self.params

    def get_canbus_status_word(self):
        # TO DO
        return "WARNING"

    def get_canbus_status(self):
        # TO DO
        params = {
            "submodule is not ready": "",
        }
        return params

    def define_control_sound_state(self, state):
        # TO DO
        pass

    def define_control_debug_state(self, state):
        # TO DO
        pass

    def get_control_parameters(self):
        # TO DO
        return self.params

    def get_control_status_word(self):
        # TO DO
        return "WARNING"

    def get_control_status(self):
        # TO DO
        params = {
            "submodule is not ready": "",
        }
        return params

    def define_perception_sound_state(self, state):
        # TO DO
        pass

    def define_perception_debug_state(self, state):
        # TO DO
        pass

    def get_perception_parameters(self):
        # TO DO
        return self.params

    def get_perception_status_word(self):
        # TO DO
        return "WARNING"

    def get_perception_status(self):
        # TO DO
        params = {
            "submodule is not ready": "",
        }
        return params

    def define_localization_sound_state(self, state):
        # TO DO
        pass

    def define_localization_debug_state(self, state):
        # TO DO
        pass

    def get_localization_parameters(self):
        # TO DO
        return self.params

    def get_localization_status_word(self):
        # TO DO
        return "WARNING"

    def get_localization_status(self):
        # TO DO
        params = {
            "submodule is not ready": "",
        }
        return params

    def define_planning_sound_state(self, state):
        # TO DO
        pass

    def define_planning_debug_state(self, state):
        # TO DO
        pass

    def get_planning_parameters(self):
        # TO DO
        return self.params

    def get_planning_status_word(self):
        # TO DO
        return "WARNING"

    def get_planning_status(self):
        # TO DO
        params = {
            "submodule is not ready": "",
        }
        return params

    def stop(self):
        print('STOP')
        os.kill(os.getpid(), signal.SIGTERM)
