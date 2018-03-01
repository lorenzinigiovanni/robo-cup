import atexit
from robot import Robot

WhyNot = Robot()


def main():
    WhyNot.start()


@atexit.register
def clean():
    print("Program exiting")
    WhyNot.cleanUp()
    print("Robot cleanUp")


if __name__ == "__main__":
    main()
