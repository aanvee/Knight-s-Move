@echo off
cd /d "%~dp0"

echo.
echo === Building FULLY STATIC chess.dll ===
echo.

if not exist "..\python_GUI" mkdir "..\python_GUI"

rem --- FULL STATIC: No external DLLs ---
gcc -shared -o "..\python_GUI\chess.dll" *.c -O2 -Wall ^
    -static-libgcc -static-libstdc++ ^
    -static ^
    -Wl,--subsystem,windows

set "BUILD_OK=0"
if %ERRORLEVEL% EQU 0 set "BUILD_OK=1"

if "%BUILD_OK%"=="1" (
    echo.
    echo SUCCESS: chess.dll built (NO DEPENDENCIES)
    echo.
    dir "..\python_GUI\chess.dll"
) else (
    echo.
    echo BUILD FAILED
)
echo.
pause