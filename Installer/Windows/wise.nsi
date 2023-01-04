!searchparse /file "../../WISE/include/prom_vers.h" `#define VER_VERSION_MAJOR ` VER_MAJOR
!searchparse /file "../../WISE/include/prom_vers.h" `#define VER_VERSION_MINOR ` VER_MINOR
!searchparse /file "../../WISE/include/prom_vers.h" `#define VER_VERSION_MAINTENANCE ` VER_MAINTENANCE
!searchparse /file "../../WISE/include/prom_vers.h" `#define VER_VERSION_BUILD ` VER_BUILD
!searchparse /file "../../WISE/include/prom_vers.h" `#define VER_RELEASE_DATE_SHORT ` VER_RELEASE_DATE

!define PRODUCT_NAME "W.I.S.E."
!define PRODUCT_VERSION "${VER_MINOR}.${VER_MAINTENANCE}.${VER_BUILD}"
!define SETUP_VERSION "${PRODUCT_VERSION}_${VER_RELEASE_DATE}"
!define PRODUCT_PUBLISHER "The W.I.S.E. Development Team"
!define REGISTRY_LOCATION "CWFGM Project Steering Committee"
!define PRODUCT_WEB_SITE "http://www.firegrowthmodel.ca"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Prometheus.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define MULTIUSER_EXECUTIONLEVEL "Highest"
!define sysGetDiskFreeSpaceEx 'kernel32::GetDiskFreeSpaceExA(t, *l, *l, *l) i'

!define /date TimeStamp "%Y%m%d"
!ifdef OUTPUT_FILENAME
  OutFile "${OUTPUT_FILENAME}"
!else
  OutFile "WISE_${PRODUCT_VERSION}.exe"
!endif

!addplugindir ".\"

Var PrereqErrorCount
Var JavaPrereq
Var JavaPrereqLabel
Var JavaPrereqImage
Var JavaPrereqLink
Var JavaPrereq2
Var JavaPrereq2Label
Var JavaPrereq2Image
Var JavaPrereq2Link
Var JavaPrereq3
Var JavaPrereq3Label
Var JavaPrereq3Image
Var JavaPrereq3Link
Var NetPrereq
Var NetPrereqLabel
Var NetPrereqImage
Var NetPrereqLink
Var WindowsPrereq
Var WindowsPrereqLabel
Var WindowsPrereqImage
Var WindowsPrereqLink
Var VcPrereq
Var VcPrereqLabel
Var VcPrereqImage
Var VcPrereqLink
var JobsInstDir
Var ConfigFileExists

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "Locate.nsh"
!include "nsDialogs.nsh"
!include 'nsdialogs_setimageole.nsh'
!include "StrFunc.nsh"
!include "WinVer.nsh"
!include "x64.nsh"
!include "ReplaceInFile.nsh"
!include "FileFunc.nsh"
${StrTok}
${StrTrimNewLines}

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

!define MUI_FINISHPAGE_NOAUTOCLOSE

; define the UI
!define MUI_WELCOMEFINISHPAGE_BITMAP "wizard.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "header.bmp"
; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Prerequisites page
page custom ChkPrereqCreate ChkPrqLeave
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; License page
!define MUI_PAGE_CUSTOMFUNCTION_PRE promPre
!define MUI_PAGE_HEADER_TEXT "W.I.S.E. License Agreement"
!insertmacro MUI_PAGE_LICENSE "license_v53.rtf"
; for accepting third party licenses
Page custom thirdPartyLicenseCreate
; Directory page
!define MUI_PAGE_CUSTOMFUNCTION_PRE promPre
!insertmacro MUI_PAGE_DIRECTORY
; Jobs directory
!define MUI_PAGE_CUSTOMFUNCTION_PRE promPre
!define MUI_PAGE_HEADER_SUBTEXT "Choose the folder in which the W.I.S.E. jobs will be created"
!define MUI_DIRECTORYPAGE_TEXT_TOP "Setup will configure W.I.S.E. to run jobs from the following folder. To use a different folder, click browse and select another folder. Click next to continue."
!define MUI_DIRECTORYPAGE_VARIABLE $JobsInstDir
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Run configuration script"
!define MUI_FINISHPAGE_RUN_FUNCTION LaunchConfigScript
!define MUI_PAGE_CUSTOMFUNCTION_SHOW finishShow
!insertmacro MUI_PAGE_FINISH
; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Strings
LangString DESC_SEC03 ${LANG_ENGLISH} "This will install W.I.S.E."

