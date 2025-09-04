# ARM25 Course Repository
This repository contains the exercises and final project for the ARM25 Embedded Systems course. It is structured to keep each exercise and the project separate and self-contained.
## Repository Structure
<img width="337" height="130" alt="image" src="https://github.com/user-attachments/assets/ee291e73-fd3a-4c30-b4d7-dba6753bfd88" />

## Step-by-Step: First-Time Setup
Follow these steps exactly to get started.

### 1. Get the Code
1. Open Git Bash (Windows) or a Terminal (Mac/Linux).
2. Navigate to where you want to store the project (e.g., cd ~/Documents).
3. Clone this repository:
```
git clone https://github.com/zzvllvzz/arm25.git
cd arm25
```
### 2. Open the Project in CLion
1. Open Project --> Project Wizard will start.
<img width="1675" height="1401" alt="image" src="https://github.com/user-attachments/assets/60cd67cd-1938-4511-b905-bf404969c440" />
  
2. You can either enter the environment variables by hand in the Project Wizard by typing PICO_SDK_PATH=your/path-to/pico-sdk;PICO_TOOLCHAIN_PATH=your/path-to/gcc-arm-none-eabi in the 'Environment' or you can set the required environment variables to your profile before starting CLion. You can do both as well to be sure. You can find PICO_SDK_PATH by starting Pico – Developer command Prompt or Pico – Developer PowerShell. The command to print all environment variables is “set” in command prompt and “direnv:” in PowerShell.
<img width="298" height="136" alt="image" src="https://github.com/user-attachments/assets/9f8a17b9-2a50-4337-b937-67a03e83bc63" />

3. To set the variables to your profile type “Edit the system environment variables” in Windows search box. Add PICO_SDK_PATH your profile’s environment variables to make it available to CLion. Add PICO_TOOLCHAIN_PATH to the environment. The toolchain directory is near the sdk directory – only the last  folder is different. The toolchain directory is called “gcc-arm-none-eabi”. See the examples below.
<img width="470" height="55" alt="image" src="https://github.com/user-attachments/assets/edfd00fb-412d-4ece-9d35-5ff2655ada45" />

4. When you have entered the values click OK.
5. Your project is now configured and target “ex1” should be selected by default
6. Follow Pico debugger installation guide that can be found in the repository by the name "pico_debugger_installation.pdf". Note that instead of "blink" in the "Target" and "Executable binary" you should see "ex1".

## How to Work on a New Exercise
This is the workflow for when we add ``ex2``, ``ex3``, etc.

1. **Get the Latest Code:** Always start by pulling the latest changes from the repository.
```
git pull origin main
```
2. **The New Exercise will Appear:** If a teammate has already added ``ex2``, it will now be on your computer. Reload the **CMake Project (Tools > CMake > Reset Cache and Reload Project)**, and the new ``ex2`` target will appear in the CMake window.
3. **To Create a New Exercise Yourself:**
   * Create a new folder in the root (e.g., ``ex3/``).
   * Copy the ``CMakeLists.txt`` from ``ex1/`` into ``ex3/``.
   * Change the ``project(ex1 C CXX ASM)`` line to ``project(ex3 C CXX ASM)``.
   * Create your ``ex3/main.cpp`` file.
   * Edit the top-level ``CMakeLists.txt`` file and add the line ``add_subdirectory(ex3)``.
   * Commit your changes and push them:
```
git add .
git commit -m "Adds structure for exercise 3"
git push
```
