@echo off
pscp -r -pw maker "%CD%\src\*" robot@192.168.43.137:/home/robot/project/src
PAUSE