; MUI end ------

Name "${PRODUCT_NAME}"
Icon "Prometheus.ico"
InstallDir "$PROGRAMFILES64\WISE"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

BrandingText "W.I.S.E. Installer"

Function .onInit
  InitPluginsDir
  BringToFront

  ReadRegStr $JobsInstDir HKCU "SOFTWARE\PSaaS_Manager" "JobDirectory"
  StrCmp $JobsInstDir "" 0 +2
  StrCpy $JobsInstDir "C:\jobs"

  File /oname=$PLUGINSDIR\missing.gif "missing.gif"
  File /oname=$PLUGINSDIR\check.gif "check.gif"
  File /oname=$PLUGINSDIR\question.gif "question.gif"
  File /oname=$PLUGINSDIR\info.gif "info.gif"
FunctionEnd

Section "WISE" SEC03
  w7tbp::Start
  
  ; check to see if this machine already has a config.json file in the job directory
  IfFileExists "$JobsInstDir\config.json" 0 +3
  StrCpy $ConfigFileExists 1
  Goto +2
  StrCpy $ConfigFileExists 0
  
  SetOutPath "$INSTDIR"

  File "${BUILD_DIR}\*.jar"
  File "${BUILD_DIR}\*.dll"
  File "${PROTOBUF_DIR}\*.dll"
  File "${BOOST_DIR}\*.dll"
  File "${GDAL_DIR}\dll\*.dll"
  File "${BUILD_DIR}\wise.exe"
  File "install_script.bat"
  
  RmDir /r "$INSTDIR\gdal-data"
  CreateDirectory "$INSTDIR\gdal-data"
  SetOutPath "$INSTDIR\gdal-data"
  File "${GDAL_DIR}\data\gdal\*"
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" GDAL_DATA "$INSTDIR\gdal-data\"
  RmDir /r "$INSTDIR\proj_nad"
  CreateDirectory "$INSTDIR\proj_nad"
  SetOutPath "$INSTDIR\proj_nad"
  File /r "${GDAL_DIR}\data\proj\*"
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" PROJ_LIB "$INSTDIR\proj_nad\"
  
  ; Write user modifiable data to the program data directory
  SetOutPath "$APPDATA\CWFGM\Prometheus"
  File "run\Prometheus.clrtbl"
  WriteRegStr HKCU "SOFTWARE\${REGISTRY_LOCATION}\Prometheus" "ColourTable" "$APPDATA\CWFGM\Prometheus\Prometheus.clrtbl"
  File "run\Prometheus.fuelmapx"
  WriteRegStr HKCU "SOFTWARE\${REGISTRY_LOCATION}\Prometheus" "FuelMap" "$APPDATA\CWFGM\Prometheus\Prometheus.fuelmapx"
  File "run\Prometheus_NZ.fuelmapx"
  WriteRegStr HKCU "SOFTWARE\${REGISTRY_LOCATION}\Prometheus" "FuelMapNZ" "$APPDATA\CWFGM\Prometheus\Prometheus_NZ.fuelmapx"
  File "run\Prometheus_TAZ.fuelmapx"
  WriteRegStr HKCU "SOFTWARE\${REGISTRY_LOCATION}\Prometheus" "FuelMapTAZ" "$APPDATA\CWFGM\Prometheus\Prometheus_TAZ.fuelmapx"

  ; Grant access to all users to the data written to the program data directory
  AccessControl::GrantOnFile "$APPDATA\CWFGM" "(S-1-5-32-545)" "FullAccess"
  AccessControl::GrantOnFile "$APPDATA\CWFGM\Prometheus" "(S-1-5-32-545)" "FullAccess"
  AccessControl::GrantOnFile "$APPDATA\CWFGM\Prometheus\*" "(S-1-5-32-545)" "FullAccess"
