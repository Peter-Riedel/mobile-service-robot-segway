@echo off
pscp -r -pw maker "%CD%\src\*" robot@ev3dev:/home/robot/project/src
PAUSE