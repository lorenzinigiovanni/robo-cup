"""
Robot Class
"""

import RPi.GPIO as GPIO
from sensors.button import Button
from sensors.color import Color
from sensors.distance import Distance
from sensors.gyroscope import Gyroscope
from sensors.temperature import Temperature
from actuators.led import Led
from actuators.motor import Motor
from actuators.servo import Servo
from field.maze import Maze
from field.area import Area
import time


class Robot:
    actualX = 25
    actualY = 25
    actualZ = 0
    startTime = 0

    def __init__(self):
        GPIO.setmode(GPIO.BCM)
        self.Color = Color(7)
        self.Distance = [Distance(2), Distance(3), Distance(0), Distance(1)]
        self.Gyroscope = Gyroscope()
        self.Temperature = [Temperature(i) for i in range(85, 89)]
        self.Led1 = Led(20)
        self.Led2 = Led(21)
        self.Button1 = Button(9)
        self.Button2 = Button(10)
        self.Motor = [Motor(19, 11, 26), Motor(13, 6, 5)]
        self.Servo = Servo(12)
        self.Sensors = [self.Color, self.Distance, self.Gyroscope, self.Temperature]
        self.Actuators = [self.Led1, self.Servo]
        self.Maze = Maze(self.Sensors, self.Actuators)

    def start(self):
        while True:
            if self.Gyroscope.isCalibrated()[0] == 3:
                self.Led2.on()
            else:
                self.Led2.off()
            print(self.Gyroscope.isCalibrated())

            # TODO: add temperature sensors calibration? t victim - t initial wall

            if self.Button1.pressed():
                break

        self.startTime = time.time()
        
        while True:
            self.move(self.Maze.findPath(self.actualX, self.actualY, self.actualZ))
            print("moved")

            if self.Button2.pressed():
                self.actualX = self.Maze.LastCheckPoint[0]
                self.actualY = self.Maze.LastCheckPoint[1]
                self.actualZ = self.Maze.LastCheckPoint[2]

                print("return checkpoint: " + str(self.actualX) + " " + str(self.actualY) + " " + str(self.actualZ))

                while True:
                    if self.Button1.pressed():
                        break

            # if self.startTime - time.time() > 500:
                # TODO: return to start cell after an amount of time
                # pass

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

        if tmp == 4:
            self.actualZ = 1
        elif tmp == 5:
            self.actualZ = 0
        elif xy == 0:
            if tmp == 3:
                self.Maze.Areas[self.actualX + 1][self.actualY][self.actualZ].Type = Area.AreaType.NoGo
            else:
                self.actualX += 1
        elif xy == 1:
            if tmp == 3:
                self.Maze.Areas[self.actualX][self.actualY + 1][self.actualZ].Type = Area.AreaType.NoGo
            else:
                self.actualY += 1
        elif xy == 2:
            if tmp == 3:
                self.Maze.Areas[self.actualX - 1][self.actualY][self.actualZ].Type = Area.AreaType.NoGo
            else:
                self.actualX -= 1
        elif xy == 3:
            if tmp == 3:
                self.Maze.Areas[self.actualX][self.actualY - 1][self.actualZ].Type = Area.AreaType.NoGo
            else:
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

    # TODO: tuning movimentation method (speed and distance)

    def moveForward(self):
        return self._move(requiredDistance=280, speed=75, direction=True)

    def turnRight(self):
        return self._turn(requiredHeading=88, speed=75)

    def moveBackward(self):
        return self._move(requiredDistance=280, speed=75, direction=False)

    def turnLeft(self):
        return self._turn(requiredHeading=-88, speed=75)

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
        wallDistance = 60       # Minimum distance by robot and wall in mm
        ramp = False            # Initially not a ramp
        tmp = 0                 # ramp return

        initialHeading = self.Gyroscope.getHeading()

        d0 = self.Distance[0].distance()
        d2 = self.Distance[2].distance()

        if d0 > 0 and d2 > 0:
            dSensor = self.Distance[0] if d0 < d2 else self.Distance[2]
        elif d2 > 0:
            dSensor = self.Distance[2]
        else:
            dSensor = self.Distance[0]

        dWall = self.Distance[0] if direction else self.Distance[2]

        initialDistance = dSensor.distance()

        # TODO: check if the right distance sensor is chosen

        if direction:
            self.Motor[0].forward()
            self.Motor[1].forward()
        else:
            self.Motor[0].reverse()
            self.Motor[1].reverse()
            Kh = -Kh

        while True:
            actualDistance = dSensor.distance()
            distanceWall = dWall.distance()
            errorHeading = initialHeading - self.Gyroscope.getHeading()
            pitch = self.Gyroscope.getPitch()

            self.Motor[0].speed(speed + errorHeading * Kh + pitch * Kp)
            self.Motor[1].speed(speed - errorHeading * Kh + pitch * Kp)

            # TODO: check the heading correction and the pitch correction

            # TODO: tuning the pitch to recognise a ramp

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
                elif 0 < distanceWall < wallDistance:
                    self._stop()
                    return 2

            if control and not ramp:
                if -5 <= pitch <= 5 and self.Color.isBlack():
                    self._stop()
                    self._move(abs(initialDistance - dSensor.distance()), speed, not direction, False)
                    return 3

            # TODO: check that happen with a black area

            if ramp and abs(pitch) < 5:
                self._move(75, speed, True, False)
                self._stop()
                return tmp

    def _turn(self, requiredHeading, speed):
        """
        Turn the robot on it pivot
        requiredHeading is the angle of the turn and can be positive or negative
        speed is the medium speed of the motors always positive
        Return 1 for turn OK
        """
        Kh = 1                  # Constant by heading

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