SectionEnd

Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\WISE\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"

  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\wise.exe"

  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$\"$INSTDIR\uninst.exe$\""
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "QuietUninstallString" "$\"$INSTDIR\uninst.exe$\" /S"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$\"$INSTDIR\wise.exe$\""
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  ${locate::GetSize} "$INSTDIR" "/S=OK" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD SHCTX "${PRODUCT_UNINST_KEY}" "EstimatedSize" "$0"
SectionEnd

Function promPre
  ${If} ${SectionIsSelected} ${SEC03}
  ${Else}
    Abort
  ${EndIf}
FunctionEnd

Function finishShow
  ${If} $ConfigFileExists == 1
    GetDlgItem $R9 $MUI_HWND 1203
    SendMessage $R9 ${BM_SETCHECK} ${BST_UNCHECKED} 0
  ${EndIf}
FunctionEnd

Function GetWindowsVersion
  Push $R0
  Push $R1
 
  ; check if Windows 10 family (CurrentMajorVersionNumber is new introduced in Windows 10)
  ReadRegStr $R0 HKLM \
    "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentMajorVersionNumber
 
  StrCmp $R0 '' 0 lbl_winnt
 
  ClearErrors
 
  ; check if Windows NT family
  ReadRegStr $R0 HKLM \
  "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
 
  IfErrors 0 lbl_winnt
 
  ; we are not NT
  ReadRegStr $R0 HKLM \
  "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber
 
  StrCpy $R1 $R0 1
  StrCmp $R1 '4' 0 lbl_error
 
  StrCpy $R1 $R0 3
 
  StrCmp $R1 '4.0' lbl_win32_95
  StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98
 
  lbl_win32_95:
    StrCpy $R0 '95'
  Goto lbl_done
 
  lbl_win32_98:
    StrCpy $R0 '98'
  Goto lbl_done
 
  lbl_win32_ME:
    StrCpy $R0 'ME'
  Goto lbl_done
 
  lbl_winnt:
 
  StrCpy $R1 $R0 1
 
  StrCmp $R1 '3' lbl_winnt_x
  StrCmp $R1 '4' lbl_winnt_x
 
  StrCpy $R1 $R0 3
 
  StrCmp $R1 '5.0' lbl_winnt_2000
  StrCmp $R1 '5.1' lbl_winnt_XP
  StrCmp $R1 '5.2' lbl_winnt_2003
  StrCmp $R1 '6.0' lbl_winnt_vista
  StrCmp $R1 '6.1' lbl_winnt_7
  StrCmp $R1 '6.2' lbl_winnt_8
  StrCmp $R1 '6.3' lbl_winnt_81
  StrCmp $R1 '10' lbl_winnt_10
 
  StrCpy $R1 $R0 4
 
  StrCmp $R1 '10.0' lbl_winnt_10
  Goto lbl_error
 
  lbl_winnt_x:
    StrCpy $R0 "NT $R0" 6
  Goto lbl_done
 
  lbl_winnt_2000:
    Strcpy $R0 '2000'
  Goto lbl_done
 
  lbl_winnt_XP:
    Strcpy $R0 'XP'
  Goto lbl_done
 
  lbl_winnt_2003:
    Strcpy $R0 '2003'
  Goto lbl_done
 
  lbl_winnt_vista:
    Strcpy $R0 'Vista'
  Goto lbl_done
 
  lbl_winnt_7:
    Strcpy $R0 '7'
  Goto lbl_done
 
  lbl_winnt_8:
    Strcpy $R0 '8'
  Goto lbl_done
 
  lbl_winnt_81:
    Strcpy $R0 '8.1'
  Goto lbl_done
 
  lbl_winnt_10:
    Strcpy $R0 '10.0'
  Goto lbl_done
 
  lbl_error:
    Strcpy $R0 ''
  lbl_done:
 
  Pop $R1
  Exch $R0
FunctionEnd

!macro GetWindowsVersion OUTPUT_VALUE
	Call GetWindowsVersion
	Pop `${OUTPUT_VALUE}`
!macroend
 
!define GetWindowsVersion '!insertmacro "GetWindowsVersion"'

