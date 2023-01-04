@echo off
setlocal EnableDelayedExpansion

title Configuring W.I.S.E....

goto script_start

REM Resolve a potentially relative path name to an absolute path
REM :resolve_path (path to resolve), (variable to store the resolved path)
:resolve_path

pushd .
cd %~dp0
set %~2=%~f1
popd

exit /b 0
REM End :resolve_path

REM Detect the Windows version and version dependant features
REM :detect_windows (variable to store formatting enabled)
:detect_windows

for /F "tokens=4-7 delims=[]. " %%i in ('ver') do (
    set /a _MajorMinor = %%i * 100 + %%j
    set /a _Build = %%k0 /10
    set /a _Revision = %%l0 /10
)
set "%~1=No"
if %_MajorMinor% GEQ 1000 if %_Build% GTR 10586 ( set "%~1=Yes" ) else (
    if %_Build% EQU 10586 if %_Revision% GEQ 11 set "%~1=Yes"
)

exit /b 0
REM End :detect_windows

REM Get a random IP address from the list of addresses on this machine
REM :get_machine_ip (variable to store the IP address)
:get_machine_ip

for /f "delims=[] tokens=2" %%a in ('ping -4 -n 1 %ComputerName% ^| findstr [') do set "%~1=%%a"

exit /b 0
REM End :get_machine_ip

REM Build the W.I.S.E. Builder and API configuration file
REM :build_api_config (path to build at)
:build_api_config

(echo {)> "%~1"
(echo     "log": {)>> "%~1"
(echo         "filename": "logfile.log",)>> "%~1"
(echo         "verbosity": 2)>> "%~1"
(echo     },)>> "%~1"
(echo     "signals": {)>> "%~1"
(echo         "start": "start.txt",)>> "%~1"
(echo         "complete": "complete.txt")>> "%~1"
(echo     },)>> "%~1"
(echo     "hardware": {)>> "%~1"
(echo         "processes": 4,)>> "%~1"
(echo         "cores": 2)>> "%~1"
(echo     },)>> "%~1"
if "!useMqtt!"=="yes" (
(echo     "mqtt": {)>> "%~1"
if not defined mqttAddress (
(echo         "hostname": "",)>> "%~1"
) else (
    if "!mqttAddress!"=="" (
(echo         "hostname": "",)>> "%~1"
    ) else (
(echo         "hostname": "!mqttAddress:\=\\!",)>> "%~1"
    )
)
(echo         "port": !mqttPort!,)>> "%~1"
(echo         "topic": "wise",)>> "%~1"
(echo         "verbosity": 3,)>> "%~1"
(echo         "qos": 1,)>> "%~1"
if not defined mqttUsername (
(echo         "username": "",)>> "%~1"
) else (
    if "!mqttUsername!"=="" (
(echo         "username": "",)>> "%~1"
    ) else (
(echo         "username": "!mqttUsername:\=\\!",)>> "%~1"
    )
)
if not defined mqttPassword (
(echo         "password": "")>> "%~1"
) else (
    if "!mqttPassword!"=="" (
(echo         "password": "")>> "%~1"
    ) else (
(echo         "password": "!mqttPassword:\=\\!")>> "%~1"
    )
)
(echo     },)>> "%~1"
) else (
(echo     "socket": {)>> "%~1"
if not defined managerIp (
(echo         "address": "",)>> "%~1"
) else (
    if "!managerIp!"=="" (
(echo         "address": "",)>> "%~1"
    ) else (
(echo         "address": "!managerIp:\=\\!",)>> "%~1"
    )
)
(echo         "port": !managerPort!)>> "%~1"
(echo     },)>> "%~1"
)
(echo     "builder": {)>> "%~1"
if not defined builderAddress (
(echo         "hostname": "",)>> "%~1"
) else (
    if "!builderAddress!"=="" (
(echo         "hostname": "",)>> "%~1"
    ) else (
(echo         "hostname": "!builderAddress:\=\\!",)>> "%~1"
    )
)
(echo         "port": !apiPort!)>> "%~1"
(echo     },)>> "%~1"
if not defined jobDirectory (
(echo|set /p="    "exampleDirectory": """)>> "%~1"
) else (
    if "!jobDirectory!"=="" (
(echo|set /p="    "exampleDirectory": """)>> "%~1"
    ) else (
(echo     "exampleDirectory": "!jobDirectory:\=\\!",)>> "%~1"
    )
)
if !isV2Settings! EQU 1 (
(echo     "already_v2": true,)>> "%~1"
)
(echo     "managerSettings": {)>> "%~1"
(echo         "maxConcurrent": 2,)>> "%~1"
if not defined wiseLocation (
(echo         "wiseLocation": "",)>> "%~1"
) else (
    if "!wiseLocation!"=="" (
(echo         "wiseLocation": "",)>> "%~1"
    ) else (
(echo         "wiseLocation": "!wiseLocation:\=\\!",)>> "%~1"
    )
)
(echo         "internalBuilder": {)>> "%~1"
if not defined builderLoc (
(echo             "builderLocation": "",)>> "%~1"
) else (
    if "!builderLoc!"=="" (
(echo             "builderLocation": "",)>> "%~1"
    ) else (
(echo             "builderLocation": "!builderLoc:\=\\!",)>> "%~1"
    )
)
(echo             "logLevel": "all")>> "%~1"
(echo         },)>> "%~1"
(echo         "mqttSettings": {)>> "%~1"
(echo             "maxBufferSize": 4096,)>> "%~1"
(echo             "useInternalBroker": false,)>> "%~1"
(echo             "useInternalAuthentication": false)>> "%~1"
(echo         },)>> "%~1"
(echo         "numaLock": true,)>> "%~1"
(echo         "skipCores": 0,)>> "%~1"
(echo         "rpcSettings": {)>> "%~1"
if "!useRpc!"=="yes" (
    set rpcPart=true
) else (
    set rpcPart=false
)
(echo             "enabled": !rpcPart!,)>> "%~1"
if not defined rpcAddress (
(echo             "externalAddress": "",)>> "%~1"
) else (
    if "!rpcAddress!"=="" (
(echo             "externalAddress": "",)>> "%~1"
    ) else (
(echo             "externalAddress": "!rpcAddress:\=\\!",)>> "%~1"
    )
)
(echo             "port": !rpcPort!)>> "%~1"
(echo         })>> "%~1"
(echo     })>> "%~1"
(echo })>> "%~1"

