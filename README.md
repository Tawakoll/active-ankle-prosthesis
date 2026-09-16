# Active Ankle Prosthesis

**A powered ankle built by four engineering students in Cairo, for a fraction of what a commercial one costs.**

![The assembled prosthesis](media/hardware/assembled-ankle.jpeg)

> **Archived academic project, 2021.** This was our B.Sc. graduation project in Mechatronics Engineering at the Arab Academy for Science, Technology & Maritime Transport (AASTMT), Cairo. It is kept here as a record of the work. Nobody maintains it, and it is not a medical device.

A DC motor turns a ball screw, which drives the ankle joint. Load cells in the foot tell the controller which part of the gait cycle the wearer is in, and the ankle follows a reference angle trajectory to match.

The project won a sponsorship scholarship from the **Academy of Scientific Research and Technology (ASRT)**, Egypt, which paid for the build.

---

## Why we built it

Most below-knee amputees in the MENA region walk on passive prostheses, which amount to a spring and a hinge. A passive ankle returns some energy but generates none of its own. Stairs, slopes and long walks therefore cost the wearer far more effort, and years of asymmetric gait bring problems of their own.

Active ankles are sold commercially. Between import duties and regional pricing, they are out of reach for nearly every amputee here.

So we asked a narrow question: could we build one locally, from parts available locally, at a price that made sense locally? The bill of materials below is the honest answer.

---

## The demo

[**`media/demo/gait-cycle-demo.webm`**](media/demo/gait-cycle-demo.webm) shows the ankle working through one complete gait cycle.

---

## The machine

### Actuator

The drivetrain is the part we are least proud of and learned the most from. We could not source a brushless motor with the torque-to-size ratio we wanted inside our budget, so we took the motor out of a cordless drill.

| Part | Spec |
|---|---|
| **Motor** | RS-550S brushed DC (HRS-550S-18V), salvaged from a cordless drill |
| Nominal voltage | 18 V DC (operating range 3–24 V) |
| No-load speed | 22,000 rpm |
| No-load current | 0.9 A |
| Stall current | 62 A |
| Peak efficiency | 8 A, 103 W output |
| **Driver** | Cytron MD10C H-bridge, 10 A continuous, PWM + direction pin |
| **Transmission** | SFU1605 ball screw with nut |
| Screw diameter | 16 mm (13 mm root) |
| Lead | 5 mm per revolution |
| Length | 400 mm |
| Material | Steel alloy, 147 MPa permissible strength |
| **Joint bearing** | SKF 6002 |
| **Power** | 18 V Li-ion pack, 1200 mAh, with a step-down regulator for the logic rail |

We sized the screw against an approximate 1500 N design load with a safety factor, and checked it for buckling as a fixed-supported column over a 130 mm span. The full calculation, along with the Inventor FEA (Von Mises stress, displacement, contact pressure), is worked through in detail in the graduation book.

The drill motor is the root cause of most of the limitations further down this page. Worth knowing that going in.

### Sensing

| Sensor | Qty | What it does |
|---|---|---|
| **AS5600** magnetic rotary encoder (I²C, 12-bit) | 1 | Joint position. Absolute within one turn; firmware counts revolutions for multi-turn travel. |
| **Load cells**, 50 kg full-bridge strain gauge | 4 | Ground contact. Two under the toe, two under the heel. |
| **HX711** 24-bit ADC | 4 | One amplifier per load cell. |
| **ACS712** current sensor, 30 A | 1 | Motor current, for characterisation and keeping the motor inside safe limits. |

Toe and heel contact read together identify the gait phase, whether that is heel strike, flat foot, toe off or swing. That is what tells the controller where the wearer actually is in the cycle, rather than assuming it from a timer.

### Wiring

The system was designed around an Arduino Uno and then moved to an ESP32 partway through, for the extra I/O, the speed and the WiFi. Both diagrams survive, which makes the migration easy to see:

**First design, Arduino Uno:**

![Arduino Uno wiring diagram](media/hardware/wiring-diagram-arduino-uno.png)

**Final design, ESP32:**

![ESP32 wiring diagram](media/hardware/wiring-diagram-esp32.png)

The ESP32 build drops the ACS712 from the loop and swaps the 5 V regulator for a 3.3 V one. Everything else carries over.

---

## Control

An **ESP32** runs the controller as concurrent **FreeRTOS** tasks (using `TridentTD_EasyFreeRTOS32`), one task per job:

