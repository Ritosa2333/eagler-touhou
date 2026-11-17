param([int]$Port = 8140)
$ErrorActionPreference = "Stop"
$pythonCommand = Get-Command python -ErrorAction SilentlyContinue
if ($pythonCommand) {
  & $pythonCommand.Source -X utf8 (Join-Path $PSScriptRoot "serve.py") --port $Port
} else {
  & py -3 -X utf8 (Join-Path $PSScriptRoot "serve.py") --port $Port
}