REM Set the file owner to the current user
icacls "%~1" /setowner "%username%" /q
REM Set the file permissions to full control for everyone
icacls "%~1" /grant Everyone:F /q

exit /b 0
REM End :build_api_config

REM Get the current time in UTC
REM :get_utc_time (variable to hold ISO8601 time)
:get_utc_time

for /F "skip=1 tokens=1-6" %%G in ('WMIC Path Win32_UtcTime Get Day^,Hour^,Minute^,Month^,Second^,Year /Format:table') DO (
    IF "%%~L"=="" goto done_utc_get_time
    set _yyyy=%%L
    set _mm=00%%J
    set _dd=00%%G
    set _hour=00%%H
    set _minute=00%%I
    set _second=00%%K
)
:done_utc_get_time

set _mm=%_mm:~-2%
set _dd=%_dd:~-2%
set _hour=%_hour:~-2%%
set _minute=%_minute:~-2%
set _second=%_second:~-2%

set "%~1=%_yyyy%-%_mm%-%_dd%T%_hour%:%_minute%:%_second%Z"

exit /b 0
REM End :get_utc_time

REM Write the manager configuration file
REM :build_manager_config (path to build at)
:build_manager_config

(echo {)> "%~1"
if not defined jobDirectory (
(echo   "jobDirectory": "",)>> "%~1"
) else (
    if "!jobDirectory!"=="" (
(echo   "jobDirectory": "",)>> "%~1"
    ) else (
(echo   "jobDirectory": "!jobDirectory:\=\\!",)>> "%~1"
    )
)
call :get_utc_time utcTime
(echo   "lastUpdate": "%utcTime%")>> "%~1"
(echo })>> "%~1"

REM Set the file owner to the current user
icacls "%~1" /setowner "%username%" /q
REM Set the file permissions to full control for everyone
icacls "%~1" /grant Everyone:F /q

exit /b 0
REM End :build_manager_config

REM Create the batch script that is used to starts Builder
REM :build_builder_script (path to build script)
:build_builder_script

