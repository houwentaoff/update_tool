@echo off
tasklist /nh | find /i "FiUpdateLoader.exe"
if ERRORLEVEL 1 (echo "Update Down %time%" >> Restart_%date:~0,4%%date:~5,2%%date:~8,2%.log & start "" "FiUpdateLoader.exe" ) else (echo "Update Alive %time%" >> Restart_%date:~0,4%%date:~5,2%%date:~8,2%.log)
exit
