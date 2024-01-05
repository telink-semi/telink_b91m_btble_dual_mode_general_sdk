@echo off
set "folderToDelete=output"
set "folderCount=0"

for /d /r %%i in (*%folderToDelete%) do (
    if /i "%%~nxi"=="%folderToDelete%" (
        echo Deleting folder: "%%~i"
        rd /s /q "%%~i"
        set /a "folderCount+=1"
    )
)

echo %folderCount% folders were deleted.
pause