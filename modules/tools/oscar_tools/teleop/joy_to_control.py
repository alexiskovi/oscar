# Released by rdb under the Unlicense (unlicense.org)
# Based on information from:
# https://www.kernel.org/doc/Documentation/input/joystick-api.txt

import os, struct, array
from fcntl import ioctl
import threading
import time
import sys
import signal
from cyber_py3 import cyber
from cyber_py3 import cyber_time
from modules.canbus.proto import chassis_pb2
from modules.control.proto import control_cmd_pb2

CONTROL_FRAMERATIO = 75.0
MAX_STEERING_PERCENTAGE = 65.0
MAX_THROTTLE = 90.0
MAX_BRAKE = 80.0
STEERING_MAX_RATIO = 1.0

class Apollo_Teleop:


    def __init__(self):

        self.params = {
            'throttle': 0.0,
            'brake': 0.0,
            'steering_eps': 0.0,
            'left_turn_signal': False,
            'right_turn_signal': False,
            'low_beam': False,
            'high_beam': False,
            'horn': False,
            'emergency_light': False,
            'emergency_stop': False,
            'gear': 0,
            'exit': False
        }

        self.prepare_joy()
        cyber.init()
        node = cyber.Node("teleop_gamepad")
        canbussub = node.create_reader('/apollo/canbus/chassis',
                                       chassis_pb2.Chassis,
                                       self.callback_canbus)
        self.controlpub = node.create_writer('/apollo/control',
                                        control_cmd_pb2.ControlCommand)
        #Creating read and publish threads
        self.ctl_pub_thread = threading.Thread(target = self.publish_control)
        self.joy_read_thread = threading.Thread(target = self.joy_read)
        self.ctl_pub_thread.start()
        self.joy_read_thread.start()

    def prepare_joy(self):
        global jsdev, button_map, axis_map
        global button_states, axis_states
        global last_axis_states, last_button_states

        # Iterate over the joystick devices.
        print('Available devices:')

        for fn in os.listdir('/dev/input'):
            if fn.startswith('js'):
                print('  /dev/input/%s' % (fn))
        
        self.axis_states = {}
        self.button_states = {}

        # These constants were borrowed from linux/input.h
        axis_names = {
            0x00 : 'x',
            0x01 : 'y',
            0x02 : 'z',
            0x03 : 'rx',
            0x04 : 'ry',
            0x05 : 'rz',
            0x06 : 'throttle',
            0x07 : 'rudder',
            0x08 : 'wheel',
            0x09 : 'gas',
            0x0a : 'brake',
            0x10 : 'hat0x',
            0x11 : 'hat0y',
            0x12 : 'hat1x',
            0x13 : 'hat1y',
            0x14 : 'hat2x',
            0x15 : 'hat2y',
            0x16 : 'hat3x',
            0x17 : 'hat3y',
            0x18 : 'pressure',
            0x19 : 'distance',
            0x1a : 'tilt_x',
            0x1b : 'tilt_y',
            0x1c : 'tool_width',
            0x20 : 'volume',
            0x28 : 'misc',
        }

        button_names = {
            0x120 : 'trigger',
            0x121 : 'thumb',
            0x122 : 'thumb2',
            0x123 : 'top',
            0x124 : 'top2',
            0x125 : 'pinkie',
            0x126 : 'base',
            0x127 : 'base2',
            0x128 : 'base3',
            0x129 : 'base4',
            0x12a : 'base5',
            0x12b : 'base6',
            0x12f : 'dead',
            0x130 : 'a',
            0x131 : 'b',
            0x132 : 'c',
            0x133 : 'x',
            0x134 : 'y',
            0x135 : 'z',
            0x136 : 'tl',
            0x137 : 'tr',
            0x138 : 'tl2',
            0x139 : 'tr2',
            0x13a : 'select',
            0x13b : 'start',
            0x13c : 'mode',
            0x13d : 'thumbl',
            0x13e : 'thumbr',

            0x220 : 'dpad_up',
            0x221 : 'dpad_down',
            0x222 : 'dpad_left',
            0x223 : 'dpad_right',

            # XBox 360 controller uses these codes.
            0x2c0 : 'dpad_left',
            0x2c1 : 'dpad_right',
            0x2c2 : 'dpad_up',
            0x2c3 : 'dpad_down',
        }

        self.axis_map = []
        self.button_map = []

        # Open the joystick device.
        fn = '/dev/input/js0'
        print('Opening %s...' % fn)
        self.jsdev = open(fn, 'rb')

        # Get the device name.
        #buf = bytearray(63)
        buf = array.array('B', [0] * 64)
        ioctl(self.jsdev, 0x80006a13 + (0x10000 * len(buf)), buf) # JSIOCGNAME(len)
        js_name = buf.tobytes().rstrip(b'\x00').decode('utf-8')
        print('Device name: %s' % js_name)

        # Get number of axes and buttons.
        buf = array.array('B', [0])
        ioctl(self.jsdev, 0x80016a11, buf) # JSIOCGAXES
        num_axes = buf[0]

        buf = array.array('B', [0])
        ioctl(self.jsdev, 0x80016a12, buf) # JSIOCGBUTTONS
        num_buttons = buf[0]

        # Get the axis map.
        buf = array.array('B', [0] * 0x40)
        ioctl(self.jsdev, 0x80406a32, buf) # JSIOCGAXMAP

        for axis in buf[:num_axes]:
            axis_name = axis_names.get(axis, 'unknown(0x%02x)' % axis)
            self.axis_map.append(axis_name)
            self.axis_states[axis_name] = 0.0

        # Get the button map.
        buf = array.array('H', [0] * 200)
        ioctl(self.jsdev, 0x80406a34, buf) # JSIOCGBTNMAP

        for btn in buf[:num_buttons]:
            btn_name = button_names.get(btn, 'unknown(0x%03x)' % btn)
            self.button_map.append(btn_name)
            self.button_states[btn_name] = 0

        print('%d axes found: %s' % (num_axes, ', '.join(self.axis_map)))
        print('%d buttons found: %s' % (num_buttons, ', '.join(self.button_map)))

        self.last_axis_states = self.axis_states.copy()
        self.last_button_states = self.button_states.copy()

    def callback_canbus(self, msg):
        self.current_gear = msg.gear_location
        self.current_speed = msg.speed_mps

    def publish_control(self):

        left_turn_state = False
        right_turn_state = False
        low_beam_state = False
        high_beam_state = False

        while True:

            controlcmd = control_cmd_pb2.ControlCommand()
            #Emergency stop
            if self.params['emergency_stop']:
                controlcmd.throttle = 0.0
                controlcmd.brake = 50.0
                controlcmd.signal.emergency_light = True
                if self.params['horn']:
                    controlcmd.signal.horn = True
                controlcmd.header.timestamp_sec = cyber_time.Time.now().to_sec()
                self.controlpub.write(controlcmd)

            else:
                if self.params['left_turn_signal']:
                    controlcmd.signal.turn_signal = 1
                if self.params['right_turn_signal']:
                    controlcmd.signal.turn_signal = 2
                if self.params['low_beam']:
                    controlcmd.signal.low_beam = True
                if self.params['high_beam']:
                    controlcmd.signal.high_beam = True
                if self.params['emergency_light']:
                    controlcmd.signal.emergency_light = True
                if self.params['horn']:
                    controlcmd.signal.horn = True
                
                controlcmd.gear_location = self.params['gear']
                controlcmd.throttle = self.params['throttle']
                controlcmd.brake = self.params['brake']
                if abs(self.params['steering_eps'] - self.axis_states['x'] * STEERING_MAX_RATIO) < MAX_STEERING_PERCENTAGE:
                    self.params['steering_eps'] -= self.axis_states['x'] * STEERING_MAX_RATIO 
                controlcmd.steering_target = self.params['steering_eps']

                #Teleop exit condition
                if self.params['exit']:
                    controlcmd = control_cmd_pb2.ControlCommand()
                    controlcmd.throttle = 0.0
                    controlcmd.brake = 0.0
                    controlcmd.header.timestamp_sec = cyber_time.Time.now().to_sec()
                    self.controlpub.write(controlcmd)
                    os.kill(os.getpid(), signal.SIGTERM)


            controlcmd.header.timestamp_sec = cyber_time.Time.now().to_sec()
            self.controlpub.write(controlcmd)

            time.sleep(1/CONTROL_FRAMERATIO)

    def joy_read(self):

        while True:
            evbuf = self.jsdev.read(8)

            if evbuf:
                timestamp, value, type, number = struct.unpack('IhBB', evbuf)

                #if type & 0x80:
                #    print("(initial)", end="")

                if type & 0x01:
                    button = self.button_map[number]
                    if button:
                        self.button_states[button] = value
                        if value:
                            if button == 'tr':
                                #if self.current_speed < 0.1:
                                # R to N
                                if self.params['gear'] == 2:
                                    self.params['gear'] = 0
                                # N to D
                                elif self.params['gear'] == 0:
                                    self.params['gear'] = 1
                            if button == 'tl':
                                #if self.current_speed < 0.1:
                                # D to N
                                if self.params['gear'] == 1:
                                    self.params['gear'] = 0
                                # N to R
                                elif self.params['gear'] == 0:
                                    self.params['gear'] = 2

                            if button == 'b':
                                self.params['emergency_stop'] = not(self.params['emergency_stop'])
                            if button == 'y':
                                self.params['emergency_light'] = not(self.params['emergency_light'])
                            if button == 'lthumb':
                                self.params['horn'] = True
                            if button == 'start':
                                self.params['exit'] = True
                        #    print("%s pressed" % (button))
                        else:
                            if button == 'lthumb':
                                self.params['horn'] = False
                        #    print("%s released" % (button))

                if type & 0x02:
                    axis = self.axis_map[number]
                    if axis:
                        fvalue = value / 32767.0
                        self.axis_states[axis] = fvalue
                        if axis == 'hat0x':
                            if fvalue == -1.0:
                                self.params['left_turn_signal'] = not(self.params['left_turn_signal'])
                                self.params['right_turn_signal'] = False
                            if fvalue == 1.0:
                                self.params['right_turn_signal'] = not(self.params['right_turn_signal'])
                                self.params['left_turn_signal'] = False
                        if axis == 'hat0y':
                            if fvalue == -1.0:
                                self.params['high_beam'] = not(self.params['high_beam'])
                                self.params['low_beam'] = False
                            if fvalue == 1.0:
                                self.params['low_beam'] = not(self.params['low_beam'])
                                self.params['high_beam'] = False
                        if axis == 'z':
                            self.params['brake'] = (fvalue + 1.0) / 2.0 * MAX_BRAKE
                        if axis == 'rz':
                            self.params['throttle'] = (fvalue + 1.0) / 2.0 * MAX_THROTTLE
                        #print("%s: %.3f" % (axis, fvalue))


def main():
    Apollo_Teleop()


if __name__ == "__main__":
    main()
