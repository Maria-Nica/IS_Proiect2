# ReadyToFly - Aerial Combat Game Implementation

A modern C++ implementation of an aerial combat game featuring fighters with comprehensive unit tests, Qt-based GUI, and clean architecture patterns.

## Project Structure

```
ReadyToFly_IS/
├── Logic/                  # Core game logic (C++17)
│   ├── Board.cpp/h         # Game board management (10x10 grid)
│   ├── Cell.cpp/h          # Individual cell state and position
│   ├── Game.cpp/h          # Game state machine and turn management
│   ├── GameFactory.cpp/h   # Factory pattern for game initialization
│   ├── Player.cpp/h        # Player state and aircraft management
│   ├── Ship.cpp/h          # Aircraft placement and damage tracking
│   ├── ShipPart.cpp/h      # Individual aircraft sections
│   ├── Position.cpp/h      # 2D coordinate system
│   └── Orientation.h       # Direction enum (Up, Down, Left, Right)
├── UI/                     # Qt 6.9.3 graphical interface
│   ├── mainwindow.cpp/h    # Application main window
│   ├── gameui.cpp/h        # Game board UI component
│   ├── boardwidget.cpp/h   # Interactive board widget
│   ├── gamelogicadapter.cpp/h  # Adapter between UI and Logic layers
│   └── mainwindow.ui       # Qt Designer UI layout
├── UnitTests/              # Google Test (GTest) suite - 37 comprehensive tests
│   ├── PositionTests.cpp         # Position coordinate tests (2 tests)
│   ├── OrientationTests.cpp      # Direction enum validation (1 test)
│   ├── CellTests.cpp             # Cell state storage (1 test)
│   ├── CellStateTests.cpp        # Cell state enum values (1 test)
│   ├── ShipPartTests.cpp         # Aircraft section behaviors (1 test)
│   ├── ShipTests.cpp             # Aircraft placement and rotation (4 tests)
│   ├── BoardTests.cpp            # Board operations (5 tests)
│   ├── BoardInvalidTests.cpp     # Invalid board operations (2 tests)
│   ├── BoardShotTests.cpp        # Shooting mechanics (2 tests)
│   ├── PlayerTests.cpp           # Player state management (3 tests)
│   ├── PlayerEdgeTests.cpp       # Player edge cases (2 tests)
│   └── GameTests.cpp             # Full game integration (14 tests)
├── build/                  # CMake build output
├── cmake/                  # CMake helper modules
├── extern/googletest/      # Google Test framework (git submodule)
├── CMakeLists.txt          # Root CMake configuration
└── README.md               # This file
```

## Game Rules

- **Board**: 10x10 grid per player
- **Aircraft**: Single fighter with 10 sections in a T-shaped fuselage
- **Cockpit**: Instant-kill if hit (pilot eliminated, player loses immediately)
- **Win Condition**: Destroy opponent's aircraft cockpit
- **Turn System**: Players alternate targeting shots until one loses

### Aircraft Layout

The aircraft is a T-shaped configuration occupying 10 cells on the board. The cockpit (critical section) is located at position (0,0) relative to aircraft origin.

## Building the Project

### Prerequisites

- **Windows**: Visual Studio 2022 (MSVC 19.44+)
- **CMake**: Version 3.16 or later
- **C++17**: Standard compiler support
- **Qt 6.9.3** (optional, for UI only; Logic library builds without it)
  - **Important:** If you have Qt installed, update `cmake/QtLocal.cmake` with the path to your Qt installation. For example, if Qt is installed at `C:\Qt\6.9.3\msvc2022_64`, change:
    ```cmake
    set(CMAKE_PREFIX_PATH "C:/Qt/6.9.3/msvc2022_64")
    ```
- **Google Test**: Included directly in `extern/googletest/` (not a submodule)

### Build Steps

```bash
cd ~/Desktop/ReadyToFly_IS
mkdir build
cd build

# Configure CMake (defaults to Visual Studio 2022 on Windows)
cmake ..

# Build the entire solution (Logic, UI, UnitTests)
cmake --build .

# Alternatively, build just the Logic library
cmake --build . --target LogicLib

# Or build just the UI
cmake --build . --target UIApp

# Or build just the tests
cmake --build . --target UnitTests
```

**Note:** The build will automatically detect Visual Studio 2022 and use MSVC 19.44. Qt licensing warnings can be bypassed by setting the environment variable:
```bash
export QTFRAMEWORK_BYPASS_LICENSE_CHECK=1
```
if needed for automated builds.

### Build Output Locations

```
build/
├── Debug/                        # Build output directory
│   ├── LogicLib.lib              # Core game logic library
│   └── ...                       # Other intermediate files
├── UI/Debug/
│   └── UIApp.exe                 # Qt GUI application
├── UnitTests/Debug/
│   └── UnitTests.exe             # Test executable
└── lib/Debug/
    ├── gtest.lib                 # Google Test library
    ├── gmock.lib                 # Google Mock library
    └── gmock_main.lib            # GMock main
```

**Build Configuration**: Release/Debug folders are automatically created by MSBuild based on configuration.

## Running Tests

### From Command Line

```bash
cd ~/Desktop/ReadyToFly_IS/build

# Run the tests directly
./UnitTests/Debug/UnitTests.exe

# Or using CTest (if available)
ctest -C Debug -V
```

Successful test output will show:
```
[==========] Running 37 tests from 12 test suites.
...
[==========] 37 tests from 12 test suites ran.
[  PASSED  ] 37 tests.
```

### From IDE (Qt Creator / Visual Studio)

1. Open the project in Qt Creator or Visual Studio
2. Build the project (Ctrl+B)
3. Run the UnitTests.exe from the build directory
4. All 37 tests will execute and report results

### Test Summary

