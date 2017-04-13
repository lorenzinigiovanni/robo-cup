"""
Victim Class
"""

from enum import Enum
import time


class Victim:
    VictimType = Enum('VictimType', 'Heated Harmed Stable Unharmed')
    RemainingKit = 12

    def __init__(self, led, servo):
        self.Led = led
        self.Servo = servo

        self.Type = self.VictimType.Heated

        self.Present = False
        self.Saved = False

    def save(self):
        if self.Present and not self.Saved:
            print("Save Victim")

            self.Led.on()
            time.sleep(1)
            self.Led.off()
            time.sleep(1)
            self.Led.on()
            time.sleep(1)
            self.Led.off()
            time.sleep(1)
            self.Led.on()
            time.sleep(1)
            self.Led.off()

            n = 0

            if self.Type == Victim.VictimType.Heated:
                n = 1
            elif self.Type == Victim.VictimType.Harmed:
                n = 2
            elif self.Type == Victim.VictimType.Stable:
                n = 1
            elif self.Type == Victim.VictimType.Unharmed:
                n = 0

            for i in range(n):
                if Victim.RemainingKit > 0:
                    if Victim.RemainingKit % 2 == 0:
                        self.Servo.angle(180)
                        time.sleep(0.5)
                        self.Servo.angle(90)
                    else:
                        self.Servo.angle(0)
                        time.sleep(0.5)
                        self.Servo.angle(90)
                    Victim.RemainingKit -= 1

                    print("Remaining Kit = " + str(Victim.RemainingKit))

            self.Saved = True
            print("Victim Saved")
            return True
        return False
