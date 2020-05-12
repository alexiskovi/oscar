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
from math import copysign, pi
from thread import allocate_lock
import atexit

import os, sys, time

from cyber_py import cyber
from cyber_py import cyber_time
from modules.localization.proto import localization_pb2
from modules.planning.proto import planning_pb2
from modules.common.proto import pnc_point_pb2
from modules.common.proto import drive_state_pb2

from cars import Lexus
from trajectories import Trajectory
from fileio import TrajectoryFileHandler

APOLLO_ROOT = os.path.dirname(os.path.abspath(__file__)) + "/../../../.."
DEFAULT_TRAJ_PUB_RATE = 10.0
XY_GOAL_TOLERANCE = 0.2

class TrajectoryPlayer:

    def __init__(self):

        self.car = Lexus()
        self.car_state_mutex = allocate_lock()

        self.params = self.pars_arguments()
        self.trajectory_file_handler = self.create_trajectory_file_handler()
        self.trajectory = self.load_trajectory()
        self.nearest_point_id = 0

        cyber.init()
        self.cyber_node = cyber.Node("simple_trajectory_player")
        self.cyber_trajectory_pub = self.cyber_node.create_writer('/apollo/planning',
                                                                  planning_pb2.ADCTrajectory)

        self.localization_received = False
        self.planning_msg_count = 0
        self.min_along_path_neighbors_dist = 2 * pi * self.car.min_radius * 0.8
        self.estop = False

        atexit.register(self.shutdown)


    def pars_arguments(self):
        parser = ArgumentParser(description="Script load and paly simple trajectories for cars.")
        parser.add_argument('-l', '--loop', action='store_true', dest='loop',
                            help='Loop trajectory for endless movement')
        parser.add_argument('--no-loop', action='store_false', dest='loop')
        parser.set_defaults(loop=False)
        parser.add_argument('-r', '--rate', default=DEFAULT_TRAJ_PUB_RATE, dest='rate',
                            help='Desired trajectory publishing rate in Hz')
        parser.add_argument('-n', '--file_name', help='Name of trajectory file, will be ' + \
                                                      'saved in oscar_tools/data')
        parser.add_argument('--rtk-recorder', required=False, action='store_true', dest='from_rtk_recorder',
                            help='Trajectory file will be loaded from ' + \
                            '/data/log/garage.csv (recorded by rtk_recorder)')

        return parser.parse_args()


    def create_trajectory_file_handler(self):

        if (self.params.from_rtk_recorder):
            trjectory_dir = APOLLO_ROOT + "/data/log"
            if not os.path.exists(trjectory_dir):
                os.makedirs(trjectory_dir)
            trajectory_file = trjectory_dir + "/garage.csv"

        else:
            trjectory_dir = os.path.dirname(os.path.abspath(__file__)) + "/../data/trajectories"
            if not os.path.exists(trjectory_dir):
                os.makedirs(trjectory_dir)

            if (self.params.file_name):
                trajectory_file = trjectory_dir + "/" + self.params.file_name + ".csv"
            else:
                trajectory_file = trjectory_dir + "/trajectory.csv"

        return TrajectoryFileHandler(trajectory_file)


    def load_trajectory(self):

        trajectory = self.trajectory_file_handler.load_trajectory(self.params.from_rtk_recorder)

        if not trajectory:
            sys.exit("Trajectory can\'t be loaded!")

        return trajectory


    def localization_callback(self, msg):

        with self.car_state_mutex:
            self.car.x   = msg.pose.position.x
            self.car.y   = msg.pose.position.y
            self.car.z   = msg.pose.position.z
            self.car.yaw = msg.pose.heading

        self.localization_received = True


    def reset_trajectory_tracking(self):
        self.nearest_point_id = 0


    def find_nearest_point_on_trajectory(self):

        with self.car_state_mutex:
            x   = self.car.x
            y   = self.car.y
            yaw = self.car.yaw

        shortest_dist_sqr = float('inf')
        for i in range(self.nearest_point_id, self.trajectory.points_num):
            dist_sqr = (x - self.trajectory.points[i].x) ** 2 + \
                       (y - self.trajectory.points[i].y) ** 2

            along_path_neighbors_dist = abs(self.trajectory.points[self.nearest_point_id].l - \
                                            self.trajectory.points[i].l)

            if (dist_sqr <= shortest_dist_sqr and
                along_path_neighbors_dist < self.min_along_path_neighbors_dist):
                shortest_dist_sqr = dist_sqr
                nearest_point_id = i

        if shortest_dist_sqr == float('inf'):
            nearest_point_id = self.nearest_point_id

        return nearest_point_id


    def publish_trajectory(self):
        planning_msg = planning_pb2.ADCTrajectory()
        planning_msg.header.timestamp_sec = cyber_time.Time.now().to_sec()
        planning_msg.header.module_name = "planning"
        planning_msg.header.sequence_num = self.planning_msg_count
        self.planning_msg_count += 1

        last_point_id = self.trajectory.points_num - 1
        self.nearest_point_id = self.find_nearest_point_on_trajectory()

        planning_msg.total_path_length = self.trajectory[last_point_id].l - \
                                         self.trajectory[self.nearest_point_id].l
        planning_msg.total_path_time = self.trajectory[last_point_id].t - \
                                       self.trajectory[self.nearest_point_id].t
        planning_msg.gear = 1
        planning_msg.engage_advice.advice = drive_state_pb2.EngageAdvice.READY_TO_ENGAGE
        planning_msg.estop.is_estop = self.estop

        for point in self.trajectory[self.nearest_point_id:last_point_id+1]:
            adc_point = pnc_point_pb2.TrajectoryPoint()
            adc_point.path_point.x = point.x
            adc_point.path_point.y = point.y
            adc_point.path_point.z = point.z
            adc_point.v = point.v
            adc_point.a = point.a
            adc_point.path_point.kappa = point.curvature
            adc_point.path_point.dkappa = 0.0               # TODO! (does it matter for control?)
            adc_point.path_point.theta = point.yaw
            adc_point.path_point.s = point.l

            adc_point.relative_time = point.t - self.trajectory[self.nearest_point_id].t

            # adc_point.relative_time = time_diff  - (now - self.starttime)

            planning_msg.trajectory_point.extend([adc_point])

        self.cyber_trajectory_pub.write(planning_msg)


    def at_end_of_trajectory(self):

        along_path_dist_to_end = abs(self.trajectory.points[self.trajectory.points_num-1].l - \
                                     self.trajectory.points[self.nearest_point_id].l)

        if(along_path_dist_to_end <= XY_GOAL_TOLERANCE):
            return True
        else:
            return False


    def spin(self):

        self.cyber_node.create_reader('/apollo/localization/pose',
                                      localization_pb2.LocalizationEstimate,
                                      self.localization_callback)

        # wait for first localization message
        while not cyber.is_shutdown() and not self.localization_received:
            time.sleep(0.1)

        while not cyber.is_shutdown():

            now = cyber_time.Time.now().to_sec()
            self.publish_trajectory()

            if (self.at_end_of_trajectory()):
                if (self.params.loop):
                    self.reset_trajectory_tracking()
                else:
                    sys.exit(0)

            sleep_time = 1.0 / self.params.rate - (cyber_time.Time.now().to_sec() - now)
            if sleep_time > 0.0:
                time.sleep(sleep_time)


    def shutdown(self):
        cyber.shutdown()
        time.sleep(0.2)


if __name__ == '__main__':

    player = TrajectoryPlayer()
    player.spin()
