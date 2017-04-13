"""
Area Class
"""

import time
from enum import Enum
from field.victim import Victim


class Area:
    AreaType = Enum('AreaType', 'Standard CheckPoint NoGo')

    WallDistance = 150          # TODO: tuning the distance to recognise a wall
    TemperatureDifference = 1   # TODO: tuning the difference between victim temperature and ambient temperature

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

        self.Walls = [False, False, False, False]
        self.Ramps = [False, False, False, False]
        self.Victims = [Victim(self.Led, self.Servo) for i in range(4)]

        self.Passages = 0
        self.Scanned = False
        self.Type = self.AreaType.Standard

    def _isWall(self, n):
        n -= self.Gyroscope.getOrientation()

        if n == -3:
            n = 1
        elif n == -2:
            n = 2
        elif n == -1:
            n = 3

        distance = 0
        for i in range(0, 3):
            distance += self.Distance[n].distance()
        distance /= 3

        print("Distance at " + str(n) + " is = " + str(distance))

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

        tempDifference = 0
        for i in range(0, 3):
            tempDifference += self.Temperature[n].getDifference()
        tempDifference /= 3

        print("Temperature difference at " + str(n) + " is = " + str(tempDifference))

        if tempDifference > self.TemperatureDifference:
            return True
        else:
            return False

    def findVisualVictim(self, n):
        if self.Victims[n].Present or not self.Walls[n]:
            return

        # TODO: Aggiungere codice ricerca vittime visuali

        """self.Victims[n].Present = True
        self.Victims[n].Type = Victim.VictimType.Stable"""

    def scan(self):
        print("Scan X = " + str(self.X) + " Y = " + str(self.Y) + " Z = " + str(self.Z))

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

        self.Scanned = True
