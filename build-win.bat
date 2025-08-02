@echo off
mkdir "build/win" 2>NUL
xcopy /E /I "res" "build/win/res" >NUL

REM Set your SDL2 include and lib paths
set SDL2_INC=C:\sdl2\include
set SDL2_LIB=C:\sdl2\lib

REM Compile all cpp files in src/
g++ src\*.cpp ^
    -I%SDL2_INC%\SDL2 -L%SDL2_LIB% ^
    -g -ggdb -std=c++17 -Wall ^
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf ^
    -o build\win\chip8.exe
