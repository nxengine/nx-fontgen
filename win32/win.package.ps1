#prepare

mkdir release
cd release

#prepare and copy data
if ($env:PLATFORM -eq "x86") {
  cp ..\win32\fontbm\Release\fontbm.exe .\fontbm.exe
} else {
  cp ..\x64\fontbm\Release\fontbm.exe .\fontbm.exe
}

if ($env:PLATFORM -eq "x86") {
  #bundle libs
  cp ..\win32\ext\runtime\x86\* .\
  cp -r ..\assets .\assets

  #create zip
  $name = "fontbm-$env:APPVEYOR_REPO_TAG_NAME-Win32.zip"
  7z a $name .\

  Push-AppveyorArtifact $name
} else {
  #bundle libs
  cp ..\win32\ext\runtime\x64\* .\
  cp -r ..\assets .\assets

  #create zip
  $name = "fontbm-$env:APPVEYOR_REPO_TAG_NAME-Win64.zip"
  7z a $name .\

  Push-AppveyorArtifact $name
}