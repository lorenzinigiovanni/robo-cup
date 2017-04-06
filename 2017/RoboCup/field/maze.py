"""
Maze Class
"""

from field.area import Area


class Maze:
    Areas = []
    LastCheckPoint = [25, 25, 0]

    def __init__(self, sensors, actuators):
        self.Areas = [[[Area(sensors, actuators, i, j, k) for k in range(2)] for j in range(50)] for i in range(50)]

    def findPath(self, x, y, z):
        self.Areas[x][y][z].Passages += 1

        if not self.Areas[x][y][z].Scanned:
            print("Scan X = " + str(x) + " Y = " + str(y) + " Z = " + str(z))
            self.Areas[x][y][z].scan()

        if self.Areas[x][y][z].Type == Area.AreaType.CheckPoint:
            self.LastCheckPoint = [x, y, z]

        passages = [99, 99, 99, 99]
        print(self.Areas[x][y][z])
        print(self.Areas[x][y][z].Walls)

        if not self.Areas[x][y][z].Walls[0] and not self.Areas[x + 1][y][z].Type == Area.AreaType.NoGo:
            passages[0] = self.Areas[x + 1][y][z].Passages
        if not self.Areas[x][y][z].Walls[1] and not self.Areas[x][y + 1][z].Type == Area.AreaType.NoGo:
            passages[1] = self.Areas[x][y + 1][z].Passages
        if not self.Areas[x][y][z].Walls[2] and not self.Areas[x - 1][y][z].Type == Area.AreaType.NoGo:
            passages[2] = self.Areas[x - 1][y][z].Passages
        if not self.Areas[x][y][z].Walls[3] and not self.Areas[x][y - 1][z].Type == Area.AreaType.NoGo:
            passages[3] = self.Areas[x][y - 1][z].Passages

        print(self.Areas[x][y][z].Walls)

        print(passages)
        print(passages.index(min(passages)))
        return passages.index(min(passages))

    def findReturnPath(self, x, y, z):
        # TODO: write an algorithm to find return path
        pass
