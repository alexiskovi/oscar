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
""" Classes and methods to realtime drawing of tajectories, cars and other additional car info"""

import numpy as np
import matplotlib.pyplot as plt

from datetime import datetime

class Plotter():

    def __init__(self, xlim=[-12,12], ylim=[-12,12]):
        self.fig, self.ax = plt.subplots(1, 1, figsize=[8.0, 8.0], facecolor='w')
        self.ax.set_aspect('equal')
        self.ax.set_xlim(xlim[0], xlim[1])
        self.ax.set_ylim(ylim[0], ylim[1])
        self.ax.grid(True)
        self.ax.set_xlabel('X, [m]')
        self.ax.set_ylabel('Y, [m]')


    def reset_ax_limits(self, xlimits, ylimits):
        self.ax.set_xlim(xlimits[0], xlimits[1])
        self.ax.set_ylim(ylimits[0], ylimits[1])


    class CarPlot():
        def __init__(self, ax, car, color, lw):

            self.car = car
            self.car_front_plot, =      ax.plot([], [], lw=lw, color=color)
            self.car_left_side_plot, =  ax.plot([], [], lw=lw, color=color)
            self.car_right_side_plot, = ax.plot([], [], lw=lw, color=color)
            self.car_rear_plot, =       ax.plot([], [], lw=lw, color=color)
            self.rr_wheel_plot, =       ax.plot([], [], lw=lw, color=color)
            self.rl_wheel_plot, =       ax.plot([], [], lw=lw, color=color)
            self.fr_wheel_plot, =       ax.plot([], [], lw=lw, color=color)
            self.fl_wheel_plot, =       ax.plot([], [], lw=lw, color=color)


        def update_plot(self, x=0.0, y=0.0, yaw=0.0, steering=0.0):

            car_footprint = draw_car(self.car, x, y, yaw, steering)

            self.car_front_plot.set_data(car_footprint['front']['x'], car_footprint['front']['y'])
            self.car_left_side_plot.set_data(car_footprint['left_side']['x'], car_footprint['left_side']['y'])
            self.car_right_side_plot.set_data(car_footprint['right_side']['x'], car_footprint['right_side']['y'])
            self.car_rear_plot.set_data(car_footprint['rear']['x'], car_footprint['rear']['y'])

            self.rr_wheel_plot.set_data(car_footprint['rr_wheel']['x'], car_footprint['rr_wheel']['y'])
            self.rl_wheel_plot.set_data(car_footprint['rl_wheel']['x'], car_footprint['rl_wheel']['y'])
            self.fr_wheel_plot.set_data(car_footprint['fr_wheel']['x'], car_footprint['fr_wheel']['y'])
            self.fl_wheel_plot.set_data(car_footprint['fl_wheel']['x'], car_footprint['fl_wheel']['y'])

            return [self.car_front_plot,
                    self.car_left_side_plot,
                    self.car_right_side_plot,
                    self.car_rear_plot,
                    self.rr_wheel_plot,
                    self.rl_wheel_plot,
                    self.fr_wheel_plot,
                    self.fl_wheel_plot]


    def create_car_plot(self, car, color="#0000CD", lw=2):
        return Plotter.CarPlot(self.ax, car, color=color, lw=lw)


    class AdditionalCarGeometryPlot():

        def __init__(self, ax, car, color, lw, ls):
            self.car = car
            self.wheels_to_pivot_plot,    = ax.plot([], [], color=color, lw=lw, ls=ls)
            self.car_frame_to_pivot_plot, = ax.plot([], [], color=color, lw=lw, ls=ls)


        def update_plot(self, x=0.0, y=0.0, yaw=0.0, steering=0.0):

            add_car_geometry_footprint = draw_additional_car_geometry(self.car, x, y, yaw, steering)

            self.wheels_to_pivot_plot.set_data(add_car_geometry_footprint['wheels_to_pivot']['x'],
                                               add_car_geometry_footprint['wheels_to_pivot']['y'])

            self.car_frame_to_pivot_plot.set_data(add_car_geometry_footprint['car_frame_to_pivot']['x'],
                                                  add_car_geometry_footprint['car_frame_to_pivot']['y'])

            return [self.wheels_to_pivot_plot,
                    self.car_frame_to_pivot_plot]


    def create_additional_car_geometry_plot(self, car, color="#A0A0A0", lw=1, ls=":"):
        return Plotter.AdditionalCarGeometryPlot(self.ax, car, color=color, lw=lw, ls=ls)


    class FramePlot():
        def __init__(self, ax, x_color, x_length, y_color, y_length, z_color, lw):

            self.x_plot, = ax.plot([], [], lw=lw, color=x_color)
            self.y_plot, = ax.plot([], [], lw=lw, color=y_color)
            self.z_plot, = ax.plot([], [], marker='o', markersize=lw, color=z_color)

            self.x_length = x_length
            self.y_length = y_length


        def update_plot(self, x=0.0, y=0.0, yaw=0.0):

            frame_footprint = draw_frame(x, y, yaw, self.x_length, self.y_length)

            self.x_plot.set_data(frame_footprint['x_axis']['x'], frame_footprint['x_axis']['y'])
            self.y_plot.set_data(frame_footprint['y_axis']['x'], frame_footprint['y_axis']['y'])
            self.z_plot.set_data(frame_footprint['z_axis']['x'], frame_footprint['z_axis']['y'])

            return [self.x_plot, self.y_plot, self.z_plot]


    def create_frame_plot(self, x_color="#C00000", x_length=1.0,
                                y_color="#008000", y_length=1.0,
                                z_color="#4682B4", lw=2):

        return Plotter.FramePlot(self.ax, x_color=x_color, x_length=x_length,
                                          y_color=y_color, y_length=y_length,
                                          z_color=z_color, lw=lw)


    class TrajectoryPlot():
        def __init__(self, ax, color, lw, ls):

            self.ax = ax

            self.trajectory_plot, = ax.plot([], [], color=color, lw=lw, ls=ls)
            self.car_frames_plots = []
            self.car_footprints_plots = []

            self.dt = None
            self.dl = None

            self.with_car_frames_ = False
            self.car_frames_dl = None
            self.with_last_car_frame = True
            self.x_axis_color = None
            self.y_axis_color = None
            self.z_axis_color = None
            self.x_axis_length = None
            self.y_axis_length = None
            self.frame_lw = None

            self.with_car_footprints_ = False
            self.car_footprints_dl = None
            self.with_last_car_footprint = True
            self.car = None
            self.car_footprints_color = None
            self.car_footprints_lw = None


        def with_dt(self, dt):
            self.dt = dt


        def with_dl(self, dl):
            self.dl = dl


        def with_car_frames(self, dl = None,
                                  with_last_frame = True,
                                  x_color = "#c25353", x_length = 1.0,
                                  y_color = "#53c27f", y_length = 1.0,
                                  z_color = "#53a4c2", lw = 1):

            self.with_car_frames_ = True
            if (dl):
                self.car_frames_dl = dl
            self.with_last_car_frame = with_last_frame
            self.x_axis_color = x_color
            self.y_axis_color = y_color
            self.z_axis_color = z_color
            self.x_axis_length = x_length
            self.y_axis_length = y_length
            self.frame_lw = lw


        def without_car_frames(self):
            self.with_car_frames_ = False
            self.car_frames_dl = None


        def with_car_footprints(self, car,
                                      dl = None,
                                      with_last_car_footprint = True,
                                      color="#FFC6D3", lw = 1):

            self.with_car_footprints_ = True
            self.car = car
            if (dl):
                self.car_footprints_dl = dl
            self.with_last_car_footprint = with_last_car_footprint
            self.car_footprints_color = color
            self.car_footprints_lw = lw


        def without_car_footprints(self):
            self.with_car_footprints_ = False
            self.car_footprints_dl = None


        def update_plot(self, trajectory):

            plots = []

            trajectory_footprint = draw_trajectory(trajectory, self.dt, self.dl)

            self.trajectory_plot.set_data(trajectory_footprint['x'], trajectory_footprint['y'])
            plots.append(self.trajectory_plot)

            if self.with_car_frames_:
                car_frames_footprints = draw_trajectory_frames(trajectory,
                                                               self.car_frames_dl,
                                                               self.x_axis_length,
                                                               self.y_axis_length,
                                                               self.with_last_car_frame)

                car_frames_plots_num = len(self.car_frames_plots)

                for fp_id in range(len(car_frames_footprints)):

                    plots_id = fp_id * 3
                    if plots_id <= (car_frames_plots_num - 3):

                        self.car_frames_plots[plots_id].set_data(car_frames_footprints[fp_id]['x_axis']['x'],
                                                                 car_frames_footprints[fp_id]['x_axis']['y'])

                        self.car_frames_plots[plots_id+1].set_data(car_frames_footprints[fp_id]['y_axis']['x'],
                                                                   car_frames_footprints[fp_id]['y_axis']['y'])

                        self.car_frames_plots[plots_id+2].set_data(car_frames_footprints[fp_id]['z_axis']['x'],
                                                                   car_frames_footprints[fp_id]['z_axis']['y'])

                    else:

                        x_plot, = self.ax.plot(car_frames_footprints[fp_id]['x_axis']['x'],
                                               car_frames_footprints[fp_id]['x_axis']['y'],
                                               color=self.x_axis_color, lw=self.frame_lw)

                        y_plot, = self.ax.plot(car_frames_footprints[fp_id]['y_axis']['x'],
                                               car_frames_footprints[fp_id]['y_axis']['y'],
                                               color=self.y_axis_color, lw=self.frame_lw)

                        if (self.frame_lw > 1):
                            z_axis_lw = self.frame_lw - 1
                        else:
                            z_axis_lw = self.frame_lw

                        z_plot, = self.ax.plot(car_frames_footprints[fp_id]['z_axis']['x'],
                                               car_frames_footprints[fp_id]['z_axis']['y'],
                                               color=self.z_axis_color, lw=z_axis_lw,
                                               marker='.')

                        self.car_frames_plots.extend([x_plot, y_plot, z_plot])


                if (car_frames_plots_num > len(car_frames_footprints) * 3):
                    for i in range(len(car_frames_footprints) * 3, car_frames_plots_num):
                        self.car_frames_plots[i].set_data([], [])

                plots.extend(self.car_frames_plots)

            else:

                if (len(self.car_frames_plots) > 0 and len(self.car_frames_plots[0].get_xdata) > 0):

                    for plot in self.car_frames_plots:
                        plot.set_data([], [])

                    plots.extend(self.car_frames_plots)


            if self.with_car_footprints_:
                car_footprints = draw_trajectory_car_footprints(trajectory,
                                                                self.car,
                                                                self.car_footprints_dl,
                                                                self.with_last_car_footprint)

                car_footprints_plots_num = len(self.car_footprints_plots)

                for fp_id in range(len(car_footprints)):

                    plots_id = fp_id * 8
                    if plots_id <= (car_footprints_plots_num - 8):

                        self.car_footprints_plots[plots_id].set_data(car_footprints[fp_id]['front']['x'],
                                                                     car_footprints[fp_id]['front']['y'])

                        self.car_footprints_plots[plots_id+1].set_data(car_footprints[fp_id]['left_side']['x'],
                                                                       car_footprints[fp_id]['left_side']['y'])

                        self.car_footprints_plots[plots_id+2].set_data(car_footprints[fp_id]['right_side']['x'],
                                                                       car_footprints[fp_id]['right_side']['y'])

                        self.car_footprints_plots[plots_id+3].set_data(car_footprints[fp_id]['rear']['x'],
                                                                       car_footprints[fp_id]['rear']['y'])

                        self.car_footprints_plots[plots_id+4].set_data(car_footprints[fp_id]['rr_wheel']['x'],
                                                                       car_footprints[fp_id]['rr_wheel']['y'])

                        self.car_footprints_plots[plots_id+5].set_data(car_footprints[fp_id]['rl_wheel']['x'],
                                                                       car_footprints[fp_id]['rl_wheel']['y'])

                        self.car_footprints_plots[plots_id+6].set_data(car_footprints[fp_id]['fr_wheel']['x'],
                                                                       car_footprints[fp_id]['fr_wheel']['y'])

                        self.car_footprints_plots[plots_id+7].set_data(car_footprints[fp_id]['fl_wheel']['x'],
                                                                       car_footprints[fp_id]['fl_wheel']['y'])

                    else:

                        front_plot, =      self.ax.plot(car_footprints[fp_id]['front']['x'],
                                                        car_footprints[fp_id]['front']['y'],
                                                        color=self.car_footprints_color,
                                                        lw=self.car_footprints_lw)

                        left_side_plot, =  self.ax.plot(car_footprints[fp_id]['left_side']['x'],
                                                        car_footprints[fp_id]['left_side']['y'],
                                                        color=self.car_footprints_color,
                                                        lw=self.car_footprints_lw)

                        right_side_plot, = self.ax.plot(car_footprints[fp_id]['right_side']['x'],
                                                        car_footprints[fp_id]['right_side']['y'],
                                                        color=self.car_footprints_color,
                                                        lw=self.car_footprints_lw)

                        rear_plot, =       self.ax.plot(car_footprints[fp_id]['rear']['x'],
                                                        car_footprints[fp_id]['rear']['y'],
                                                        color=self.car_footprints_color,
                                                        lw=self.car_footprints_lw)

                        rr_wheel_plot, =   self.ax.plot(car_footprints[fp_id]['rr_wheel']['x'],
                                                        car_footprints[fp_id]['rr_wheel']['y'],
                                                        color=self.car_footprints_color,
                                                        lw=self.car_footprints_lw)

                        rl_wheel_plot, =   self.ax.plot(car_footprints[fp_id]['rl_wheel']['x'],
                                                        car_footprints[fp_id]['rl_wheel']['y'],
                                                        color=self.car_footprints_color,
                                                        lw=self.car_footprints_lw)

                        fr_wheel_plot, =   self.ax.plot(car_footprints[fp_id]['fr_wheel']['x'],
                                                        car_footprints[fp_id]['fr_wheel']['y'],
                                                        color=self.car_footprints_color,
                                                        lw=self.car_footprints_lw)

                        fl_wheel_plot, =   self.ax.plot(car_footprints[fp_id]['fl_wheel']['x'],
                                                        car_footprints[fp_id]['fl_wheel']['y'],
                                                        color=self.car_footprints_color,
                                                        lw=self.car_footprints_lw)

                        self.car_footprints_plots.extend([front_plot,
                                                          left_side_plot,
                                                          right_side_plot,
                                                          rear_plot,
                                                          rr_wheel_plot,
                                                          rl_wheel_plot,
                                                          fr_wheel_plot,
                                                          fl_wheel_plot])

                if (car_footprints_plots_num > len(car_footprints) * 8):
                    for i in range(len(car_footprints) * 8, car_footprints_plots_num):
                        self.car_footprints_plots[i].set_data([], [])

                plots.extend(self.car_footprints_plots)

            else:

                if (len(self.car_footprints_plots) > 0 and len(self.car_footprints_plots[0].get_xdata) > 0):

                    for plot in self.car_footprints_plots:
                        plot.set_data([], [])

                    plots.extend(self.car_footprints_plots)

            return plots


    def create_trajectory_plot(self, color="#000000", lw=1, ls="-"):
        return Plotter.TrajectoryPlot(self.ax, color=color, lw=lw, ls=ls)


