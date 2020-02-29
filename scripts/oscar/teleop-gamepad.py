import pygame
from cyber_py3 import cyber
from cyber_py3 import cyber_time
from modules.canbus.proto import chassis_pb2
from modules.control.proto import control_cmd_pb2

MAX_STEERING_PERCENTAGE = 65
MAX_THROTTLE = 50
MAX_BRAKE = 40

def callback_canbus(msg):
  global current_gear, current_speed
  current_gear = msg.gear_location
  current_speed = msg.speed_mps

def publish_control(params):
  global controlpub

  if params['estop']:
    controlcmd = control_cmd_pb2.ControlCommand()
    controlcmd.throttle = 0.0
    controlcmd.brake = 50.0
    controlcmd.header.timestamp_sec = cyber_time.Time.now().to_sec()
    controlpub.write(controlcmd)
    return

  controlcmd = control_cmd_pb2.ControlCommand()
  controlcmd.throttle = params['throttle_cmd']
  controlcmd.brake = params['brake_cmd']
  controlcmd.steering_target = params['target_steering']
  controlcmd.gear_location = params['gear']
  controlcmd.header.timestamp_sec = cyber_time.Time.now().to_sec()
  controlpub.write(controlcmd)



def main():

  params = {
'throttle_cmd' : 0.0,
'brake_cmd' : 0.0,
'target_steering' : 0.0,
'gear' : 1,
'estop' : False,
  }

  global controlpub, current_speed, current_gear
  current_speed = 0.0
  pygame.init()
  clock = pygame.time.Clock()
  joysticks = []
  for i in range(0, pygame.joystick.get_count()):
    joysticks.append(pygame.joystick.Joystick(i))
    joysticks[-1].init()

  cyber.init()
  node = cyber.Node("teleop_gamepad")
  canbussub = node.create_reader('/apollo/canbus/chassis',
                                       chassis_pb2.Chassis,
                                       callback_canbus)
  controlpub = node.create_writer('/apollo/control',
                                        control_cmd_pb2.ControlCommand)

  while 1:
    clock.tick(60)
    for event in pygame.event.get():
      
      # STICKS CALLBACKS
      if event.type == pygame.JOYAXISMOTION:

        # THROTTLE / BRAKE (LEFT STICK)
        if event.axis == 1:

          if event.value >= 0.0:
            if event.value * MAX_BRAKE >= 5.0:
              params['brake_cmd'] = event.value * MAX_BRAKE
            else:
              params['brake_cmd'] = 0.0


          else:
            if -event.value * MAX_BRAKE >= 5.0:
              params['throttle_cmd'] = -event.value * MAX_BRAKE
            else:
              params['throttle_cmd'] = 0.0

        # STEERING (RIGHT STICK)
        if event.axis == 3:
          params['target_steering'] = -event.value * MAX_STEERING_PERCENTAGE

      #HAT (ARROWS) MOTION CALLBACK
      if event.type == pygame.JOYHATMOTION:
        
        if event.value == (-1, 0):
          params['target_steering'] += 5

        if event.value == (1, 0):
          params['target_steering'] -= 5

        if event.value == (0, 1):
          params['throttle_cmd'] += 5
        
        if event.value == (0, -1):
          params['throttle_cmd'] -= 5


      # BUTTONS CALLBACKS
      if event.type == pygame.JOYBUTTONDOWN:
        
        # GEARS SWITCHING (L1 - R1)
        if event.button == 5:
          if current_speed < 0.03:
            if current_gear == 2:
              params['gear'] = 0
            if current_gear == 0:
              params['gear'] = 1
          
        if event.button == 4:
          if current_speed < 0.03:
            if current_gear == 1:
              params['gear'] = 0
            if current_gear == 0:
              params['gear'] = 2

        # ESTOP (B)
        if event.button == 1:
          params['estop'] = not(params['estop'])

        # EXIT (START)
        if event.button == 7:
          print("Process terminated")
          return
    #print(params)
    publish_control(params)
    
if __name__ == "__main__":
  main()