"""
Area Class
"""

import time
from enum import Enum
from field.victim import Victim


class Area:
    AreaType = Enum('AreaType', 'Standard CheckPoint NoGo')

    Scanned = False
    Type = AreaType.Standard
    Walls = []
    Ramps = []
    Victims = []

    X = -1
    Y = -1
    Z = -1

    Passages = 0

    WallDistance = 150          # TODO: tuning the distance to recognise a wall
    TemperatureDifference = 3   # TODO: tuning the difference between victim temperature and ambient temperature

    def __init__(self, sensors, actuators, x, y, z):
        self.Color = sensors[0]
        self.Distance = sensors[1]
        self.Gyroscope = sensors[2]
        self.Temperature = sensors[3]
        self.Led = actuators[0]
        self.Servo = actuators[1]
        self.X = x
        self.Y = y
        self.Z = z
        self.Victims = [Victim(self.Led, self.Servo)for i in range(4)]
        self.Walls = [False, False, False, False]
        self.Ramps = [False, False, False, False]

    def _isWall(self, n):
        n -= self.Gyroscope.getOrientation()

        if n == -3:
            n = 1
        elif n == -2:
            n = 2
        elif n == -1:
            n = 3

        distance = 0
        for i in range(0, 5):
            distance += self.Distance[n].distance()
        distance /= 5

        print(distance)

        if 0 < distance < self.WallDistance:
            return True
        else:
            return False

    def _isHeated(self, n):
        n -= self.Gyroscope.getOrientation()

        if n == -3:
            n = 1
        elif n == -2:
            n = 2
        elif n == -1:
            n = 3

        tempDifference = self.Temperature[n].getDifference()
        print(tempDifference)

        if tempDifference > self.TemperatureDifference:
            return True
        else:
            return False

    def scan(self):
        if self.Color.isBlack():
            self.Type = Area.AreaType.NoGo
            print("NoGo Area")
        elif self.Color.isWhite():
            self.Type = Area.AreaType.Standard
            print("Standard Area")
        elif self.Color.isSilver():
            self.Type = Area.AreaType.CheckPoint
            print("CheckPoint Area")

        for i in range(4):
            self.Walls[i] = self._isWall(i)
            if self.Walls[i]:
                if self._isHeated(i):
                    self.Victims[i].Type = Victim.VictimType.Heated
                    self.Victims[i].Present = True
                else:
                    # TODO: add visual victim scan algorithm
                    pass

        self.saveVictims()

        self.Scanned = True

    def saveVictims(self):
        for victim in self.Victims:
            victim.save()