Function PrereqChecker
  StrCpy $PrereqErrorCount 0
  
  ;check for vcredist 2010
  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x64" "Installed"
  ${If} $0 == ""
    ReadRegDWORD $0 HKLM "SOFTWARE\WOW6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x64" "Installed"
  ${EndIf}

  ${If} $0 == ""
    ${NSD_SetImageOLE} $JavaPrereq3 $PLUGINSDIR\missing.gif $JavaPrereq3Image
    ShowWindow $JavaPrereq3Link ${SW_SHOW}
    ShowWindow $JavaPrereq3Label ${SW_HIDE}
  ${Else}
    IntOp $PrereqErrorCount $PrereqErrorCount + 1
    ${NSD_SetImageOLE} $JavaPrereq3 $PLUGINSDIR\check.gif $JavaPrereq3Image
    ${NSD_SetText} $JavaPrereq3Label "VC redist 2010"
    ShowWindow $JavaPrereq3Link ${SW_HIDE}
    ShowWindow $JavaPrereq3Label ${SW_SHOW}
  ${EndIF}

  ;check for vcredist 2012
  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\VisualStudio\11.0\VC\Designtimes\x64\1033" "Installed"
  ${If} $0 == ""
    ReadRegDWORD $0 HKLM "SOFTWARE\WOW6432Node\Microsoft\VisualStudio\11.0\VC\Designtimes\x64\1033" "Installed"

    ${If} $0 == ""
      ReadRegDWORD $0 HKLM "SOFTWARE\Classes\Installer\Dependencies\{ca67548a-5ebe-413a-b50c-4b9ceb6d66c6}" "Version"
    ${EndIf}
  ${EndIf}

  ${If} $0 == ""
    ${NSD_SetImageOLE} $JavaPrereq2 $PLUGINSDIR\missing.gif $JavaPrereq2Image
    IntOp $PrereqErrorCount $PrereqErrorCount + 1
    ShowWindow $JavaPrereq2Link ${SW_SHOW}
    ShowWindow $JavaPrereq2Label ${SW_HIDE}
  ${Else}
    ${NSD_SetImageOLE} $JavaPrereq2 $PLUGINSDIR\check.gif $JavaPrereq2Image
    ${NSD_SetText} $JavaPrereq2Label "VC redist 2012"
    ShowWindow $JavaPrereq2Link ${SW_HIDE}
    ShowWindow $JavaPrereq2Label ${SW_SHOW}
  ${EndIf}

  ;check the installed Java version
  StrCpy $1 0
  ;get a temporary file and add a .bat extension. possible file collision if the temporary filename + .bat already exists
	GetTempFileName $8
	Rename $8 $8.bat
	StrCpy $8 $8.bat
	;write the batch file
	File /oname=$8 "java_check.bat"
	;execute the batch file
	nsExec::ExecToStack '$8'
	Pop $9 # return value/error/timeout
	Pop $R2 # printed text, up to ${NSIS_MAX_STRLEN}
  StrCpy $R3 $R2
	Delete $8 # delete the temporary file
  ${If} $9 != "0"
    FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
      FileSeek $4 0 END
      FileWrite $4 "Unable to execute Java detection script: $9$\r$\n"
    FileClose $4
  ${EndIf}
  
	strcmp $R3 "" JavaNotInstalled
	strcmp $R3 "Java Not Found" JavaNotInstalled
	${StrTrimNewLines} $R3 $R3
	${StrTok} $0 $R3 "." "0" "1"
	${StrTok} $1 $R3 "." "1" "1"
	; old Java version format checks, should stop being valid at Java 9
	${If} $0 == "1"
		strcmp $1 "8" +4
		strcmp $1 "9" +3
		strcmp $1 "10" +2
    Goto JavaNotInstalled
		DetailPrint "Java Version $R3"
    FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
      FileSeek $4 0 END
      FileWrite $4 "Java version: $1$\r$\n"
    FileClose $4
  ; new Java version format checks
	${Else}
		${If} $0 > 9
			DetailPrint "Java Version $R3"
      FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
        FileSeek $4 0 END
        FileWrite $4 "Java version: $0$\r$\n"
      FileClose $4
			Goto JavaInstalled
		${Else}
			Goto JavaNotInstalled
		${EndIf}
	${EndIf}
JavaInstalled:
  StrCpy $1 1
  StrCpy $8 ""
  Goto AfterJava
