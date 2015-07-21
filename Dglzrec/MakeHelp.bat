@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by DGLZREC.HPJ. >"hlp\dglzrec.hm"
echo. >>"hlp\dglzrec.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\dglzrec.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\dglzrec.hm"
echo. >>"hlp\dglzrec.hm"
echo // Prompts (IDP_*) >>"hlp\dglzrec.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\dglzrec.hm"
echo. >>"hlp\dglzrec.hm"
echo // Resources (IDR_*) >>"hlp\dglzrec.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\dglzrec.hm"
echo. >>"hlp\dglzrec.hm"
echo // Dialogs (IDD_*) >>"hlp\dglzrec.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\dglzrec.hm"
echo. >>"hlp\dglzrec.hm"
echo // Frame Controls (IDW_*) >>"hlp\dglzrec.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\dglzrec.hm"
REM -- Make help for Project DGLZREC


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\dglzrec.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\dglzrec.hlp" goto :Error
if not exist "hlp\dglzrec.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\dglzrec.hlp" Debug
if exist Debug\nul copy "hlp\dglzrec.cnt" Debug
if exist Release\nul copy "hlp\dglzrec.hlp" Release
if exist Release\nul copy "hlp\dglzrec.cnt" Release
echo.
goto :done

:Error
echo hlp\dglzrec.hpj(1) : error: Problem encountered creating help file

:done
echo.
