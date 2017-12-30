# Arduino Lego Truck: Manual App

This Arduino project has the target of full autonomous driving around. To make it more usable for others, I divided it into three subproject (might be changed). As a plattform I am using a modified Lego Set 42043 "MERCEDES-BENZ AROCS 3245", which does not contains Power Functions for driving out of the box, but is large enough to be modified in a lot of ways.

1. [Manual App](https://github.com/itbrauerei/arduino-lego-truck-manual-app): basic hardware and App setup to have fun with, only manual control
2. [With Sensors](https://github.com/itbrauerei/arduino-lego-truck-app-with-sensors): extended setup with distance sensors, only manual control
3. Mapping the surroundings: creating a map of the appartment when driving around
4. Autonomous driving: from a to b, drive to the fridge and get a :beer:

---

### Attribution

Please check out these projects, since this project is based on it (especially on the first project of [Patrick Müller](https://create.arduino.cc/projecthub/Notthemarsian))
- [Take Control Over Lego Power Functions](https://create.arduino.cc/projecthub/Notthemarsian/take-control-over-lego-power-functions-ee0bfa) @ [github](https://github.com/Notthemarsian/Lego-car-Arduino)
	- On the hardware side I basically added lights and distance sensor, modified it to single power source and corrected a few minor things
	- Most changes are on App, where data is now transfered back to the app and more buttons for lighting were added
- [Serial to Parallel Shifting-Out with a 74HC595](https://www.arduino.cc/en/Tutorial/ShiftOut)

---

### Features

- Driving around controlled vie bluetooth by Android App
- Automated breaking when obstable 
- Manual Lighting
    - White front and red taillights
    - Blue flashlights on roof
    - Yellow blinking warnlights
- Automatic Lighting
    - Red breaklights
    - Yellow blinking when turning left and right
- App Buttons
	- Bluetooth: connect to Arduino
	- Servo: switch servo direction
	- STOP: stop truck immediately
	- Flash Top: en-/disable roof flashing lights
	- Warn Blink: en-/disable all blinking lights on sides
	- Head/Tail: en-/disable head- and taillights
	- Touchfield: steer and speed control

---

### Requirements

Parts:
- 1x Arduino UNO
- 1-2x Breadboards
- 1x Motor driver L293D
- 1x Shift register 74HC595
- 1x Bluetooth HC-05
- 12x LEDs of different colors, 3mm or smaller
- 8x 220Ω resistances
- 3x 1KΩ resistances
- 1x 9V battery connector to cut
- 2x Logo Power Functions Extension to cut, e.g. 8871 for 50cm
- 1x Lego car or truck with at least one Power Functions servo for steering and one motor
- A lot of wires, I used approx. 7 meters in total on the Lego truck

Upload the code:
- [Arduino IDE](https://www.arduino.cc/en/Main/Software) to upload the [code](https://github.com/itbrauerei/arduino-lego-truck-manual-app/blob/master/arduino-lego-truck-manual-app/arduino-lego-truck-manual-app.ino)
- Install the [App](https://github.com/itbrauerei/arduino-lego-truck-manual-app/raw/master/android-app/arduino-lego-truck-manual-app.apk) on Android phone or tablet


To edit the App:
- MIT App Inventor to open the [App code](https://github.com/itbrauerei/arduino-lego-truck-manual-app/raw/master/android-app/arduino-lego-truck-manual-app.aia)

---

### Breadboard layout and circuit diagram

![arduino-lego-truck-manual-app-breadboard.png](https://github.com/itbrauerei/arduino-lego-truck-manual-app/raw/master/fritzing/arduino-lego-truck-manual-app-breadboard.png)

![arduino-lego-truck-manual-app-diagram.png](https://github.com/itbrauerei/arduino-lego-truck-manual-app/raw/master/fritzing/arduino-lego-truck-manual-app-diagram.png)

---

### ToDos

- Photos of everything
- Apps
    - "Real" Android App in Java or Kotlin
    - iOS App
- At least 4 more distance sensors
    - 2x front corners (45°)
    - 2x back corners (45°)
    - front bottom (maybe, for low obstacles like higher carpets)
    - back (maybe, depending how it turns out driving around)

---

### Licence

- GPL3+
- Trademarks belongs to their respective owners