JavaNotInstalled:
  ; The script ran but couldn't find Java
  ${If} $9 == "0"
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "The installer was unable to detect Java. Without it W.I.S.E. may not run correctly.$\r$\nDo you want to continue anyways?" IDNO AfterJava
  ; The script did not run
  ${Else}
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "The installer was unable to run the Java detection script. If Java is not installed W.I.S.E. may not run correctly.$\r$\nDo you want to continue anyways?" IDNO AfterJava
  ${EndIf}
  FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
    FileSeek $4 0 END
    FileWrite $4 "User continued without Java"
  FileClose $4
  ${NSD_SetText} $JavaPrereqLabel "Skipped the Java check"
  ${NSD_SetImageOLE} $JavaPrereq $PLUGINSDIR\question.gif $JavaPrereqImage
  StrCpy $8 "1"
  StrCpy $1 1
AfterJava:

  ${If} $1 == 1
    ShowWindow $JavaPrereqLink ${SW_HIDE}
    ShowWindow $JavaPrereqLabel ${SW_SHOW}
    ${NSD_SetImageOLE} $JavaPrereq $PLUGINSDIR\check.gif $JavaPrereqImage
    ${NSD_SetText} $JavaPrereqLabel "Java version: $R3"
  ${Else}
    ShowWindow $JavaPrereqLink ${SW_SHOW}
    ShowWindow $JavaPrereqLabel ${SW_HIDE}
    ${NSD_SetImageOLE} $JavaPrereq $PLUGINSDIR\missing.gif $JavaPrereqImage
  ${EndIf}

  ; Check .NET version
  StrCpy $1 0
  ReadRegDWORD $0 HKLM 'SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full' Install
  ${If} $0 == ''
    ReadRegDWORD $0 HKLM 'SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Client' Install
    ${If} $0 == ''
    ${Else}
      StrCpy $1 1
      ReadRegDWORD $0 HKLM 'SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Client' Version
      FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
        FileSeek $4 0 END
        FileWrite $4 ".NET version: $0$\r$\n"
        ${NSD_SetText} $NetPrereqLabel ".NET version: $0"
      FileClose $4
    ${EndIf}
  ${Else}
    StrCpy $1 1
    ReadRegDWORD $0 HKLM 'SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full' Version
    FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
      FileSeek $4 0 END
      FileWrite $4 ".NET version: $0$\r$\n"
      ${NSD_SetText} $NetPrereqLabel ".NET version: $0"
    FileClose $4
  ${EndIf}

  ${If} $1 == 1
    ShowWindow $NetPrereqLink ${SW_HIDE}
    ShowWindow $NetPrereqLabel ${SW_SHOW}
    IntOp $PrereqErrorCount $PrereqErrorCount + 1
    ${NSD_SetImageOLE} $NetPrereq $PLUGINSDIR\check.gif $NetPrereqImage
  ${Else}
    ShowWindow $NetPrereqLink ${SW_SHOW}
    ShowWindow $NetPrereqLabel ${SW_HIDE}
    ${NSD_SetImageOLE} $NetPrereq $PLUGINSDIR\missing.gif $NetPrereqImage
  ${EndIf}

  ;check the Windows version
  StrCpy $1 0
  ${GetWindowsVersion} $R0
  FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
    FileSeek $4 0 END
    FileWrite $4 "Windows Version: $R0$\r$\n"
  FileClose $4
  ${If} ${AtLeastWin7}
    ${If} ${Runningx64}
      StrCpy $1 1
      ${NSD_SetText} $WindowsPrereqLabel "Windows version: $R0"
    ${EndIf}
  ${EndIf}

  ${If} $1 == 1
    ShowWindow $WindowsPrereqLink ${SW_HIDE}
    ShowWindow $WindowsPrereqLabel ${SW_SHOW}
    IntOp $PrereqErrorCount $PrereqErrorCount + 1
    ${NSD_SetImageOLE} $WindowsPrereq $PLUGINSDIR\check.gif $WindowsPrereqImage
  ${Else}
    ShowWindow $WindowsPrereqLink ${SW_SHOW}
    ShowWindow $WindowsPrereqLabel ${SW_HIDE}
    ${NSD_SetImageOLE} $WindowsPrereq $PLUGINSDIR\missing.gif $WindowsPrereqImage
  ${EndIf}

  ;check the installed vcredist version
  StrCpy $1 0
  ReadRegStr $3 HKCR64 "Installer\Dependencies\,,amd64,14.0,bundle" "Version"
  ${If} $3 == ""
    ReadRegDWORD $3 HKLM "SOFTWARE\WOW6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Major"
    ${If} $3 == ""
    ${Else}
      ${If} $3 > 14
        StrCpy $1 1
        FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
          FileSeek $4 0 END
          FileWrite $4 "vcredist 2019 already installed: $3$\r$\n"
          ${NSD_SetText} $VcPrereqLabel "VC redist: $3"
        FileClose $4
      ${ElseIf} $3 == 14
        ReadRegDWORD $3 HKLM "SOFTWARE\WOW6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Minor"
        ${If} $3 >= 20
          StrCpy $1 1
          FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
            FileSeek $4 0 END
            FileWrite $4 "vcredist 2019 already installed: $3$\r$\n"
            ReadRegStr $3 HKLM "SOFTWARE\WOW6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Version"
            ${NSD_SetText} $VcPrereqLabel "VC redist: $3"
          FileClose $4
        ${EndIf}
      ${EndIf}
    ${EndIf}
  ${Else}
    StrCpy $2 $3 2
    ${If} $2 > 14
      StrCpy $1 1
      FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
        FileSeek $4 0 END
        FileWrite $4 "vcredist 2019 already installed: $3$\r$\n"
        ${NSD_SetText} $VcPrereqLabel "VC redist: $3"
      FileClose $4
    ${ElseIf} $2 == 14
      StrCpy $2 $3 2 3
      ${If} $2 < 20
      ${Else}
        StrCpy $1 1
        FileOpen $4 "$APPDATA\CWFGM\Prometheus\install.log" a
          FileSeek $4 0 END
          FileWrite $4 "vcredist 2019 already installed: $3$\r$\n"
          ${NSD_SetText} $VcPrereqLabel "VC redist: $3"
        FileClose $4
      ${Endif}
    ${EndIf}
  ${Endif}

  ${If} $1 == 1
    ShowWindow $VcPrereqLink ${SW_HIDE}
    ShowWindow $VcPrereqLabel ${SW_SHOW}
    IntOp $PrereqErrorCount $PrereqErrorCount + 1
    ${NSD_SetImageOLE} $VcPrereq $PLUGINSDIR\check.gif $VcPrereqImage
  ${Else}
    ShowWindow $VcPrereqLink ${SW_SHOW}
    ShowWindow $VcPrereqLabel ${SW_HIDE}
    ${NSD_SetImageOLE} $VcPrereq $PLUGINSDIR\missing.gif $VcPrereqImage
  ${EndIf}

  GetDlgItem $1 $HWNDPARENT 1
  ${NSD_SetText} $1 "$(^NextBtn)"
  ${If} $PrereqErrorCount < 4
    ${NSD_SetText} $1 "&Refresh"
  ${EndIf}
