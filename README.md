# StarStudio [![License](https://img.shields.io/github/license/starbounded-dev/starstudio.svg)](https://github.com/starbounded-dev/starstudio/blob/main/LICENSE)

![StarStudio](/Resources/Branding/StarStudioLogo.png?raw=true "StarStudio")

StarStudio is primarily an early-stage interactive application and rendering engine for Windows. Currently not much is implemented.

***

## Getting Started
Visual Studio 2022 is recommended, StarStudio is officially untested on other development environments whilst we focus on a Windows build.

<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with `git clone --recursive https://github.com/starbounded-dev/starstudio`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

<ins>**2. Configuring the dependencies:**</ins>

1. Run the [GenerateProject.bat](https://github.com/starbounded-dev/starstudio/blob/main/GenerateProject.bat) file found in `root` folder. This will generate all sln and projects files required.
2. Open the generated solution file in Visual Studio 2022.
3. Build the solution.
4. Run the Sandbox project.
5. Enjoy!

***

## The Plan
The plan for StarStudio is two-fold: to create a powerful 3D engine, but also to serve as an education tool for teaching game engine design and architecture. Because of this the development inside this repository is rather slow, since everything has to be taught and implemented by myself.

### Main features to come:
- Fast 2D rendering (UI, particles, sprites, etc.)
- High-fidelity Physically-Based 3D rendering (this will be expanded later, 2D to come first)
- Support for Mac, Linux, Android and iOS
    - Native rendering API support (DirectX, Vulkan, Metal)
- Fully featured viewer and editor applications
- Fully scripted interaction and behavior
- Integrated 3rd party 2D and 3D physics engine
- Procedural terrain and world generation
- Artificial Intelligence
- Audio system