from cyber_py3 import cyber
from modules.supervisor.proto.parameter_server_pb2 import sv_set_get
from modules.supervisor.proto.sv_decision_pb2 import sv_decision

CHANGE_PARAMETER = "change_parameters"
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

    def decision_callback(self, decision_data):
        self.CURRENT_GLOBAL_STATUS = decision_data.status
        self.DEBUG_MESSAGE = decision_data.message

    def create_preferences_publisher(self):
        self.preferences_pub = self.node.create_writer("/supervisor/preferences", sv_set_get, 5)

    def create_decision_subscriber(self):
        self.node.create_reader("/supervisor/decision", sv_decision, self.decision_callback)

    def DefineGNSSSoundState(self, state):
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.module_name = GNSS_MODULE
        msg.config_name = SOUND_MODE
        msg.new_value = int(state)
        self.preferences_pub.write(msg)

    def DefineGNSSDebugState(self, state):
        msg = sv_set_get()
        msg.cmd = CHANGE_PARAMETER
        msg.module_name = GNSS_MODULE
        msg.config_name = DEBUG_MODE
        msg.new_value = int(state)
        self.preferences_pub.write(msg)

    def __exit__(self):
        cyber.shutdown()