(echo @echo off)> "%~1"
(echo setlocal EnableDelayedExpansion)>> "%~1"
(echo.)>> "%~1"
if "%useMqtt%"=="yes" (
    (echo SET ARG_1=-m)>> "%~1"
) else (
    (echo SET ARG_1=!managerIp!)>> "%~1"
)
if not defined jobDirectory (
    (echo SET ARG_2=@@JOBS@@)>> "%~1"
) else (
    if "!jobDirectory!"=="" (
        (echo SET ARG_2=@@JOBS@@)>> "%~1"
    ) else (
        (echo SET ARG_2=!jobDirectory!)>> "%~1"
    )
)
(echo SET ARG_3=!apiPort!)>> "%~1"
(echo SET ARG_4=!managerPort!)>> "%~1"
(echo.)>> "%~1"
(echo SET FOUND_JAVA=0)>> "%~1"
(echo call :ProgInPath java.exe)>> "%~1"
(echo IF "%%PROG%%"=="" ^()>> "%~1"
(echo 	IF NOT "%%JAVA_HOME%%"=="" ^()>> "%~1"
(echo 		IF EXIST "%%JAVA_HOME%%/java.exe" ^()>> "%~1"
(echo 			SET "JAVA_PATH=%%JAVA_HOME%%/")>> "%~1"
(echo 		^) ELSE ^()>> "%~1"
(echo 			IF EXIST "%%JAVA_HOME%%/jre" ^()>> "%~1"
(echo 				SET "JAVA_PATH=%%JAVA_HOME%%/jre/bin/")>> "%~1"
(echo 			^) ELSE ^()>> "%~1"
(echo 				SET "JAVA_PATH=%%JAVA_HOME%%/bin/")>> "%~1"
(echo 			^))>> "%~1"
(echo 		^))>> "%~1"
(echo 		SET FOUND_JAVA=1)>> "%~1"
(echo 	^))>> "%~1"
(echo ^) ELSE ^()>> "%~1"
(echo 	SET "JAVA_PATH=")>> "%~1"
(echo 	SET FOUND_JAVA=1)>> "%~1"
(echo ^))>> "%~1"
(echo.)>> "%~1"
(echo IF "%%FOUND_JAVA%%"=="1" ^()>> "%~1"
(echo 	IF NOT "%%1"=="" ^()>> "%~1"
(echo 		SET ARG_1=%%1)>> "%~1"
(echo 		IF NOT "%%2"=="" ^()>> "%~1"
(echo 			SET ARG_2=%%2)>> "%~1"
(echo 			IF NOT "%%3"=="" ^()>> "%~1"
(echo 				SET ARG_3=%%3)>> "%~1"
(echo 			^))>> "%~1"
(echo 		^))>> "%~1"
(echo 	^))>> "%~1"
(echo.)>> "%~1"
(echo 	IF "%%ARG_1%%"=="@@JOBS@@" ^()>> "%~1"
(echo 		SET ARG_1=C:/gitprojects/debug/psaasserver)>> "%~1"
(echo 	^))>> "%~1"
(echo.)>> "%~1"
(echo 	IF "^!ARG_1^!"=="-help" ^()>> "%~1"
(echo 		echo W.I.S.E. Builder batch usage:)>> "%~1"
(echo 		echo For socket connections to W.I.S.E. Manager)>> "%~1"
(echo 		echo builder.bat ^^^<manager host address^^^> ^^^<job directory^^^> ^^^<port to listen for API calls on^^^> ^^^<manager port^^^>)>> "%~1"
(echo 		echo ex. builder.bat 127.0.0.1 C:\jobs 32479 32478)>> "%~1"
(echo 		echo.)>> "%~1"
(echo 		echo For MQTT connections to W.I.S.E. Manager)>> "%~1"
(echo 		echo builder.bat -m ^^^<job directory^^^> ^^^<port to listen for API calls on^^^>)>> "%~1"
(echo 		echo ex. builder.bat -m C:\jobs 32479)>> "%~1"
(echo 	^) ELSE ^()>> "%~1"
(echo 		IF "^!ARG_1^!"=="--help" ^()>> "%~1"
(echo 			echo W.I.S.E. Builder batch usage:)>> "%~1"
(echo 			echo For socket connections to W.I.S.E. Manager)>> "%~1"
(echo 			echo builder.bat ^^^<manager host address^^^> ^^^<job directory^^^> ^^^<port to listen for API calls on^^^> ^^^<manager port^^^>)>> "%~1"
(echo 			echo ex. builder.bat 127.0.0.1 C:\jobs 32479 32478)>> "%~1"
(echo 			echo.)>> "%~1"
(echo 			echo For MQTT connections to W.I.S.E. Manager)>> "%~1"
(echo 			echo builder.bat -m ^^^<job directory^^^> ^^^<port to listen for API calls on^^^>)>> "%~1"
(echo 			echo ex. builder.bat -m C:\jobs 32479)>> "%~1"
(echo 		^) ELSE ^()>> "%~1"
(echo 			IF "^!ARG_1^!"=="-m" ^()>> "%~1"
(echo 				"%%JAVA_PATH%%java.exe" -jar WISE_Builder.jar -m -s -j "^!ARG_2^!" -l "^!ARG_3^!" -o json_v2 -e :memory)>> "%~1"
(echo 			^) ELSE ^()>> "%~1"
(echo 				"%%JAVA_PATH%%java.exe" -jar WISE_Builder.jar -a "^!ARG_1^!" -j "^!ARG_2^!" -l "^!ARG_3^!" -p ^^!ARG_4^^! -o json_v2 -e :memory)>> "%~1"
(echo 			^))>> "%~1"
(echo 		^))>> "%~1"
(echo 	^))>> "%~1"
(echo ^) ELSE ^()>> "%~1"
(echo 	ECHO Cannot find Java executable. Please set JAVA_HOME to the install directory of the latest version of Java.)>> "%~1"
(echo ^))>> "%~1"
(echo.)>> "%~1"
(echo GOTO :eof)>> "%~1"
(echo.)>> "%~1"
(echo :ProgInPath)>> "%~1"
(echo SET PROG=%%~$PATH:1)>> "%~1"
(echo GOTO :eof)>> "%~1"

