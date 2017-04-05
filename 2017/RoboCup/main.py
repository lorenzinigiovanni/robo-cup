import atexit
from robot import Robot

WhyNot = Robot()


def main():
    WhyNot.start()


@atexit.register
def clean():
    WhyNot.stop()


if __name__ == "__main__":
    main()
