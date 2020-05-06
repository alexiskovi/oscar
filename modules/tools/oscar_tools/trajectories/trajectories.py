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
""" Classes and methods for generate and manipulate of car trajectories """

import numpy as np

from math import pi, sqrt, cos, sin, copysign

class CarTrajectoryGenerator():

    class Point():
        def __init__(self):
            self.t          = 0.0
            self.l          = 0.0
            self.x          = 0.0
            self.y          = 0.0
            self.z          = 0.0
            self.v          = 0.0
            self.a          = 0.0
            self.yaw        = 0.0
            self.dyaw       = 0.0
            self.ddyaw      = 0.0
            self.steering   = 0.0
            self.dsteering  = 0.0
            self.ddsteering = 0.0
            self.curvature  = 0.0
            self.dcurvature = 0.0
            self.sharpness  = 0.0


    class Trajectory():

        def __init__(self):
            self.length = 0.0
            self.time   = 0.0
            self.points = []
            self.points_num = 0
            self.closest_point_id = 0

        def transform_to(self, x, y, yaw, z=0):

            R = np.array([[np.cos(yaw), -np.sin(yaw)],
                          [np.sin(yaw),  np.cos(yaw)]])

            for point in self.points:

                rotated_pose = R.dot(np.array([point.x, point.y]).T).T
                point.x = rotated_pose[0] + x
                point.y = rotated_pose[1] + y
                point.yaw = point.yaw + yaw
                point.z = z

            return self


    class ClothoidGenerator():

        def Cf_by_int(self, x, step):
            Cf = 0.0
            l = int(x/step)
            s = 0.0
            for i in range(0, l):
                Cf += cos(pi / 2 * s ** 2) * step
                s += step
            return Cf


        def Sf_by_int(self, x, step):
            Sf = 0.0
            l = int(x/step)
            s = 0.0
            for i in range(0, l):
                Sf += sin(pi / 2 * s ** 2) * step
                s += step
            return Sf


        def xCoordByS(self, gamma, alpha, s, step):
            return gamma * sqrt(pi / abs(alpha)) * self.Cf_by_int(sqrt(abs(alpha) / pi) * s, step)


        def yCoordByS(self, gamma, alpha, s, step):
            return gamma * copysign(1, alpha) * sqrt(pi / abs(alpha)) * self.Sf_by_int(sqrt(abs(alpha) / pi) * s, step)


        def tCoordByS(self, gamma, alpha, s):
            return 0.5 * alpha * s ** 2


        def xCoordByD(self, d_cc, k_cc, d, step):
            return copysign(1, d_cc) * sqrt(2 * pi * abs(d_cc)) / k_cc * self.Cf_by_int(sqrt(2 * abs(d) / pi), step)


        def yCoordByD(self, d_cc, k_cc, d, step):
            return sqrt(2 * pi * abs(d_cc)) / k_cc * self.Sf_by_int(sqrt(2 * abs(d) / pi), step)


        def tCoordByD(self, d):
            return d


        # [new_x, new_y] = ( R(to_yaw) * [cur_x, cur_y]^(T) + [to_x, to_y]^(T) )^(T)
        def transform(self, cur_x, cur_y, cur_yaw, to_x, to_y, to_yaw):

            if (to_yaw == 0):
                new_yaw = cur_yaw
                new_xy = np.array([cur_x, cur_y])
            else:
                R = np.array([[np.cos(to_yaw), -np.sin(to_yaw)],
                              [np.sin(to_yaw),  np.cos(to_yaw)]])

                new_xy = R.dot(np.array([cur_x, cur_y]).T).T

                new_yaw = cur_yaw + to_yaw

            return new_xy[0] + to_x, new_xy[1] + to_y, new_yaw


        # [new_x, new_y] = ( R(from_yaw)^(T) * [cur_x, cur_y]^(T) - R(from_yaw)^(T) * [from_x, from_y]^(T) )^(T)
        def itransform(self, cur_x, cur_y, cur_yaw, from_x, from_y, from_yaw):

            Rinv = np.array([[ np.cos(from_yaw), np.sin(from_yaw)],
                             [-np.sin(from_yaw), np.cos(from_yaw)]])

            new_xy = (Rinv.dot(np.array([cur_x, cur_y]).T) - Rinv.dot(np.array([from_x, from_y]).T)).T

            new_yaw = cur_yaw - from_yaw

            return new_xy[0], new_xy[1], new_yaw


        def calcClothoidPointsByS(self, gamma, alpha, s_end, type, step):
            poses = np.ndarray(shape=(int(10000*s_end) / int(10000*step) + 2, 3))
            if (type == "in"):
                i = 0
                for s in np.arange(0, s_end, step):
                    poses[i] = np.array([self.xCoordByS(gamma, alpha, s, step),
                                         self.yCoordByS(gamma, alpha, s, step),
                                         self.tCoordByS(gamma, alpha, s)]).reshape((3))
                    i += 1
                poses[i] = np.array([self.xCoordByS(gamma, alpha, s_end, step),
                                     self.yCoordByS(gamma, alpha, s_end, step),
                                     self.tCoordByS(gamma, alpha, s_end)]).reshape((3))
            else:

                origin_x, origin_y, origin_yaw = self.xCoordByS(-gamma, alpha, s_end, step), \
                                                 self.yCoordByS(-gamma, alpha, s_end, step), \
                                                 self.tCoordByS(gamma, alpha, s_end)
                i = 0
                for s in np.arange(0, s_end, step):
                    poses[i] = np.array([self.xCoordByS(-gamma, alpha, s_end - s, step),
                                         self.yCoordByS(-gamma, alpha, s_end - s, step),
                                         self.tCoordByS(gamma, alpha, s_end - s)]).reshape((1, 3))
                    poses[i] = self.itransform(poses[i,0], poses[i,1], poses[i,2], origin_x, origin_y, origin_yaw)
                    i += 1

                poses[i] = np.array([self.xCoordByS(-gamma, alpha, 0.0, step),
                                     self.yCoordByS(-gamma, alpha, 0.0, step),
                                     self.tCoordByS(gamma, alpha, s_end - s)]).reshape((1, 3))
                poses[i] = self.itransform(poses[i,0], poses[i,1], poses[i,2], origin_x, origin_y, origin_yaw)

            return poses


        def calcClothoidPointsByD(self, d_cc, turn_radius_cc, type, step):

            k_cc = 1.0 / turn_radius_cc

            traj_size = int(abs((10000*d_cc)) / (10000*step)) + 1
            poses = np.ndarray(shape=(traj_size, 3))

            # step = copysign(abs(step), d_cc)

            if (type == "in"):
                i = 0
                for d in np.arange(0, d_cc, copysign(step, d_cc)):
                    poses[i] = np.array([self.xCoordByD(d_cc, k_cc, d, step),
                                         self.yCoordByD(d_cc, k_cc, d, step),
                                         self.tCoordByD(d)]).reshape((3))
                    i += 1
                poses[i] = np.array([self.xCoordByD(d_cc, k_cc, d_cc, step),
                                     self.yCoordByD(d_cc, k_cc, d_cc, step),
                                     self.tCoordByD(d_cc)]).reshape((3))
            else:

                origin_x, origin_y, origin_yaw = self.xCoordByD(-d_cc, k_cc, d_cc, step), \
                                                 self.yCoordByD(-d_cc, k_cc, d_cc, step), \
                                                 self.tCoordByD(-d_cc)
                i = 0
                for d in np.arange(0, d_cc, copysign(step, d_cc)):
                    poses[i] = np.array([self.xCoordByD(-d_cc, k_cc, d-d_cc, step),
                                         self.yCoordByD(-d_cc, k_cc, d-d_cc, step),
                                         self.tCoordByD(d-d_cc)]).reshape((1, 3))
                    poses[i] = self.itransform(poses[i,0], poses[i,1], poses[i,2], origin_x, origin_y, origin_yaw)
                    i += 1

                poses[i] = np.array([self.xCoordByD(-d_cc, k_cc, 0.0, step),
                                     self.yCoordByD(-d_cc, k_cc, 0.0, step),
                                     self.tCoordByD(0.0)]).reshape((1, 3))
                poses[i] = self.itransform(poses[i,0], poses[i,1], poses[i,2], origin_x, origin_y, origin_yaw)

            return poses


        def calcClothoidPointsByDwithS(self, d_cc, turn_radius_cc, type, step):
            k_cc = 1.0 / turn_radius_cc
            alpha = (k_cc ** 2) / (2 * d_cc)
            s_end = abs(k_cc / alpha)
            gamma = copysign(1, k_cc) * copysign(1, d_cc)

            if type == "out":
                alpha *= -1


            return self.calcClothoidPointsByS(gamma, alpha, s_end, type, step)


        def calc_x_y_yaw_by_length(self, gamma, alpha, length, full_length, type, step):
            if (type == "in"):
                x, y, yaw,curv = self.xCoordByS(gamma, alpha, length, step), \
                                 self.yCoordByS(gamma, alpha, length, step), \
                                 self.tCoordByS(gamma, alpha, length), \
                                 alpha * length
            else:

                origin_x, origin_y, origin_yaw = self.xCoordByS(-gamma, alpha, full_length, step), \
                                                 self.yCoordByS(-gamma, alpha, full_length, step), \
                                                 self.tCoordByS( gamma, alpha, full_length)

                x, y, yaw, curv = self.xCoordByS(-gamma, alpha, full_length - length, step), \
                                  self.yCoordByS(-gamma, alpha, full_length - length, step), \
                                  self.tCoordByS( gamma, alpha, full_length - length), \
                                  - alpha * (full_length - length)
                x, y, yaw = self.itransform(x, y, yaw, origin_x, origin_y, origin_yaw)

            return x, y, yaw, curv


        def calc_x_y_yaw_by_curv_on_length(self, max_clothoid_angle, min_turn_radius, length, type, step):
            max_curvature = 1.0 / min_turn_radius
            sharpness = (max_curvature ** 2) / (2 * max_clothoid_angle)
            clothoid_length = abs(max_curvature / sharpness)
            gamma = copysign(1, max_curvature) * copysign(1, max_clothoid_angle)

            if type == "out":
                sharpness *= -1

            x, y, yaw, curvature = self.calc_x_y_yaw_by_length(gamma, sharpness, length, clothoid_length, type, step)
            return x, y, yaw, curvature, sharpness


    def calc_trapezoidal_trajectory_linear_part(self, trajectory, max_vel, acc, dec, frequency):

        period = 1.0 / frequency
        possible_max_vel = sqrt(2 * trajectory.length * acc * abs(dec) / (acc + abs(dec)))
        max_vel = min (max_vel, possible_max_vel)

        time_to_get_max_vel       = abs(max_vel / acc)
        time_to_stop_from_max_vel = abs(max_vel / dec)

        acc_path_length       = acc * (time_to_get_max_vel ** 2) / 2.0
        dec_path_length       = abs(dec) * (time_to_stop_from_max_vel ** 2 / 2.0)
        const_vel_path_length = trajectory.length - (acc_path_length + dec_path_length)

        trajectory_time = time_to_get_max_vel + time_to_stop_from_max_vel
        if (const_vel_path_length > max_vel * 2.0 * period):
            trajectory_time += const_vel_path_length / max_vel

        trajectory_points_num = int(trajectory_time * frequency) + 1

        trajectory.points_num = trajectory_points_num
        trajectory.time       = trajectory_time

        for i in range(trajectory.points_num):
            trajectory_point = self.Point()
            trajectory_point.t = i * period

            if (trajectory_point.t <= time_to_get_max_vel):
                trajectory_point.a = acc
                trajectory_point.v = trajectory_point.a * trajectory_point.t
                trajectory_point.l = trajectory_point.a * trajectory_point.t ** 2 / 2.0

            elif (trajectory_point.t > time_to_get_max_vel and
                  trajectory_point.t < (trajectory_time - time_to_stop_from_max_vel)):
                const_vel_time = trajectory_point.t - time_to_get_max_vel
                trajectory_point.a = 0.0
                trajectory_point.v = max_vel
                trajectory_point.l = acc_path_length + max_vel * const_vel_time

            else:
                dec_time = (trajectory_point.t - (trajectory_time - time_to_stop_from_max_vel))
                trajectory_point.a = dec
                trajectory_point.v = max_vel + dec * dec_time
                trajectory_point.l = trajectory.length - dec_path_length + \
                                     max_vel * dec_time + trajectory_point.a * dec_time ** 2 / 2.0

            trajectory.points.append(trajectory_point)


    def get_8type_traj_sectors_params(self, clothoid_generator, max_clothoid_angle, min_turn_radius, clothoid_length, calc_accuracy):

        trajectory_sectors = {0:{'angle':     max_clothoid_angle,
                                 'radius':    min_turn_radius,
                                 'type':      "in",
                                 'length':    clothoid_length,
                                 'end_pose':  {'x':0.,'y':0.,'yaw':0.}},
                              1:{'angle':     max_clothoid_angle,
                                 'radius':    min_turn_radius,
                                 'type':      "out",
                                 'length':    clothoid_length,
                                 'end_pose':  {'x':0.,'y':0.,'yaw':0.}},
                              2:{'angle':    -max_clothoid_angle,
                                 'radius':   -min_turn_radius,
                                 'type':      "in",
                                 'length':    clothoid_length,
                                 'end_pose':  {'x':0.,'y':0.,'yaw':0.}},
                              3:{'angle':    -max_clothoid_angle,
                                 'radius':   -min_turn_radius,
                                 'type':      "out",
                                 'length':    clothoid_length,
                                 'end_pose':  {'x':0.,'y':0.,'yaw':0.}}}

        x, y, yaw, c, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[0]['angle'],
                                                                             trajectory_sectors[0]['radius'],
                                                                             trajectory_sectors[0]['length'],
                                                                             trajectory_sectors[0]['type'],
                                                                             calc_accuracy)

        trajectory_sectors[0]['end_pose']['x'] = x
        trajectory_sectors[0]['end_pose']['y'] = y
        trajectory_sectors[0]['end_pose']['yaw'] = yaw

        x, y, yaw, c, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[1]['angle'],
                                                                             trajectory_sectors[1]['radius'],
                                                                             trajectory_sectors[1]['length'],
                                                                             trajectory_sectors[1]['type'],
                                                                             calc_accuracy)

        x, y, yaw = clothoid_generator.transform(x, y, yaw, trajectory_sectors[0]['end_pose']['x'],
                                                            trajectory_sectors[0]['end_pose']['y'],
                                                            trajectory_sectors[0]['end_pose']['yaw'])

        trajectory_sectors[1]['end_pose']['x'] = x
        trajectory_sectors[1]['end_pose']['y'] = y
        trajectory_sectors[1]['end_pose']['yaw'] = yaw

        x, y, yaw, c, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[2]['angle'],
                                                                             trajectory_sectors[2]['radius'],
                                                                             trajectory_sectors[2]['length'],
                                                                             trajectory_sectors[2]['type'],
                                                                             calc_accuracy)

        x, y, yaw = clothoid_generator.transform(x, y, yaw, trajectory_sectors[1]['end_pose']['x'],
                                                            trajectory_sectors[1]['end_pose']['y'],
                                                            trajectory_sectors[1]['end_pose']['yaw'])

        trajectory_sectors[2]['end_pose']['x'] = x
        trajectory_sectors[2]['end_pose']['y'] = y
        trajectory_sectors[2]['end_pose']['yaw'] = yaw

        x, y, yaw, c, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[3]['angle'],
                                                                             trajectory_sectors[3]['radius'],
                                                                             trajectory_sectors[3]['length'],
                                                                             trajectory_sectors[3]['type'],
                                                                             calc_accuracy)

        x, y, yaw = clothoid_generator.transform(x, y, yaw, trajectory_sectors[2]['end_pose']['x'],
                                                            trajectory_sectors[2]['end_pose']['y'],
                                                            trajectory_sectors[2]['end_pose']['yaw'])

        trajectory_sectors[3]['end_pose']['x'] = x
        trajectory_sectors[3]['end_pose']['y'] = y
        trajectory_sectors[3]['end_pose']['yaw'] = yaw

        return trajectory_sectors


    def generate_8type_traj_using_4_clothoids(self,
                                              car,
                                              min_turn_radius,
                                              max_vel = 1.0,
                                              acc=1.0,
                                              dec=1.5,
                                              frequency=100.0):

        max_clothoid_angle = copysign(2.27884, min_turn_radius)
        max_curvature = 1.0 / min_turn_radius                           # dtheta_max / ds
        sharpness = (max_curvature ** 2) / (2 * max_clothoid_angle)     # ddtheta / ds
        clothoid_length = abs(max_curvature / sharpness)
        calc_accuracy = 0.01

        clothoid_generator = self.ClothoidGenerator()

        trajectory_sectors = self.get_8type_traj_sectors_params(clothoid_generator,
                                                                max_clothoid_angle,
                                                                min_turn_radius,
                                                                clothoid_length,
                                                                calc_accuracy)

        trajectory = self.Trajectory()
        trajectory.length = clothoid_length * 4.0

        dec = float(copysign(dec, -1))
        acc = float(copysign(acc,  1))

        self.calc_trapezoidal_trajectory_linear_part(trajectory, max_vel, acc, dec, frequency)

        for point in trajectory.points:
            sector = int(point.l / clothoid_length)
            x, y, yaw, curv, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[sector]['angle'],
                                                                                    trajectory_sectors[sector]['radius'],
                                                                                    point.l - sector * clothoid_length,
                                                                                    trajectory_sectors[sector]['type'],
                                                                                    calc_accuracy)

            if (sector > 0):
                x, y, yaw = clothoid_generator.transform(x, y, yaw, trajectory_sectors[sector-1]['end_pose']['x'],
                                                                    trajectory_sectors[sector-1]['end_pose']['y'],
                                                                    trajectory_sectors[sector-1]['end_pose']['yaw'])

            turn_radius = 10000.0
            if (abs(curv) > 0.0001):
                turn_radius = 1. / curv

            steering = car.calc_steering_by_turn_radius(turn_radius)

            # print(str(sector)+"\t"+str(round(point.l,2))+"\t"+str(round(sh,3))+"\t"+ \
            #                     str(round(curv,3))+"\t"+str(round(turn_radius,1))+"\t"+ \
            #                     str(round(steering,2)))

            point.x = x
            point.y = y
            point.yaw = yaw
            point.steering = steering
            point.curvature = curv
            point.sharpness = sh

        return trajectory.transform_to(0, 0, -(max_clothoid_angle-copysign(pi/2, max_clothoid_angle)))


    def get_0type_traj_sectors_params(self, clothoid_generator, max_clothoid_angle, min_turn_radius, clothoid_length, calc_accuracy):

        trajectory_sectors = {0:{'angle':     max_clothoid_angle,
                                 'radius':    min_turn_radius,
                                 'type':      "in",
                                 'length':    clothoid_length,
                                 'end_pose':  {'x':0.,'y':0.,'yaw':0.}},
                              1:{'angle':     max_clothoid_angle,
                                 'radius':    min_turn_radius,
                                 'type':      "out",
                                 'length':    clothoid_length,
                                 'end_pose':  {'x':0.,'y':0.,'yaw':0.}},
                              2:{'angle':     max_clothoid_angle,
                                 'radius':    min_turn_radius,
                                 'type':      "in",
                                 'length':    clothoid_length,
                                 'end_pose':  {'x':0.,'y':0.,'yaw':0.}},
                              3:{'angle':     max_clothoid_angle,
                                 'radius':    min_turn_radius,
                                 'type':      "out",
                                 'length':    clothoid_length,
                                 'end_pose':  {'x':0.,'y':0.,'yaw':0.}}}

        x, y, yaw, c, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[0]['angle'],
                                                                             trajectory_sectors[0]['radius'],
                                                                             trajectory_sectors[0]['length'],
                                                                             trajectory_sectors[0]['type'],
                                                                             calc_accuracy)

        trajectory_sectors[0]['end_pose']['x'] = x
        trajectory_sectors[0]['end_pose']['y'] = y
        trajectory_sectors[0]['end_pose']['yaw'] = yaw

        x, y, yaw, c, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[1]['angle'],
                                                                             trajectory_sectors[1]['radius'],
                                                                             trajectory_sectors[1]['length'],
                                                                             trajectory_sectors[1]['type'],
                                                                             calc_accuracy)

        x, y, yaw = clothoid_generator.transform(x, y, yaw, trajectory_sectors[0]['end_pose']['x'],
                                                            trajectory_sectors[0]['end_pose']['y'],
                                                            trajectory_sectors[0]['end_pose']['yaw'])

        trajectory_sectors[1]['end_pose']['x'] = x
        trajectory_sectors[1]['end_pose']['y'] = y
        trajectory_sectors[1]['end_pose']['yaw'] = yaw

        x, y, yaw, c, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[2]['angle'],
                                                                             trajectory_sectors[2]['radius'],
                                                                             trajectory_sectors[2]['length'],
                                                                             trajectory_sectors[2]['type'],
                                                                             calc_accuracy)

        x, y, yaw = clothoid_generator.transform(x, y, yaw, trajectory_sectors[1]['end_pose']['x'],
                                                            trajectory_sectors[1]['end_pose']['y'],
                                                            trajectory_sectors[1]['end_pose']['yaw'])

        trajectory_sectors[2]['end_pose']['x'] = x
        trajectory_sectors[2]['end_pose']['y'] = y
        trajectory_sectors[2]['end_pose']['yaw'] = yaw

        x, y, yaw, c, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[3]['angle'],
                                                                             trajectory_sectors[3]['radius'],
                                                                             trajectory_sectors[3]['length'],
                                                                             trajectory_sectors[3]['type'],
                                                                             calc_accuracy)

        x, y, yaw = clothoid_generator.transform(x, y, yaw, trajectory_sectors[2]['end_pose']['x'],
                                                            trajectory_sectors[2]['end_pose']['y'],
                                                            trajectory_sectors[2]['end_pose']['yaw'])

        trajectory_sectors[3]['end_pose']['x'] = x
        trajectory_sectors[3]['end_pose']['y'] = y
        trajectory_sectors[3]['end_pose']['yaw'] = yaw

        return trajectory_sectors


    def generate_0type_traj_using_4_clothoids(self, car,
                                                    min_turn_radius,
                                                    max_vel = 1.0,
                                                    acc=1.0,
                                                    dec=1.5,
                                                    frequency=100.0):

        max_clothoid_angle = copysign(pi/2, min_turn_radius)
        max_curvature = 1.0 / min_turn_radius                           # dtheta_max / ds
        sharpness = (max_curvature ** 2) / (2 * max_clothoid_angle)     # ddtheta / ds
        clothoid_length = abs(max_curvature / sharpness)
        calc_accuracy = 0.01

        clothoid_generator = self.ClothoidGenerator()

        trajectory_sectors = self.get_0type_traj_sectors_params(clothoid_generator,
                                                                max_clothoid_angle,
                                                                min_turn_radius,
                                                                clothoid_length,
                                                                calc_accuracy)

        trajectory = self.Trajectory()
        trajectory.length = clothoid_length * 4.0

        dec = float(copysign(dec, -1))
        acc = float(copysign(acc,  1))

        self.calc_trapezoidal_trajectory_linear_part(trajectory, max_vel, acc, dec, frequency)

        for point in trajectory.points:
            sector = int(point.l / clothoid_length)
            x, y, yaw, curv, sh = clothoid_generator.calc_x_y_yaw_by_curv_on_length(trajectory_sectors[sector]['angle'],
                                                                                    trajectory_sectors[sector]['radius'],
                                                                                    point.l - sector * clothoid_length,
                                                                                    trajectory_sectors[sector]['type'],
                                                                                    calc_accuracy)

            if (sector > 0):
                x, y, yaw = clothoid_generator.transform(x, y, yaw, trajectory_sectors[sector-1]['end_pose']['x'],
                                                                    trajectory_sectors[sector-1]['end_pose']['y'],
                                                                    trajectory_sectors[sector-1]['end_pose']['yaw'])

            turn_radius = 10000.0
            if (abs(curv) > 0.0001):
                turn_radius = 1. / curv

            steering = car.calc_steering_by_turn_radius(turn_radius)

            point.x = x
            point.y = y
            point.yaw = yaw
            point.steering = steering
            point.curvature = curv
            point.sharpness = sh

        return trajectory
