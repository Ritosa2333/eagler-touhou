@echo off
zun -2
zun -4 -z
zun -5
if errorlevel==1 goto fin
zun -1
if errorlevel==5 goto ong6
if errorlevel==4 goto ong5
if errorlevel==3 goto ong4
if errorlevel==2 goto ong3
if errorlevel==1 goto ong2
if errorlevel==0 goto ong1
:ong1
pmd /M8 /V0 /E2 /K 
webmusic
if errorlevel 1 goto fin
webstart
if errorlevel 1 goto fin
op
webmusic /R
pmd /R
goto fin
:ong2
pmd /M8 /V0 /E2 /K 
webmusic
if errorlevel 1 goto fin
webstart
if errorlevel 1 goto fin
op 
webmusic /R
pmd /R
goto fin
:ong3
pmdb2 /M8 /V0 /E2 /K 
webmusic
if errorlevel 1 goto fin
webstart
if errorlevel 1 goto fin
op
webmusic /R
pmdb2 /R
goto fin
:ong4
pmd86 /M8 /V0 /E2 /K 
webmusic
if errorlevel 1 goto fin
webstart
if errorlevel 1 goto fin
op
webmusic /R
pmd86 /R
goto fin
:ong5
pmdb2 /M8 /V0 /E2 /K 
webmusic
if errorlevel 1 goto fin
webstart
if errorlevel 1 goto fin
op
webmusic /R
pmdb2 /R
goto fin
:ong6
pmd86 /M8 /V0 /E2 /K 
webmusic
if errorlevel 1 goto fin
webstart
if errorlevel 1 goto fin
op
webmusic /R
pmd86 /R
goto fin
:fin
zun -2 -r
zun -4 -r
zun -5 -r
ECHO  GAME [ﾘﾀｰﾝ] で、ゲームを始められます。


