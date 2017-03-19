import RPi.GPIO as GPIO
from sensors.color import Color
from sensors.distance import Distance
from sensors.gyroscope import Gyroscope
from sensors.temperature import Temperature
from actuators.led import Led
from actuators.motor import Motor
from actuators.servo import Servo
from field.maze import Maze
import time


class Robot:
    actualX = 25
    actualY = 25
    actualZ = 0
    startTime = 0

    def __init__(self):
        GPIO.setmode(GPIO.BCM)
        self.Color = Color(7)
        self.Distance = [Distance(i) for i in range(4)]
        self.Gyroscope = Gyroscope()
        self.Temperature = [Temperature(i) for i in range(85, 89)]
        self.Led = Led(20)
        self.Motor = [Motor(13, 5, 6), Motor(19, 26, 16)]
        self.Servo = Servo(12)
        self.Sensors = [self.Color, self.Distance, self.Gyroscope, self.Temperature]
        self.Actuators = [self.Led, self.Servo]
        self.Maze = Maze(self.Sensors, self.Actuators)

    def start(self):
        self.startTime = time.time()
        
        while True:
            self.move(self.Maze.findPath(self.actualX, self.actualY, self.actualZ))

    def move(self, direction):
        """
        Move the robot to absolute direction
        0 is  X+
        1 is Y+
        2 is X-
        3 is Y-
        """
        tmp = 0
        xy = direction
        direction -= self.Gyroscope.getOrientation()

        if direction == -3:
            self.turnRight()
            tmp = self.moveForward()
        elif direction == -2:
            tmp = self.moveBackward()
        elif direction == -1:
            self.turnLeft()
            tmp = self.moveForward()
        elif direction == 0:
            tmp = self.moveForward()
        elif direction == 1:
            self.turnRight()
            tmp = self.moveForward()
        elif direction == 2:
            tmp = self.moveBackward()
        elif direction == 3:
            self.turnLeft()
            tmp = self.moveForward()

        if tmp == 3:
            pass
        elif tmp == 4:
            self.actualZ = 1
        elif tmp == 5:
            self.actualZ = 0
        elif xy == 0:
            self.actualX += 1
        elif xy == 1:
            self.actualY += 1
        elif xy == 2:
            self.actualX -= 1
        elif xy == 3:
            self.actualY -= 1

    def turn(self, direction):
        """
        Turn the robot to absolute direction
        0 is powerOn front
        1 is powerOn right
        2 is powerOn back
        3 is powerOn left
        """
        direction -= self.Gyroscope.getOrientation()

        if direction == -3:
            self.turnRight()
        elif direction == -2:
            self.turnRight()
            self.turnRight()
        elif direction == -1:
            self.turnLeft()
        elif direction == 0:
            pass
        elif direction == 1:
            self.turnRight()
        elif direction == 2:
            self.turnLeft()
            self.turnLeft()
        elif direction == 3:
            self.turnLeft()

    def moveForward(self):
        return self._move(requiredDistance=300, speed=50, direction=True)

    def turnRight(self):
        return self._turn(requiredHeading=90, speed=50)

    def moveBackward(self):
        return self._move(requiredDistance=300, speed=50, direction=False)

    def turnLeft(self):
        return self._turn(requiredHeading=-90, speed=50)

    def _move(self, requiredDistance, speed, direction, control=True):
        """
        Move the robot straight
        requiredDistance is always positive
        speed is the medium speed of the motors always positive
        direction must be True to go forward and False to go reverse
        control is True by default and is used internally
        Return 1 for distance OK, 2 for wall stop, 3 for black, 4 for up ramp, 5 for down ramp
        """
        Kh = 5                  # Constant by heading error
        Kp = 2                  # Constant by pitch for ramp
        wallDistance = 40       # Minimum distance by robot and wall in mm
        ramp = False            # Initially not a ramp
        tmp = 0                 # ramp return

        initialHeading = self.Gyroscope.getHeading()
        dSensor = self.Distance[2] if self.Distance[0].distance() > self.Distance[2].distance() else self.Distance[0]
        initialDistance = dSensor.distance()

        if direction:
            self.Motor[0].forward()
            self.Motor[1].forward()
        else:
            self.Motor[0].reverse()
            self.Motor[1].reverse()
            Kh = -Kh

        while True:
            actualDistance = dSensor.distance()
            errorHeading = initialHeading - self.Gyroscope.getHeading()
            pitch = self.Gyroscope.getPitch()

            self.Motor[0].speed(speed + errorHeading * Kh + pitch * Kp)
            self.Motor[1].speed(speed - errorHeading * Kh + pitch * Kp)

            if pitch > 15:
                ramp = True
                tmp = 4
            elif pitch < -15:
                ramp = True
                tmp = 5

            if actualDistance != -1 and not ramp:
                if abs(initialDistance - actualDistance) >= abs(requiredDistance):
                    self._stop()
                    return 1
                elif actualDistance < wallDistance:
                    self._stop()
                    return 2

            if control and not ramp:
                if -5 <= pitch <= 5 and self.Color.isBlack():
                    self._stop()
                    self._move(abs(initialDistance - dSensor.distance()), speed, not direction, False)
                    return 3

            if ramp and abs(pitch) < 5:
                self._stop()
                return tmp

    def _turn(self, requiredHeading, speed):
        """
        Turn the robot on it pivot
        requiredHeading is the angle of the turn and can be positive or negative
        speed is the medium speed of the motors always positive
        Return 1 for turn OK
        """
        Kh = 0.5                  # Constant by heading

        initialHeading = self.Gyroscope.getHeading()

        if requiredHeading < 0:
            self.Motor[0].reverse()
            self.Motor[1].forward()
        else:
            self.Motor[0].forward()
            self.Motor[1].reverse()

        while True:
            actualHeading = self.Gyroscope.getHeading()
            errorHeading = abs(requiredHeading + initialHeading - actualHeading)

            self.Motor[0].speed(speed + errorHeading * Kh)
            self.Motor[1].speed(speed + errorHeading * Kh)

            if abs(initialHeading - actualHeading) >= abs(requiredHeading):
                self._stop()
                return 1

    def _stop(self):
        """Stop the robot"""
        self.Motor[0].stop()
        self.Motor[1].stop()
