param([string]$EngineRoot="E:\UE_5.7")
$ErrorActionPreference="Stop"
$ProjectRoot=Split-Path $PSScriptRoot -Parent
& (Join-Path $PSScriptRoot "Play.ps1") -EngineRoot $EngineRoot -BuildOnly
$Exe=Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$ProjectFile=Join-Path $ProjectRoot "VelocityX.uproject"
$Report=Join-Path $ProjectRoot "Saved\Automation"
& $Exe $ProjectFile -unattended -nullrhi -nosound -ddc=VelocityLocalDDC "-ShaderWorkingDir=$ProjectRoot\Intermediate\ShaderWork" -run=DriveValidation
if ($LASTEXITCODE -ne 0) { throw "Automation failed. See Saved\DriveValidation.txt and Saved\Logs\VelocityX.log" }
$Result=Join-Path $ProjectRoot "Saved\DriveSmokeResult.txt"
if (Test-Path -LiteralPath $Result) { Remove-Item -LiteralPath $Result }
& $Exe $ProjectFile -game -unattended -nullrhi -nosound -ddc=VelocityLocalDDC "-ShaderWorkingDir=$ProjectRoot\Intermediate\ShaderWork" -DriveSmokeTest
if ($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath $Result)) { throw "Runtime smoke test failed" }
Get-Content -LiteralPath $Result
