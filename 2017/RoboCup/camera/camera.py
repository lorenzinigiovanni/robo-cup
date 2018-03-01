"""
Camera Class
"""

from picamera.array import PiRGBArray
from picamera import PiCamera
from camera import imutils
import numpy as np
import cv2
import copy


class Camera:
    MarginUD = 1.3
    MarginLR = 1.1
    Threshold = 0.65
    MinDimension = 0.2
    MaxDimension = 1.0
    Number = 10

    K = np.array([[3593.18463,  0.,         322.],
                  [0.,          3664.87320, 201.],
                  [0.,          0.,         1.]])

    D = np.array([-4.07510655, 23.5098640, -0.0529764969, 0.0906216614, -96.3755091])

    Knew = K.copy()

    def __init__(self):
        self.Camera = PiCamera()
        self.Camera.resolution = (640, 480)
        self.Camera.rotation = 180
        self.Camera.brightness = 60
        self.RawCapture = PiRGBArray(self.Camera, size=(640, 480))

        self.Sift = cv2.xfeatures2d.SIFT_create()

        tmp1 = cv2.imread("/home/pi/Desktop/RoboCup/camera/H.png")
        tmp1 = cv2.cvtColor(tmp1, cv2.COLOR_BGR2GRAY)
        sift1 = self.Sift.detectAndCompute(tmp1, None)

        tmp2 = cv2.imread("/home/pi/Desktop/RoboCup/camera/S.png")
        tmp2 = cv2.cvtColor(tmp2, cv2.COLOR_BGR2GRAY)
        sift2 = self.Sift.detectAndCompute(tmp2, None)

        tmp3 = cv2.imread("/home/pi/Desktop/RoboCup/camera/U.png")
        tmp3 = cv2.cvtColor(tmp3, cv2.COLOR_BGR2GRAY)
        sift3 = self.Sift.detectAndCompute(tmp3, None)

        self.Template = [sift1, sift2, sift3]
        self.Pippo = [tmp1, tmp2, tmp3]

    def matchTemplate(self):
        print("Camera capture")

        self.RawCapture.truncate(0)

        self.Camera.capture(self.RawCapture, format='bgr', resize=(640, 480))
        frame = self.RawCapture.array

        h, w = frame.shape[:2]
        newcameramtx, roi = cv2.getOptimalNewCameraMatrix(self.K, self.D, (w, h), 1, (w, h))

        frame = cv2.undistort(frame, self.K, self.D, None, newcameramtx)

        print("Elaborating frame")

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        gray = cv2.bilateralFilter(gray, 11, 17, 17)
        edged = cv2.Canny(gray, 30, 200)

        _, contours, _ = cv2.findContours(edged.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        contours = sorted(contours, key=cv2.contourArea, reverse=True)[:10]

        matches = []
        warpeds = []

        # TODO: do an if for contours number

        for contour in contours:
            perimeter = cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, 0.001 * perimeter, True)

            print("Ho trovato = " + str(len(approx)) + " contorni")

            if 25 <= len(approx) <= 110:
                screenCnt = approx
                warpeds.append(self._fourPointTransform(frame, np.reshape(screenCnt, (len(approx), 2))))
                break

        print("Searching for visual victim")

        for warped in warpeds:
            img = cv2.cvtColor(warped, cv2.COLOR_BGR2GRAY)
            img = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 55, 15)
            img = cv2.medianBlur(img, 7)

            x, y = img.shape[::-1]
            r = x / y

            print("Ratio = " + str(r))
            print("Dimension X = " + str(x))
            print("Dimension Y = " + str(y))

            if r < 0.55 or r > 1.35 or x < 150 or x > 360 or y < 150 or y > 360:
                print("Non ho trovato niente")
                return -1

            kp, des = self.Sift.detectAndCompute(img, None)

            for template in self.Template:
                bf = cv2.BFMatcher()
                bfmatches = bf.knnMatch(des, template[1], k=2)

                good = []
                for m, n in bfmatches:
                    if m.distance < 0.85 * n.distance:
                        good.append([m])

                matches.append(len(good))

        print(matches)

        try:
            if max(matches) > self.Threshold:
                index = matches.index(max(matches))
                index = index % 3
            else:
                index = -1
        except ValueError:
            index = -1

        print("Finish to search visual victim")

        print("Find victim = " + str(index))

        self.RawCapture.truncate(0)

        return index

    def _orderPoints(self, pts):
        points = copy.deepcopy(pts)

        rect = np.zeros((4, 2), dtype="float32")

        a = sorted(points, key=lambda point: point[0] + point[1])
        b = sorted(points, key=lambda point: point[0] - point[1])

        a[0][0] /= self.MarginLR
        a[0][1] /= self.MarginUD

        a[-1][0] *= self.MarginLR
        a[-1][1] *= self.MarginUD

        b[0][0] /= self.MarginLR
        b[0][1] *= self.MarginUD

        b[-1][0] *= self.MarginLR
        b[-1][1] /= self.MarginUD

        rect[0] = a[0]
        rect[2] = a[-1]

        rect[1] = b[-1]
        rect[3] = b[0]

        return rect

    def _fourPointTransform(self, image, pts):
        rect = self._orderPoints(pts)
        (tl, tr, br, bl) = rect

        widthA = np.sqrt(((br[0] - bl[0]) ** 2) + ((br[1] - bl[1]) ** 2))
        widthB = np.sqrt(((tr[0] - tl[0]) ** 2) + ((tr[1] - tl[1]) ** 2))
        maxWidth = max(int(widthA), int(widthB))

        heightA = np.sqrt(((tr[0] - br[0]) ** 2) + ((tr[1] - br[1]) ** 2))
        heightB = np.sqrt(((tl[0] - bl[0]) ** 2) + ((tl[1] - bl[1]) ** 2))
        maxHeight = max(int(heightA), int(heightB))

        dst = np.array([
            [0, 0],
            [maxWidth - 1, 0],
            [maxWidth - 1, maxHeight - 1],
            [0, maxHeight - 1]], dtype="float32")

        M = cv2.getPerspectiveTransform(rect, dst)
        warped = cv2.warpPerspective(image, M, (maxWidth, maxHeight))

        return warped
