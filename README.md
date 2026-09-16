# Active Ankle Prosthesis

**A powered ankle prosthesis built by four engineering students in Cairo, from parts bought off the shelf.**

![The assembled prosthesis](media/hardware/assembled-ankle.jpeg)

> **Archived academic project, 2021.** This was our B.Sc. graduation project in Mechatronics Engineering at the Arab Academy for Science, Technology & Maritime Transport (AASTMT), Cairo. It is kept here as a record of the work. Nobody maintains it, and it is not a medical device.

A DC motor turns a ball screw, which drives the ankle joint. Load cells in the foot tell the controller which part of the gait cycle the wearer is in, and the ankle follows a reference angle trajectory to match.

The project won a sponsorship scholarship from the **Academy of Scientific Research and Technology (ASRT)**, Egypt.

---

## Why we built it

Most below-knee amputees in the MENA region walk on passive prostheses, which amount to a spring and a hinge. A passive ankle returns some energy but generates none of its own. Stairs, slopes and long walks therefore cost the wearer far more effort, and years of asymmetric gait bring problems of their own.

Active ankles are sold commercially. Between import duties and regional pricing, they are out of reach for nearly every amputee here.

So we asked a narrow question: could we build one locally, from parts available locally, at a price that made sense locally? Everything in the components list below was bought off the shelf in Cairo, and the motor came out of a cordless drill.

---

## The demo

[**`media/demo/gait-cycle-demo.webm`**](media/demo/gait-cycle-demo.webm) shows the ankle working through one complete gait cycle.

---

## Components

### Actuation

| Part | Role |
|---|---|
| RS-550S brushed DC motor | Drives the joint. Taken from a cordless drill: we could not source a brushless motor with the torque-to-size ratio we wanted within the budget. |
| Cytron MD10C H-bridge | Motor driver, 10 A continuous, driven by a PWM pin and a direction pin. |
| SFU1605 ball screw and nut | Converts motor rotation into the linear travel that moves the joint. |
| SKF 6002 bearing | Carries the ankle joint. |
| 18 V Li-ion pack, 1200 mAh | Supplies the motor, and the logic rail through a step-down regulator. |

Motor and ball screw figures, from the graduation book:

| Property | Value |
|---|---|
| Motor nominal voltage | 18 V DC (operating range 3-24 V) |
| Motor no-load speed | 22,000 rpm |
| Motor no-load current | 0.9 A |
| Motor stall current | 62 A |
| Motor peak efficiency | 8 A, 103 W output |
| Screw diameter | 16 mm (13 mm root) |
| Screw lead | 5 mm per revolution |
| Screw length | 400 mm |
| Screw material | Steel alloy, 147 MPa permissible strength |

The screw was sized against an approximate 1500 N design load and checked for buckling as a fixed-supported column over a 130 mm span. That calculation and the Inventor FEA (Von Mises stress, displacement, contact pressure) are worked through in the graduation book.

The drill motor is the root of most of the limitations further down this page.

### Sensing

| Part | Role |
|---|---|
| AS5600 magnetic rotary encoder (I²C, 12-bit) | Joint position. Absolute within one turn; firmware counts revolutions for multi-turn travel. |
| 50 kg full-bridge load cells | Ground contact. Four of them: two under the toe, two under the heel. |
| HX711 24-bit ADC | One amplifier per load cell, so four. |
| ACS712 current sensor, 30 A | Motor current. Used in the first design and on the test bench; it is not in the final ESP32 wiring or firmware. |

Toe and heel contact read together identify the gait phase, whether that is heel strike, flat foot, toe off or swing. That is what tells the controller where the wearer is in the cycle, rather than assuming it from a timer.

### Control electronics

| Part | Role |
|---|---|
| ESP32-WROOM-32 | The controller on the assembled ankle. Chosen over the first board for the extra I/O the four load cells needed, the clock speed, and onboard WiFi. |
| ATmega328 board | Ran the motor test bench while the ankle was being machined: motor, H-bridge, encoder and PID tuning. |
| Step-down regulator | Logic rail off the 18 V pack: 3.3 V for the ESP32, 5 V in the first design. |

