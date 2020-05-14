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
""" All rutines with trajectory file io operations """

# TODO!: Nice error checking, too kludged.

import os
import sys
import time

from shutil import copyfile
import json, uuid
from collections import OrderedDict as odict
from numpy import genfromtxt

from trajectories import Trajectory, Point


class NoIndent(object):
    def __init__(self, value):
        self.value = value


class NoIndentEncoder(json.JSONEncoder):
    def __init__(self, *args, **kwargs):
        super(NoIndentEncoder, self).__init__(*args, **kwargs)
        self.kwargs = dict(kwargs)
        del self.kwargs['indent']
        self._replacement_map = {}

    def default(self, o):
        if isinstance(o, NoIndent):
            key = uuid.uuid4().hex
            self._replacement_map[key] = json.dumps(o.value, **self.kwargs)
            return "@@%s@@" % (key,)
        else:
            return super(NoIndentEncoder, self).default(o)

    def encode(self, o):
        result = super(NoIndentEncoder, self).encode(o)
        for k, v in self._replacement_map.iteritems():
            result = result.replace('"@@%s@@"' % (k,), v)
        return result


class TrajectoryFileHandler():

    def __init__(self, file_path = None):
        self.handler = None
        self.file_path = file_path


    def set_file(self, file_path):
        self.file_path = file_path


    def open_file_to_write(self):
        return self.open_file('w')


    def open_file_to_read(self):
        return self.open_file('r')


    def open_file(self, mode):

        if not self.file_path:
            print("File is not set!")
            return False

        try:
            self.handler = open(self.file_path, mode)
        except Exception as err:
            print("Can't open file " + str(self.file_path))
            print(err)
            return False

        return True


    def close_file(self):
        if (self.handler):
            self.handler.flush()
            self.handler.close()
            self.handler = None


    def check_for_old_and_make_backup(self):
        if (self.file_path):
            if (os.path.isfile(self.file_path)):
                mtime_since_epoc = os.path.getmtime(self.file_path)
                file_last_mdata = time.strftime('%Y-%m-%d-%H:%M:%S', time.localtime(mtime_since_epoc))
                new_name_for_prev_file = self.file_path[:-4] + "-" + file_last_mdata + ".csv"
                copyfile(self.file_path, new_name_for_prev_file)


    def load_trajectory(self, from_rtk_recorder=False):

        if not self.open_file_to_read():
            return None

        trajectory = Trajectory()

        if (from_rtk_recorder):
            try:
                trajectory_raw = genfromtxt(self.handler, delimiter=',', names=True)
            except Exception as err:
                print("Can't load trajectory from file: " + str(self.file_path))
                print(err)
                return None

            for point_raw in trajectory_raw:
                point = Point()
                point.x = point_raw['x']
                point.y = point_raw['y']
                point.z = point_raw['z']
                point.v = point_raw['speed']
                point.a = point_raw['acceleration']
                point.curvature = point_raw['curvature']
                point.dcurvature = point_raw['curvature_change_rate']
                point.t = point_raw['time']
                point.yaw = point_raw['theta']
                point.l = point_raw['s']
                trajectory.add_point(point)

            # trajectory.points_num = len(trajectory_raw)
            trajectory.time = trajectory.points[-1].t - trajectory.points[0].t
            trajectory.length = trajectory.points[-1].l - trajectory.points[0].l

        else:
            try:
                trajectory_raw = json.load(self.handler)
            except Exception as err:
                print("Can't load trajectory from file: " + str(self.file_path))
                print(err)
                return None

            trajectory.length = trajectory_raw['header']['length']
            trajectory.time = trajectory_raw['header']['time']
            trajectory.type = trajectory_raw['header']['type']
            # trajectory.points_num = trajectory_raw['header']['size']
            for point_raw in trajectory_raw['points']:
                point = Point()
                point.x = float(point_raw['x'])
                point.y = float(point_raw['y'])
                point.z = float(point_raw['z'])
                point.v = float(point_raw['v'])
                point.a = float(point_raw['a'])
                point.curvature = float(point_raw['c'])
                point.sharpness = float(point_raw['sh'])
                point.t = float(point_raw['t'])
                point.yaw = float(point_raw['yaw'])
                point.steering = float(point_raw['st'])
                point.l = float(point_raw['l'])
                trajectory.add_point(point)

        self.close_file()
        return trajectory


    def save_trajectory(self, trajectory, rtk_player_compatible=False):

        self.check_for_old_and_make_backup()

        if not self.open_file_to_write():
            return False

        if (rtk_player_compatible):
            self.handler.write("x,y,z,speed,acceleration,curvature,curvature_change_rate," + \
                       "time,theta,gear,s,throttle,brake,steering\n")

            for point in trajectory.points:

                gear = 1
                if (point.v < 0.0):
                    gear = 2

                steering = 0
                throttle = 0
                brake    = 0

                self.handler.write(str(format(point.x, '.2f')) + ",\t" + \
                                   str(format(point.y, '.2f')) + ",\t" + \
                                   str(format(point.z, '.2f')) + ",\t" + \
                                   str(format(point.v, '.2f')) + ",\t" + \
                                   str(format(point.a, '.2f')) + ",\t" + \
                                   str(format(point.curvature, '.3f')) + ",\t" + \
                                   str(format(point.dcurvature, '.3f')) + ",\t" + \
                                   str(format(point.t, '.2f')) + ",\t" + \
                                   str(format(point.yaw, '.3f')) + ",\t" + \
                                   str(gear) + ",\t" + \
                                   str(format(point.l, '.2f')) + ",\t" + \
                                   str(throttle) + ",\t" + \
                                   str(brake) + ",\t" + \
                                   str(steering) + "\n")

        else:
            traj = odict([
                ('header', odict([('type',   trajectory.type),
                                       ('time',   trajectory.time),
                                       ('length', trajectory.length),
                                       ('size',   trajectory.points_num)])),
                ('points', [])
            ])

            i = 0
            for point in trajectory.points:
                traj['points'].append(NoIndent(odict([('id', i),
                                                      ('t', format(point.t, '.2f')),
                                                      ('l', format(point.l, '.2f')),
                                                      ('x', format(point.x, '.2f')),
                                                      ('y', format(point.y, '.2f')),
                                                      ('z', format(point.z, '.2f')),
                                                      ('v', format(point.v, '.2f')),
                                                      ('a', format(point.a, '.2f')),
                                                      ('yaw', format(point.yaw, '.3f')),
                                                      ('st', format(point.steering, '.3f')),
                                                      ('c', format(point.curvature, '.4f')),
                                                      ('sh', format(point.sharpness, '.4f'))])))

                i += 1

            self.handler.write(json.dumps(traj, indent=2, cls=NoIndentEncoder))

        self.close_file()
        return True
