from math import sin, cos, pi
import argparse

def main(args):
    q = [0, 0, 0, 0]

    rol, yaw, pit = [float(i) for i in raw_input('roll, yaw, pitch in degrees: ').split()]
    #yaw pit rol
    rol*=pi/180
    yaw*=pi/180
    pit*=pi/180

    c1 = cos(0.5*rol)
    c2 = cos(0.5*yaw)
    c3 = cos(0.5*pit)
    s1 = sin(0.5*rol)
    s2 = sin(0.5*yaw)
    s3 = sin(0.5*pit)

    q[0] = s1*s2*c3 + c1*c2*s3
    q[1] = s1*c2*c3 + c1*s2*s3
    q[2] = c1*s2*c3 - s1*c2*s3
    q[3] = c1*c2*c3 - s1*s2*s3

    print("""x: {0};
y: {1};
z: {2};
w: {3};""".format(q[0], q[1], q[2], q[3]))

    if args.yaml:
        device_type = raw_input('Child-frame: ')
        null_device = raw_input('Null-frame: ')
        x_m, y_m, z_m = [float(i) for i in raw_input('x, y, z shift in meters, divided by space: ').split()]
        yaml = open('{}_extrinsics.yaml'.format(device_type),'w')

        yaml.write("""header:
  seq: 0
  stamp:
    secs: 0
    nsecs: 0
  frame_id: {0}
child_frame_id: {1}
transform:
  rotation:
    x: {2}
    y: {3}
    z: {4}
    w: {5}
  translation:
    x: {6}
    y: {7}
    z: {8}""".format(null_device, device_type, q[0], q[1], q[2], q[3], x_m, y_m, z_m))

        yaml.close()

def parse_args():
    parser = argparse.ArgumentParser(description='Euler angles to Quaternion')
    parser.add_argument('-y', '--yaml', action='store_true', help='Generate yaml_extrinsics file')
    return parser.parse_args()

if __name__ == '__main__':
    main(parse_args())
