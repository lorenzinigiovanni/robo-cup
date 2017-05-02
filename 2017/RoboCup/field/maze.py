"""
Maze Class
"""

from field.area import Area


class Maze:
    StartX = 25
    StartY = 25
    StartZ = 1

    def __init__(self, sensors, actuators, camera):
        self.RampPassages = 0
        self.LastCheckPoint = [self.StartX, self.StartY, self.StartZ]
        self.Areas = [[[Area(sensors, actuators, camera, i, j, k) for k in range(3)] for j in range(50)] for i in range(50)]

    def findPath(self, x, y, z, control=True):
        self.Areas[x][y][z].Passages += 1

        if not self.Areas[x][y][z].Scanned:
            self.Areas[x][y][z].scan()

        if self.Areas[x][y][z].Type == Area.AreaType.CheckPoint:
            self.LastCheckPoint = [x, y, z]

        passages = [99, 99, 99, 99]

        if self.Areas[x][y][z].Ramps[0]:
            passages[0] = self.RampPassages
        elif not self.Areas[x][y][z].Walls[0] and not self.Areas[x + 1][y][z].Type == Area.AreaType.NoGo:
            passages[0] = self.Areas[x + 1][y][z].Passages

        if self.Areas[x][y][z].Ramps[1]:
            passages[1] = self.RampPassages
        elif not self.Areas[x][y][z].Walls[1] and not self.Areas[x][y + 1][z].Type == Area.AreaType.NoGo:
            passages[1] = self.Areas[x][y + 1][z].Passages

        if self.Areas[x][y][z].Ramps[2]:
            passages[2] = self.RampPassages
        elif not self.Areas[x][y][z].Walls[2] and not self.Areas[x - 1][y][z].Type == Area.AreaType.NoGo:
            passages[2] = self.Areas[x - 1][y][z].Passages

        if self.Areas[x][y][z].Ramps[3]:
            passages[3] = self.RampPassages
        elif not self.Areas[x][y][z].Walls[3] and not self.Areas[x][y - 1][z].Type == Area.AreaType.NoGo:
            passages[3] = self.Areas[x][y - 1][z].Passages

        if control and passages[0] == 99 and passages[1] == 99 and passages[2] == 99 and passages[3] == 99:
            self.Areas[x][y][z].scan()
            return self.findPath(x, y, z, control=False)

        return passages.index(min(passages))

    def findReturnPath(self, x, y, z):
        movementList = []

        targetX = x
        targetY = y
        targetZ = z
        rampDirection = 0

        if z != self.StartZ:
            for i in range(50):
                for j in range(50):
                    area = self.Areas[i][j][z]
                    for l in range(4):
                        if area.Ramps[l]:
                            targetX = i
                            targetY = j
                            rampDirection = l
                            break

            movementList += self._findShortestPath(x, y, z, targetX, targetY, targetZ)
            movementList.append(rampDirection)

            targetZ = self.StartZ

        movementList += self._findShortestPath(targetX, targetY, targetZ, self.StartX, self.StartY, self.StartZ)

        return movementList

    def _findShortestPath(self, x, y, z, targetX, targetY, targetZ):
        movementLists = []
        forbiddenAreas = []

        while True:
            r = self._findPath(x, y, z, targetX, targetY, targetZ, [], forbiddenAreas)

            state = r[0]
            movementList = r[1]
            forbiddenAreas2 = r[2]

            if state == "end":
                forbiddenArea = forbiddenAreas2[-1:]
                try:
                    if forbiddenArea[0] in forbiddenAreas:
                        break
                except IndexError:
                    break
                forbiddenAreas += forbiddenArea
                movementLists.append(movementList)
            else:
                break

        length = 999
        returnList = []

        for movementList in movementLists:
            if len(movementList) < length:
                length = len(movementList)
                returnList = movementList

        return returnList

    def _findPath(self, x, y, z, targetX, targetY, targetZ, mvList, forbidden):
        movementList = mvList[:]
        returnList = [0, 0, 0, 0]
        forbiddenAreas = forbidden[:]
        forbiddenAreas.append((x, y))

        if x == targetX and y == targetY and z == targetZ:
            forbiddenAreas.pop()
            return "end", movementList, forbiddenAreas

        for i in range(0, 4):
            if i == 0:
                x1 = x + 1
                y1 = y
            elif i == 1:
                x1 = x
                y1 = y + 1
            elif i == 2:
                x1 = x - 1
                y1 = y
            else:
                x1 = x
                y1 = y - 1

            if (x1, y1) in forbiddenAreas or self.Areas[x][y][z].Walls[i] or self.Areas[x][y][z].Ramps[i] \
                    or self.Areas[x1][y1][z].Type == Area.AreaType.NoGo or not self.Areas[x1][y1][z].Scanned:
                returnList[i] = -1
            else:
                movementList.append(i)
                r = self._findPath(x1, y1, z, targetX, targetY, targetZ, movementList, forbiddenAreas)

                if r[0] == -1 and r[1] == -1 and r[2] == -1 and r[3] == -1:
                    returnList[i] = -1
                    movementList.pop()
                else:
                    e, f, g = r
                    if e == "end":
                        return r

        return returnList
