#!/usr/bin/python

###############################################################################
# Copyright 2020 Nikolay Dema. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###############################################################################

from argparse import ArgumentParser
from math import copysign
from datetime import datetime
from shutil import copyfile

import os
import sys
import time

from cyber_py import cyber
from modules.localization.proto import localization_pb2

from cars import Lexus
from trajectories import CarTrajectoryGenerator


def pars_arguments():
    parser = ArgumentParser(description="Script generates simple trajectories for cars.")
    parser.add_argument('-a', '--acceleration', type=float, required=True, dest='acc')
    parser.add_argument('-d', '--deceleration', type=float, required=True, dest='dec')
    parser.add_argument('-v', '--max_velocity', type=float, required=True, dest='max_vel')
    parser.add_argument('-t', '--type', required=True, dest='trajectory_type',
                        help='type of trajectory: 8type, 0type')
    parser.add_argument('-r', '--min_radius', type=float, required=True, dest='min_turn_radius',
                        help='minimal turn radius on trajectory')
    parser.add_argument('-n', '--file_name', required=False,
                        help='name of trajectory file, will be ' + \
                        'saved in oscar_tools/data')
    parser.add_argument('--rtk_player', required=False, action='store_true', dest='for_rtk_player',
                        help='trajectory file will be saved in ' + \
                        '/data/log/ to be used by rtk_layer')

    args = parser.parse_args()

    if args.trajectory_type not in ['8type', '0type']:
        print('Wrong trajectory type!\n')
        parser.print_help()
        sys.exit(1)

    return args


class FileHandler():

    def __init__(self, file_path=None):
        self.handler = None
        if (file_path):
            self.open(file_path)


    def open(self, file_path):

        if (os.path.isfile(file_path)):
            mtime_since_epoc = os.path.getmtime(file_path)
            file_last_mdata = time.strftime('%Y-%m-%d-%H:%M:%S', time.localtime(mtime_since_epoc))
            new_name_for_prev_file = file_path[:-4] + "-" + file_last_mdata + ".csv"
            copyfile(file_path, new_name_for_prev_file)

        try:
            self.handler = open(file_path, 'w')
        except IOError:
            print("Can't open file " + file_path)
            sys.exit(1)

    def write(self, data):
        if (self.handler):
            self.handler.write(data)
            # self.handler.flush()


    def __del__(self):
        if (self.handler):
            self.handler.flush()
            self.handler.close()


def localization_callback(msg):

    global localization_recived, car

    if localization_recived:
        return

    car.x   = msg.pose.position.x
    car.y   = msg.pose.position.y
    car.z   = msg.pose.position.z
    car.yaw = msg.pose.heading

    localization_recived = True


if __name__ == '__main__':

    global localization_recived, car

    params = pars_arguments()

    if (params.for_rtk_player):
        trjectory_dir = os.path.dirname(os.path.abspath(__file__)) + "/../../../../data/log"
        if not os.path.exists(trjectory_dir):
            os.makedirs(trjectory_dir)
        trajectory_file = trjectory_dir + "/garage.csv"

    else:
        trjectory_dir = os.path.dirname(os.path.abspath(__file__)) + "/../data/trajectories"
        if not os.path.exists(trjectory_dir):
            os.makedirs(trjectory_dir)

        if (params.file_name):
            trajectory_file = trjectory_dir + "/" + params.file_name + ".csv"
        else:
            trajectory_file = trjectory_dir + "/" + params.trajectory_type + ".csv"

    trajectory_file_handler = FileHandler(trajectory_file)

    car = Lexus()
    localization_recived = False

    cyber.init()
    cyber_rt_node = cyber.Node("simple_trajectory_generator")
    cyber_rt_node.create_reader('/apollo/localization/pose',
                          localization_pb2.LocalizationEstimate,
                          localization_callback)

    trajGenerator = CarTrajectoryGenerator()

    if (params.trajectory_type == "8type"):
        trajectory = trajGenerator.generate_8type_traj_using_4_clothoids(car,
                                                                     params.min_turn_radius,
                                                                     params.max_vel,
                                                                     params.acc,
                                                                     params.dec,
                                                                     frequency=20)
    else:
        trajectory = trajGenerator.generate_0type_traj_using_4_clothoids(car,
                                                                     params.min_turn_radius,
                                                                     params.max_vel,
                                                                     params.acc,
                                                                     params.dec,
                                                                     frequency=20)

    while (not localization_recived and not cyber.is_shutdown()):
        time.sleep(0.2)

    cyber.shutdown()
    del cyber_rt_node

    trajectory.transform_to(car.x, car.y, car.yaw, car.z)

    # cube_town
    # trajectory.transform_to(592697.488346100, 4134465.908913612, 0.0, -0.000195292)

    trajectory_file_handler.write("x,y,z,speed,acceleration,curvature,curvature_change_rate," + \
                                  "time,theta,gear,s,throttle,brake,steering\n")
    for point in trajectory.points:

        gear = int(copysign(1,point.v))

        steering = int(point.steering/car.max_steering*100)

        brake = int(0)
        throttle = int(0)
        if (point.v * point.a >= 0):
            throttle = int(abs(point.a) / car.max_acceleration*100)
        else:
            brake = int(abs(point.a) / car.max_deceleration*100)

        trajectory_file_handler.write(str(round(point.x, 2)) + ",\t" + \
                                      str(round(point.y, 2)) + ",\t" + \
                                      str(round(point.z, 2)) + ",\t" + \
                                      str(round(point.v, 2)) + ",\t" + \
                                      str(round(point.a, 2)) + ",\t" + \
                                      str(round(point.curvature, 3)) + ",\t" + \
                                      str(round(point.dcurvature,  3)) + ",\t" + \
                                      str(round(point.t, 2)) + ",\t" + \
                                      str(round(point.yaw, 2)) + ",\t" + \
                                      str(gear) + ",\t" + \
                                      str(round(point.l,   2)) + ",\t" + \
                                      str(throttle) + ",\t" + \
                                      str(brake) + ",\t" + \
                                      str(steering) + "\n")
