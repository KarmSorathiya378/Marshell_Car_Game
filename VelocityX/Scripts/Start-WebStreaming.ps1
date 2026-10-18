param(
    [string]$EngineRoot = 'E:\UE_5.7',
    [ValidateRange(1024, 65535)][int]$HttpPort = 8080,
    [ValidateRange(1024, 65535)][int]$StreamerPort = 8888,
    [switch]$SkipBuild
)
$ErrorActionPreference = 'Stop'
$ProjectRoot = Split-Path $PSScriptRoot -Parent
$RepoRoot = Split-Path $ProjectRoot -Parent
$Infrastructure = Join-Path $RepoRoot '.local\PixelStreamingInfrastructure'
$ServerRoot = Join-Path $Infrastructure 'SignallingWebServer'
$ServerEntry = Join-Path $ServerRoot 'dist\index.js'
$PlayerRoot = Join-Path $ServerRoot 'www'
$Runtime = Join-Path $RepoRoot '.local\streaming'
$StateFile = Join-Path $Runtime 'processes.json'
$Unreal = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
if (!(Test-Path -LiteralPath $Unreal)) { throw "Unreal Editor not found at $Unreal" }
if ($HttpPort -eq $StreamerPort) { throw 'HTTP and streamer ports must be different.' }
if (Test-Path -LiteralPath $StateFile) { throw 'Run Stop-WebStreaming.ps1 before starting another session.' }
if (!(Test-Path -LiteralPath $ServerEntry) -or !(Test-Path -LiteralPath (Join-Path $PlayerRoot 'player.html'))) {
    & (Join-Path $PSScriptRoot 'Setup-WebStreaming.ps1')
}
if (!$SkipBuild) {
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'Play.ps1') -EngineRoot $EngineRoot -BuildOnly
    if ($LASTEXITCODE -ne 0) { throw 'Unreal build failed.' }
}
foreach ($Port in @($HttpPort, $StreamerPort)) {
    if (Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue) {
        throw "Port $Port is already in use. Stop the existing server or choose another port."
    }
}
New-Item -ItemType Directory -Path $Runtime -Force | Out-Null
$Server = $null
$Game = $null
try {
    $ServerArgs = @(('"' + $ServerEntry + '"'), '--no_config', '--serve', '--player_port', $HttpPort,
        '--streamer_port', $StreamerPort, '--http_root', ('"' + $PlayerRoot + '"'), '--homepage', 'player.html')
    $Server = Start-Process -FilePath (Get-Command node.exe).Source -ArgumentList $ServerArgs -WorkingDirectory $ServerRoot -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $Runtime 'server.log') -RedirectStandardError (Join-Path $Runtime 'server-error.log')
    $Ready = $false
    for ($Attempt = 0; $Attempt -lt 30; $Attempt++) {
        if ($Server.HasExited) { throw 'Streaming server exited. See .local/streaming/server-error.log.' }
        try {
            $Response = Invoke-WebRequest -UseBasicParsing -Uri "http://127.0.0.1:$HttpPort/player.html" -TimeoutSec 1
            if ($Response.StatusCode -eq 200) { $Ready = $true; break }
        } catch { Start-Sleep -Milliseconds 500 }
    }
    if (!$Ready) { throw 'Streaming server did not become ready.' }
    $GameArgs = @(('"' + (Join-Path $ProjectRoot 'VelocityX.uproject') + '"'), '/Game/Maps/Sunridge',
        '-game', '-RenderOffscreen', '-ForceRes', '-ResX=1280', '-ResY=720', '-AudioMixer', '-unattended', '-nosplash',
        '-preferNvidia', '-ddc=VelocityLocalDDC', ('"-ShaderWorkingDir=' + $ProjectRoot + '\Intermediate\ShaderWork"'),
        "-PixelStreamingURL=ws://127.0.0.1:$StreamerPort", '-PixelStreamingID=VelocityX',
        '-ExecCmds="t.MaxFPS 60"', ('"-abslog=' + (Join-Path $Runtime 'unreal.log') + '"'))
    $Game = Start-Process -FilePath $Unreal -ArgumentList $GameArgs -WorkingDirectory $ProjectRoot -WindowStyle Hidden -PassThru
    @{
        ServerId = $Server.Id; ServerStart = $Server.StartTime.ToUniversalTime().ToString('o')
        GameId = $Game.Id; GameStart = $Game.StartTime.ToUniversalTime().ToString('o')
        PlayerUrl = "http://127.0.0.1:$HttpPort/player.html"
    } | ConvertTo-Json | Set-Content -LiteralPath $StateFile
    Write-Host 'Unreal is loading and connecting to the streaming server...'
    $GameReady = $false
    $GameLog = Join-Path $Runtime 'unreal.log'
    for ($Attempt = 0; $Attempt -lt 180; $Attempt++) {
        if ($Game.HasExited) { throw 'Unreal exited during startup. See .local/streaming/unreal.log.' }
        if ((Test-Path -LiteralPath $GameLog) -and (Get-Item -LiteralPath $GameLog).LastWriteTimeUtc -ge $Game.StartTime.ToUniversalTime()) {
            $LogText = Get-Content -LiteralPath $GameLog -Raw
            if ($LogText -match 'VELOCITY_READY: car spawned' -and $LogText -match 'OnJoined\. Local participant joined') {
                $GameReady = $true
                break
            }
        }
        Start-Sleep -Seconds 1
    }
    if (!$GameReady) { throw 'Unreal did not become ready within three minutes. See .local/streaming/unreal.log.' }
} catch {
    if ($Game -and !$Game.HasExited) { Stop-Process -Id $Game.Id }
    if ($Server -and !$Server.HasExited) { Stop-Process -Id $Server.Id }
    if (Test-Path -LiteralPath $StateFile) { Remove-Item -LiteralPath $StateFile }
    throw
}
Write-Host "Open http://127.0.0.1:$HttpPort/player.html in a browser."
Write-Host 'Allow Unreal to load. Click to connect/play, then press Enter to start the Sunridge race.'
Write-Host 'Stop this session with VelocityX/Scripts/Stop-WebStreaming.ps1.'
