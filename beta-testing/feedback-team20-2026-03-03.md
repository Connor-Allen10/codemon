# Beta Testing Feedback - Team 20

**Date:** 2026-03-03  
**Project Tested:** Codémon (Team 20)  
**Compiled by:** Team 20 from received peer feedback

---

## Tester 1: Nurmukhanbetov Aman

### Q1: README/Purpose
- README is clear, fluent, and easy to follow.
- Purpose and interesting features were understandable.
- Suggestion: add more detail on process history (testing progress, implementation history, risks, technical approach, effects).

### Q2: Installation/Setup
- Needed to install SFML and CMake manually on macOS.
- Encountered difficulties with CMake command until installing with Homebrew.
- Reported software worked better on Windows than their macOS setup.

### Q3: Functional Testing
- Character movement works and feels responsive.
- Issue: After exiting battle, sometimes immediately re-enters another battle without moving.
- Issue: Battle text does not appear (purple screen only).

---

## Tester 2: Kaitlyn McLaughlin

### Q1: README/Purpose
- Build instructions exist.
- Suggestion: move platform-specific notes before build commands.

### Q2: Installation/Setup
- Windows build instructions did not work in PowerShell.
- g++ command in main.cpp did not work on their Windows system.
- Linux build worked with provided instructions.
- Suggestion: include exact command to install libsfml-dev on Linux.
- Suggestion: include movement controls in documentation.

### Q3: Functional Testing
- Movement feels smooth.
- Issue: Battle text did not appear (solid purple screen only).
- Issue: Immediate battle re-trigger after exit, potentially causing endless loop.

---

## Tester 3: Max Leibowitz

### Q1: README/Purpose
- README is concise and includes needed information.
- Suggestion: add screenshots to better convey gameplay.

### Q2: Installation/Setup
- Setup docs straightforward on Linux.
- Needed to install CMake; suggested adding install script/command.

### Q3: Functional Testing
- Reported use cases appear partially implemented.
- Issue: Battle screen appears but fonts fail to load, so text does not display.
- Could not trigger crash or other errors.

---

## Consolidated Issues (Created)

1. **#14** - Bug: Battle text doesn't appear on Windows  
   https://github.com/Connor-Allen10/codemon/issues/14

2. **#15** - Bug: Immediate battle re-trigger after exiting battle  
   https://github.com/Connor-Allen10/codemon/issues/15

3. **#16** - Documentation: Move platform-specific notes before build instructions  
   https://github.com/Connor-Allen10/codemon/issues/16

4. **#17** - Documentation: Add player movement controls to README  
   https://github.com/Connor-Allen10/codemon/issues/17

5. **#18** - Documentation: Windows build instructions don't work in PowerShell  
   https://github.com/Connor-Allen10/codemon/issues/18

6. **#19** - Documentation: Add CMake to prerequisites list  
   https://github.com/Connor-Allen10/codemon/issues/19

7. **#20** - Documentation: Expand User Guide with FAQ and troubleshooting  
   https://github.com/Connor-Allen10/codemon/issues/20

---

## Priority Summary

**Critical/High:**
- #15 Immediate battle re-trigger loop
- #14 Missing battle text on Windows
- #18 Windows PowerShell build instructions

**Medium:**
- #16 Reorder platform notes in README
- #17 Add controls documentation
- #20 Expand User Guide with FAQ/troubleshooting

**Low:**
- #19 Add CMake prerequisite mention
