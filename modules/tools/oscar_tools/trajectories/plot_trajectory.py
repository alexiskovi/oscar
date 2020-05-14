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
from math import copysign, pi, isnan, sqrt
from thread import allocate_lock

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

import os, sys, time

from cyber_py import cyber
from cyber_py import cyber_time
from modules.localization.proto import localization_pb2
from modules.planning.proto import planning_pb2
from modules.canbus.proto import chassis_pb2

from cars import Lexus
from trajectories import Trajectory, Point
from fileio import TrajectoryFileHandler
from plots import Plotter

APOLLO_ROOT = os.path.dirname(os.path.abspath(__file__)) + "/../../../.."
DEFAULT_PLOT_RATE = 20.0
AX_TRAJECTORY_INDENT = 5.0
CAR_PATH_DL = 0.5
MAX_CAR_PATH_POINTS_NUM = 500

class TrajectoryPlotter:

    def __init__(self):

        self.car = Lexus()
        self.car_state_mutex = allocate_lock()

        self.params = self.pars_arguments()
        self.trajectory_file_handler = self.create_trajectory_file_handler()
        self.trajectory = self.load_trajectory()
        self.target_point_id = 0

        cyber.init()
        self.cyber_node = cyber.Node("oscar_tools_trajectory_plotter")

        self.xlimits, self.ylimits = self.find_plot_limits()
        self.plotter = Plotter(self.xlimits, self.ylimits)
        self.last_plot_update_time = cyber_time.Time.now().to_sec()

        self.target_trajectory_plot = self.plotter.create_trajectory_plot(color="#B3506C")

        self.car_trajectory_mutex = allocate_lock()
        self.car_trajectory = Trajectory()
        self.car_trajectory_plot = self.plotter.create_trajectory_plot(color="#3346c4")

        self.car_frame_plot = self.plotter.create_frame_plot(x_length=0.8, y_length=0.5)
        self.car_plot = self.plotter.create_car_plot(self.car, color="#3346c4")
        self.additional_car_geometry_plot = self.plotter.create_additional_car_geometry_plot(self.car)

        self.target_car_plot = self.plotter.create_car_plot(self.car, color="#6cc2cc", lw=1)
        self.target_car_frame_plot = self.plotter.create_frame_plot(x_length=0.8,
                                                                    y_length=0.5,
                                                                    x_color="#c25f73",
                                                                    y_color="#4ed48d",
                                                                    z_color="#5c97f7")


    def pars_arguments(self):
        parser = ArgumentParser(description="Script plots current trajectory, trarget and car state.")
        parser.add_argument('-r', '--rate', type=float, default=DEFAULT_PLOT_RATE, dest='rate',
                            help='Desired plotting update rate in Hz')
        parser.add_argument('--car-path', action='store_true', dest='with_car_path',
                            help='Plots traveled car path')
        parser.add_argument('--traj-frames', action='store_true', dest='with_trajectory_frames',
                            help='Plots frames on target trajectory')
        parser.add_argument('--traj-car-footprints', action='store_true', dest='with_car_footprints',
                            help='Plots car footprints on target trajectory')
        parser.add_argument('-n', '--file-name', dest='file_name', required=False,
                            help='Name of trajectory file, will be ' + \
                            'loaded from oscar_tools/data/trajectories')

        return parser.parse_args()


    def create_trajectory_file_handler(self):

        trjectory_dir = os.path.dirname(os.path.abspath(__file__)) + "/../data/trajectories"
        if not os.path.exists(trjectory_dir):
            os.makedirs(trjectory_dir)

        if (self.params.file_name):
            trajectory_file = trjectory_dir + "/" + self.params.file_name + ".csv"
        else:
            trajectory_file = trjectory_dir + "/trajectory.csv"

        return TrajectoryFileHandler(trajectory_file)


    def load_trajectory(self):

        trajectory = self.trajectory_file_handler.load_trajectory()

        if not trajectory:
            sys.exit("Trajectory can\'t be loaded!")

        return trajectory


    def find_plot_limits(self):

        x_lowest  = self.trajectory[0].x
        x_highest = self.trajectory[0].x
        y_lowest  = self.trajectory[0].y
        y_highest = self.trajectory[0].y

        for point in self.trajectory.points:
            if (point.x < x_lowest):  x_lowest  = point.x
            if (point.x > x_highest): x_highest = point.x
            if (point.y < y_lowest):  y_lowest  = point.y
            if (point.y > y_highest): y_highest = point.y

        x_length = x_highest - x_lowest
        y_length = y_highest - y_lowest

        x = x_highest - x_length / 2.0
        y = y_highest - y_length / 2.0

        if (x_length > y_length):
            y_mult = x_length / y_length
            y_highest = y + y_length / 2.0 * y_mult
            y_lowest  = y - y_length / 2.0 * y_mult
        else:
            x_mult = y_length / x_length
            x_highest = x + x_length / 2.0 * x_mult
            x_lowest  = x - x_length / 2.0 * x_mult

        return ([x_lowest - AX_TRAJECTORY_INDENT, x_highest + AX_TRAJECTORY_INDENT],
                [y_lowest - AX_TRAJECTORY_INDENT, y_highest + AX_TRAJECTORY_INDENT])


    def localization_callback(self, msg):

        with self.car_state_mutex:
            self.car.x   = msg.pose.position.x
            self.car.y   = msg.pose.position.y
            self.car.yaw = msg.pose.heading

        if (self.params.with_car_path):
            if self.car_trajectory.points_num > 0:
                cur_x = msg.pose.position.x
                cur_y = msg.pose.position.y
                dist = sqrt((cur_x - self.car_trajectory[-1].x) ** 2 + (cur_y - self.car_trajectory[-1].y) ** 2)
                if (dist > CAR_PATH_DL):
                    point = Point()
                    point.x   = cur_x
                    point.y   = cur_y
                    point.yaw = msg.pose.heading

                    with self.car_trajectory_mutex:
                        if (self.car_trajectory.points_num > MAX_CAR_PATH_POINTS_NUM):
                            self.car_trajectory.remove_point(0)
                        self.car_trajectory.add_point(point)
                        self.car_trajectory.length += dist
            else:
                point = Point()
                point.x   = msg.pose.position.x
                point.y   = msg.pose.position.y
                point.yaw = msg.pose.heading

                with self.car_trajectory_mutex:
                    self.car_trajectory.add_point(point)



    def planning_callback(self, msg):
        remaining_trajectory_points = len(msg.trajectory_point)
        if remaining_trajectory_points > 0:
            self.target_point_id = self.trajectory.points_num - remaining_trajectory_points


    def chassis_callback(self, msg):

        if not isnan(msg.steering_percentage):
            steering = msg.steering_percentage / 100.0 * self.car.max_steering
            self.car.set_steering(steering)


    def update_plot(self, frame, car, plotter, trajectory):

        now = cyber_time.Time.now().to_sec()

        # print("Plot update delay [sec]: " + str(format(now - self.last_plot_update_time, '.2f')))

        self.last_plot_update_time = now

        plots_to_update = []

        # if (self.car_trajectory.points_num == 40):
        #     self.xlimits = [self.xlimits[0]-30, self.xlimits[1]+30]
        #     self.ylimits = [self.ylimits[0]-30, self.ylimits[1]+30]
        #     self.plotter.reset_ax_limits(self.xlimits, self.ylimits)
        #     plots_to_update.extend(self.target_trajectory_plot.update_plot(self.trajectory))

        with self.car_state_mutex:
            x   = self.car.x
            y   = self.car.y
            yaw = self.car.yaw

        s   = self.car.steering

        target_id = self.target_point_id
        target_x = self.trajectory[target_id].x
        target_y = self.trajectory[target_id].y
        target_yaw = self.trajectory[target_id].yaw
        target_s = self.trajectory[target_id].steering

        plots_to_update.extend(self.car_frame_plot.update_plot(x, y, yaw))
        plots_to_update.extend(self.car_plot.update_plot(x, y, yaw, s))
        plots_to_update.extend(self.additional_car_geometry_plot.update_plot(x, y, yaw, s))

        plots_to_update.extend(self.target_car_plot.update_plot(target_x,
                                                                target_y,
                                                                target_yaw,
                                                                target_s))

        plots_to_update.extend(self.target_car_frame_plot.update_plot(target_x,
                                                                      target_y,
                                                                      target_yaw))

        if (self.params.with_car_path and self.car_trajectory.points_num > 0):
            with self.car_trajectory_mutex:
                plots_to_update.extend(self.car_trajectory_plot.update_plot(self.car_trajectory))
        #
        info_data = "Time[s]:" +      str(format(self.trajectory[target_id].t, '.2f')) + \
                    "\nSpeed[m/s]:" + str(format(self.trajectory[target_id].v, '.2f'))
        #
        info, = plotter.ax.text(0.75, 0.92, info_data,
                               bbox={'facecolor':'w', 'alpha':0.5, 'pad':5},
                               ha="left", transform=plotter.ax.transAxes, ),
        plots_to_update.append(info)

        return plots_to_update


    def spin(self):

        self.cyber_node.create_reader('/apollo/localization/pose',
                                      localization_pb2.LocalizationEstimate,
                                      self.localization_callback)

        self.cyber_node.create_reader('/apollo/planning',
                                      planning_pb2.ADCTrajectory,
                                      self.planning_callback)

        self.cyber_node.create_reader('/apollo/canbus/chassis',
                                      chassis_pb2.Chassis,
                                      self.chassis_callback)

        trajectory_plot_dlength = self.trajectory.length / 50.0

        self.target_trajectory_plot.with_dl(trajectory_plot_dlength)
        # self.target_trajectory_plot.with_dt(0.7)

        if self.params.with_trajectory_frames:
            self.target_trajectory_plot.with_car_frames(trajectory_plot_dlength * 2.0, lw=2)

        if self.params.with_car_footprints:
            self.target_trajectory_plot.with_car_footprints(self.car, trajectory_plot_dlength * 4.0)

        self.target_trajectory_plot.update_plot(self.trajectory)

        # trajectory_plot = self.plotter.draw_car_trajectory_by_len(self.car,
        #                                                           self.trajectory,
        #                                                           dl = trajectory_plot_dlength,
        #                                                           frames = True,
        #                                                           frames_dl = trajectory_plot_dlength * 2,
        #                                                           footprints = False,
        #                                                           footprints_dl = trajectory_plot_dlength * 10)
        animation_dt = 1.0 / self.params.rate
        animation_period = int(1000*animation_dt)

        animation = FuncAnimation(self.plotter.fig,
                                  self.update_plot,
                                  fargs=(self.car, self.plotter, self.trajectory),
                                  interval=animation_period,          # if too large and blit is True
                                  blit=True)                          # throw an exeption but works somehow

        plt.show()


if __name__ == '__main__':

    plotter = TrajectoryPlotter()
    plotter.spin()
