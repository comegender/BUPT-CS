@echo off
rem Launch a local copy of dynamips 
set dynamips=%CD%\dynamips.exe
cd %TEMP%
start /belownormal "Dynamips"  cmd /c ""%dynamips%" -H 7200 & pause"

