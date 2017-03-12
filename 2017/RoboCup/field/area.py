from enum import Enum
from field.victim import Victim
import sensors.distance
import sensors.temperature
import sensors.color


class Area:
    AreaType = Enum('AreaType', 'Standard CheckPoint NoGo')

    Visited = False
    Scanned = False
    Type = AreaType.Standard
    Walls = [False, False, False, False]
    Ramps = [False, False, False, False]
    Victims = [Victim() for i in range(4)]

    X = -1
    Y = -1
    Z = -1

    def __init__(self):
        pass

    @staticmethod
    def _isWall(n):
        if 0 < sensors.distance.Sensors[n].distance() < 100:
            return True
        else:
            return False

    @staticmethod
    def _isHeated(n):
        if sensors.temperature.Sensors[n].getObjTemp() - sensors.temperature.Sensors[n].getAmbTemp() > 5:
            return True
        else:
            return False

    def scan(self):
        sensors.color.Sensor.getColors()
        if sensors.color.Sensor.isBlack():
            self.Type = self.AreaType.NoGo
        elif sensors.color.Sensor.isSilver():
            self.Type = self.AreaType.CheckPoint
        elif sensors.color.Sensor.isWhite():
            self.Type = self.AreaType.Standard

        for i in range(4):
            self.Walls[i] = self._isWall(i)
            if self._isHeated(i):
                self.Victims[i].Type = Victim.VictimType.Heated
                self.Victims[i].Present = True

        self.Scanned = True

    def saveVictims(self):
        for victim in self.Victims:
            victim.save()
