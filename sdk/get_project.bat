@echo off
echo Select your chip model to get the corresponding project file, you can input 'B91' or 'B92'.

set /p name=Please input chip model: 

if "%name%"=="B91" (del /f %CD%\B91.cproject 2>nul & del /f %CD%\B91.project 2>nul)
if "%name%"=="B92" (del /f %CD%\B92.cproject 2>nul & del /f %CD%\B92.project 2>nul)

findstr /n "telink_b91_btble_sdk" %CD%\.project 2>nul
if %errorlevel% equ 0 (
	echo Found "telink_b91_btble_sdk" in .project
	copy .project core\B91\B91.project & copy .cproject core\B91\B91.cproject 
) else (
	echo [Backup-Info]: The .project does not exist in the current directory or .project not a B91.
)
findstr /n "telink_b92_btble_sdk" %CD%\.project 2>nul
if %errorlevel% equ 0 (
	echo Found "telink_b92_btble_sdk" in .project
	copy .project core\B92\B92.project & copy .cproject core\B92\B92.cproject 
) else (
	echo [Backup-Info]: The .project does not exist in the current directory or .project not a B92.
)

del /f %CD%\.cproject 2>nul & del /f %CD%\.project 2>nul

if  "%name%"=="B91" copy core\B91\B91.cproject %CD% & copy core\B91\B91.project %CD%
if  "%name%"=="B92" copy core\B92\B92.cproject %CD% & copy core\B92\B92.project %CD%

if exist %cd%\%name%.project if exist %cd%\%name%.project (echo success.) 

ren %name%.project .project & ren %name%.cproject .cproject


pause
