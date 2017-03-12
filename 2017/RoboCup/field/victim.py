from enum import Enum


class Victim:
    VictimType = Enum('VictimType', 'Heated Harmed Stable Unharmed')
    Type = VictimType.Heated

    Present = False
    Saved = False

    def __init__(self):
        pass

    def save(self):
        self.Saved = True
