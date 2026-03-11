# Installation Guide for Codemon

## What is Codemon?

Codemon is an interactive educational game where players explore a world, encounter coding challenges, and battle trainers by solving programming problems. It combines game mechanics with learning-by-doing code challenges to make programming practice engaging.

## System Requirements

- **Operating System**: macOS, Linux, or Windows
- **RAM**: 2 GB minimum
- **Disk Space**: 500 MB (including dependencies)
- **Display**: 1024x768 or higher resolution

## Prerequisites

Before installing Codemon, you must have the following software installed on your system:

### 1. C++ Compiler
- **macOS**: Install Xcode Command Line Tools
  ```bash
  xcode-select --install
  ```
- **Linux (Ubuntu/Debian)**: Install build essentials
  ```bash
  sudo apt-get update
  sudo apt-get install build-essential
  ```
- **Windows**: Install Visual Studio Community with C++ support or MinGW

### 2. CMake (3.20 or higher)
- **macOS**: 
  ```bash
  brew install cmake
  ```
- **Linux**:
  ```bash
  sudo apt-get install cmake
  ```
- **Windows**: Download from [cmake.org](https://cmake.org/download)

### 3. Git
- **macOS**:
  ```bash
  brew install git
  ```
- **Linux**:
  ```bash
  sudo apt-get install git
  ```
- **Windows**: Download from [git-scm.com](https://git-scm.com)

### 4. SFML 3.0 (or 2.6+)
This dependency is required. Codemon supports both SFML 3.0 and SFML 2.6+.

- **macOS**:
  ```bash
  brew install sfml
  ```
- **Linux**:
  ```bash
  sudo apt-get install libsfml-dev
  ```
- **Windows**: Download precompiled binaries from [sfml-dev.org](https://www.sfml-dev.org/download.php) or use vcpkg:
  ```bash
  vcpkg install sfml
  ```

### 5. Google Test (for running tests)
This is included as a CMake dependency and downloads automatically during build.

## Installation Steps

### Step 1: Clone the Repository

```bash
git clone https://github.com/Connor-Allen10/codemon.git
cd codemon
```

### Step 2: Create a Build Directory

```bash
mkdir build
cd build
```

### Step 3: Configure with CMake

```bash
cmake ..
```

For macOS users with specific SFML installation paths:
```bash
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix sfml)
```

For Windows users with Visual Studio:
```bash
cmake .. -G "Visual Studio 16 2019"
```

### Step 4: Build the Project

```bash
cmake --build . -j4
```

Or on Unix/Linux/macOS:
```bash
make -j4
```

The compiled executable will be created as `codemon` (or `codemon.exe` on Windows) in the `build/` directory.

### Step 5: Verify Installation

Run the executable to verify it works:

**macOS/Linux**:
```bash
./codemon
```

**Windows**:
```bash
.\codemon.exe
```

If the game window opens without errors, the installation is complete.

## Running the Application

### From the Build Directory
```bash
cd /path/to/codemon/build
./codemon
```

### From Any Location (Optional)
Copy the executable to a location in your PATH or create a symbolic link:

**macOS/Linux**:
```bash
sudo cp /path/to/codemon/build/codemon /usr/local/bin/
```

Then run from anywhere:
```bash
codemon
```

Note: this step is optional and may require administrator access.

## Game Controls

- **WASD** or **Arrow Keys**: Move character
- **F1** or **E**: Open debug challenge editor (when encountering trainers)
- **Enter**: Submit answer in editor
- **ESC**: Close debug window
- **Mouse**: Navigate menus and UI

## Running Tests

Codemon includes automated tests for the challenge loading system and other components.

### Run All Tests
```bash
cd /path/to/codemon/build
./run_tests
```

Or using CTest:
```bash
ctest --output-on-failure
```

### Expected Output
All tests should pass (43 tests total):
```
[==========] 43 tests from X test suites ran.
[  PASSED  ] 43 tests.
```

## Troubleshooting

### Issue: CMake configuration fails with "SFML not found"
**Solution**: 
- Ensure SFML is installed (see Prerequisites above)
- On macOS with Homebrew, set the prefix path:
  ```bash
  cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix sfml)
  ```

### Issue: Build fails with linker errors
**Solution**:
- Ensure all dependencies are installed
- Try a clean rebuild:
  ```bash
  cd build
  rm -rf *
  cmake ..
  cmake --build .
  ```

### Issue: Application crashes on startup
**Solution**:
- Verify asset files are in the correct location:
  ```bash
  ls src/assets/data/map01.csv
  ls src/assets/player_south.png
  ```
- Rebuild so assets are copied into `build/assets`:
  ```bash
  cd build
  cmake ..
  cmake --build .
  ```

### Issue: TGUI window (debug editor) fails to open
**Solution**:
- TGUI support is optional. If unavailable, a text overlay is used instead
- To enable TGUI, ensure it's installed and rebuild:
  ```bash
  cmake .. -DCMAKE_PREFIX_PATH=/path/to/tgui
  cmake --build .
  ```

### Issue: Tests fail to run
**Solution**:
- Ensure Google Test was downloaded during CMake configuration
- Rebuild and try again:
  ```bash
  cd build
  cmake --build .
  ctest --output-on-failure
  ```

## Project Structure

```
codemon/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project overview
├── INSTALL.md                  # Installation and usage instructions
├── challenges.txt              # Debug challenge data
├── docs/                       # Developer and user guides
├── external/                   # Vendored dependencies (e.g., TGUI)
├── src/
│   ├── assets/                 # Source assets copied to build/assets
│   ├── battle/                 # Battle system
│   ├── debug/                  # Debug challenge system
│   ├── game/                   # Core game/state stack
│   ├── states/                 # World/menu/battle state logic
│   └── world/                  # Tile map and overworld logic
│   ├── main.cpp                # Entry point
├── tests/                      # Unit tests (GoogleTest)
└── build/                      # Generated build outputs (local)
```

## Getting Help

- **Documentation**: See [README.md](README.md) for project overview
- **Developer Guide**: See [docs/DEVELOPER_GUIDE.md](docs/DEVELOPER_GUIDE.md)
- **Issues**: Report bugs on [GitHub Issues](https://github.com/Connor-Allen10/codemon/issues)

## Building on Different Platforms

### Detailed macOS Instructions
```bash
# Install Xcode tools
xcode-select --install

# Install CMake (if not already installed)
brew install cmake

# Clone and build
git clone https://github.com/Connor-Allen10/codemon.git
cd codemon
mkdir build && cd build
cmake ..
cmake --build . -j4

# Run
./codemon
```

### Detailed Linux Instructions (Ubuntu 20.04+)
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake git libsfml-dev

# Clone and build
git clone https://github.com/Connor-Allen10/codemon.git
cd codemon
mkdir build && cd build
cmake ..
cmake --build . -j4

# Run
./codemon
```

### Detailed Windows Instructions (Visual Studio 2019)
```bash
# Install Visual Studio 2019 Community with C++ support
# Install CMake from cmake.org
# Install Git from git-scm.com

# Clone repository
git clone https://github.com/Connor-Allen10/codemon.git
cd codemon

# Create build directory
mkdir build
cd build

# Configure
cmake .. -G "Visual Studio 16 2019"

# Build
cmake --build . --config Release

# Run
Release\codemon.exe
```

## Performance Notes

- The game is optimized for 60 FPS on modern hardware
- Minimum CPU: Intel Core i5 or equivalent
- The debug challenge system supports loading challenges from external files (see `challenges.txt`)

## License

This repository currently does not include a root-level `LICENSE` file.
Check with the project maintainers for licensing details before redistribution.