def draw_frame(x=0.0, y=0.0, yaw=0.0, x_length=1.0, y_length=1.0):

    R = np.array([[np.cos(yaw), -np.sin(yaw)],
                  [np.sin(yaw),  np.cos(yaw)]])

    frame_x = R.dot(np.array([[0.0, 0.0], [x_length, 0.0]]).T).T
    frame_y = R.dot(np.array([[0.0, 0.0], [0.0, y_length]]).T).T

    return {'x_axis':    {'x': frame_x[:,0] + x, 'y': frame_x[:,1] + y},
            'y_axis':    {'x': frame_y[:,0] + x, 'y': frame_y[:,1] + y},
            'z_axis':    {'x': x,                'y': y}}


def draw_car(car, x=0.0, y=0.0, yaw=0.0, steering=0.0):
    # car Rot matrix in global frame
    R_car = np.array([[np.cos(yaw), -np.sin(yaw)],
                      [np.sin(yaw),  np.cos(yaw)]])

    car_front_footprint = np.array([[car.wheelbase + 1.3*car.wheel_radius, car.footprint[0][1] - 0.25*car.wheel_width],
                                    [car.wheelbase + 1.3*car.wheel_radius, car.footprint[0][1]],
                                    car.footprint[0],
                                    car.footprint[1],
                                    [car.wheelbase + 1.3*car.wheel_radius, car.footprint[1][1]],
                                    [car.wheelbase + 1.3*car.wheel_radius, car.footprint[1][1] + 0.25*car.wheel_width]])

    car_front_footprint = R_car.dot(car_front_footprint.T).T
    car_front_footprint[:,0] += x
    car_front_footprint[:,1] += y

    car_left_side_footprint = np.array([[car.wheelbase - 1.3*car.wheel_radius, car.footprint[0][1] - 0.25*car.wheel_width],
                                        [car.wheelbase - 1.3*car.wheel_radius, car.footprint[0][1]],
                                        [1.3*car.wheel_radius, car.footprint[0][1]],
                                        [1.3*car.wheel_radius, car.footprint[0][1] - 0.25*car.wheel_width]])

    car_left_side_footprint = R_car.dot(car_left_side_footprint.T).T
    car_left_side_footprint[:,0] += x
    car_left_side_footprint[:,1] += y

    car_right_side_footprint = np.array([[car.wheelbase - 1.3*car.wheel_radius, car.footprint[1][1] + 0.25*car.wheel_width],
                                         [car.wheelbase - 1.3*car.wheel_radius, car.footprint[1][1]],
                                         [1.3*car.wheel_radius, car.footprint[1][1]],
                                         [1.3*car.wheel_radius, car.footprint[1][1] + 0.25*car.wheel_width]])

    car_right_side_footprint = R_car.dot(car_right_side_footprint.T).T
    car_right_side_footprint[:,0] += x
    car_right_side_footprint[:,1] += y

    car_rear_footprint = np.array([[- 1.3*car.wheel_radius, car.footprint[2][1] + 0.25*car.wheel_width],
                                   [- 1.3*car.wheel_radius, car.footprint[2][1]],
                                   car.footprint[2],
                                   car.footprint[3],
                                   [- 1.3*car.wheel_radius, car.footprint[0][1]],
                                   [- 1.3*car.wheel_radius, car.footprint[0][1] - 0.25*car.wheel_width]])

    car_rear_footprint = R_car.dot(car_rear_footprint.T).T
    car_rear_footprint[:,0] += x
    car_rear_footprint[:,1] += y

    wheel_footprint = np.array([[ car.wheel_radius,  car.wheel_width/2],
                                [ car.wheel_radius, -car.wheel_width/2],
                                [-car.wheel_radius, -car.wheel_width/2],
                                [-car.wheel_radius,  car.wheel_width/2],
                                [ car.wheel_radius,  car.wheel_width/2]])

    rr_wheel_footprint = np.array(wheel_footprint)
    rr_wheel_footprint [:,1] -= car.axle_track / 2.0
    rr_wheel_footprint = R_car.dot(rr_wheel_footprint.T).T
    rr_wheel_footprint[:,0] += x
    rr_wheel_footprint[:,1] += y

    rl_wheel_footprint = np.array(wheel_footprint)
    rl_wheel_footprint [:,1] += car.axle_track / 2.0
    rl_wheel_footprint = R_car.dot(rl_wheel_footprint.T).T
    rl_wheel_footprint[:,0] += x
    rl_wheel_footprint[:,1] += y

    steer_fl, steer_fr = car.calc_front_wheels_steerings_by_steering(steering)

    # wheels Rot matrices in car frame
    R_steer_fl = np.array([[np.cos(steer_fl), -np.sin(steer_fl)],
                           [np.sin(steer_fl),  np.cos(steer_fl)]])

    R_steer_fr = np.array([[np.cos(steer_fr), -np.sin(steer_fr)],
                           [np.sin(steer_fr),  np.cos(steer_fr)]])

    fr_wheel_footprint = R_steer_fr.dot(wheel_footprint.T).T
    fr_wheel_footprint [:,1] -= car.axle_track / 2.0
    fr_wheel_footprint [:,0] += car.wheelbase
    fr_wheel_footprint = R_car.dot(fr_wheel_footprint.T).T
    fr_wheel_footprint[:,0] += x
    fr_wheel_footprint[:,1] += y

    fl_wheel_footprint = R_steer_fl.dot(wheel_footprint.T).T
    fl_wheel_footprint [:,1] += car.axle_track / 2.0
    fl_wheel_footprint [:,0] += car.wheelbase
    fl_wheel_footprint = R_car.dot(fl_wheel_footprint.T).T
    fl_wheel_footprint[:,0] += x
    fl_wheel_footprint[:,1] += y

    return {'front': {'x': car_front_footprint[:,0], "y": car_front_footprint[:,1]},
            'rear':  {'x': car_rear_footprint[:,0],  "y": car_rear_footprint[:,1]},

            'left_side':  {'x': car_left_side_footprint[:,0],  "y": car_left_side_footprint[:,1]},
            'right_side': {'x': car_right_side_footprint[:,0], "y": car_right_side_footprint[:,1]},

            'rr_wheel':  {'x': rr_wheel_footprint[:,0],  "y": rr_wheel_footprint[:,1]},
            'rl_wheel':  {'x': rl_wheel_footprint[:,0],  "y": rl_wheel_footprint[:,1]},
            'fr_wheel':  {'x': fr_wheel_footprint[:,0],  "y": fr_wheel_footprint[:,1]},
            'fl_wheel':  {'x': fl_wheel_footprint[:,0],  "y": fl_wheel_footprint[:,1]}}


