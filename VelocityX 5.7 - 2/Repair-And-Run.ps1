$ErrorActionPreference = 'Stop'
try {
    $project = Join-Path $PSScriptRoot 'VelocityX.uproject'
    $engine = 'E:\UE_5.7'
    $installer = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\setup.exe'
    $sdkKey = 'HKLM:\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\NETFXSDK\4.8'
    if (-not (Test-Path $sdkKey)) {
        Write-Host 'Installing .NET Framework 4.8 SDK and targeting pack. Accept the Windows administrator prompt.'
        $installArgs = 'modify --installPath "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools" --channelId VisualStudio.18.Release --add Microsoft.Net.Component.4.8.SDK --add Microsoft.Net.Component.4.8.TargetingPack --passive --norestart'
        $installation = Start-Process -FilePath $installer -ArgumentList $installArgs -Verb RunAs -Wait -PassThru
        if ($installation.ExitCode -eq 3010) {
            throw 'Installation requires a restart. Restart Windows, then run Run-VelocityX.cmd again.'
        }
        if ($installation.ExitCode -ne 0) {
            throw "Visual Studio Installer exited with code $($installation.ExitCode). Close any other installer windows and retry."
        }
        if (-not (Test-Path $sdkKey)) {
            throw 'The .NET Framework 4.8 SDK is still missing after installation.'
        }
    }
    $log = Join-Path $PSScriptRoot 'Saved\Logs\Rebuild.log'
    Write-Host 'Building VelocityXEditor...'
    & "$engine\Engine\Build\BatchFiles\Build.bat" VelocityXEditor Win64 Development "-Project=$project" "-Log=$log" -WaitMutex -NoHotReloadFromIDE
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed. Compiler details are saved in $log"
    }
    Write-Host 'Build succeeded. Opening Unreal Editor...'
    Start-Process -FilePath "$engine\Engine\Binaries\Win64\UnrealEditor.exe" -ArgumentList ('"' + $project + '"')
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    Read-Host 'Press Enter to close'
    exit 1
}
