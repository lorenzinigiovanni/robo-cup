"""
Robot Class
"""

import time
from arduinoSerial import ArduinoSerial
from actuators.motor import Motor
from actuators.servo import Servo
from camera.camera import Camera
from field.area import Area
from field.maze import Maze
from sensors.color import Color
from sensors.distance import Distance
from sensors.gyroscope import Gyroscope
from sensors.temperature import Temperature


class Robot:
    ReturnTime = 360

    def __init__(self):
        self.Serial = ArduinoSerial('/dev/ttyACM0', 115200)  # TODO: Riconoscere seriale

        self.Color = Color(self.Serial)
        self.Distance = [Distance(self.Serial, i) for i in range(1, 4)]
        self.Gyroscope = Gyroscope(self.Serial)
        self.Temperature = [Temperature(self.Serial, i) for i in range(1, 4)]

        self.Camera = Camera()

        self.Motor = Motor(self.Serial)
        self.Servo = Servo(self.Serial)

        self.Sensors = [self.Color, self.Distance, self.Gyroscope, self.Temperature]

        self.Maze = Maze(self.Sensors, self.Servo, self.Camera)
        self.ActualX = self.Maze.StartX
        self.ActualY = self.Maze.StartY
        self.ActualZ = self.Maze.StartZ

        self.StartTime = 0
        self.StartHeading = 0

        self.stop()

    def start(self):
        print("Robot ready")

        """
        while True:
            pass  # TODO: Attendere pressione pulsante
        """

        self.StartTime = time.time()
        print("Start Time = " + str(self.StartTime))

        returnList = [-1, -1]
        movementCounter = 0

        while True:
            area = self.Maze.Areas[self.ActualX][self.ActualY][self.ActualZ]

            if not area.Scanned:
                area.scan()
                if self.Gyroscope.getPitch() == 0:
                    for i in range(4):
                        if area.Walls[i]:
                            self.turn(i)
                            area.findVisualVictim(i)
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
                if area.Walls[orientation] and not area.Victims[orientation].Present:
                    self.reset()
                    movementCounter = 0

            tmp = self.move(self.Maze.findPath(self.ActualX, self.ActualY, self.ActualZ))
            returnList.append(tmp)

            if tmp == 8:
                print("Stop Pressed")
                self.stop()

                """
                while True:
                    if self.ButtonStart.pressed():  # TODO: Attendere pressione pulsante
                        self.StartHeading = self.Gyroscope.getHeading()
                        break
                """
            else:
                print("Moved to X = " + str(self.ActualX) + " Y = " + str(self.ActualY) + " Z = " + str(self.ActualZ))
                movementCounter += 1
                if returnList[-1] == 9 and returnList[-2] == 9:
                    self.turnRight()

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

        self.stop()

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
        return self.Motor.forward()

    def turnRight(self):
        return self.Motor.right()

    def moveBackward(self):
        return self.Motor.reverse()

    def turnLeft(self):
        return self.Motor.left()

    def stop(self):
        self.ActualX = self.Maze.LastCheckPoint[0]
        self.ActualY = self.Maze.LastCheckPoint[1]
        self.ActualZ = self.Maze.LastCheckPoint[2]

        print("Return to X = " + str(self.ActualX) + " Y = " + str(self.ActualY) + " Z = " + str(self.ActualZ))

    def reset(self):
        self.Motor.reset()
