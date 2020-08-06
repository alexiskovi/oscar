#!/usr/bin/python

###############################################################################
# Copyright 2020 ScPA StarLine Ltd. All Rights Reserved.
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

import os, sys, time

from cyber_py import cyber
from modules.localization.proto import localization_pb2

from cars import Lexus
from trajectories import CarTrajectoryGenerator
from fileio import TrajectoryFileHandler

APOLLO_ROOT = os.path.dirname(os.path.abspath(__file__)) + "/../../../.."
CTRL_FREQUENCY = 20


def pars_arguments():
    parser = ArgumentParser(description="Script generates simple trajectories for cars.")
    parser.add_argument('-a', '--acceleration', type=float, required=True, dest='acc')
    parser.add_argument('-d', '--deceleration', type=float, required=True, dest='dec')
    parser.add_argument('-v', '--max_velocity', type=float, required=True, dest='max_vel')
    parser.add_argument('-t', '--type', required=True, dest='trajectory_type',
                        help='type of trajectory: 8type, 0type')
    parser.add_argument('-r', '--min_radius', type=float, required=True, dest='min_turn_radius',
                        help='minimal turn radius on trajectory')
    parser.add_argument('-n', '--file-name', required=False, dest='file_name',
                        help='name of trajectory file, will be ' + \
                        'saved in oscar_tools/data')
    parser.add_argument('--rtk-player', required=False, action='store_true', dest='rtk_player_compatible',
                        help='trajectory file will be saved in ' + \
                        '/data/log/ to be used by rtk_layer')

    args = parser.parse_args()

    if args.trajectory_type not in ['8type', '0type']:
        print('Wrong trajectory type!\n')
        parser.print_help()
        sys.exit(1)

    return args


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

    if (params.rtk_player_compatible):
        trjectory_dir = APOLLO_ROOT + "/data/log"
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
            trajectory_file = trjectory_dir + "/trajectory.csv"

    trajectory_file_handler = TrajectoryFileHandler(trajectory_file)

    car = Lexus()
    localization_recived = False

    cyber.init()
    cyber_rt_node = cyber.Node("oscar_tools_trajectory_generator")
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
                                                                         frequency=CTRL_FREQUENCY)
    else:
        trajectory = trajGenerator.generate_0type_traj_using_4_clothoids(car,
                                                                         params.min_turn_radius,
                                                                         params.max_vel,
                                                                         params.acc,
                                                                         params.dec,
                                                                         frequency=CTRL_FREQUENCY)

    while (not localization_recived and not cyber.is_shutdown()):
        time.sleep(0.2)

    cyber.shutdown()

    if not localization_recived:
        sys.exit("Cyber had been shutted down before localization message was recived!\n" + \
                 "Trajectory was not saved!")

    trajectory.transform_to(car.x, car.y, car.yaw, car.z)

    if not trajectory_file_handler.save_trajectory(trajectory, params.rtk_player_compatible):
        sys.exit("Trajectory was not saved!")
