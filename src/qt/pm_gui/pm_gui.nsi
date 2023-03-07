;**************************************************************************
; pm_gui.nsi:  NSIS (Nullsoft Scriptable Install System) script for the
; 	Marine Mammal Detection (MMD) Power Module (PM) GUI software
;
; Written by:
; 	Sandra Mercer, P.Eng.
; 	Engineer / Software Developer
; 	eSonar Inc.
;
; Date:
; 	February 2022
;**************************************************************************


Name "PM_GUI"
OutFile "PM GUI Setup 0.1.exe"
InstallDir "c:\eSonar\PM GUI"
SetCompressor /SOLID lzma


Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles


!define ExeLocation "..\build-pm_gui-Desktop_Qt_5_14_1_MinGW_32_bit-Release"


Section ""
	SetOutPath $INSTDIR
	File "${ExeLocation}\pm_gui.exe"
	File "${ExeLocation}\libgcc_s_dw2-1.dll"
	File "${ExeLocation}\libstdc++-6.dll"
	File "${ExeLocation}\libwinpthread-1.dll"
	File "${ExeLocation}\Qt5Core.dll"
	File "${ExeLocation}\Qt5Gui.dll"
	File "${ExeLocation}\Qt5SerialPort.dll"
	File "${ExeLocation}\Qt5Widgets.dll"
	SetOutPath $INSTDIR\platforms
	File "${ExeLocation}\plugins\platforms\qwindows.dll"
	CreateDirectory "$SMPROGRAMS\PM GUI"
	CreateShortCut "$SMPROGRAMS\PM GUI\PM_GUI.lnk" "$INSTDIR\pm_gui.exe"
	CreateShortCut "$SMPROGRAMS\PM GUI\uninstall.lnk" "$INSTDIR\Uninstall.exe"
	CreateShortCut "$DESKTOP\PM GUI.lnk" "$INSTDIR\pm_gui.exe"
	writeUninstaller "$INSTDIR\uninstall.exe"
SectionEnd


Section "Uninstall"
	delete "$INSTDIR\*.*"
	delete "$INSTDIR\platforms\*.*"
	delete "$DESKTOP\PM GUI.lnk"
	RMDir "$INSTDIR\platforms"
	RMDir "$INSTDIR"
	delete "$SMPROGRAMS\PM GUI\PM_GUI.lnk"
	delete "$SMPROGRAMS\PM GUI\uninstall.lnk"
	RMDir "$SMPROGRAMS\PM GUI"
SectionEnd