```
TrajGen        steps through the reference trajectory, advancing on gait phase
read_ANGLE     reads the AS5600, counts revolutions, converts to degrees
pid            PID position loop; drives motor PWM and direction
toe_cells      2x HX711, toe contact, threshold 400
heel_cells     2x HX711, heel contact, threshold 400
Communication  serial telemetry and live gain tuning
```

The position loop is an ordinary **PID** on joint angle, tuned empirically to **Kp = 0.14, Ki = 0.2, Kd = 0**.

### Tuning it

We tuned by hand over the serial link, changing one gain at a time and watching the step response in the Arduino plotter. Green is the setpoint, blue is the measured angle, red is the PID output:

![PID step response at Kp 0.2 and 0.3](media/results/pid-tuning-kp-0.2-vs-0.3.png)

The rest of the sweep is in [`media/results/`](media/results), along with an ACS712 current trace from the motor.

### About the trajectory units

`WalkingTraj[101]` holds the reference trajectory: 101 samples covering one gait cycle, derived from published human ankle kinematics.

**Those values are encoder-shaft degrees, not ankle joint degrees.** The firmware converts like this:

```c
raw = revolution * 4096 + raw_angle;   // AS5600 is 12-bit, so 4096 counts/turn
ang = raw * 0.087;                     // 360/4096 is about 0.0879 deg per count
```

The array runs from roughly -74 to +156, which is motor-side travel. The ball screw and foot linkage map it onto the anatomical ankle range, somewhere around 20 degrees of plantarflexion to 10 degrees of dorsiflexion. Read the array as joint angles and the numbers will look impossible. They are not.

---

## What it cost

Prices are from 2021, in Egyptian pounds.

| Part | Description | Qty | Price |
|---|---|---|---|
| Load cell | 50 kg full-bridge strain gauge | 8 | EGP 4,250 |
| Cytron MD10C | H-bridge motor driver | 1 | EGP 380 |
| HX711 | 24-bit ADC | 5 | EGP 310 |
| AS5600 | Magnetic encoder, 12-bit | 3 | EGP 270 |
| MEGA328PAU | Arduino Uno (first design) | 1 | EGP 135 |
| ACS712 | Current sensor, 30 A | 2 | EGP 125 |
| RS-550S | DC motor | 1 | EGP 85 |

Quantities include spares; the assembled ankle uses four load cells, four HX711s and one encoder. Full bill of materials in [`docs/bill-of-materials.pdf`](docs/bill-of-materials.pdf).

For context, the motor that drives the whole thing cost EGP 85, roughly five US dollars at the time.

---

## What worked and what didn't

Setting this out plainly, since an archive that oversells itself is no use to anyone reading it.

**We got working:**

- Closed-loop position control of the ankle joint, following the reference trajectory
- Gait-phase detection from the toe and heel load cells, demonstrated on hardware
- A complete mechanical design with FEA behind it, manufactured into a working prototype

**The limitations are real:**

- **A cycle takes about 5 seconds**, against roughly 1 second for a person walking. The drill motor simply could not follow the trajectory any faster. This demonstrates the control concept on a bench. It is not a device anyone could walk on.
- **Motor authority is capped at 100 of 255 PWM**, about 39 percent duty. We set that ceiling deliberately to keep the improvised drivetrain from tearing itself apart.
- **There is no torque or impedance control.** The ankle tracks position and nothing else. A real prosthesis needs compliance that changes through the gait cycle; ours is stiff the whole way through.
- **It was never tested on an amputee.** All of our testing happened on the bench.

---

## What is in this repository

```
firmware/
  ankle_controller_esp32/   the final ESP32 build, the one that ran
  loadcell_bench_esp32/     load-cell isolation test: sensors on, motor loop off
  development_avr/          earlier AVR development sketches
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

**The book says Arduino Uno; the code says ESP32.** The electrical chapter of the graduation book describes an "Arduino Uno ATmega328" as the main board, and the bill of materials lists one. That chapter was written before we moved to the ESP32 and never revised afterwards. Trust the firmware and the second wiring diagram: the final controller is an ESP32. We used the Arduino IDE throughout, which is probably where the confusion started.

While you are at it, note that `firmware/development_avr/FINAL_PID_CODE_WITH_RTOS/` has "FINAL" in its name but includes `Arduino_FreeRTOS.h` and calls `analogWrite()`, both of which are AVR-only. The name is misleading. It is an earlier milestone, not the final build.

### About the commit history

The first commit holds the 2021 files exactly as they were archived. The second re-enables the gait-phase tasks in `ankle_controller_esp32.ino`, which had been commented out during a test session and left that way when the file was saved. Keeping both states means the archive stays honest about what was found and what was changed.

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
