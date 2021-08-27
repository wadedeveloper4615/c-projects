@echo off

REM This is needed so as to avoid static expansion of environment variables
REM inside if (...) conditionals.
REM See http://stackoverflow.com/questions/305605/weird-scope-issue-in-bat-file
REM for more explanation.
REM Precisely needed for configuring Visual Studio Environment.
setlocal enabledelayedexpansion

REM Does the user need help?
if /I "%1" == "help" goto help
if /I "%1" == "/?" (
:help
    echo Help for configure script
    echo Syntax: path\to\source\configure.cmd [script-options] [Cmake-options]
    echo Available script-options: Codeblocks, Eclipse, Makefiles, clang, VSSolution
    echo Cmake-options: -DVARIABLE:TYPE=VALUE
    goto quit
)

REM Get the source root directory
set BOOTMANAGER_SOURCE_DIR=%~dp0

REM Ensure there's no spaces in the source path
echo %BOOTMANAGER_SOURCE_DIR%| find " " > NUL
if %ERRORLEVEL% == 0 (
    echo. && echo   Your source path contains at least one space.
    echo   This will cause problems with building.
    echo   Please rename your folders so there are no spaces in the source path,
    echo   or move your source to a different folder.
    goto quit
)

REM Set default generator
set CMAKE_GENERATOR="Ninja"
set CMAKE_ARCH=

REM Detect presence of cmake
cmd /c cmake --version 2>&1 | find "cmake version" > NUL || goto cmake_notfound

SET ROS_ARCH=i386

REM Detect build environment (MinGW, VS, WDK, ...)
if defined ROS_ARCH (
    echo Detected RosBE for %ROS_ARCH%
    set BUILD_ENVIRONMENT=MinGW
    set ARCH=%ROS_ARCH%
    set MINGW_TOOCHAIN_FILE=config/toolchain-gcc.cmake
REM ) else if defined VCINSTALLDIR (
REM    REM VS command prompt does not put this in environment vars
REM    cl 2>&1 | find "x86" > NUL && set ARCH=i386
REM    cl 2>&1 | find "x64" > NUL && set ARCH=amd64
REM    cl 2>&1 | find "ARM" > NUL && set ARCH=arm
REM    cl 2>&1 | find "16.00." > NUL && set VS_VERSION=10
REM    cl 2>&1 | find "17.00." > NUL && set VS_VERSION=11
REM    cl 2>&1 | find "18.00." > NUL && set VS_VERSION=12
REM    cl 2>&1 | find "19.00." > NUL && set VS_VERSION=14
REM    cl 2>&1 | findstr /R /c:"19\.1.\." > NUL && set VS_VERSION=15
REM    cl 2>&1 | findstr /R /c:"19\.2.\." > NUL && set VS_VERSION=16
REM    if not defined VS_VERSION (
REM        echo Error: Visual Studio version too old ^(before 10 ^(2010^)^) or version detection failed.
REM        goto quit
REM    )
REM    set BUILD_ENVIRONMENT=VS
REM    set VS_SOLUTION=0
REM    echo Detected Visual Studio Environment !BUILD_ENVIRONMENT!!VS_VERSION!-!ARCH!
) else (
    echo Error: Unable to detect build environment. Configure script failure.
    goto quit
)

REM Checkpoint
if not defined ARCH (
    echo Unknown build architecture
    goto quit
)

set USE_CLANG_CL=0

REM Parse command line parameters
:repeat
    if "%BUILD_ENVIRONMENT%" == "MinGW" (
        if /I "%1" == "Codeblocks" (
            set CMAKE_GENERATOR="CodeBlocks - MinGW Makefiles"
        ) else if /I "%1" == "Eclipse" (
            set CMAKE_GENERATOR="Eclipse CDT4 - MinGW Makefiles"
        ) else if /I "%1" == "Makefiles" (
            set CMAKE_GENERATOR="MinGW Makefiles"
        ) else if /I "%1" == "VSSolution" (
            echo. && echo Error: Creation of VS Solution files is not supported in a MinGW environment.
            echo Please run this command in a [Developer] Command Prompt for Visual Studio.
            goto quit
        ) else if /I "%1" NEQ "" (
            echo %1| find /I "-D" > NUL
            if %ERRORLEVEL% == 0 (
                REM User is passing a switch to CMake
                REM Ignore it, and ignore the next parameter that follows
                Shift
            ) else (
                echo. && echo   Warning: Unrecognized switch "%1" && echo.
            )
        ) else (
            goto continue
        )
    ) else (
        if /I "%1" == "CodeBlocks" (
            set CMAKE_GENERATOR="CodeBlocks - NMake Makefiles"
        ) else if /I "%1" == "Eclipse" (
            set CMAKE_GENERATOR="Eclipse CDT4 - NMake Makefiles"
        ) else if /I "%1" == "Makefiles" (
            set CMAKE_GENERATOR="NMake Makefiles"
        ) else if /I "%1" == "clang" (
            set USE_CLANG_CL=1
        ) else if /I "%1" == "VSSolution" (
            set VS_SOLUTION=1
            REM explicitly set VS version for project generator
            if /I "%2" == "-VS_VER" (
                set VS_VERSION=%3
                echo Visual Studio Environment set to !BUILD_ENVIRONMENT!!VS_VERSION!-!ARCH!
            )
            set CMAKE_GENERATOR="Visual Studio !VS_VERSION!"
            if "!ARCH!" == "i386" (
                set CMAKE_ARCH=-A Win32
            ) else if "!ARCH!" == "amd64" (
                set CMAKE_ARCH=-A x64
            ) else if "!ARCH!" == "arm" (
                set CMAKE_ARCH=-A ARM
            )
        ) else if /I "%1" NEQ "" (
            echo %1| find /I "-D" > NUL
            if %ERRORLEVEL% == 0 (
                REM User is passing a switch to CMake
                REM Ignore it, and ignore the next parameter that follows
                Shift
            ) else (
                echo. && echo   Warning: Unrecognized switch "%1" && echo.
            )
        ) else (
            goto continue
        )
    )

    REM Go to next parameter
    SHIFT
    goto repeat
