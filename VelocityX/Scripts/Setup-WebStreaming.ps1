param()
$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Infrastructure = Join-Path $RepoRoot '.local\PixelStreamingInfrastructure'
$Revision = '15f616198b7b33b1544f2a6a2d53b5f44e6ae707'
Get-Command git, node, npm.cmd -ErrorAction Stop | Out-Null
if (!(Test-Path -LiteralPath $Infrastructure)) {
    & git clone --depth 1 --branch UE5.7 https://github.com/EpicGamesExt/PixelStreamingInfrastructure.git $Infrastructure
    if ($LASTEXITCODE -ne 0) { throw 'Could not download Epic Pixel Streaming infrastructure.' }
}
Push-Location $Infrastructure
try {
    & git fetch --depth 1 origin $Revision
    if ($LASTEXITCODE -ne 0) { throw 'Could not fetch the tested UE5.7 infrastructure revision.' }
    & git checkout --detach $Revision
    if ($LASTEXITCODE -ne 0) { throw 'Could not select the tested infrastructure revision.' }
    & npm.cmd ci --workspace Common --workspace Signalling --workspace SignallingWebServer --workspace Frontend/library --workspace Frontend/ui-library --workspace Frontend/implementations/typescript --include-workspace-root --no-audit --no-fund
    if ($LASTEXITCODE -ne 0) { throw 'Streaming dependency installation failed.' }
    & npm.cmd run build:all:cjs
    if ($LASTEXITCODE -ne 0) { throw 'Streaming server/player build failed.' }
} finally {
    Pop-Location
}
Write-Host 'Streaming infrastructure is ready. Run Start-WebStreaming.ps1 next.'
