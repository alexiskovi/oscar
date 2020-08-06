#
# LATITUDE CONTROLLER REAL-TIME MONITOR
#

import argparse
import sys
import time
import threading
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

from cyber_py import cyber
from modules.canbus.proto import chassis_pb2
from modules.planning.proto import planning_pb2
from modules.control.proto import control_cmd_pb2

query_size = 1500 #msgs num in query

control = {'steering_target': [], 
'tstamp': [] }

canbus = {'steering': [], 
'torque': [], 
'tstamp': [] }

for keys in control:
    control[keys] = [0.0 for _ in range(query_size)]

for keys in canbus:
    canbus[keys] = [0.0 for _ in range(query_size)]

TICK_control = query_size
TICK_canbus = query_size

def bias(control):
    for keys in control:
        for i in range(len(control[keys]) - 1):
            control[keys][i] = control[keys][i+1]

def callback_control(cmd):
    bias(control)
    global TICK_control
    if TICK_control > 0:
        TICK_control -= 1
    control['steering_target'][-1] = cmd.steering_target
    control['tstamp'][-1] = cmd.header.timestamp_sec

def callback_canbus(cmd):
    bias(canbus)
    global TICK_canbus
    if TICK_canbus > 0:
        TICK_canbus -= 1
    canbus['steering'][-1] = cmd.steering_percentage
    canbus['torque'][-1] = cmd.steering_torque_nm / 100.0
    canbus['tstamp'][-1] = cmd.header.timestamp_sec


cyber.init()
node = cyber.Node("control_analyzer")
controlsub = node.create_reader('/apollo/control',
                                        control_cmd_pb2.ControlCommand,
                                        callback_control)
canbussub = node.create_reader('/apollo/canbus/chassis',
                                       chassis_pb2.Chassis,
                                       callback_canbus)

time_control = [0.0 for _ in range(query_size)]
time_canbus = [0.0 for _ in range(query_size)]

fig, ax = plt.subplots(2, 2)
plt.subplot(2, 2, 1)
steering_real, = plt.plot(time_canbus, canbus['steering'], '-b', label = 'Real steering')
steering_target, = plt.plot(time_control, control['steering_target'], '-r', label = 'Goal steering')
torque, = plt.plot(time_canbus, canbus['torque'], '-y', label = 'Torque')
plt.xlabel('Time, s')
plt.ylabel('Steering, %')
plt.ylim(-100, 100)
plt.xlim(-10, 1)
plt.legend()
plt.subplot(2, 2, 2)
speeds, = plt.plot(time_canbus, canbus['steering'])
plt.xlabel('Time')
plt.ylabel('Speed')
plt.ylim(-2, 7)
plt.xlim(-10, 1)
plt.subplot(2, 2, 3)
cmds, = plt.plot(time_canbus, canbus['steering'])
plt.xlabel('Time')
plt.ylabel('Acceleration')
plt.ylim(-1, 5)
plt.xlim(-10, 1)
plt.subplot(2, 2, 4)
accs, = plt.plot(time_canbus, canbus['steering'])
plt.xlabel('Time')
plt.ylabel('Commands')
plt.ylim(0, 100)
plt.xlim(-10, 1)

def update_plot(num):
    global control, canbus
    time_control = [x - control['tstamp'][-1] if x - control['tstamp'][-1] > -100.0 else 0.0 for x in control['tstamp']]
    time_canbus = [x - canbus['tstamp'][-1] if x - canbus['tstamp'][-1] > -100.0 else 0.0 for x in canbus['tstamp']]
    steering_real.set_data(time_canbus[TICK_canbus:], canbus['steering'][TICK_canbus:])
    steering_target.set_data(time_control[TICK_canbus:], control['steering_target'][TICK_canbus:])
    torque.set_data(time_canbus[TICK_canbus:], canbus['torque'][TICK_canbus:])
    speeds.set_data(time_canbus, canbus['steering'])
    cmds.set_data(time_canbus, canbus['steering'])
    accs.set_data(time_canbus, canbus['steering'])
    return steering_real, steering_target, torque, speeds, cmds, accs

ani = animation.FuncAnimation(fig, update_plot, interval=1, blit=True)
plt.show()