### Wiring

The first design was built around an Uno board (ATmega328); we moved to an ESP32 partway through, for the extra I/O, the speed and the WiFi. Both diagrams survive, which makes the migration easy to see:

**First design, Uno board:**

![First wiring diagram, Uno board](media/hardware/wiring-diagram-uno.png)

**Final design, ESP32:**

![ESP32 wiring diagram](media/hardware/wiring-diagram-esp32.png)

The ESP32 build drops the ACS712 from the loop and swaps the 5 V regulator for a 3.3 V one. Everything else carries over.

---

## Control system

An **ESP32** runs the controller as concurrent **FreeRTOS** tasks (using `TridentTD_EasyFreeRTOS32`), one task per job:

```
TrajGen        steps through the reference trajectory, advancing on gait phase
read_ANGLE     reads the AS5600, counts revolutions, applies the zero offset
pid            PID position loop; drives motor PWM and direction
toe_cells      2x HX711, toe contact, threshold 400
heel_cells     2x HX711, heel contact, threshold 400
Communication  serial telemetry and live gain tuning
```

The position loop is an ordinary **PID** on joint angle, tuned empirically to **Kp = 0.14, Ki = 0.2, Kd = 0**.

`TrajGen` picks which part of the cycle to play from the two contact booleans:

| heel | toe | phase |
|---|---|---|
| 1 | 0 | heel strike, moving to flat foot |
| 1 | 1 | flat foot, moving to heel off |
| 0 | 1 | heel off, moving to toe off |
| 0 | 0 | swing |

A latch flag per phase makes each segment play once per step instead of restarting while the contact state holds.

### About the code

It is written in a procedural C style: plain functions, global state, no classes of our own. It compiles as **C++**, because the framework core and every library it uses (`PID_v1`, `HX711_ADC`, `AS5600`, `WiFi`) are C++ and are used as objects.

We built it in the **Arduino IDE**, chosen because it is free, open source and got out of the way while we were bringing hardware up. It compiles `.ino` as C++ behind the scenes, which is why the original files never had to say so.

[`firmware/ankle_controller_esp32/`](firmware/ankle_controller_esp32) holds the same code with its structure made explicit: shared state in a header, one task per source file, and a PlatformIO config so it builds from the command line. The original sketches are archived untouched in [`firmware/original-sketches/`](firmware/original-sketches).

### The test bench came first

None of the control work waited for the mechanical build. While the ankle was still being machined, we put a bench together with just the motor, the H-bridge and the encoder, and developed against that.

[`firmware/original-sketches/motor-test-bench/`](firmware/original-sketches/motor-test-bench) is that work, and the sketches read as the sequence we actually went through:

| Stage | Sketches |
|---|---|
| Encoder alone, magnet detection and angle read | `encoder_angle` |
| Encoder driving the motor through the H-bridge | `read_angle_with_motor` |
| Current sensing, standalone then under load | `CURRENT_SENSOR`, `CURRENT_SENSOR_motor` |
| Load cell characterisation | `Read_1x_load_cell_renewed` |
| Moving to an RTOS, one concern per task | `FreeRTOS_sketch`, `testing_encoder_with_rtos`, `target_angle_and_printing` |
| PID under the scheduler | `rtos_with_pid`, `rtos_with_pid_library` |
| Position control, then position plus current | `pid_position_control_code`, `pid_position_control_current_code` |
| Full bench controller with the gait trajectory | `FINAL_PID_CODE_WITH_RTOS` |

By the time the assembled ankle existed, the loop was already tuned and the gains were known. The move to the ESP32 and the load cell array came after that, on a controller we already trusted.

### Tuning it

We tuned by hand over the serial link, changing one gain at a time and watching the step response on the serial plot. Green is the setpoint, blue is the measured angle, red is the PID output:

![PID step response at Kp 0.2 and 0.3](media/results/pid-tuning-kp-0.2-vs-0.3.png)

