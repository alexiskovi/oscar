#
# LONGITUDE CONTROLLER REAL-TIME MONITOR
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

control = {'speed_target': [],
'acc_target': [],
'acc': [],
'station_error': [],
'speed_error': [],
'acc_error': [],
'throttle_cmd': [],
'brake_cmd': [],
'tstamp': [] }

canbus = {'speed': [], 
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
    control['speed_target'][-1] = cmd.debug.simple_lon_debug.speed_reference
    control['acc_target'][-1] = cmd.acceleration
    control['acc'][-1] = cmd.debug.simple_lon_debug.current_acceleration
    control['station_error'][-1] = cmd.debug.simple_lon_debug.station_error
    control['speed_error'][-1] = cmd.debug.simple_lon_debug.speed_error
    control['acc_error'][-1] = cmd.debug.simple_lon_debug.acceleration_error
    control['throttle_cmd'][-1] = cmd.throttle
    control['brake_cmd'][-1] = cmd.brake
    control['tstamp'][-1] = cmd.header.timestamp_sec

def callback_canbus(cmd):
    bias(canbus)
    global TICK_canbus
    if TICK_canbus > 0:
        TICK_canbus -= 1
    canbus['speed'][-1] = cmd.speed_mps
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
station_error, = plt.plot(time_control, control['station_error'], '-b', label = 'Station error')
speed_error, = plt.plot(time_control, control['speed_error'], '-r', label = 'Speed error')
acc_error, = plt.plot(time_control, control['acc_error'], '-y', label = 'Acceleration error')
plt.xlabel('Time, s')
plt.ylabel('Errors')
plt.ylim(-5, 5)
plt.xlim(-10, 1)
plt.legend()

plt.subplot(2, 2, 2)
real_speed, = plt.plot(time_canbus, canbus['speed'], '-b', label = 'Real speed')
goal_speed, = plt.plot(time_control, control['speed_target'], '-r', label = 'Goal speed')
plt.xlabel('Time, s')
plt.ylabel('Speed, m/s')
plt.ylim(-2, 7)
plt.xlim(-10, 1)
plt.legend()

plt.subplot(2, 2, 3)
real_acc, = plt.plot(time_control, control['acc'], '-b', label = 'Real acceleration')
goal_acc, = plt.plot(time_control, control['acc_target'], '-r', label = 'Goal acceleration')
plt.xlabel('Time, s')
plt.ylabel('Acceleration, m/s^2')
plt.ylim(-1, 5)
plt.xlim(-10, 1)
plt.legend()

plt.subplot(2, 2, 4)
throttle_cmd, = plt.plot(time_control, control['throttle_cmd'], '-b', label = 'Throttle cmd')
brake_cmd, = plt.plot(time_control, control['brake_cmd'], '-r', label = 'Brake cmd')
plt.xlabel('Time, s')
plt.ylabel('Commands, %')
plt.ylim(0, 100)
plt.xlim(-10, 1)
plt.legend()

def update_plot(num):

    global control, canbus

    time_control = [x - control['tstamp'][-1] if x - control['tstamp'][-1] > -100.0 else 0.0 for x in control['tstamp']]
    time_canbus = [x - canbus['tstamp'][-1] if x - canbus['tstamp'][-1] > -100.0 else 0.0 for x in canbus['tstamp']]

    station_error.set_data(time_control[TICK_control:], control['station_error'][TICK_control:])
    speed_error.set_data(time_control[TICK_control:], control['speed_error'][TICK_control:])
    acc_error.set_data(time_control[TICK_control:], control['acc_error'][TICK_control:])
    real_speed.set_data(time_canbus[TICK_canbus:], canbus['speed'][TICK_canbus:])
    goal_speed.set_data(time_control[TICK_control:], control['speed_target'][TICK_control:])
    real_acc.set_data(time_control[TICK_control:], control['acc'][TICK_control:])
    goal_acc.set_data(time_control[TICK_control:], control['acc_target'][TICK_control:])
    throttle_cmd.set_data(time_control[TICK_control:], control['throttle_cmd'][TICK_control:])
    brake_cmd.set_data(time_control[TICK_control:], control['brake_cmd'][TICK_control:])
    return station_error, speed_error, acc_error, real_speed, goal_speed, real_acc, goal_acc, throttle_cmd, brake_cmd

ani = animation.FuncAnimation(fig, update_plot, interval=1, blit=True)
plt.show()