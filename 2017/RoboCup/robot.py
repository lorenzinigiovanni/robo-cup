"""
Robot Class
"""

import math
import time
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
from camera.camera import Camera


class Robot:
    RobotDimensionX = 160
    RobotDimensionY = 150
    AreaDimension = 300

    def __init__(self):
        GPIO.setmode(GPIO.BCM)

        self.Color = Color(7)
        self.Distance = [Distance(2), Distance(3), Distance(0), Distance(1)]
        self.Gyroscope = Gyroscope('/dev/ttyAMA0')
        self.Temperature = [Temperature(i) for i in range(85, 89)]

        self.Camera = Camera()

        self.LedRed = Led(20)
        self.LedYellow = Led(21)

        self.ButtonStart = Button(9)
        self.ButtonStop = Button(10)

        self.Motor = [Motor(19, 11, 26), Motor(13, 6, 5)]
        self.Servo = Servo(12)

        self.Sensors = [self.Color, self.Distance, self.Gyroscope, self.Temperature]
        self.Actuators = [self.LedRed, self.Servo]

        self.Maze = Maze(self.Sensors, self.Actuators, self.Camera)
        self.ActualX = self.Maze.StartX
        self.ActualY = self.Maze.StartY
        self.ActualZ = self.Maze.StartZ

        self.StartTime = 0
        self.StartHeading = 0

        self._stop()

    def start(self):
        while True:
            if self.Gyroscope.isCalibrated():
                self.LedYellow.on()
            else:
                self.LedYellow.off()

            if self.ButtonStart.pressed():
                break

        self.StartTime = time.time()
        print("Start Time = " + str(self.StartTime))

        self.StartHeading = self.Gyroscope.getHeading()
        print("Start Heading = " + str(self.StartHeading))

        print("Calibrating temperature sensors")
        for t in self.Temperature:
            t.calibrate()

        while True:
            area = self.Maze.Areas[self.ActualX][self.ActualY][self.ActualZ]

            if not area.Scanned:
                area.scan()
                for i in range(4):
                    if area.Walls[i]:
                        self.turn(i)
                        area.findVisualVictim(i)
                        area.Victims[i].save()

            tmp = self.move(self.Maze.findPath(self.ActualX, self.ActualY, self.ActualZ))

            if self.ButtonStop.pressed() or tmp == 8:
                print("Stop Pressed")
                self.stop()

                while True:
                    if self.ButtonStart.pressed():
                        break
            else:
                print("Moved to X = " + str(self.ActualX) + " Y = " + str(self.ActualY) + " Z = " + str(self.ActualZ))

            if self.StartTime - time.time() > 420:
                break

        print("Searching for a returning home path")
        movementList = self.Maze.findReturnPath(self.ActualX, self.ActualY, self.ActualZ)
        if len(movementList) > 0:
            print("Returning home")
            for movement in movementList:
                self.move(movement)
        else:
            print("Path not found")
        self.stop()

    def stop(self):
        self._stop()

        self.ActualX = self.Maze.LastCheckPoint[0]
        self.ActualY = self.Maze.LastCheckPoint[1]
        self.ActualZ = self.Maze.LastCheckPoint[2]

        print("Return to X = " + str(self.ActualX) + " Y = " + str(self.ActualY) + " Z = " + str(self.ActualZ))

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

        orientation = self.Gyroscope.getOrientation()
        if orientation == -1:
            return -1
        else:
            direction -= orientation

        if direction == -3:
            if self.turnRight() == 8:
                return 8
            tmp = self.moveForward()
        elif direction == -2:
            tmp = self.moveBackward()
        elif direction == -1:
            if self.turnLeft() == 8:
                return 8
            tmp = self.moveForward()
        elif direction == 0:
            tmp = self.moveForward()
        elif direction == 1:
            if self.turnRight() == 8:
                return 8
            tmp = self.moveForward()
        elif direction == 2:
            tmp = self.moveBackward()
        elif direction == 3:
            if self.turnLeft() == 8:
                return 8
            tmp = self.moveForward()

        if tmp == 8:
            return 8
        elif tmp == 9:
            print("Movement Timeout")
        elif tmp == 4:
            self.Maze.Areas[self.ActualX][self.ActualY][self.ActualZ].Ramps[xy] = True
            self.ActualZ = 1 if self.ActualZ == 0 else 2
            self.Maze.RampPassages += 1
        elif tmp == 5:
            self.Maze.Areas[self.ActualX][self.ActualY][self.ActualZ].Ramps[xy] = True
            self.ActualZ = 0 if self.ActualZ == 1 else 1
            self.Maze.RampPassages += 1
        elif xy == 0:
            if tmp == 3:
                self.Maze.Areas[self.ActualX + 1][self.ActualY][self.ActualZ].Type = Area.AreaType.NoGo
            else:
                self.ActualX += 1
        elif xy == 1:
            if tmp == 3:
                self.Maze.Areas[self.ActualX][self.ActualY + 1][self.ActualZ].Type = Area.AreaType.NoGo
            else:
                self.ActualY += 1
        elif xy == 2:
            if tmp == 3:
                self.Maze.Areas[self.ActualX - 1][self.ActualY][self.ActualZ].Type = Area.AreaType.NoGo
            else:
                self.ActualX -= 1
        elif xy == 3:
            if tmp == 3:
                self.Maze.Areas[self.ActualX][self.ActualY - 1][self.ActualZ].Type = Area.AreaType.NoGo
            else:
                self.ActualY -= 1

    def turn(self, direction):
        """
        Turn the robot to absolute direction
        0 is powerOn front
        1 is powerOn right
        2 is powerOn back
        3 is powerOn left
        """

        orientation = self.Gyroscope.getOrientation()
        if orientation == -1:
            return -1
        else:
            direction -= orientation

        if direction == -3:
            if self.turnRight() == 8:
                return 8
        elif direction == -2:
            if self.turnRight() == 8:
                return 8
            if self.turnRight() == 8:
                return 8
        elif direction == -1:
            if self.turnLeft() == 8:
                return 8
        elif direction == 0:
            pass
        elif direction == 1:
            if self.turnRight() == 8:
                return 8
        elif direction == 2:
            if self.turnLeft() == 8:
                return 8
            if self.turnLeft() == 8:
                return 8
        elif direction == 3:
            if self.turnLeft() == 8:
                return 8

    def moveForward(self):
        return self._move(requiredDistance=self.AreaDimension, speed=75, direction=True)

    def turnRight(self):
        return self._turn(requiredHeading=90, speed=75)

    def moveBackward(self):
        return self._move(requiredDistance=self.AreaDimension, speed=75, direction=False)

    def turnLeft(self):
        return self._turn(requiredHeading=-90, speed=75)

    def _move(self, requiredDistance, speed, direction, control=True, center=True):
        """
        Move the robot straight
        requiredDistance is always positive
        speed is the medium speed of the motors always positive
        direction must be True to go forward and False to go reverse
        control is True by default and is used internally
        Return 1 for distance OK, 2 for wall stop, 3 for black, 4 for up ramp, 5 for down ramp, 9 for timeout
        """
        Kh = 5                  # Constant by heading error
        Kp = 2                  # Constant by pitch for ramp
        Kd = 0.2                # Constant by distance error
        wallDistance = (self.AreaDimension - self.RobotDimensionX) // 2
        ramp = False            # Initially not a ramp
        tmp = 0                 # ramp return
        timeout = 10
        initialTime = time.time()

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

        if direction:
            self.Motor[0].forward()
            self.Motor[1].forward()
        else:
            self.Motor[0].reverse()
            self.Motor[1].reverse()
            Kh = -Kh
            Kp = -Kp
            Kd = -Kd

        while True:
            actualDistance = dSensor.distance()
            if dSensor is not dWall:
                distanceWall = dWall.distance()
            else:
                distanceWall = actualDistance

            d1 = math.fmod(self.Distance[1].distance(), self.AreaDimension)
            d3 = math.fmod(self.Distance[3].distance(), self.AreaDimension)
            if d1 != -1 and d3 != -1:
                errorDistance = d1 - d3
            else:
                errorDistance = 0

            actualHeading = self.Gyroscope.getHeading()
            errorHeading = math.fmod(self.StartHeading - actualHeading, 90)

            if errorHeading > 45:
                errorHeading -= 90
            elif errorHeading < -45:
                errorHeading += 90

            pitch = self.Gyroscope.getPitch()

            self.Motor[0].speed(speed + errorHeading * Kh + pitch * Kp + errorDistance * Kd)
            self.Motor[1].speed(speed - errorHeading * Kh + pitch * Kp - errorDistance * Kd)

            if self.ButtonStop.pressed():
                return 8

            if time.time() - initialTime > timeout and not ramp:
                return 9

            # TODO: tuning the pitch to recognise a ramp

            if pitch > 15:
                ramp = True
                tmp = 4
            elif pitch < -15:
                ramp = True
                tmp = 5

            if actualDistance != -1 and abs(initialDistance - actualDistance) >= abs(requiredDistance) and not ramp:
                self._stop()
                if center:
                    self._center()
                return 1

            if 0 < distanceWall < wallDistance:
                self._stop()
                if center:
                    self._center()
                return 2

            if control and not ramp:
                if -5 <= pitch <= 5 and self.Color.isBlack():
                    self._stop()
                    self._move(abs(initialDistance - dSensor.distance()), speed, not direction, control=False)
                    return 3

            if ramp and abs(pitch) < 5:
                self._move(150, speed, True, control=False, center=False)
                self._stop()
                return tmp

    def _center(self):
        d0 = math.fmod(self.Distance[0].distance(), self.AreaDimension)
        d2 = math.fmod(self.Distance[2].distance(), self.AreaDimension)
        if d0 != -1 and d0 < 200 and d2 != -1 and d2 < 200:
            errorDistance = d0 - d2
        else:
            return -1

        if errorDistance > 0:
            direction = True
        elif errorDistance < 0:
            direction = False
        else:
            return 0

        errorDistance /= 2

        self._move(abs(errorDistance), 50, direction, control=False, center=False)
        return 1

    def _turn(self, requiredHeading, speed, control=True):
        """
        Turn the robot on it pivot
        requiredHeading is the angle of the turn and can be positive or negative
        speed is the medium speed of the motors always positive
        Return 1 for turn OK, 9 for timeout
        """
        Kh = 1                  # Constant by heading
        timeout = 2.5
        initialTime = time.time()

        initialHeading = self.Gyroscope.getHeading()

        print("Initial Heading = " + str(initialHeading))

        if control:
            start = math.fmod(initialHeading - self.StartHeading, requiredHeading)
            start = start if requiredHeading > 0 else -start

            if start > (abs(requiredHeading) / 2):
                start -= abs(requiredHeading)
            elif start < -(abs(requiredHeading) / 2):
                start += abs(requiredHeading)

            if requiredHeading >= 0:
                requiredHeading -= start
            elif requiredHeading < 0:
                requiredHeading += start

        print("Required Heading = " + str(requiredHeading))

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

            if self.ButtonStop.pressed():
                return 8

            if time.time() - initialTime > timeout:
                return 9

            if abs(initialHeading - actualHeading) >= abs(requiredHeading):
                self._stop()
                if control:
                    time.sleep(0.5)
                    self._turn((requiredHeading + initialHeading - self.Gyroscope.getHeading()), 60, False)
                return 1

    def _stop(self):
        """Stop the robot"""
        self.Motor[0].stop()
        self.Motor[1].stop()

    def cleanUp(self):
        self._stop()
        self.Servo.stop()
        self.LedRed.off()
        self.LedRed.off()