FunctionEnd

Function ChkPrqLeave
FunctionEnd

!define CreatePrq "!insertmacro CreatePrq"

!macro CreatePrq Top Text LinkText LinkClickHandler ChkBox LinkBox LblBox
  ; Checkbox Top
  StrCpy $1 ${Top}
  IntOp $1 $1 - 1

  ${NSD_CreateBitmap} 0u $1u 16u 16u ""
  Pop ${ChkBox}

  ${NSD_CreateLabel} 18u ${Top}u 130u 10u "${Text}"
  Pop ${LblBox}

  ; LinkText Top
  ${NSD_CreateLink} 18u ${Top}u 100% 10u "${LinkText}"
  Pop ${LinkBox}
  ${NSD_OnClick} ${LinkBox} ${LinkClickHandler}
!macroend

Function JavaPrereqClick
  Pop $0
  ExecShell "open" "https://java.com/en/download/manual.jsp"
FunctionEnd

Function JavaPrereq2Click
  Pop $0
  ExecShell "open" "https://www.microsoft.com/en-ca/download/details.aspx?id=30679"
FunctionEnd

Function JavaPrereq3Click
  Pop $0
  ExecShell "open" "https://www.microsoft.com/en-ca/download/details.aspx?id=26999"
FunctionEnd

Function NetPrereqClick
  Pop $0
  ExecShell "open" "https://www.microsoft.com/en-ca/download/details.aspx?id=48130"
