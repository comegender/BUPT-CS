@echo off
taskkill /F /T /IM aicompanion*
aicompanion --load-extension=runtime --enable-gcm --enable-webgl --ignore-gpu-blacklist ai2
@echo on