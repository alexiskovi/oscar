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
DEBUG_MODE = "debug_mode_on"
SOUND_MODE = "sound_on"


class SupervisorPreferences:

    def __init__(self):
        cyber.init()
        self.node = cyber.Node("api_node")
        self.CURRENT_GLOBAL_STATUS = 0
        self.DEBUG_MESSAGE = "no msg recieved"
        self.params = {
            "sound_on" : "",
            "debug_mode" : "",
        }
        self.create_callback_subscriber()
        self.create_decision_subscriber()
        self.create_preferences_publisher()


    def decision_callback(self, decision_data):
        self.CURRENT_GLOBAL_STATUS = decision_data.status
        self.DEBUG_MESSAGE = decision_data.message

    def parameters_callback(self, submodule_data):
        self.params["sound_on"] = submodule_data.sound_on
        self.params["debug_mode"] = submodule_data.debug_mode

    def create_preferences_publisher(self):
        self.preferences_pub = self.node.create_writer("/supervisor/preferences", sv_set_get, 5)

    def create_decision_subscriber(self):
        self.node.create_reader("/supervisor/decision", sv_decision, self.decision_callback)
    
    def create_callback_subscriber(self):
        self.node.create_reader("/supervisor/callback", submodule_parameters, self.parameters_callback)

    def DefineGNSSSoundState(self, state):
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.module_name = GNSS_MODULE
        msg.config_name = SOUND_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def DefineGNSSDebugState(self, state):
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.submodule.module_name = GNSS_MODULE
        msg.submodule.config_name = DEBUG_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def GetGNSSParameters(self):
        msg = sv_set_get()
        msg.cmd = GET_PARAMETERS
        msg.submodule.module_name = GNSS_MODULE
        self.preferences_pub.write(msg)
        time.sleep(0.1)
        return self.params

    def DefineSVSoundState(self, state):
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.submodule.module_name = SUPERVISOR_MODULE
        msg.submodule.config_name = SOUND_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def DefineSVDebugState(self, state):
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.submodule.module_name = SUPERVISOR_MODULE
        msg.submodule.config_name = DEBUG_MODE
        if state:
            msg.submodule.new_value = "true"
        else:
            msg.submodule.new_value = "false"
        self.preferences_pub.write(msg)

    def GetSVParameters(self):
        msg = sv_set_get()
        msg.cmd = GET_PARAMETERS
        msg.submodule.module_name = SUPERVISOR_MODULE
        self.preferences_pub.write(msg)
        time.sleep(0.1)
        return self.params

    def SaveCurrentParameters(self):
        msg = sv_set_get()
        msg.cmd = SAVE_PARAMETERS
        self.preferences_pub.write(msg)

    def __exit__(self):
        cyber.shutdown()