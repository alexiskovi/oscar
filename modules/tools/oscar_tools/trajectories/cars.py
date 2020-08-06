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

import numpy as np
from math import pi, sqrt, cos, sin, copysign

class Car():
    def __init__(self):
        self.wheel_radius     = 0.35
        self.wheel_width      = 0.25
        self.wheelbase        = 2.5
        self.axle_track       = 1.5
        self.max_steering     = 1.0
        self.steering_ratio   = 1.0
        self.max_acceleration = 5.0
        self.max_deceleration = 10.0
        self.footprint = np.array([[ self.wheelbase + 1.0,  (self.axle_track+self.wheel_width)], # quadrants order
                                   [ self.wheelbase + 1.0, -(self.axle_track+self.wheel_width)],
                                   [-1.0,                  -(self.axle_track+self.wheel_width)],
                                   [-1.0,                   (self.axle_track+self.wheel_width)]])

        # bounds
        self.min_radius = None

        self.x   = 0.0
        self.y   = 0.0
        self.z   = 0.0
        self.yaw = 0.0
        self.v   = 0.0
        self.a   = 0.0

        self.steering = 0.0
        self.steering_fr = 0.0
        self.steering_fl = 0.0
        self.turn_radius = 10000.0

        self.calc_bounds()

        self.history_t = []
        self.history_x = []
        self.history_y = []
        self.history_yaw = []
        self.history_steer = []
        self.history_v = []


    def calc_bounds(self):
        self.min_radius = self.wheelbase / np.tan(self.max_steering)


    def set_steering(self, steering):

        if (abs(steering) > self.max_steering):
            self.steering = copysign(self.max_steering, steering)
            # warn!
        else:
            self.steering = steering

        if (abs(steering) < 0.001):         # ~0.056 deg
            self.turn_radius = copysign(10000.0, self.steering)
        else:
            self.turn_radius = self.wheelbase / np.tan(self.steering)

        self.steering_fl, self.steering_fr = self.calc_front_wheels_steerings_by_turn_radius(self.turn_radius)


    def calc_turn_radius_by_steering(self, steering):

        if (abs(steering) > self.max_steering):
            steering = copysign(self.max_steering, steering)
            # warn!

        if (abs(steering) < 0.001):         # ~0.056 deg
            turn_radius = copysign(10000.0, steering)
        else:
            turn_radius = self.wheelbase / np.tan(steering)

        return turn_radius


    def calc_front_wheels_steerings_by_turn_radius(self, turn_radius):
        return np.arctan2(self.wheelbase, turn_radius - self.axle_track / 2.0), \
               np.arctan2(self.wheelbase, turn_radius + self.axle_track / 2.0)


    def calc_front_wheels_steerings_by_steering(self, steering):
        if (abs(steering) < 0.0001):
            return 0.0, 0.0
        else:
            return np.arctan2(self.wheelbase, self.wheelbase / np.tan(steering) - self.axle_track / 2.0), \
                   np.arctan2(self.wheelbase, self.wheelbase / np.tan(steering) + self.axle_track / 2.0)


    def calc_steering_by_turn_radius(self, turn_radius):

        if (abs(turn_radius) < self.min_radius):
            turn_radius = copysign(self.min_radius, turn_radius)
            # warn!

        if (abs(turn_radius) > 10 ** 4):
            steering = 0.0
        else:
            steering = np.arctan2(self.wheelbase, turn_radius)

            if (turn_radius < 0):
                steering -= pi

        return steering


    def calc_traj_step_by_control(self, u_1, u_2, dt=0.01):

        if (abs(u_2) > self.max_steering):
            u_2 = copysign(self.max_steering, u_2)

        dx = cos(self.yaw) * self.v
        dy = sin(self.yaw) * self.v
        dv = u_1
        dyaw = self.v / self.wheelbase * np.tan(u_2)

        self.x += dt * dx
        self.y += dt * dy
        self.yaw += dt * dyaw
        self.v += dt * dv
        self.a = dv
        self.set_steering(u_2)

        self.history_x.append(self.x)
        self.history_y.append(self.y)
        self.history_yaw.append(self.yaw)
        self.history_steer.append(self.steering)
        self.history_v.append(self.v)

        if self.history_t:
            self.history_t.append(self.history_t[-1] + dt)
        else:
            self.history_t.append(0.0)


    def generate_8_traj_by_control(self, dt = 0.01):

        # self.yaw = np.pi/2

        #     for t in range(1, int(5.0/dt)):
        #         self.calc_traj_step_by_control(0.0, self.calc_steering_by_turn_radius(-6.0), dt)

        while (self.y < 5.0):
            if (self.v < 16):
                self.calc_traj_step_by_control(16.0, 0.0, dt)
            else:
                self.calc_traj_step_by_control(0.0, 0.0, dt)

        while (self.y > 5.0 or self.x < -5.0):
            self.calc_traj_step_by_control(0.0, self.calc_steering_by_turn_radius(5.0), dt)

        while (self.x < 5.0):
            self.calc_traj_step_by_control(0.0, 0.0, dt)

        while (self.y < -5.0 or self.x > 5.0):
            self.calc_traj_step_by_control(0.0, self.calc_steering_by_turn_radius(-5.0), dt)

        while (self.v > 0):
            self.calc_traj_step_by_control(-16.0, 0.0, dt)


class Lexus(Car):
    def __init__(self):
        Car.__init__(self)
        self.wheel_radius   = 0.37
        self.wheel_width    = 0.25
        self.wheelbase      = 2.75
        self.axle_track     = 1.64
        self.max_steering   = 0.576           # max 0.576 rad ~ 33.78 deg
        self.steering_ratio = 14.8
        self.max_acceleration = 6.0
        self.max_deceleration = 6.0
        self.footprint = np.array([[ 3.77,  0.94],        # quadrants order
                                   [ 3.77, -0.94],
                                   [-1.0,  -0.94],
                                   [-1.0,   0.94]])

        self.calc_bounds()
