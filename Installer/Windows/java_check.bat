@echo off
setlocal enabledelayedexpansion

goto script_start

@REM Test if a valid installation of Java exists at the given path
@REM :test_path [path]
:test_path
IF NOT "%~1" == "" (
    IF EXIST "%~1\bin\server\jvm.dll" (
        SET "JAVA_IS_HERE=%~1\bin"
    ) ELSE (
        IF EXIST "%~1\bin\client\jvm.dll" (
            SET "JAVA_IS_HERE=%~1\bin"
        ) ELSE (
            IF EXIST "%~1\jre\bin\server\jvm.dll" (
                SET "JAVA_IS_HERE=%~1\jre\bin"
            ) ELSE (
                IF EXIST "%~1\jre\bin\client\jvm.dll" (
                    SET "JAVA_IS_HERE=%~1\jre\bin"
                )
            )
        )
    )
)
exit /b 0
@REM End :test_path

@REM Find the parent of the given path, will modify the passed path
@REM :parent_path [path] [result]
:parent_path

@REM Create a temporary variable to work on
SET TEMP=%~1
@REM If the passed path ends in a backslash, remove it
IF !TEMP:~-1!==\ SET TEMP=!TEMP:~0,-1!
@REM Find the parent path
FOR %%a in ("!TEMP!") DO SET "TEMP=%%~dpa"
@REM If the path has a backslash at the end again, remove it
IF !TEMP:~-1!==\ (
    SET "%~2=!TEMP:~0,-1!"
) ELSE (
    SET "%~2=!TEMP!"
)

exit /b 0
@REM End :parent_path

@REM Read a value from the registry. May fail if [value] contains spaces
@REM :read_registry [path] [value] [result]
:read_registry

FOR /F "tokens=2* USEBACKQ" %%A IN (`REG QUERY "%~1" /v "%~2" /reg:64 2^>NUL ^| FIND "REG_SZ"`) DO SET "%~3=%%B"

exit /b 0
@REM End :read_registry

@REM The start of the Java detection script
:script_start

@REM try to use the environment variable JAVA_HOME
CALL :test_path "%JAVA_HOME:"=%"
IF "!JAVA_IS_HERE!"=="" (
    @REM try to use the location of Java in the path
    FOR /F "tokens=* USEBACKQ" %%F IN (`where java`) DO (
        IF "!JAVA_IS_HERE!"=="" (
            SET PATH_JAVA=%%F
            @REM Go up two levels which should be the java root directory
            CALL :parent_path "!PATH_JAVA!" PATH_JAVA
            CALL :parent_path "!PATH_JAVA!" PATH_JAVA
            CALL :test_path "!PATH_JAVA:"=!"
        )
    )
    
    IF "!JAVA_IS_HERE!"=="" (
        SET JAVA_REGISTRY_PATH="HKLM\SOFTWARE\JavaSoft\JRE"
        @REM try to use the registry
        CALL :read_registry "!JAVA_REGISTRY_PATH:"=!" "CurrentVersion" JAVA_CURRENT_VERSION
        SET FOUND_CURRENT_VERSION=1
        IF "!JAVA_CURRENT_VERSION!"=="" SET FOUND_CURRENT_VERSION=0
        IF "!JAVA_CURRENT_VERSION:~0,6!"=="ERROR:" SET FOUND_CURRENT_VERSION=0
        IF "!FOUND_CURRENT_VERSION!"=="0" (
            SET JAVA_REGISTRY_PATH="HKLM\SOFTWARE\JavaSoft\Java Runtime Environment"
            CALL :read_registry "!JAVA_REGISTRY_PATH:"=!" "CurrentVersion" JAVA_CURRENT_VERSION
        )

        IF NOT "!JAVA_CURRENT_VERSION!"=="" (
            CALL :read_registry "!JAVA_REGISTRY_PATH:"=!\!JAVA_CURRENT_VERSION!" "JavaHome" PATH_JAVA
            CALL :test_path "!PATH_JAVA:"=!"
        )
    )
)

@REM end of the document, prints out the location that was found
:docend
IF "!JAVA_IS_HERE!"=="" (
    echo Java Not Found
) ELSE (
    FOR /F tokens^=2-5^ delims^=.-+_^" %%j in ('"!JAVA_IS_HERE!\java.exe" -fullversion 2^>^&1') do (
        SET "major=%%j"
        SET "minor=%%k"
        SET "build=%%l"
    )
    echo !major!.!minor!.!build!
)

endlocal
