# systemy_wbudowane — Embedded Systems Project

## Overview
A collection of firmware, hardware schematics, and tools for embedded systems experiments and exercises. Focused on microcontroller development, peripherals, and educational examples.

## Features
- Minimal RTOS-free firmware examples
- Peripheral drivers (GPIO, UART, I2C, SPI, ADC, PWM)
- Build and flash scripts for common toolchains
- Example projects demonstrating sensors and actuators
- Tests and CI-friendly build targets

## Requirements
- Host: Linux/macOS/Windows with POSIX-like shell (Windows users: WSL or Git Bash recommended)
- Toolchains: arm-none-eabi-gcc (or vendor SDK), OpenOCD or vendor programmer
- Utilities: make, cmake (optional), python3 (for scripts/tools)

## Supported Hardware (examples)
- STM32F0/F1/F4 series (change per project folder)
- ESP32
- AVR (Atmega328P)
- Generic ARM Cortex-M development boards

## Quick Start
1. Install toolchain and programmer.
2. Clone repository:
    ```sh
    git clone
    cd systemy_wbudowane
    ```
3. Build example (replace example-name as needed):
    ```sh
    cd examples/blink
    make
    ```
4. Flash to device:
    ```sh
    make flash
    ```
5. Monitor serial output:
    ```sh
    picocom --baud 115200 /dev/ttyUSB0
    ```

## Project Layout
- /boards — board support packages and pinmaps
- /drivers — peripheral drivers and HALs
- /examples — standalone example applications
- /tools — scripts for build, flash, and debugging
- /docs — design notes and schematics

## Contributing
- Fork → branch → PR. Follow existing code style and document changes.
- Add tests/examples for new drivers.

## License
Specify a license in LICENSE file (e.g., MIT). If absent, add one before publishing.

## Notes
Adjust board-specific settings in `Makefile` or build configuration before building/flashing.
