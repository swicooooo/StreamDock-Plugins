param([switch]$Build)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$config = Get-Content -Raw -LiteralPath (Join-Path $root 'plugin-config.json') | ConvertFrom-Json
$folder = "$($config.pluginId).sdPlugin"
$source = Join-Path $root "dist\$folder"
$destination = Join-Path $env:APPDATA "HotSpot\StreamDock\plugins\$folder"
$streamDockShortcut = 'C:\Users\Public\Desktop\Stream Dock.lnk'

if ($Build) {
  & (Join-Path $root 'build.bat')
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
if (-not (Test-Path -LiteralPath (Join-Path $source "$($config.executableName).exe"))) {
  throw "Release output not found. Run build.bat first."
}
if (-not (Test-Path -LiteralPath $streamDockShortcut)) {
  throw "Stream Dock shortcut not found: $streamDockShortcut"
}

# Stream Dock keeps plugin files open while running. Close the host first so the
# plugin directory can always be replaced cleanly.
$streamDockProcesses = @(Get-Process -Name 'StreamDock' -ErrorAction SilentlyContinue)
if ($streamDockProcesses.Count -gt 0) {
  Write-Host '[INFO] Closing Stream Dock...'
  foreach ($process in $streamDockProcesses) {
    if ($process.MainWindowHandle -ne 0) {
      [void]$process.CloseMainWindow()
    }
  }
  $streamDockProcesses | Wait-Process -Timeout 5 -ErrorAction SilentlyContinue
  $streamDockProcesses = @(Get-Process -Name 'StreamDock' -ErrorAction SilentlyContinue)
  if ($streamDockProcesses.Count -gt 0) {
    Write-Host '[INFO] Stream Dock did not exit in time; stopping it now.'
    $streamDockProcesses | Stop-Process -Force
    $streamDockProcesses | Wait-Process -Timeout 5 -ErrorAction SilentlyContinue
  }
}

New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
if (Test-Path -LiteralPath $destination) {
  $processName = [System.IO.Path]::GetFileNameWithoutExtension($config.executableName)
  $running = @(Get-Process -Name $processName -ErrorAction SilentlyContinue)
  if ($running.Count -gt 0) {
    Write-Host "[INFO] Stopping running plugin process: $processName"
    $running | Stop-Process -Force
    $running | Wait-Process -Timeout 5 -ErrorAction SilentlyContinue
  }

  $removed = $false
  for ($attempt = 1; $attempt -le 10; $attempt++) {
    try {
      Remove-Item -LiteralPath $destination -Recurse -Force -ErrorAction Stop
      $removed = $true
      break
    }
    catch {
      if ($attempt -eq 10) {
        throw "Cannot replace the running plugin. Close Stream Dock and run build.bat again. $($_.Exception.Message)"
      }
      Start-Sleep -Milliseconds 300
    }
  }
}
Copy-Item -LiteralPath $source -Destination $destination -Recurse
Write-Host "[OK] Deployed Release plugin to: $destination"

Write-Host '[INFO] Starting Stream Dock...'
Start-Process -FilePath $streamDockShortcut
Write-Host '[OK] Stream Dock started.'
