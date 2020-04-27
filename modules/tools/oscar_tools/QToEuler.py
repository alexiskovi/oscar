from math import pi, atan2, asin
q = [float(i) for i in raw_input('w, x, y, z: ').split()]

w = q[0]
x = q[1]
y = q[2]
z = q[3]

sqw = w*w
sqx = x*x
sqy = y*y
sqz = z*z

eulerZ = (atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw)) * (180/pi))
eulerX = (atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw)) * (180/pi))
eulerY = (asin(-2.0 * (x*z - y*w)) * (180/pi))

print ('yaw = {}'.format(eulerZ))
print ('pitch = {}'.format(eulerX))
print ('roll = {}'.format(eulerY))
