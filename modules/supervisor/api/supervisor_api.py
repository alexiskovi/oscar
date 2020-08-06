from cyber_py3 import cyber
from modules.supervisor.proto.parameter_server_pb2 import sv_set_get
from modules.supervisor.proto.sv_decision_pb2 import sv_decision


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

    def send_config_change(self, cmd, module_name="", config_name="", new_value=""):        
        msg = sv_set_get()
        msg.cmd = cmd
        if not(module_name==""):
            msg.module_name = module_name
            msg.config_name = config_name
            msg.new_value = new_value

        self.preferences_pub.write(msg)
        print(msg.cmd)

    def __exit__(self):
        cyber.shutdown()