The rest of the sweep is in [`media/results/`](media/results), along with an ACS712 current trace from the motor.

### About the trajectory units

`WalkingTraj[101]` holds the reference trajectory: 101 samples covering one gait cycle, derived from published human ankle kinematics.

**Those values are raw encoder counts, not degrees.** The loop runs directly on the count, with the joint's zero offset subtracted:

```c
raw = revolution * 4096 + raw_angle;   // AS5600 is 12-bit, so 4096 counts/turn
raw = raw - ZeroClaib;                 // measured offset of the assembled joint
```

To read the array in degrees, multiply by 360/4096, about 0.0879 degrees per count. The range of roughly -74 to +156 counts is therefore about -6.5 to +13.7 degrees at the encoder, which the ball screw and foot linkage map onto the anatomical ankle range.

Our earlier AVR build did convert to degrees in firmware (`ang = raw * 0.087`) and ran the loop on that. The line is still there in the ESP32 source, commented out. If you compare the two builds, that is the difference to watch for: the same trajectory array means counts in one and degrees in the other.

---

## What worked and what didn't

Setting this out plainly, since an archive that oversells itself is no use to anyone reading it.

**We got working:**

- Closed-loop position control of the ankle joint, following the reference trajectory
- Gait-phase detection from the toe and heel load cells, demonstrated on hardware
- A complete mechanical design with FEA behind it, manufactured into a working prototype

**The limitations are real:**

- **A cycle takes about 5 seconds**, against roughly 1 second for a person walking. The drill motor simply could not follow the trajectory any faster. This demonstrates the control concept on a bench. It is not a device anyone could walk on.
- **Motor authority is capped at 125 of 255 PWM**, about 49 percent duty. We set that ceiling deliberately to keep the improvised drivetrain from tearing itself apart. There is also a hard travel stop: outside -80 to +170 counts the motor is cut regardless of what the loop asks for.
- **There is no torque or impedance control.** The ankle tracks position and nothing else. A real prosthesis needs compliance that changes through the gait cycle; ours is stiff the whole way through.
- **It was never tested on an amputee.** All of our testing happened on the bench.

---

## What is in this repository

```
firmware/
  ankle_controller_esp32/   the final controller, ported to plain C++ sources
  original-sketches/        the 2021 .ino sketches exactly as they were written
    ankle_controller_esp32/   the final ESP32 build, the one that ran
    loadcell_bench_esp32/     load-cell isolation test: sensors on, motor loop off
    motor-test-bench/         AVR bench: motor, H-bridge, encoder, PID tuning
docs/
  graduation-book-2021-07-03.pdf                full group thesis, 111 pp.
  individual-contribution-mohamed-tawakol.docx  individual section
  project-proposal-2020.pdf
  bill-of-materials.pdf
media/
  demo/  hardware/  results/
REFERENCES.md      cited literature, by DOI
```

### One thing worth explaining before you go looking

**The book says Arduino Uno; the code says ESP32.** The electrical chapter of the graduation book describes an "Arduino Uno ATmega328" as the main board, and the bill of materials lists one. That chapter was written before we moved to the ESP32 and never revised afterwards. Trust the firmware and the second wiring diagram: the final controller is an ESP32. We used the same IDE for both boards, which is probably where the confusion started.

While you are at it, note that `firmware/original-sketches/motor-test-bench/FINAL_PID_CODE_WITH_RTOS/` has "FINAL" in its name but includes `Arduino_FreeRTOS.h` and calls `analogWrite()`, both of which are AVR-only. The name is misleading: it is the final *bench* controller, the last milestone before the ESP32 build, not the final firmware.

### About the commit history

The first commit holds the 2021 files exactly as they were archived. Two later commits change behaviour: one re-enables the load cell tasks in `setup()`, and the port commit restores the gait-phase state machine inside `TrajGen`. Both had been commented out in the saved file, leaving a build where the load cells ran without affecting motion.

Keeping the archived state as the first commit means the history shows what was found as well as what was changed. The WiFi credentials are the one exception: they were redacted throughout, including in history.

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
