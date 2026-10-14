param([string]$EngineRoot = "E:\UE_5.7", [switch]$Editor, [switch]$BuildOnly)
$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path $PSScriptRoot -Parent
$ProjectFile = Join-Path $ProjectRoot "VelocityX.uproject"
$Packaged = Join-Path $ProjectRoot "Build\Windows\VelocityX.exe"
if ((Test-Path -LiteralPath $Packaged) -and !$Editor -and !$BuildOnly) {
    Start-Process -FilePath $Packaged -WorkingDirectory (Split-Path $Packaged -Parent)
    exit 0
}
if (!(Test-Path -LiteralPath (Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor.exe"))) {
    throw "Unreal 5.7 was not found at $EngineRoot. Run Scripts\Play.ps1 -EngineRoot 'your UE_5.7 folder'."
}
$Dotnet = Join-Path $EngineRoot "Engine\Binaries\ThirdParty\DotNet\8.0.412\win-x64\dotnet.exe"
$UBT = Join-Path $EngineRoot "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.dll"
$BuildLog = Join-Path $ProjectRoot "Saved\Logs\Build.log"
New-Item -ItemType Directory -Path (Split-Path $BuildLog -Parent) -Force | Out-Null
& $Dotnet $UBT VelocityXEditor Win64 Development "-Project=$ProjectFile" -WaitMutex -NoHotReloadFromIDE -NoUBA -NoUBALocal "-Log=$BuildLog"
if ($LASTEXITCODE -ne 0) { throw "C++ build failed. See $BuildLog" }
$EditorCmd = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
if (!(Test-Path -LiteralPath (Join-Path $ProjectRoot "Content\Maps\Sunridge.umap"))) {
    $AssetScript = Join-Path $PSScriptRoot "create_drive_assets.py"
    & $EditorCmd $ProjectFile /Engine/Maps/Entry -run=pythonscript "-script=$AssetScript" -unattended -nullrhi -nosound -ddc=VelocityLocalDDC "-ShaderWorkingDir=$ProjectRoot\Intermediate\ShaderWork"
    if ($LASTEXITCODE -ne 0) { throw "Asset generation failed. See Saved\Logs\VelocityX.log" }
}
if ($BuildOnly) { exit 0 }
$Exe = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor.exe"
$GameArgs = @(('"' + $ProjectFile + '"'), "-ddc=VelocityLocalDDC", ('"-ShaderWorkingDir=' + $ProjectRoot + '\Intermediate\ShaderWork"'))
if (!$Editor) { $GameArgs += @("-game", "-windowed", "-ResX=1600", "-ResY=900") }
Start-Process -FilePath $Exe -ArgumentList $GameArgs -WorkingDirectory $ProjectRoot
