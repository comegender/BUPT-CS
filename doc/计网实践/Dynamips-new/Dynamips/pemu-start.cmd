@echo off
rem Launch a local copy of dynamips 
set pemu=%CD%\pemuwrapper.exe
cd %TEMP%
start /belownormal "Pemu" cmd /c ""%pemu%" & pause"
