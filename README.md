Table of Contents
=================
   * [Build & Run](#build--run)
      * [Prerequisites](#prerequisites)
      * [Build](#build)
      * [Run](#run)
      * [Test](#test)
      * [Clean](#clean)
   * [Description](#description)
   * [Code Overview](#code-overview)


# Build & Run
## Prerequisites
- CMake 3.27 or higher:
```bash
# For MSYS2 (MinGW64 on Windows)
pacman -S mingw-w64-x86_64-cmake
# For Ubuntu
sudo snap install cmake --classic
# For macOS
brew install cmake
```
- C++20 compatible compiler:
```bash
# For MSYS2 (MinGW64 on Windows)
pacman -S mingw-w64-x86_64-gcc
# For Ubuntu
sudo apt-get install g++
# For macOS
brew install gcc
```

## Build
1. Clone this repository:
```bash
git clone https://github.com/damikh13/YADRO_Recruitment_Task.git
cd YADRO_Recruitment_Task
```
2. Make a build directory and navigate to it:
```bash
mkdir build
cd build
```
3. Generate build files using CMake:
```bash
cmake ..
```
4. Build the project:
```bash
cmake --build .
```

This will create 3 executables in the `build` directory:
- `computer_club` - the main program
- `test_HELPERS` - unit tests for the `helpers` module
- `test_PARSING` - unit tests for the `parsing` module

## Run
```bash
./computer_club <input_file>
```
For example:
```bash
./computer_club ../input/inp1.txt
./computer_club ../input/inp2.txt
./computer_club ../input/inp3.txt
```

## Test
For Unit Tests, you can use CTest:
```bash
ctest
```
or run the tests manually:
```bash
./test_HELPERS
./test_PARSING
```

## Clean
```bash
rm -rf build
```
# Description
The full task description can be found [here](description.pdf)

# Code Overview
1. `Computer_Club_STRUCTS.h` - main data structures: Client, Time, Table, Event. Some of them have overloaded operators for comparison, arithmetics, and stream output.
2. `parsing_functions` - functions for parsing input data from a file. Checks for time and ID format (fully skips event bodies), and throws exceptions if the format is incorrect.
3. `Computer_Club.h` - main class for the program. Contains the main logic for processing events and clients.
It iterates through a vector of events, checking if new event was generated (using `std::optional<Event>`), and then replacing current processed event with the new one.
Event processing is done by means of `Computer_Club.handle_event_()` function, that checks event.ID and calls corresponding function.
These functions change the state of the club (e.g. add or remove clients, change table status, alter waiting list, etc.).
At the end of the day, all clients are asked to leave in alphabetic order.
Then, the club income is printed out.
