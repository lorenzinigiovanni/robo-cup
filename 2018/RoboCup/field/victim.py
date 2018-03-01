"""
Victim Class
"""

from enum import Enum


class Victim:
    VictimType = Enum('VictimType', 'Heated Harmed Stable Unharmed')
    RemainingKit = 12

    def __init__(self, servo):
        self.Servo = servo

        self.Type = self.VictimType.Heated

        self.Present = False
        self.Saved = False

    def save(self):
        if self.Present and not self.Saved:
            print("Save Victim")

            n = -1

            if self.Type == Victim.VictimType.Heated:
                n = 1
            elif self.Type == Victim.VictimType.Harmed:
                n = 2
            elif self.Type == Victim.VictimType.Stable:
                n = 1
            elif self.Type == Victim.VictimType.Unharmed:
                n = 0

            self.Servo.release(n)

            Victim.RemainingKit -= n
            print("Remaining Kit = " + str(Victim.RemainingKit))

            self.Saved = True
            print("Victim Saved")
            return True
        return False
