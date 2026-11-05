function Resolve-DeploymentPython {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory = $true)][string] $Python,
        [Parameter(Mandatory = $true)][string] $EnvironmentDirectory,
        [Parameter(Mandatory = $true)][string] $RequirementsPath,
        [string[]] $RequiredModules = @('soundfile'),
        [switch] $RequirePrivateEnvironment
    )

    if (-not $RequiredModules -or @($RequiredModules | Where-Object { $_ -notmatch '^[A-Za-z_][A-Za-z0-9_.]*$' }).Count) {
        throw 'RequiredModules must contain one or more valid Python module names.'
    }

    $venvPython = if ($IsWindows) {
        Join-Path $EnvironmentDirectory 'Scripts\python.exe'
    } else {
        Join-Path $EnvironmentDirectory 'bin/python'
    }
    $hasPrivatePython = Test-Path -LiteralPath $venvPython -PathType Leaf
    if (-not $hasPrivatePython) {
        $pythonCommand = Get-Command $Python -ErrorAction SilentlyContinue
        if (-not $pythonCommand) {
            throw "Python 3 is required to build Eagler Touhou Host. Install Python and ensure '$Python' is available on PATH."
        }
    }

    $resolvedPython = if ($hasPrivatePython) { $venvPython } else { $Python }
    try {
        & $resolvedPython -c 'import sys; raise SystemExit(0 if sys.version_info.major == 3 else 1)' 2>$null | Out-Null
    } catch {
        throw "Unable to run Python 3 using '$resolvedPython'. Install Python 3 and ensure the configured interpreter is usable."
    }
    if ($LASTEXITCODE -ne 0) {
        throw "Python 3 is required to build Eagler Touhou Host. Configured interpreter: $resolvedPython"
    }

    $importsReady = $false
    $importProbe = 'import ' + ($RequiredModules -join ', ')
    try {
        if ($RequirePrivateEnvironment -and -not (Test-Path -LiteralPath $venvPython -PathType Leaf)) {
            throw 'private environment not created'
        }
        & $resolvedPython -c $importProbe 2>$null | Out-Null
        $importsReady = ($LASTEXITCODE -eq 0)
    }
    catch {
        $importsReady = $false
    }

    if (-not $importsReady) {
        if (-not (Test-Path -LiteralPath $venvPython -PathType Leaf)) {
            & $Python -m venv $EnvironmentDirectory | Out-Host
            if ($LASTEXITCODE -ne 0) { throw 'Unable to create the private deployment Python environment.' }
            if (-not (Test-Path -LiteralPath $venvPython -PathType Leaf)) {
                throw "Python created no usable virtual environment interpreter at: $venvPython"
            }
        }
        $resolvedPython = $venvPython
        & $resolvedPython -m pip install --disable-pip-version-check -r $RequirementsPath | Out-Host
        if ($LASTEXITCODE -ne 0) { throw "Unable to install deployment Python requirements: $RequirementsPath" }
        & $resolvedPython -c $importProbe | Out-Null
        if ($LASTEXITCODE -ne 0) {
            throw "Private deployment Python cannot import required modules after dependency installation: $($RequiredModules -join ', ')"
        }
    }

    return $resolvedPython
}

Export-ModuleMember -Function Resolve-DeploymentPython
