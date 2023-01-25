echo off
cls

set /p message=Inserisci commit message: 
git add .
git commit -m "%message%"
git push

echo Premi qualsiasi tasto per chiudere
pause
