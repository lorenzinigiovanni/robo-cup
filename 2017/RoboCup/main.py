import sensors.color
import sensors.distance
import sensors.gyroscope
import sensors.temperature

while True:
    print(sensors.gyroscope.Sensor.getOrientation(), end="\t")
    for i in range(0, 4):
        print(sensors.temperature.Sensors[i].getObjTemp(), "\t", end="\t")
    for i in range(0, 4):
        print(sensors.distance.Sensors[i].distance(), "\t", end="\t")
    sensors.color.Sensor.getColors()
    print(sensors.color.Sensor.red_color)
