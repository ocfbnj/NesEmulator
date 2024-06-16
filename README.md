# NesEmulator

[![CI](https://github.com/ocfbnj/NesEmulator/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/ocfbnj/NesEmulator/actions/workflows/ci.yml)

This is an NES emulator. It implements Mapper0-4, which cover about 80% of iNes rom. The following games have been
tested, other games may or may not work:

- Super Mario Bros.
- Battle City
- Contra
- Teenage Mutant Ninja Turtles
- Double Dragon
- Zelda

## Usage

~~~bash
./NesEmulator <nes file path>
~~~

### Controller

#### Player1

| Button | Mapped to |
| :----: | :-------: |
|   A    |     J     |
|   B    |     K     |
| Select |   Space   |
| Start  |   Enter   |
|   Up   |     W     |
|  Down  |     S     |
|  Left  |     A     |
| Right  |     D     |

#### Player2

| Button |  Mapped to  |
| :----: | :---------: |
|   A    |    Num1     |
|   B    |    Num2     |
| Select | Right Shift |
| Start  |    Num0     |
|   Up   |     Up      |
|  Down  |    Down     |
|  Left  |    Left     |
| Right  |    Right    |

#### Other

| Button |    Effect     |
| :----: | :-----------: |
|   R    |     Reset     |
|   I    |  Quick Save   |
|   L    | Quick Restore |

## How to build

### Prerequisites

- A compiler supporting C++20.
- Python3 installed.
- CMake installed.

### Building with Conan Package Manager

1. Install Conan

    ~~~bash
    pip install conan -U
    conan profile detect --force
    ~~~

2. Clone and Build

    **Windows**

    ~~~bash
    git clone https://github.com/ocfbnj/NesEmulator.git
    cd NesEmulator
    conan install . --build=missing
    cmake --preset=conan-default
    cmake --build --preset=conan-release
    ~~~

    **Linux, macOS**

    ~~~bash
    git clone https://github.com/ocfbnj/NesEmulator.git
    cd NesEmulator
    conan install . --build=missing
    cmake --preset=conan-release
    cmake --build --preset=conan-release
    ~~~

Now, you can find the binary in `build` directory.

## Screenshots

![Super Mario Bros](./images/Super%20Mario%20Bros.png)
![Battle City](images/Battle%20City.png)
![Contra](./images/Contra.png)
![Teenage Mutant Ninja Turtles III](./images/Teenage%20Mutant%20Ninja%20Turtles%20III.png)
![Double Dragon II](./images/Double%20Dragon%20II.png)

## References

Developing an NES emulator is exciting and interesting 😀. If you also want to develop your own one, you can refer to the following tutorials and references. ❤️

### Nesdev Wiki

- <https://wiki.nesdev.org>

### Tutorials

- <https://bugzmanov.github.io/nes_ebook>
- <https://www.youtube.com/playlist?list=PLrOv9FMX8xJHqMvSGB_9G9nZZ_4IgteYf>
- <https://www.jianshu.com/u/daef9bb95a24>

### Other NES implementations

- <https://github.com/bugzmanov/rustness_monster>
- <https://github.com/fogleman/nes>
- <https://github.com/OneLoneCoder/olcNES>
- <https://github.com/amhndu/SimpleNES>

### CPU

- <https://skilldrick.github.io/easy6502>
- <https://www.masswerk.at/6502/6502_instruction_set.html>
- <http://obelisk.me.uk/6502/> (This page was closed.)

### Graphic

- <https://learnopengl-cn.github.io>
- <https://www.youtube.com/playlist?list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M->
- <https://github.com/OneLoneCoder/olcPixelGameEngine>

### Audio

- <https://www.sfml-dev.org/tutorials/2.5/audio-streams.php>
- <https://github.com/SFML/SFML/blob/HEAD/src/SFML/Audio/SoundStream.cpp>
- <https://www.youtube.com/watch?v=tgamhuQnOkM>
- <https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf>
