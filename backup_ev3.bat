@echo off
pscp -r -pw maker robot@ev3dev:/home/robot/project/src/* "%CD%\src"
PAUSE