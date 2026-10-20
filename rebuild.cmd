@echo off
setlocal
REM ==========================================================
REM  CombatDemo -- full rebuild (generates UnrealEditor-CombatDemo.dll)
REM  Close UnrealEditor completely before running this.
REM ==========================================================

set "UE_ROOT=H:\Epic Games\UE_5.6"
set "PROJECT=%~dp0CombatDemo.uproject"
set "DLL=%~dp0Binaries\Win64\UnrealEditor-CombatDemo.dll"

echo ==========================================
echo  CombatDemo full rebuild
echo ==========================================
echo.

tasklist /FI "IMAGENAME eq UnrealEditor.exe" 2>nul | find /I "UnrealEditor.exe" >nul
if not errorlevel 1 (
    echo [ERROR] UnrealEditor is still running. Close it completely first.
    echo.
    pause
    exit /b 1
)

if not exist "%PROJECT%" (
    echo [ERROR] Project file not found: %PROJECT%
    pause
    exit /b 1
)

if exist "%DLL%" for %%F in ("%DLL%") do echo DLL before: %%~tF
echo.
echo Building...
echo.

call "%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" CombatDemoEditor Win64 Development -project="%PROJECT%" -waitmutex
if errorlevel 1 (
    echo.
    echo [FAILED] Build did not succeed. Fix the errors above and run again.
    echo.
    pause
    exit /b 1
)

echo.
echo [OK] Build succeeded.
if exist "%DLL%" for %%F in ("%DLL%") do echo DLL after:  %%~tF
echo.
echo You can open the editor now.
pause