:continue

REM Inform the user about the default build
if "!CMAKE_GENERATOR!" == "Ninja" (
    echo This script defaults to Ninja. Type "configure help" for alternative options.
)

REM Create directories
set REACTOS_OUTPUT_PATH=output-%BUILD_ENVIRONMENT%-%ARCH%

REM if "%VS_SOLUTION%" == "1" (
REM     set REACTOS_OUTPUT_PATH=%REACTOS_OUTPUT_PATH%-sln
REM )

if "%BOOTMANAGER_SOURCE_DIR%" == "%CD%\" (
    set CD_SAME_AS_SOURCE=1
    echo Creating directories in %REACTOS_OUTPUT_PATH%

    if not exist %REACTOS_OUTPUT_PATH% (
        mkdir %REACTOS_OUTPUT_PATH%
    )
    cd %REACTOS_OUTPUT_PATH%
)

REM if "%VS_SOLUTION%" == "1" (
REM
REM    if exist build.ninja (
REM        echo. && echo Error: This directory has already been configured for ninja.
REM        echo An output folder configured for ninja can't be reconfigured for VSSolution.
REM        echo Use an empty folder or delete the contents of this folder, then try again.
REM        goto quit
REM    )
REM ) else if exist REACTOS.sln (
REM    echo. && echo Error: This directory has already been configured for Visual Studio.
REM    echo An output folder configured for VSSolution can't be reconfigured for ninja.
REM    echo Use an empty folder or delete the contents of this folder, then try again. && echo.
REM    goto quit
REM )

echo Preparing BootManager...

if EXIST CMakeCache.txt (
    del CMakeCache.txt /q
)


REM if "%BUILD_ENVIRONMENT%" == "MinGW" (
    cmake -G %CMAKE_GENERATOR% -DENABLE_CCACHE:BOOL=0 -DCMAKE_TOOLCHAIN_FILE:FILEPATH=%MINGW_TOOCHAIN_FILE% -DARCH:STRING=%ARCH% %BUILD_TOOLS_FLAG% %* "%BOOTMANAGER_SOURCE_DIR%"
REM ) else if %USE_CLANG_CL% == 1 (
REM     cmake -G %CMAKE_GENERATOR% -DCMAKE_TOOLCHAIN_FILE:FILEPATH=toolchain-msvc.cmake -DARCH:STRING=%ARCH% %BUILD_TOOLS_FLAG% -DUSE_CLANG_CL:BOOL=1 %* "%BOOTMANAGER_SOURCE_DIR%"
REM ) else (
REM     cmake -G %CMAKE_GENERATOR% %CMAKE_ARCH% -DCMAKE_TOOLCHAIN_FILE:FILEPATH=toolchain-msvc.cmake -DARCH:STRING=%ARCH% %BUILD_TOOLS_FLAG% %* "%BOOTMANAGER_SOURCE_DIR%"
REM )

if %ERRORLEVEL% NEQ 0 (
    goto quit
)

if "%CD_SAME_AS_SOURCE%" == "1" (
    set ENDV= from %REACTOS_OUTPUT_PATH%
)

REM if "%VS_SOLUTION%" == "1" (
REM     set ENDV= You can now use msbuild or open REACTOS.sln%ENDV%.
REM ) else (
    set ENDV= Execute appropriate build commands ^(ex: ninja, make, nmake, etc...^)%ENDV%
REM )

echo. && echo Configure script complete^^!%ENDV%

goto quit

:cmake_notfound
echo Unable to find cmake, if it is installed, check your PATH variable.

:quit
endlocal
exit /b
