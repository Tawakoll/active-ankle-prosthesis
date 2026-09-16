# Ankle controller, ported to C++

This is the 2021 ESP32 controller with the `.ino` structure unwound
into ordinary translation units: shared state declared in `include/config.h`,
defined once in `src/main.cpp`, one task per source file.

Behaviour is unchanged. The port adds the includes and prototypes that the
IDE used to generate, and nothing else.

| File | Task |
|---|---|
| `src/main.cpp` | globals, `setup()`, task startup |
| `src/trajectory.cpp` | `TrajGen`, gait-phase state machine and reference trajectory |
| `src/encoder.cpp` | `read_ANGLE`, AS5600 with revolution counting |
| `src/pid_control.cpp` | `pid`, position loop and motor output |
| `src/load_cells.cpp` | `toe_cells`, `heel_cells`, contact detection |
| `src/telemetry.cpp` | `Communication`, serial output and live gain tuning |
| `src/blink.cpp` | `myblink`, bring-up heartbeat, not started by default |
| `src/wifi_server.cpp` | `wifi_communication`, HTTP start/stop, not started by default |

## A note on the language

The original was written in a procedural C style: plain functions, global
state, no user-defined classes. It is compiled as **C++**, because the framework
core and every library it depends on (`PID_v1`, `HX711_ADC`, `AS5600`,
`WiFi`) are C++ and are used as objects. A C compiler cannot build this
source, and renaming the files would not change that.

The Arduino IDE was the build environment throughout, chosen for simplicity
and because it is free and open source. It compiles `.ino` as C++ behind the
scenes, which is why the original files never had to say so.

## Building

With PlatformIO:

```
pio run -t upload
pio device monitor
```

The original sketches are archived, unmodified apart from redacted WiFi
credentials, under [`../original-sketches/`](../original-sketches).
