function Get-EaglerWorkspaceLayout {
    [CmdletBinding()]
    param([Parameter(Mandatory)] [string] $ProjectRoot)

    $project = (Resolve-Path -LiteralPath $ProjectRoot).Path
    $configPath = Join-Path $project 'config\workspace.json'
    $config = Get-Content -LiteralPath $configPath -Raw | ConvertFrom-Json
    if ($config.schema -ne 'eagler-touhou/workspace-layout/1' -or -not $config.repositories) {
        throw "Invalid workspace layout: $configPath"
    }
    $repositoryProperties = @($config.repositories.PSObject.Properties)
    if ($repositoryProperties.Count -eq 0) { throw "Workspace layout has no repositories: $configPath" }
    foreach ($property in $repositoryProperties) {
        $name = [string]$property.Name
        $value = [string]$property.Value
        $segments = @($value -split '/')
        if (-not $name -or $name -notmatch '^[A-Za-z0-9_-]+$' -or -not $value -or
            $value -match '\\' -or $value -match '^[A-Za-z]:' -or [IO.Path]::IsPathRooted($value) -or
            @($segments | Where-Object { -not $_ -or $_ -in @('.', '..') }).Count -gt 0) {
            throw "Invalid workspace repository '$name' in $configPath"
        }
    }
    foreach ($required in @('launcher', 'thprac', 'dependencies', 'toolchains')) {
        if (-not $config.repositories.PSObject.Properties[$required]) {
            throw "Missing workspace repository '$required' in $configPath"
        }
    }
    $root = if ($env:EAGLER_WORKSPACE_ROOT) {
        [IO.Path]::GetFullPath($env:EAGLER_WORKSPACE_ROOT)
    } else {
        Split-Path $project -Parent
    }
    [pscustomobject]@{
        ProjectRoot = $project
        Root = $root
        Repositories = $config.repositories
    }
}

function Get-EaglerWorkspacePath {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)] $Layout,
        [Parameter(Mandatory)] [string] $Repository,
        [string] $RelativePath = ''
    )
    if ($Repository -eq 'launcher') {
        $base = $Layout.ProjectRoot
    } else {
        $directory = [string]$Layout.Repositories.$Repository
        if (-not $directory) { throw "Unknown workspace repository: $Repository" }
        $base = Join-Path $Layout.Root $directory
    }
    if ($RelativePath) { return Join-Path $base $RelativePath }
    return $base
}

Export-ModuleMember -Function Get-EaglerWorkspaceLayout, Get-EaglerWorkspacePath
