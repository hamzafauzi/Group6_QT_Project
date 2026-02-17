# Embedded Software Design and Programming — Exam Project (WS 25/26)

**Course:** Embedded Software Design and Programming

**Instructor:** Prof. Dr.-Ing. Mürsel Yildiz

**Group 6:** Vincent Siebert (43130), Hamza Mubashir Fauzi (471554), Wasif Gul (477118), Hüseyin Safa Ünlü (477019)

## Project Overview
This project:Implements an Arduino simulator using Qt 6 and C++.
The primary objective is to learn or improve skills in programming with C++, developing
graphical user interfaces (GUI) and controlling external processes.

1. The GUI is composed of multiple sections:
i. a control panel including at least two control buttons (Build and Run)
ii. a text editor for users to write the main code
iii. a simulator that displays connections and animations
iv. a panel showing status and debug messages from the main software and
v. the Arduino software compilation output.
2. The user writes the Arduino code, and upon clicking the Build button, the main
software compiles the code and displays the outcome in the compilation/debug
section.
3. The main software performs compilation using available tools for Windows or Linux
environments. These tools are basic Arduino compilers and must be integrated into the
main software. In other words, students shall not attempt on compiling the software
with their own efforts.
4. The main software does not allow the simulation to run unless the compilation is
completed successfully.
5. All activities and software-related issues are reported in the debug messages section.
6. Although the external compiler is responsible for syntax checking, students are
expected to interpret the code logic and perform the simulation accordingly.
7. The number of external components is kept to a minimum, such as LEDs, resistors,
and buttons. Users are allowed to add and remove multiple of these components from
the simulator. Full freedom is provided to create connectio

## How it works

- Edit code using the text box on the left side of the application.
- After changing code, build the project (the app invokes `arduino-cli` to compile sketches).
- After a successful build, run the simulator from within the application.

## Important notes / Known issues

- CLI path error: The application tries to locate `arduino-cli.exe` using several common locations and a team-specific override at the top of `ArduinoCompiler::findArduinoCLI()`.
  - If `arduino-cli` is not found, edit the CLI path in `arduinocompiler.cpp` to match your local installation. The override is near line ~42 in `arduinocompiler.cpp` and looks like this:

    ```cpp
    possiblePaths << "C:/Users/ibaha/arduino-cli_1.4.1_Windows_64bit/arduino-cli.exe"; // Safa
    ```

  - Change that path to your local `arduino-cli.exe` location or ensure `arduino-cli` is on your `PATH`.

- Build/startup errors: If the `arduino-cli` binary cannot be started, the app suggests moving `arduino-cli.exe` to `C:\arduino-cli\` or installing a supported CLI release.

## Supported syntax

The simulator supports a restricted set of Arduino-like constructs. Use only the following in your editor code to ensure compatibility with the simulator and build pipeline:

- `if` / `else`
- `while`
- `digitalRead` / `digitalWrite`
- `setPin` (project-specific helper)
- `setup` and `loop` functions

Avoid other or non-standard syntax to prevent parsing or runtime errors in the simulator.

## Build & Run (quick)

Using Qt Creator (recommended):

- Open the project folder (the directory containing `CMakeLists.txt`) in Qt Creator.
- Configure an appropriate kit (Qt + compiler), configure the CMake build.
- Build the project and run the application from Qt Creator.

## Where to look in source

- Main UI and app logic: `main.cpp`, `mainwindow.ui`, and related items such as `simulatorscene.cpp` and `simulationengine.cpp`.
- Compiler wrapper and CLI lookup: `arduinocompiler.cpp` / `arduinocompiler.h`.
- Code parser and runtime: `arduinoparser.cpp`, `arduinoruntime.cpp`.

## License / Attribution

This repository was created as part of the WS 25/26 course assignment for Prof. Dr.-Ing. Mürsel Yildiz.

---

**Notes**

- `startwindow.cpp` is the central file where the UI, controls, editor and simulator flow come together — it is the most important file.
- The arrows indicate control or data flow (e.g., `startwindow.cpp` uses `arduinocompiler.cpp` to build sketches).

**Files and short descriptions**

- `startwindow.cpp`: Central application window and coordinator — UI control, editor actions, and simulator start/stop.
- `main.cpp`: Application entry point; creates the application instance and opens `startwindow.cpp`.
- `mainwindow.ui`: Main window layout resources referenced by the UI code.
- `simulatorscene.cpp`: Manages the visual scene for the simulator; placement and rendering of components.
- `simulationengine.cpp`: Core simulation logic, tick/update loop and event handling.
- `arduinocompiler.cpp`: Wraps `arduino-cli` to compile temporary sketches; contains the CLI path lookup (edit here if CLI not found).
- `arduinoparser.cpp`: Parses the user's Arduino-like code (limited syntax) into a representation consumable by the runtime.
- `arduinoruntime.cpp`: Executes the parsed representation, simulating `setup`/`loop`, digitalRead/digitalWrite, and pin state.
- `arduinoitem.cpp`: Represents Arduino board UI element used in the scene/editor.
- `componentitem.cpp`: Base class for circuit components; manages child components like pins and visual behavior.
- `pinitem.cpp`: Represents a pin on a component or Arduino board; manages connectivity and I/O state.
- `resistoritem.cpp`: Resistor component behavior and visual representation.
- `leditem.cpp`: LED component — handles on/off state and visual updates.
- `buttonitem.cpp`: Button (input) component — handles user interaction and input state.
- `cableitem.cpp`: Cable/connection between `pinitem`s — represents connections in the scene.
- `addNewElement.cpp` / `addNewElement.ui`: Dialog and UI for adding new components to the scene.
- `info.cpp` / `info.ui`: Information/help dialog.

**CLI path & build reminder**

- If `arduino-cli` is not detected, update the team-specific override in `arduinocompiler.cpp` (near the top of `ArduinoCompiler::findArduinoCLI()`) to point to your `arduino-cli.exe` location, or install `arduino-cli` and add it to your `PATH`.

---

If you'd like, I can also convert this ASCII diagram to a Mermaid diagram and embed it, or place the architecture content into a separate `ARCHITECTURE.md`. Tell me which you prefer and I'll update the repo accordingly.