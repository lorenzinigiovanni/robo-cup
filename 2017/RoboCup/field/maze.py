from field.area import Area


class Maze:
    Areas = []

    def __init__(self, sensors, actuators):
        self.Sensors = sensors
        self.Actuators = actuators
        self.Areas = [[[Area(self.Sensors, self.Actuators, k, j, i) for k in range(50)] for j in range(50)] for i in range(2)]

    def addArea(self, x, y, z):
        self.Areas += Area(self.Sensors, self.Actuators, x, y, z)

    def findPath(self, x, y, z):
        if not self.Areas[x][y][z].Scanned:
            self.Areas[x][y][z].scan()

        passages = [99, 99, 99, 99]

        if not self.Areas[x][y][z].Walls[0]:
            passages[0] = self.Areas[x + 1][y][z].Passages
        if not self.Areas[x][y][z].Walls[1]:
            passages[1] = self.Areas[x][y + 1][z].Passages
        if not self.Areas[x][y][z].Walls[2]:
            passages[2] = self.Areas[x - 1][y][z].Passages
        if not self.Areas[x][y][z].Walls[3]:
            passages[3] = self.Areas[x][y - 1][z].Passages

        return passages.index(min(passages))

    def findReturnPath(self, x, y, z):
        pass
