import time
import argparse
import sys
from cyber_py3 import cyber
from modules.supervisor.proto.general_msg_pb2 import SV_info

TIMEOUT_CONST_TIME = 0.5
STD_DELAY = 0.2
timer_set = True

parser = argparse.ArgumentParser(description='Specify message content')
parser.add_argument('--f', type=float, default=0, help='FATAL after F seconds')
parser.add_argument('--t', type=float, default=0, help='TIMEOUT after T seconds')
args = parser.parse_args()

if args.f == 0 and args.t > 0:
    timer_s = args.t
    timer_kind = 'TIMEOUT'
elif args.t == 0 and args.f > 0:
    timer_s = args.f
    timer_kind = 'FATAL'
else:
    if args.t == 0 and args.f == 0:
        timer_s = 0
        timer_kind = 'OK'
        timer_set = False
        print("Starting infinite OK message loop")
    else:
        print("Only one timer can be non-zero value!")
        sys.exit()

cyber.init("Dump_IPC_Supervisor")
dump_sv_node = cyber.Node("dump_sv_talker")
writer = dump_sv_node.create_writer("supervisor/general", SV_info, 5)

begin_time = time.time()

while not(cyber.is_shutdown()):

    current_time = time.time()

    msg = SV_info()
    msg.header.timestamp_sec = current_time

    if current_time - begin_time < timer_s or not(timer_set):
        msg.status = 1
        msg.message = "It's okay, chill"
        time.sleep(STD_DELAY)
    else:
        if timer_kind == 'FATAL':
            msg.status = 4
            msg.message = 'Oops, something gone wrong!'
            time.sleep(STD_DELAY)
        else:
            msg.status = 1
            msg.message = "It's okay but actually not"
            time.sleep(STD_DELAY + TIMEOUT_CONST_TIME)
    
    writer.write(msg)

cyber.shutdown()