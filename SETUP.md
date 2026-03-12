This guide explains how to set up, build, and run **CodeMon** on a new machine.
It is written for system administrators and developers preparing a deployable build.

---

## 1. System Requirements

| Component | Minimum |
|---|---|
| OS | macOS 13+, Ubuntu 22.04+, Windows 10 |
| CPU | x86-64 or ARM64 |
| RAM | 512 MB |
| Disk | ~500 MB |
| Display | 1024×768 |

---

## 2. Required Software

| Software | Version | Purpose |
|---|---|---|
| C++ Compiler | C++17+ | Build source |
| CMake | 3.20+ | Configure/build |
| Git | Recent | Clone repo |
| SFML | 2.6+ or 3.x | Graphics/audio/input |
| GoogleTest | Auto-fetched | Test framework |

Notes:
- TGUI is vendored in `external/TGUI/` (when used by platform config).
- GoogleTest is fetched by CMake during first configure.

---

## 3. Platform Setup

### macOS

Install tools:

```bash
xcode-select --install
brew install cmake sfml
```

### Ubuntu / Debian

Install tools:

```bash
sudo apt update
sudo apt install -y \
build-essential cmake git libsfml-dev xvfb
```

### Windows

Install:
1. Visual Studio 2022 (Desktop development with C++)
2. CMake 3.20+
3. Git
4. vcpkg + SFML:

```powershell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install sfml:x64-windows
```

Configure with vcpkg toolchain:

```powershell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

---

## 4. Get Source Code

```bash
git clone https://github.com/Connor-Allen10/codemon.git
cd codemon
```

---

## 5. Configure and Build

From repo root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Build outputs:
- `build/codemon`
- `build/run_tests`
- `build/assets/` (copied assets)

---

## 6. Run Application

From repo root:

```bash
./build/codemon
```

Run from repository root so runtime files (like `challenges.txt`) resolve correctly.

---

## 7. Run Tests

Primary test command:

```bash
./build/run_tests
```

CTest command:

```bash
ctest --test-dir build --output-on-failure
```

Linux CI/headless environments may require Xvfb:

```bash
xvfb-run -a ctest --test-dir build --output-on-failure
```

---

## 8. Deployment Notes

- Ensure `build/assets/` exists after build.
- Keep `challenges.txt` in repository root for challenge loading.
- Track known issues in GitHub Issues.

---

## 9. Troubleshooting

### SFML not found
- macOS: check Homebrew path (`/opt/homebrew` on Apple Silicon).
- Linux: verify `libsfml-dev` is installed.
- Windows: verify `CMAKE_TOOLCHAIN_FILE` for vcpkg.

### Missing runtime images/fonts
- Re-run configure/build from repo root.
- Verify `build/assets/` contents.

### Tests fail in headless Linux
- Use `xvfb-run -a` when running CTest.

### Challenge file fallback message
- Ensure `challenges.txt` exists in the working directory (repo root recommended).