FunctionEnd

Function WindowsPrereqClick
  Pop $0
FunctionEnd

Function VcPrereqClick
  Pop $0
  ExecShell "open" "https://support.microsoft.com/en-ca/help/2977003/the-latest-supported-visual-c-downloads"
FunctionEnd

Function ChkPrereqCreate
  ClearErrors
  ${GetParameters} $R0
  ${GetOptions} $R0 "/no-prereq" $R1
  IfErrors +2 0
  Abort

  !insertmacro MUI_HEADER_TEXT "W.I.S.E. Prerequisites" "Checking to make sure all required prerequisites for W.I.S.E. have been installed."
  nsDialogs::Create 1018
  Pop $0

  ${If} $0 == error
    Abort
  ${EndIf}

  ${CreatePrq} 15u "Missing vcredist 2010 for Java" "Microsoft Visual C++ 2010 Service Pack 1 Redistributable Package MFC Security Update" JavaPrereq3Click $JavaPrereq3 $JavaPrereq3Link $JavaPrereq3Label
  ${CreatePrq} 30u "Missing vcredist 2012 for Java" "Visual C++ Redistributable for Visual Studio 2012 Update 4" JavaPrereq2Click $JavaPrereq2 $JavaPrereq2Link $JavaPrereq2Label
  ${CreatePrq} 45u "Incorrect Java Version" "Java 8 (x64) for Windows." JavaPrereqClick $JavaPrereq $JavaPrereqLink $JavaPrereqLabel
  ${CreatePrq} 60u "Incorrect .NET Version" "Microsoft .NET Framework 4.6 (Web Installer)" NetPrereqClick $NetPrereq $NetPrereqLink $NetPrereqLabel
  ${CreatePrq} 75u "Incorrect Windows Version" "Windows 10 (x64)" WindowsPrereqClick $WindowsPrereq $WindowsPrereqLink $WindowsPrereqLabel
  ${CreatePrq} 90u "Incorrect VC Redist Version" "Microsoft Visual C++ Redistributable 2022 (x64)" VcPrereqClick $VcPrereq $VcPrereqLink $VcPrereqLabel

  Call PrereqChecker
  nsDialogs::Show
FunctionEnd

Function thirdPartyLicenseCreate
!insertmacro MUI_HEADER_TEXT "License Agreement" "Please review the license terms for third party libraries before installing ${PRODUCT_NAME} ${PRODUCT_VERSION}"

GetDlgItem $R0 $HWNDPARENT 1
SendMessage $R0 ${WM_SETTEXT} 0 `STR:I Agree`

nsDialogs::Create 1018
Pop $0
${If} $0 == error
  Abort
${EndIf}

${NSD_CreateLabel} 0 0 110u 10u "protobuf"
Pop $0
${NSD_CreateLink} 115u 0 100% 10u "BSD"
Pop $0
${NSD_OnClick} $0 onProtobufLinkClick

${NSD_CreateLabel} 0 10u 110u 10u "GDAL"
Pop $0
${NSD_CreateLink} 115u 10u 100% 10u "GDAL License"
Pop $0
${NSD_OnClick} $0 onGdalLinkClick

${NSD_CreateLabel} 0 20u 110u 10u "ZLib"
Pop $0
${NSD_CreateLink} 115u 20u 100% 10u "zlib license"
Pop $0
${NSD_OnClick} $0 onZlibLinkClick

${NSD_CreateLabel} 0 30u 110u 10u "XERCES"
Pop $0
${NSD_CreateLink} 115u 30u 100% 10u "Apache 2.0"
Pop $0
${NSD_OnClick} $0 onApacheLinkClick

${NSD_CreateLabel} 0 40u 110u 10u "CURL"
Pop $0
${NSD_CreateLink} 115u 40u 100% 10u "CURL license"
Pop $0
${NSD_OnClick} $0 onCurlLinkClick

