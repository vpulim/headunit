; The name of the installer
Name "HeadUnit"

; The file to write
OutFile "headunit-0.2.exe"

; The default installation directory
InstallDir $PROGRAMFILES\HeadUnit

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\mp3car\headunit" "install_dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "HeadUnit (required)"

  SectionIn RO
  
  ; Set output directory for skin files first
  SetOutPath $INSTDIR\skins\Default
  ; Put skin files there
  File "skins\Default\*.jpg"
  File "skins\Default\*.skin"

  ; Set output directory for rest of files
  SetOutPath $INSTDIR  
  ; Put files there
  File "headunit.exe"
  File "D:\QT\3.2.1NonCommercial\bin\qt-mtnc321.dll"
  File "C:\WINDOWS\system32\msvcp70.dll"
  File /oname=plugins\sqldrivers\qsqlite.dll "D:\QT\3.2.1NonCommercial\plugins\sqldrivers\qsqlite.dll"

  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\mp3car\headunit" "install_dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\headunit" "DisplayName" "HeadUnit"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\headunit" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\headunit" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\headunit" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\HeadUnit"
  CreateShortCut "$SMPROGRAMS\HeadUnit\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\HeadUnit\HeadUnit.lnk" "$INSTDIR\headunit.exe" "" "$INSTDIR\headunit.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\headunit"
  DeleteRegKey HKLM SOFTWARE\mp3car

  ; Remove files and uninstaller
  Delete $INSTDIR\headunit.exe
  Delete $INSTDIR\headunit.db
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\qt-mtnc321.dll
  Delete $INSTDIR\msvcp70.dll
  Delete $INSTDIR\plugins\sqldrivers
  Delete $INSTDIR\skins\Default\*
  RMDir $INSTDIR\plugins\sqldrivers
  RMDir $INSTDIR\plugins
  RMDir $INSTDIR\skins\Default
  RMDir $INSTDIR\skins

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\HeadUnit\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\HeadUnit"
  RMDir "$INSTDIR"

SectionEnd