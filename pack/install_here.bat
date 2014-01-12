@ECHO off
reg ADD HKEY_LOCAL_MACHINE\Software\Durante\GeDoSaTo /v InstallPath /t REG_SZ /d %~dp0 /f
echo Installed GeDoSaTo at %~dp0
echo Do not move/remove the configuration files and asset folder.
echo If you want to install to a different location, move the files there and run this file again.
pause