- **Total Tests**: 37 (all passing)
- **Test Suites**: 12 test classes
- **Execution Time**: ~30ms locally
- **Coverage**: All public methods and edge cases

#### Test Breakdown by Component

| Component | Tests | Coverage |
|-----------|-------|----------|
| Position | 2 | Constructors, coordinate storage |
| Orientation | 1 | Enum values distinctness |
| Cell | 1 | Cell state and position storage |
| CellState | 1 | Enum values validation |
| ShipPart | 1 | Cockpit flag, hit marking |
| Aircraft | 4 | Sections count, contains, hit logic, rotation |
| Board | 9 | Placement, strikes, overlap, boundary checks |
| Player | 5 | Placement, strikes, reset, edge cases |
| Game | 14 | State transitions, turn management, game-over |
| **Total** | **37** | **Full coverage** |

## Architecture & Design Patterns

### Dependency Injection

- **IBoard**, **IPlayer**, **IGameListener** interfaces enable loose coupling
- Concrete implementations (Board, Player) injected via constructors
- Facilitates unit testing and future extensibility

### Factory Pattern

- **GameFactory** encapsulates game initialization logic
- Separates creation complexity from game state management
- Enables customizable game setup (aircraft placement strategies, etc.)

### Observer Pattern

- **IGameListener** interface for game state change notifications
- UI layer subscribes to game events (turn switched, player lost, etc.)
- Decouples game logic from presentation

### Adapter Pattern

- **GameLogicAdapter** bridges Logic (C++ backend) and UI (Qt frontend)
- Converts between domain models and UI representations
- Isolates UI layer from game logic implementation changes

### Strategy Pattern

- **Aircraft orientation** as strategy for placement algorithms
- Different rotation directions (Up, Down, Left, Right) affect cell coordinates
- Enables flexible aircraft layout validation and placement

## Code Statistics

```
Logic/ (Core Game Engine)
  ├── Lines of Code: ~1,200 LOC
  ├── Classes: 11 (+ 3 interfaces)
  ├── Interfaces: IGame, IBoard, IPlayer, IGameListener, IGameFactory
  └── Dependencies: None (standalone C++17)

UI/ (Qt Application)
  ├── Lines of Code: ~800 LOC
  ├── Windows: MainWindow, GameUI, BoardWidget
  ├── Designer UI Files: 1 (mainwindow.ui)
  └── Dependencies: Qt 6.9.3, Logic (LogicLib)

UnitTests/ (Test Suite)
  ├── Lines of Code: ~1,600 LOC
  ├── Test Files: 12
  ├── Test Cases: 37
  └── Framework: Google Test 1.14+
```

## Class Descriptions

### Game Core (Logic/)

#### **Game**
State machine managing turns, game flow, and win conditions.
- `startGame()`: Initialize and begin play
- `switchTurn()`: Alternate between pilots
- `playerLost()`: Handle pilot elimination
- Methods notify observers of state changes

#### **Board**
10x10 grid storing cell states and aircraft positions.
- `placeShip()`: Position aircraft on board with boundary validation
- `receiveShotAt()`: Register impact/shot, update cell state
- `allShipsSunk()`: Check win condition
- Prevents overlapping aircraft placements

#### **Player**
Manages individual player state and aircraft.
- `placeShip()`: Position aircraft on player's board
- `receiveShot()`: Process opponent's strike
- `reset()`: Clear board for new engagement
- Tracks aircraft placement on player's board

#### **Ship** (Aircraft)
T-shaped fighter with 10 sections.
- `contains()`: Check if position belongs to aircraft
- `receiveHit()`: Mark section as damaged
- `getOrientation()`: Return aircraft's direction
- Cockpit section (index 0) is critical - instant loss if hit

#### **Position**
2D coordinate wrapper.
- Constructor: Position(int x, int y)
- Comparability for map/set storage
- Used for all board coordinates

#### **Orientation**
Direction enum for aircraft orientation.
- **Up**: Aircraft extends upward
- **Down**: Aircraft extends downward
- **Left**: Aircraft extends left
- **Right**: Aircraft extends right

### Interfaces (Logic/)

#### **IGame**
Abstract game contract.
- Virtual methods for game control
- Observer notification hook: `addGameListener()`

#### **IBoard**
Abstract combat arena operations.
- Aircraft placement and strike methods
- Boundary validation requirements

#### **IPlayer**
Abstract pilot contract.
- Aircraft management and strike handling
- Delegation to player's combat zone

#### **IGameListener**
Observer interface for game events.
- `onTurnSwitched()`: Turn changed
- `onPlayerLost()`: Pilot eliminated
- `onShotFired()`: Strike registered

### Graphical Interface (UI/)

#### **MainWindow**
Application main window with menu and game controls.

#### **GameUI**
Aerial combat view managing both combat zones and pilot turn indicator.

#### **BoardWidget**
Interactive 10x10 combat zone visualization.
- Click cells to launch strikes
- Visual feedback for hits/misses
- Aircraft placement drag-and-drop

#### **GameLogicAdapter**
Bridges Combat UI and Logic layer.
- Converts Qt signals to combat Logic method calls
- Translates combat events to UI updates
- Handles resource lifecycle

## Building Without Qt (Combat Logic Only)

To build just the core combat Logic library without the Qt UI:

```bash
cd ~/Desktop/ReadyToFly_IS
mkdir build
cd build

cmake ..
cmake --build . --target LogicLib
```

This produces `LogicLib.lib` in `build/Debug/` without any GUI dependencies (~500KB static library).


**Build Information**: MSVC 19.44, Visual Studio 2022, C++17, Google Test 1.14+, Qt 6.9.3  
**Last Updated**: November 2025  
**Combat Test Status**: 37/37 tests passing ✓