def draw_additional_car_geometry(car, x=0.0, y=0.0, yaw=0.0, steering=0.0):

    turn_radius = car.calc_turn_radius_by_steering(steering)

    if (abs(turn_radius) < 20):

        # car Rot matrix in global frame
        R_car = np.array([[np.cos(yaw), -np.sin(yaw)],
                          [np.sin(yaw),  np.cos(yaw)]])

        wheels_to_pivot_footprint = np.array([[car.wheelbase, -car.axle_track/2.0],
                                          [0, turn_radius],
                                          [car.wheelbase,  car.axle_track/2.0]])

        wheels_to_pivot_footprint = R_car.dot(wheels_to_pivot_footprint.T).T

        car_frame_to_pivot_footprint = np.array([[0, 0], [0, turn_radius]])
        car_frame_to_pivot_footprint = R_car.dot(car_frame_to_pivot_footprint.T).T

        return {'wheels_to_pivot':    {'x': wheels_to_pivot_footprint[:,0] + x,
                                       'y': wheels_to_pivot_footprint[:,1] + y},
                'car_frame_to_pivot': {'x': car_frame_to_pivot_footprint[:,0] + x,
                                       'y': car_frame_to_pivot_footprint[:,1] + y}}

    else:

        return {'wheels_to_pivot':    {'x': [],
                                       'y': []},
                'car_frame_to_pivot': {'x': [],
                                       'y': []}}