${NSD_CreateLabel} 0 50u 110u 10u "MINIZIP"
Pop $0
${NSD_CreateLink} 115u 50u 100% 10u "zlib license"
Pop $0
${NSD_OnClick} $0 onZlibLinkClick

${NSD_CreateLabel} 0 60u 110u 10u "BOOST"
Pop $0
${NSD_CreateLink} 115u 60u 100% 10u "BOOST license"
Pop $0
${NSD_OnClick} $0 onBoostLinkClick

${NSD_CreateLabel} 0 70u 110u 10u "Apache Commons"
Pop $0
${NSD_CreateLink} 115u 70u 100% 10u "Apache 2.0"
Pop $0
${NSD_OnClick} $0 onApacheLinkClick

${NSD_CreateLabel} 0 80u 110u 10u "poi"
Pop $0
${NSD_CreateLink} 115u 80u 100% 10u "Apache 2.0"
Pop $0
${NSD_OnClick} $0 onApacheLinkClick

${NSD_CreateLabel} 0 90u 110u 10u "xmlbeans"
Pop $0
${NSD_CreateLink} 115u 90u 100% 10u "Apache 2.0"
Pop $0
${NSD_OnClick} $0 onApacheLinkClick

${NSD_CreateLabel} 0 100u 110u 10u "jaxb"
Pop $0
${NSD_CreateLink} 115u 100u 100% 10u "CDDL version 1.1"
Pop $0
${NSD_OnClick} $0 onJaxbLinkClick

${NSD_CreateLabel} 0 110u 110u 10u "Eclipse Paho"
Pop $0
${NSD_CreateLink} 115u 110u 100% 10u "EPL v1.0"
Pop $0
${NSD_OnClick} $0 onEclipseLinkClick

nsDialogs::Show
FunctionEnd

Function onProtobufLinkClick
  Pop $0
  ExecShell "open" "https://github.com/protocolbuffers/protobuf/blob/master/LICENSE"
FunctionEnd

Function onGdalLinkClick
  Pop $0
  ExecShell "open" "https://gdal.org/license.html"
FunctionEnd

Function onZlibLinkClick
  Pop $0
  ExecShell "open" "https://directory.fsf.org/wiki/License:Zlib"
FunctionEnd

Function onApacheLinkClick
  Pop $0
  ExecShell "open" "https://www.apache.org/licenses/LICENSE-2.0.txt"
FunctionEnd

Function onCurlLinkClick
  Pop $0
  ExecShell "open" "https://curl.se/docs/copyright.html"
FunctionEnd

Function onBoostLinkClick
  Pop $0
  ExecShell "open" "https://www.boost.org/users/license.html"
FunctionEnd

Function onJaxbLinkClick
  Pop $0
  ExecShell "open" "https://javaee.github.io/jaxb-v2/LICENSE"
FunctionEnd

Function onEclipseLinkClick
  Pop $0
  ExecShell "open" "https://www.eclipse.org/org/documents/epl-v10.php"
FunctionEnd

Function LaunchConfigScript
  Exec '"$INSTDIR\install_script.bat" "$JobsInstDir"'
FunctionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} $(DESC_SEC03)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


; uninstaller
Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Section Uninstall
  w7tbp::Start

  SetShellVarContext current
  Delete "$SMPROGRAMS\WISE\Uninstall.lnk"
  Delete "$DESKTOP\Prometheus v7.lnk"
  Delete "$SMPROGRAMS\WISE\WISE.lnk"
  RMDir "$SMPROGRAMS\WISE"
  SetShellVarContext all ; this exists so that all icons/menu entries are properly cleaned up regardless of whether WISE was installed for all users or a single user
  Delete "$SMPROGRAMS\WISE\Uninstall.lnk"
  Delete "$DESKTOP\WISE.lnk"
  Delete "$SMPROGRAMS\WISE\WISE.lnk"
  RMDir "$SMPROGRAMS\WISE"
  ; force delete, leave nothing behind
  RMDir /r "$INSTDIR"

  DeleteRegKey HKLM "SOFTWARE\${REGISTRY_LOCATION}\${PRODUCT_NAME}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetRegView 32
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKCU "${PRODUCT_UNINST_KEY}"
  SetRegView 64
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKCU "${PRODUCT_UNINST_KEY}"
  SetAutoClose true
SectionEnd
