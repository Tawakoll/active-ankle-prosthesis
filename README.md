# Active Ankle Prosthesis

> **Archived academic project, 2021.** This was the B.Sc. graduation project of four Mechatronics Engineering students at the Arab Academy for Science, Technology & Maritime Transport (AASTMT) in Cairo. It is kept here as a record of the work. Nobody maintains it, and it is not a medical device.

A powered transtibial ankle prosthesis. The joint is driven by a ball screw under closed-loop position control. Load cells in the foot tell the controller which part of the gait cycle the wearer is in, and the ankle follows a reference angle trajectory accordingly.

The team won a sponsorship scholarship from the Academy of Scientific Research and Technology (ASRT) in Egypt, which funded the build.

---

## Why we built it

Most below-knee amputees in the MENA region walk on passive prostheses, which amount to a spring and a hinge. A passive ankle gives some energy back but generates none of its own. Stairs, slopes and long walks therefore cost the wearer a lot more effort, and years of asymmetric gait bring their own problems. Active ankles are sold commercially, but between import duties and regional pricing they are out of reach for most amputees here.

We wanted to find out whether a powered ankle could be built locally, for a price that made sense locally.

---

## Demo

[`media/demo/gait-cycle-demo.webm`](media/demo/gait-cycle-demo.webm) shows the ankle following one full gait cycle.

| | |
|---|---|
| ![Assembled ankle](media/hardware/assembled-ankle.jpeg) | ![Range of motion](media/hardware/ankle-range-of-motion.jpg) |

---

## How it works

### Mechanical

A DC motor turns an **SFU1605 ball screw**. The screw turns that rotation into linear travel, which drives the ankle joint through the foot linkage. An **SKF 6002** bearing carries the joint.

We checked the structural parts in Autodesk Inventor before manufacturing them: Von Mises stress, displacement and contact pressure. The full analysis, with figures and result summaries, is in the graduation book.

The motor is a **cordless drill motor we repurposed**. A brushless motor with the torque-to-size ratio we wanted was not something we could get within the budget or the timeline, so we improvised. That decision shapes almost everything in the limitations section below.

### Sensing

| Sensor | What it does |
|---|---|
| **AS5600** magnetic rotary encoder (I²C) | Joint position. Absolute within one turn; firmware counts revolutions for multi-turn travel. |
| **4 load cells** on HX711 amplifiers | Ground contact. Two under the toe, two under the heel. |
| **Current sensor** | Motor current, for characterisation and for keeping the motor inside safe limits. |

Toe and heel contact taken together identify the gait phase, whether that is heel strike, flat foot, toe off or swing. That is what tells the controller where the wearer actually is in the cycle.

### Control

An **ESP32** runs the controller as concurrent **FreeRTOS** tasks (using `TridentTD_EasyFreeRTOS32`), one task per job:

```
TrajGen       steps through the reference trajectory, advancing on gait phase
read_ANGLE    reads the AS5600, counts revolutions, converts to degrees
pid           PID position loop; drives motor PWM and direction
toe_cells     2x HX711, toe contact, threshold 400
heel_cells    2x HX711, heel contact, threshold 400
Communication serial telemetry and live gain tuning
```

The position loop is an ordinary **PID** on joint angle. We tuned it empirically to **Kp = 0.14, Ki = 0.2, Kd = 0**; the step responses at each gain are in [`media/results/`](media/results). Motor output is PWM on one pin with direction on another.

#### About the trajectory units

`WalkingTraj[101]` holds the reference trajectory: 101 samples covering one gait cycle, derived from published human ankle kinematics.

**Those values are encoder-shaft degrees, not ankle joint degrees.** The firmware converts like this:

```c
raw = revolution * 4096 + raw_angle;   // AS5600 is 12-bit, so 4096 counts/turn
ang = raw * 0.087;                     // 360/4096 is about 0.0879 deg per count
```

The array runs from roughly -74 to +156, which is motor-side travel. The ball screw and foot linkage map it onto the anatomical ankle range, somewhere around 20 degrees of plantarflexion to 10 degrees of dorsiflexion. Read the array as joint angles and the numbers will look impossible. They are not.

---

## What worked and what didn't

Setting this out plainly, since an archive that oversells itself is no use to anyone who reads it.

We got working:

- Closed-loop position control of the ankle joint, following the reference trajectory
- Gait-phase detection from the toe and heel load cells, demonstrated on hardware
- A complete mechanical design with FEA behind it, manufactured into a working prototype

The limitations are real:

- **A cycle takes about 5 seconds**, against roughly 1 second for a person walking. The drill motor simply could not follow the trajectory any faster. This demonstrates the control concept on a bench. It is not a device anyone could walk on.
- **Motor authority is capped at 100 of 255 PWM**, about 39 percent duty. We set that ceiling deliberately to keep the improvised drivetrain from tearing itself apart.
- **There is no torque or impedance control.** The ankle tracks position and nothing else. A real prosthesis needs compliance that changes through the gait cycle; ours is stiff the whole way through.
- **It was never tested on an amputee.** All of our testing happened on the bench.

---

## What is in this repository

```
firmware/
  esp32-final/     shimi/       the final ESP32 build, the one that ran
  esp32-bench/     trial_code/  load-cell isolation test: sensors on, motor loop off
  arduino-avr/                  earlier AVR development sketches
docs/
  graduation-book-2021-07-03.pdf                full group thesis, 111 pp.
  individual-contribution-mohamed-tawakol.docx  individual section
  project-proposal-2020.pdf
  bill-of-materials.pdf
media/
  demo/  hardware/  results/
REFERENCES.md      cited literature, by DOI
```

### Two things worth explaining before you go looking

**Why the folder is called `shimi`.** It is named after Ibrahim El-Shimi, one of the team, not after any part of the machine. That folder holds the final firmware.

**The book says Arduino Uno; the code says ESP32.** The electrical chapter of the graduation book describes an "Arduino Uno ATmega328" as the main board. That chapter was written early on and never revised after we moved to the ESP32 for the extra I/O, the speed and the WiFi. Trust the firmware in this repository: the final controller is an ESP32. We used the Arduino IDE throughout, which is probably where the confusion started.

While you are at it, note that `firmware/arduino-avr/FINAL_PID_CODE_WITH_RTOS/` has "FINAL" in its name but includes `Arduino_FreeRTOS.h` and calls `analogWrite()`, both of which are AVR-only. The name is misleading. It is an earlier milestone, not the final build.

### About the commit history

The first commit holds the 2021 files exactly as they were archived. The second re-enables the gait-phase tasks in `shimi.ino`, which had been commented out during a test session and left that way when the file was saved. Keeping both states means the archive stays honest about what was found and what was changed.

---

## The team

B.Sc. Mechatronics Engineering, AASTMT College of Engineering and Technology, Cairo, 2021.

- Ahmed Mohamed Ahmed Mokhtar
- Amr Samir Hassanein Mohamed
- Ibrahim Ayman Ibrahim El-Shimi
- Mohamed Ahmed Mohamed Tawakol ([@Tawakoll](https://github.com/Tawakoll))

Supervised by Dr. Ahmed Elsawaf and Dr. Moustafa A. Fouz.

Sponsored by the Academy of Scientific Research and Technology (ASRT), Egypt.

---

## Licence

The code under `firmware/` is [MIT](LICENSE). The documents and media under `docs/` and `media/` are [CC BY-NC 4.0](LICENSE-DOCS).

Papers we read during the project are not redistributed here. [REFERENCES.md](REFERENCES.md) lists them with DOIs.