REM Set the file owner to the current user
icacls "%~1" /setowner "%username%" /q
REM Set the file permissions to full control for everyone
icacls "%~1" /grant Everyone:F /q

exit /b 0
REM End :build_builder_script

REM The configuration script
:script_start

call :detect_windows _AE

set bold=
set underline=
set normal=
set blue=
set white=
set green=
set purple=
set orange=
set red=

if "!_AE!"=="Yes" (
    set bold=[1m
    set underline=[4m
    set normal=[0m
    set blue=[96m
    set white=[97m
    set green=[92m
    set purple=[95m
    set orange=[93m
    set red=[91m
)

set localAddress=127.0.0.1
call :get_machine_ip localAddress

echo Beginning to configure %bold%%underline%W.I.S.E.%normal% on !localAddress!

REM Variable initialization
set jobDirectory=
if not "%1"=="" (
    set jobDirectory=%~1
    call :resolve_path !jobDirectory!,jobDirectory
)
set managerLoc=manager\WISE_Manager_Ui.jar
if not "%2"=="" set managerLoc=%~2
set builderLoc=Other\WISE_Builder\WISE_Builder.jar
if not "%3"=="" set builderLoc=%~3
set apiLoc=Other\web_demo
if not "%4"=="" set apiLoc=%~4
set wiseLocation=wise.exe
if not "%5"=="" set wiseLocation=%~5

set useMqtt=yes
set mqttAddress=127.0.0.1
set mqttPort=1883
set managerIp=127.0.0.1
set managerPort=32478
set useMqttAuth=no
set mqttUsername=
set mqttPassword=
set builderSameMachine=yes
set builderAddress=127.0.0.1
set apiPort=32479
set useRpc=no
set rpcAddress=127.0.0.1
set rpcPort=32480

call :resolve_path !managerLoc!,managerLoc
call :resolve_path !builderLoc!,builderLoc
call :resolve_path !apiLoc!,apiLoc
call :resolve_path !wiseLocation!,wiseLocation

set isV2Settings=0
reg query HKCU\Software\WISE_Manager /v WISEExe > NUL 2>&1
if !ERRORLEVEL! NEQ 0 set isV2Settings=1

REM Find the job directory
:jobInput
if not exist !jobDirectory! (
    echo Enter the folder that will be used as the %purple%job directory%normal% ^(current !jobDirectory!^)
    set /p jobDirectory="Location> "
    call :resolve_path !jobDirectory!,jobDirectory
    goto jobInput
)

REM Find the W.I.S.E. Manager install location.
:managerInput
if not exist !managerLoc! (
    echo Enter the location of %green%WISE_Manager_Ui.jar%normal% ^(current !managerLoc!^)
    set /p managerLoc="Location> "
    call :resolve_path !managerLoc!,managerLoc
    goto managerInput
)

REM REM Find the W.I.S.E. Builder install location.
:builderInput
if not exist !builderLoc! (
    echo Enter the location of %blue%WISE_Builder.jar%normal% ^(current !builderLoc!^)
    set /p builderLoc="Location> "
    call :resolve_path !builderLoc!,builderLoc
    goto builderInput
)

REM REM Find the folder that the API was installed to
:apiInput
if not exist !apiLoc! (
    echo Enter the base directory that the %white%API%normal% is installed in ^(current !apiLoc!^)
    set /p apiLoc="Location> "
    call :resolve_path !apiLoc!,apiLoc
    goto apiInput
)

REM REM Find the folder that W.I.S.E. was installed to
:wiseInput
if not exist !wiseLocation! (
    echo Enter the location of %bold%%underline%wise.exe%normal% ^(current !wiseLocation!^)
    set /p wiseLocation="Location> "
    call :resolve_path !wiseLocation!,wiseLocation
    goto wiseInput
)

echo Configuring %bold%%underline%W.I.S.E.%normal% Installation

:use_mqtt
echo Will MQTT be used for communication between %bold%%underline%W.I.S.E.%normal% processes?
set /p useMqtt="[yes]/no> "

if "!useMqtt!"=="y" set useMqtt=yes
if "!useMqtt!"=="n" set useMqtt=no

if not "!useMqtt!"=="yes" if not "!useMqtt!"=="no" goto use_mqtt

REM Configure MQTT
if "!useMqtt!"=="yes" (
    echo Enter the IP address or hostname of the %orange%MQTT broker%normal%
    set /p mqttAddress="[!mqttAddress!]> "

    echo Enter the port number that the %orange%MQTT broker%normal% is using
    set /p mqttPort="[!mqttPort!]> "

    echo Does the %orange%MQTT broker%normal% require authentication?
    set /p useMqttAuth="yes/[no]> "

    if "!useMqttAuth!"=="y" set useMqttAuth=yes
    if "!useMqttAuth!"=="n" set useMqttAuth=no
    
    if "!useMqttAuth!"=="yes" (
        echo Enter the username for the %orange%MQTT broker%normal%
        set /p mqttUsername="> "

        echo Enter the password for the %orange%MQTT broker%normal%
        set /p mqttPassword="> "
    )
) else (
    echo Enter the IP address to connect to %green%W.I.S.E. Manager%normal%
    set /p managerIp="[!managerIp!]> "

    echo enter the port that %green%W.I.S.E. Manager%normal% is listening on
    set /p managerPort="[!managerPort!]> "
)

echo Will the %white%API%normal% run on the same computer as %blue%W.I.S.E. Builder%normal%?
set /p builderSameMachine="[yes]/no> "

if "!builderSameMachine!"=="y" set builderSameMachine=yes
if "!builderSameMachine!"=="n" set builderSameMachine=no

if "!builderSameMachine!"=="no" (
    echo What IP address will %blue%W.I.S.E. Builder%normal% use?
    set /p builderAddress="[!builderAddress!]> "
)

echo What port would you like the %white%API%normal% and %blue%W.I.S.E. Builder%normal% to communicate on?
set /p apiPort="[!apiPort!]> "

echo Would you like %green%W.I.S.E. Manager%normal% to run an RPC server for transfering job files?
set /p useRpc="yes/[no]> "

if "!useRpc!"=="y" set useRpc=yes
if "!useRpc!"=="n" set useRpc=no

if "!useRpc!"=="yes" (
    echo What address will %blue%W.I.S.E. Builder%normal% use to connect to %green%W.I.S.E. Manager%normal%'s RPC server?
    set /p rpcAddress="[!rpcAddress!]> "

    echo What port should be used to run the RPC server?
    set /p rpcPort="[!rpcPort!]> "
)

echo Building configuration files...

REM write the manager configuration
for %%i in ("%managerLoc%") do (
    set filename="%%~dpimanager.json"
    echo Updating file !filename!
    call :build_manager_config !filename!
)

REM write the builder configuration
for %%i in ("%jobDirectory%") do (
    set filename="%%~i/config.json"
    echo Updating file !filename!
    call :build_api_config !filename!
)

REM write API configurations
pushd "%apiLoc%"
for /R %%f in (*.json) do (
    set filename=%%~f
    if "!filename:node_modules=!"=="!filename!" if not "!filename:config.json=!"=="!filename!" (
        echo Updating file !filename!
        call :build_api_config "!filename!"
    )
)
popd

REM write the builder start script
for %%i in ("%builderLoc%") do (
    set filename="%%~dpibuilder.bat"
    echo Updating file !filename!
    call :build_builder_script !filename!
)

echo %bold%%underline%W.I.S.E.%normal% configuration complete.
