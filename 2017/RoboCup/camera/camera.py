"""
Camera Class
"""

from picamera.array import PiRGBArray
from picamera import PiCamera
from camera import imutils
import numpy as np
import cv2


class Camera:

    def __init__(self):
        self.Camera = PiCamera()
        self.Camera.resolution = (640, 480)
        self.Camera.rotation = 180
        self.Camera.brightness = 65
        self.RawCapture = PiRGBArray(self.Camera, size=(640, 480))

        tmp1 = cv2.imread("/home/pi/Desktop/RoboCup/camera/cutH.png")
        tmp1 = cv2.cvtColor(tmp1, cv2.COLOR_BGR2GRAY)
        tmp1 = (cv2.resize(tmp1, (128, 128)), 0.85)

        tmp2 = cv2.imread("/home/pi/Desktop/RoboCup/camera/S.png")
        tmp2 = cv2.cvtColor(tmp2, cv2.COLOR_BGR2GRAY)
        tmp2 = (cv2.resize(tmp2, (128, 128)), 0.85)

        tmp3 = cv2.imread("/home/pi/Desktop/RoboCup/camera/U.png")
        tmp3 = cv2.cvtColor(tmp3, cv2.COLOR_BGR2GRAY)
        tmp3 = (cv2.resize(tmp3, (128, 128)), 0.85)

        self.Template = [tmp1, tmp2, tmp3]

        self.TH, self.TW = self.Template[0][0].shape[::-1]

    def matchTemplate(self):
        self.Camera.capture(self.RawCapture, format='bgr', resize=(640, 480))
        frame = self.RawCapture.array

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        matches = [0, 0, 0]
        i = 0

        for template in self.Template:
            for scale in np.linspace(0.2, 1.5, 5)[::-1]:
                resized = imutils.resize(gray, width=int(gray.shape[1] * scale))

                if resized.shape[0] < self.TH or resized.shape[1] < self.TW:
                    break

                result = cv2.matchTemplate(resized, template[0], cv2.TM_CCOEFF_NORMED)

                loc = np.where(result >= template[1])

                for pt in zip(*loc[::-1]):
                    cv2.rectangle(frame, pt, (pt[0] + self.TW, pt[1] + self.TH), (0, 255, 0), 2)
                    if pt[0] > 0 and pt[1] > 0:
                        matches[i] += 1

            i += 1

        cv2.imshow("Elaborated", gray)
        cv2.waitKey(0)

        self.RawCapture.truncate(0)

        return matches
