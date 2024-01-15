### Kinnect SDK
https://www.microsoft.com/en-us/download/details.aspx?id=44561 (05.11.2023)

### Install MATLAB Runtime

https://www.mathworks.com/products/compiler/matlab-runtime.html (14.01.2024)

### Create PATH called MATLAB_LIB and MATLAB_INC
MATLAB_INC = C:\Program Files\MATLAB\MATLAB Runtime\R2023b\extern\include </br>
MATLAB_LIB = C:\Program Files\MATLAB\MATLAB Runtime\R2023b\extern\lib\win64\microsoft

### How to build MATLAB .dll file
1. Enter MatlabScripts folder with MATLAB consoles
2. Enter following commands in MATLAB terminal
    ```
    mcc -W 'cpplib:GetRotationAngle,all' -T link:lib GetRotationAngle.m
    ```
    As a result `GetRotationAngle.h`, `GetRotationAngle.lib` and `GetRotationAngle.dll` will be generated

3. Copy .lib into MovementTracker_main/\<platform>/\<configuration>/lib
4. Copy .dll into MovementTracker_main/\<platform>/\<configuration>/bin

