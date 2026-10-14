@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0VelocityX\Scripts\Play.ps1" -Editor
if errorlevel 1 pause
