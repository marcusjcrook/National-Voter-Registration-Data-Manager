# National Voter Registration Data Manager

A C++ application for loading, merging, searching, sorting, and purging voter registration records. The project keeps its hand-written containers and selected original algorithms for learning and comparison while using modern C++ features and standard-library algorithms where appropriate.

## Requirements

- Windows
- Visual Studio Build Tools 2026 with the x64 C++ toolchain
- CMake 3.25 or newer
- Visual Studio Code with the CMake Tools and C/C++ extensions

The MSVC build uses `/std:c++latest`, since the compiler exposes C++26 as its latest draft mode.

## Configure and build in VS Code

The shared `vs2026-x64` CMake preset selects the Visual Studio 18 2026 x64 generator and keeps its output in the ignored `build` directory. VS Code is configured to use that preset.

1. Open the repository folder in VS Code.
2. Run **CMake: Select Configure Preset** and choose **Visual Studio 2026 x64**.
3. Run **CMake: Configure**.
4. Run **CMake: Select Build Preset** and choose **Visual Studio 2026 x64 Debug**, then run **CMake: Build**.

Alternatively, run the **C/C++: cl.exe build ProjectOne (C++26)** task with **Terminal: Run Build Task**.

## Run the integration test

After configuring and building, run **CMake: Select Test Preset** and choose **Visual Studio 2026 x64 Debug tests**. Then run **CMake: Run Tests** from the Command Palette or use the CMake Tests view. The integration test covers application flows including CSV validation and invalid interactive numeric input.
