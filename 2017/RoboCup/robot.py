"""
Robot Class
"""

import math
import time
import pigpio
from actuators.led import Led
from actuators.motor import Motor
from actuators.servo import Servo
from camera.camera import Camera
from field.area import Area
from field.maze import Maze
from sensors.button import Button
from sensors.color import Color
from sensors.distance import Distance
from sensors.gyroscope import Gyroscope
from sensors.temperature import Temperature


class Robot:
    RobotDimensionX = 160
    RobotDimensionY = 150
    AreaDimension = 300
    ReturnTime = 360

    def __init__(self):
        gpio = pigpio.pi()

        self.Color = Color(7)
        self.Distance = [Distance(2), Distance(3), Distance(0), Distance(1)]
        self.Gyroscope = Gyroscope('/dev/ttyAMA0')
        self.Temperature = [Temperature(i) for i in range(85, 89)]

        self.Camera = Camera()

        self.LedRed = Led(gpio, 20)
        self.LedYellow = Led(gpio, 21)

        self.LedRed.off()
        self.LedYellow.off()

        self.ButtonStart = Button(gpio, 9)
        self.ButtonStop = Button(gpio, 10)

        self.Motor = [Motor(19, 11, 26), Motor(13, 6, 5)]
        self.Servo = Servo(gpio, 12)

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
        print("Robot ready")
        self.LedRed.on()

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
            time.sleep(0.1)
            t.calibrate()

        self.LedRed.off()

        returnList = [-1, -1]
        movementCounter = 0

        self._center(rear=0)

        while True:
            area = self.Maze.Areas[self.ActualX][self.ActualY][self.ActualZ]

            if not area.Scanned:
                area.scan()
                for i in range(4):
                    if area.Victims[i].Present:  # if area.Walls[i]:
                        self.turn(i)
                        # self._center()
                        # area.findVisualVictim(i)
                        area.Victims[i].save()

            if movementCounter > 10:
                orientation = self.Gyroscope.getOrientation()
                if orientation == 0:
                    orientation = 2
                elif orientation == 1:
                    orientation = 3
                elif orientation == 2:
                    orientation = 0
                elif orientation == 3:
                    orientation = 1
                if area.Walls[orientation]:
                    self._reset()
                    self._center(rear=0)
                    movementCounter = 0

            tmp = self.move(self.Maze.findPath(self.ActualX, self.ActualY, self.ActualZ))
            returnList.append(tmp)

            if self.ButtonStop.pressed() or tmp == 8:
                print("Stop Pressed")
                self.stop()

                while True:
                    if self.ButtonStart.pressed():
                        self.StartHeading = self.Gyroscope.getHeading()
                        break
            else:
                print("Moved to X = " + str(self.ActualX) + " Y = " + str(self.ActualY) + " Z = " + str(self.ActualZ))
                movementCounter += 1
                if returnList[-1] == 9 and returnList[-2] == 9:
                    self.turnRight()
                    self._center()

            if time.time() - self.StartTime > self.ReturnTime:
                break

        print("Searching for a returning home path")
        movementList = self.Maze.findReturnPath(self.ActualX, self.ActualY, self.ActualZ)

        if len(movementList) > 0:
            print("Returning home")
            print("Movement list = " + str(movementList))
            for movement in movementList:
                self.move(movement)
        else:
            print("Path not found")

        self._stop()
        self.cleanUp()

    def stop(self):
        self._stop()

        self.ActualX = self.Maze.LastCheckPoint[0]
        self.ActualY = self.Maze.LastCheckPoint[1]
        self.ActualZ = self.Maze.LastCheckPoint[2]

        print("Return to X = " + str(self.ActualX) + " Y = " + str(self.ActualY) + " Z = " + str(self.ActualZ))

    def move(self, direction):
        """
        Move the robot to absolute direction
        0 is X+
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
            return 9
        elif tmp == 4:
            self.Maze.Areas[self.ActualX][self.ActualY][self.ActualZ].Ramps[xy] = True
            self.ActualZ = 1 if self.ActualZ == 0 else 2
            if xy == 0:
                xy = 2
            elif xy == 1:
                xy = 3
            elif xy == 2:
                xy = 0
            elif xy == 3:
                xy = 1
            self.Maze.Areas[self.ActualX][self.ActualY][self.ActualZ].Ramps[xy] = True
            self.Maze.RampPassages += 1
            return 4
        elif tmp == 5:
            self.Maze.Areas[self.ActualX][self.ActualY][self.ActualZ].Ramps[xy] = True
            self.ActualZ = 0 if self.ActualZ == 1 else 1
            if xy == 0:
                xy = 2
            elif xy == 1:
                xy = 3
            elif xy == 2:
                xy = 0
            elif xy == 3:
                xy = 1
            self.Maze.Areas[self.ActualX][self.ActualY][self.ActualZ].Ramps[xy] = True
            self.Maze.RampPassages += 1
            return 5
        elif xy == 0:
            if tmp == 3:
                self.Maze.Areas[self.ActualX + 1][self.ActualY][self.ActualZ].Type = Area.AreaType.NoGo
            else:
                self.ActualX += 1
            return 0
        elif xy == 1:
            if tmp == 3:
                self.Maze.Areas[self.ActualX][self.ActualY + 1][self.ActualZ].Type = Area.AreaType.NoGo
            else:
                self.ActualY += 1
            return 1
        elif xy == 2:
            if tmp == 3:
                self.Maze.Areas[self.ActualX - 1][self.ActualY][self.ActualZ].Type = Area.AreaType.NoGo
            else:
                self.ActualX -= 1
            return 2
        elif xy == 3:
            if tmp == 3:
                self.Maze.Areas[self.ActualX][self.ActualY - 1][self.ActualZ].Type = Area.AreaType.NoGo
            else:
                self.ActualY -= 1
            return 3

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
        Kp = 2.5                # Constant by pitch for ramp
        Kd = 0.2                # Constant by distance error
        wallDistance = (self.AreaDimension - self.RobotDimensionX) // 2
        ramp = False            # Initially not a ramp
        tmp = 0                 # ramp return
        timeout = 5
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

        while True:
            actualDistance = dSensor.distance()
            if dSensor is not dWall:
                distanceWall = dWall.distance()
            else:
                distanceWall = actualDistance

            d1 = math.fmod(self.Distance[1].distance(), self.AreaDimension)
            d3 = math.fmod(self.Distance[3].distance(), self.AreaDimension)
            if d1 > 0 and d3 > 0:
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
                self._stop()
                return 8

            if time.time() - initialTime > timeout and not ramp:
                self._stop()
                return 9

            # TODO: tuning the pitch to recognise a ramp

            if pitch > 15:
                ramp = True
                tmp = 4 if direction else 5
            elif pitch < -15:
                ramp = True
                tmp = 5 if direction else 4

            if actualDistance > 0 and abs(initialDistance - actualDistance) >= abs(requiredDistance) and not ramp:
                self._stop()
                if center:
                    time.sleep(0.5)
                    self._center()
                self._stop()
                return 1

            if 0 < distanceWall < wallDistance:
                self._stop()
                if center:
                    time.sleep(0.5)
                    self._center()
                self._stop()
                return 2

            if control and not ramp:
                if -5 <= pitch <= 5 and self.Color.isBlack():
                    self._stop()
                    self._move(abs(initialDistance - dSensor.distance()), speed, not direction, control=False)
                    self._stop()
                    return 3

            if ramp and abs(pitch) < 5:
                self._move(150, speed, direction, control=False, center=False)
                self._stop()
                return tmp

    def _center(self, front=-1, rear=-1):
        d0 = [0, 0]
        d2 = [0, 0]
        for i in range(2):
            if front > 0:
                d0[i] = front
            else:
                d0[i] = math.fmod(self.Distance[0].distance(), self.AreaDimension)
            if rear > 0:
                d2[i] = rear
            else:
                d2[i] = math.fmod(self.Distance[2].distance(), self.AreaDimension)

        if d0[0] > 0 and d0[1] > 0 and d2[0] > 0 and d2[1] > 0:
            errorDistance = d0[0] + d0[1] - d2[0] - d2[1]
        elif 0 < int(d0[0]) < 1000 and 0 < int(d0[1]) < 1000:
            tmp = self.AreaDimension - self.RobotDimensionX
            errorDistance = d0[0] + d0[1] - tmp
        elif 0 < int(d2[0]) < 1000 and 0 < int(d2[1]) < 1000:
            tmp = self.AreaDimension - self.RobotDimensionX
            errorDistance = tmp - d2[0] - d2[1]
        else:
            self._stop()
            return -1

        direction = True if errorDistance >= 0 else False

        if -15 < errorDistance < 15:
            self._stop()
            return 0

        errorDistance /= 4

        self._move(abs(errorDistance), 20, direction, control=False, center=False)
        return 1

    def _reset(self):
        self.Motor[0].reverse()
        self.Motor[1].reverse()

        self.Motor[0].speed(80)
        self.Motor[1].speed(80)

        time.sleep(2)

        self.Motor[0].stop()
        self.Motor[1].stop()

        time.sleep(1)

        self.StartHeading = self.Gyroscope.getHeading()

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
                self._stop()
                return 8

            if time.time() - initialTime > timeout:
                self._stop()
                return 9

            if abs(initialHeading - actualHeading) >= abs(requiredHeading):
                self._stop()
                if control:
                    time.sleep(0.5)
                    self._turn((requiredHeading + initialHeading - self.Gyroscope.getHeading()), 50, control=False)
                self._stop()
                return 1

    def _stop(self):
        """Stop the robot"""
        self.Motor[0].stop()
        self.Motor[1].stop()

    def cleanUp(self):
        self._stop()
        self.Servo.stop()
        self.LedRed.off()
        self.LedYellow.off()
