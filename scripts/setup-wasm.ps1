$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path $PSScriptRoot -Parent
$SdkRoot = Join-Path $RepoRoot '.local\emsdk'
if (!(Test-Path -LiteralPath $SdkRoot)) {
    & git clone --depth 1 https://github.com/emscripten-core/emsdk.git $SdkRoot
    if ($LASTEXITCODE -ne 0) { throw 'Could not download the Emscripten SDK.' }
}
Push-Location $SdkRoot
try {
    & .\emsdk.bat install 6.0.12
    if ($LASTEXITCODE -ne 0) { throw 'Emscripten installation failed.' }
    & .\emsdk.bat activate 6.0.12
    if ($LASTEXITCODE -ne 0) { throw 'Emscripten activation failed.' }
} finally { Pop-Location }
Write-Host 'Run npm run build:wasm from the repository root.'