def draw_trajectory(trajectory, dt = None, dl = None):

    x = [trajectory.points[0].x]
    y = [trajectory.points[0].y]

    t_prev = 0.0
    l_prev = 0.0
    for point in trajectory.points[1:-1]:

        without_d = not (dt or dl)
        in_by_dt = False
        in_by_dl = False

        if dt:
            cur_dt = point.t - t_prev
            in_by_dt = (dt <= cur_dt)

        if dl:
            cur_dl = point.l - l_prev
            in_by_dl = (dl <= cur_dl)

        if (without_d or (in_by_dt or in_by_dl)):

            t_prev = point.t
            l_prev = point.l

            x.append(point.x)
            y.append(point.y)


    x.append(trajectory.points[-1].x)
    y.append(trajectory.points[-1].y)

    # for i in range(len(x)):
    #     print(str(x[i]) + "\t" + str(y[i]))


    return {'x': x, 'y': y}


def draw_trajectory_frames(trajectory, dl=None, x_length=1.0, y_length=1.0, with_last_frame=True):

    frames_footprints = []

    frames_footprints.append(draw_frame(trajectory[0].x,
                                        trajectory[0].y,
                                        trajectory[0].yaw,
                                        x_length,
                                        y_length))

    l_prev = 0.0
    for point in trajectory.points[1:-1]:

        in_by_dl = True

        if (dl):
            cur_dl = point.l - l_prev
            in_by_dl = (dl <= cur_dl)

        if (in_by_dl):

            l_prev = point.l

            frames_footprints.append(draw_frame(point.x,
                                                point.y,
                                                point.yaw,
                                                x_length,
                                                y_length))

        if with_last_frame:
            frames_footprints.append(draw_frame(trajectory[-1].x,
                                                trajectory[-1].y,
                                                trajectory[-1].yaw,
                                                x_length,
                                                y_length))

    return frames_footprints


def draw_trajectory_car_footprints(trajectory, car, dl=None, with_last_car_footprint=True):

    car_footprints = []

    car_footprints.append(draw_car(car,
                                   trajectory[0].x,
                                   trajectory[0].y,
                                   trajectory[0].yaw,
                                   trajectory[0].steering))

    l_prev = 0.0
    for point in trajectory.points[1:-1]:

        in_by_dl = True

        if (dl):
            cur_dl = point.l - l_prev
            in_by_dl = (dl <= cur_dl)

        if (in_by_dl):

            l_prev = point.l

            car_footprints.append(draw_car(car,
                                           point.x,
                                           point.y,
                                           point.yaw,
                                           point.steering))

        if with_last_car_footprint:
            car_footprints.append(draw_car(car,
                                           trajectory[-1].x,
                                           trajectory[-1].y,
                                           trajectory[-1].yaw,
                                           trajectory[-1].steering))

    return car_footprints
