@echo off
set dynamips=%CD%\dynamips.exe
cd %TEMP%
echo.
echo Network adapters on this machine:
echo.
for /f "usebackq delims=:/ tokens=1,2,3" %%i in (`"%dynamips%" -e`) do call :EXEC %%i %%j "%%k"	
echo.
echo.
echo Use as follows:
echo   F0/0 = NIO_gen_eth:\Device\NPF_{...}
echo.
echo.
pause 
goto ENDE

:EXEC
if not "%1"=="rpcap" goto :EOF
echo NIO_gen_eth:%2
if "%2"=="\Device\NPF_GenericDialupAdapter" goto name 
set key=%2
set key=HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Network\{4D36E972-E325-11CE-BFC1-08002BE10318}\%key:~12,39%
set key=%key%\Connection
for /f "usebackq tokens=1,2,*" %%i in (`reg query %key% /v name`) do if "%%i"=="name" set name=%%k 
echo  Name      : %name%

:name
set name=%3
set name=%name: on local host=%
set name=%name:'"=%
set name=%name: Network adapter=%
set name=%name: (Microsoft's Packet Scheduler) =%
echo  Desciption: %name:~3%
echo.
goto :EOF


:ENDE
