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
    Margin = 1.3
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

        tmp1 = cv2.imread("/home/pi/Desktop/RoboCup/camera/H.png")
        tmp1 = cv2.cvtColor(tmp1, cv2.COLOR_BGR2GRAY)
        tmp1 = cv2.resize(tmp1, (64, 64))

        tmp2 = cv2.imread("/home/pi/Desktop/RoboCup/camera/S.png")
        tmp2 = cv2.cvtColor(tmp2, cv2.COLOR_BGR2GRAY)
        tmp2 = cv2.resize(tmp2, (64, 64))

        tmp3 = cv2.imread("/home/pi/Desktop/RoboCup/camera/U.png")
        tmp3 = cv2.cvtColor(tmp3, cv2.COLOR_BGR2GRAY)
        tmp3 = cv2.resize(tmp3, (64, 64))

        self.Template = [tmp1, tmp2, tmp3]

        self.TH, self.TW = self.Template[0].shape[::-1]

    def matchTemplate(self):
        print("Camera capture")

        self.RawCapture.truncate(0)

        self.Camera.capture(self.RawCapture, format='bgr', resize=(640, 480))
        frame = self.RawCapture.array

        h, w = frame.shape[:2]
        newcameramtx, roi = cv2.getOptimalNewCameraMatrix(self.K, self.D, (w, h), 1, (w, h))

        frame = cv2.undistort(frame, self.K, self.D, None, newcameramtx)

        # frame = imutils.resize(frame, height=300)

        """cv2.imshow("Raw frame", frame)
        cv2.waitKey(0)"""

        warped = frame.copy()

        print("Elaborating frame")

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        gray = cv2.bilateralFilter(gray, 11, 17, 17)
        edged = cv2.Canny(gray, 30, 200)

        _, contours, _ = cv2.findContours(edged.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        contours = sorted(contours, key=cv2.contourArea, reverse=True)[:10]

        matches = []

        ok = False

        for contour in contours:
            perimeter = cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, 0.001 * perimeter, True)

            print("Ho trovato = " + str(len(approx)) + " contorni")

            if 25 <= len(approx) <= 110:
                screenCnt = approx
                warped = self._fourPointTransform(frame, np.reshape(screenCnt, (len(approx), 2)))
                ok = True
                break

        if not ok:
            print("Non ho trovato un cazzo")
            return -1

        img = imutils.resize(warped, width=300)

        print("Searching for visual victim")

        img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        img = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 55, 15)
        img = cv2.medianBlur(img, 7)

        """morphKernel = np.ones((5, 5), np.uint8)
        dilateKernel = np.ones((2, 2), np.uint8)
        erodeKernel = np.ones((3, 5), np.uint8)

        img = cv2.dilate(img, dilateKernel, iterations=1)
        img = cv2.erode(img, erodeKernel, iterations=1)
        img = cv2.morphologyEx(img, cv2.MORPH_CLOSE, morphKernel)"""

        cv2.imshow("Elaborated frame", img)
        cv2.waitKey(0)

        resized = img

        for scale in np.linspace(self.MinDimension, self.MaxDimension, self.Number)[::-1]:
            try:
                resized = imutils.resize(img, width=int(img.shape[1] * scale))
            except:
                print("Error in resizing")

            for template in self.Template:
                if resized.shape[0] < self.TH or resized.shape[1] < self.TW:
                    break

                result = cv2.matchTemplate(resized, template, cv2.TM_CCOEFF_NORMED)
                min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(result)

                matches.append(max_val)

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

        a[0][0] /= self.Margin
        a[0][1] /= self.Margin

        a[-1][0] *= self.Margin
        a[-1][1] *= self.Margin

        b[0][0] /= self.Margin
        b[0][1] *= self.Margin

        b[-1][0] *= self.Margin
        b[-1][1] /= self.Margin

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
