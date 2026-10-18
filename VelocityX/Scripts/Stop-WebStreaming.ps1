$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$StateFile = Join-Path $RepoRoot '.local\streaming\processes.json'
if (!(Test-Path -LiteralPath $StateFile)) { Write-Host 'No recorded streaming session.'; return }
$Session = Get-Content -LiteralPath $StateFile -Raw | ConvertFrom-Json
foreach ($Kind in @('Game', 'Server')) {
    $ProcessId = $Session.("${Kind}Id")
    $ExpectedStart = $Session.("${Kind}Start")
    $Process = Get-Process -Id $ProcessId -ErrorAction SilentlyContinue
    if ($Process -and $Process.StartTime.ToUniversalTime().ToString('o') -eq $ExpectedStart) {
        Stop-Process -Id $ProcessId
    }
}
Remove-Item -LiteralPath $StateFile
Write-Host 'Streaming session stopped.'
