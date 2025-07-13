@echo off 
rem taskkill /T /F /IM AppInventor2017* 
cd /d %~dp0 
start "Build Server Starting..." /min .\resources\appinventor\buildserver.cmd --port 9990 
start "Dev Server Starting..." /min .\resources\appinventor\AppEngine\bin\dev_appserver.cmd --disable_update_check --port=8888 --address=0.0.0.0 .\resources\appinventor\DevServer 
