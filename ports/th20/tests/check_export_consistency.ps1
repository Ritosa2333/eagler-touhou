param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot)
)
$ErrorActionPreference = 'Stop'
$rawRoot = Join-Path $ProjectRoot 'scripts\recovered\ecl_raw'
$jsonRoot = Join-Path $ProjectRoot 'reports\ecl_json'
$rows = @()
foreach ($raw in Get-ChildItem -LiteralPath $rawRoot -Filter '*.ecl.txt' | Sort-Object Name) {
    $eclName = $raw.Name.Substring(0, $raw.Name.Length - 4)
    $doc = Get-Content -LiteralPath (Join-Path $jsonRoot ($eclName + '.json')) -Raw | ConvertFrom-Json
    $textLines = @(Get-Content -LiteralPath $raw.FullName)
    $sourceNames = @($textLines | ForEach-Object { if ($_ -match '^void ([^(]+)\(') { $Matches[1] } })
    $sourceOpcodes = @($textLines | ForEach-Object { if ($_ -match '^\s+ins_(\d+)\(') { [int]$Matches[1] } })
    $parsedNames = @($doc.subroutines | ForEach-Object { $_.name })
    $parsedOpcodes = @($doc.subroutines | ForEach-Object { $_.instructions | ForEach-Object { [int]$_.opcode } })
    $namesEqual = ($sourceNames -join "`0") -ceq ($parsedNames -join "`0")
    $opcodesEqual = ($sourceOpcodes -join ',') -ceq ($parsedOpcodes -join ',')
    $rows += [pscustomobject][ordered]@{
        file = $eclName
        subroutine_count = $sourceNames.Count
        instruction_count = $sourceOpcodes.Count
        ordered_subroutine_names_match = $namesEqual
        ordered_opcodes_match = $opcodesEqual
    }
    if (-not $namesEqual -or -not $opcodesEqual) { throw "Independent thtk/C++ comparison failed: $eclName" }
}
if ($rows.Count -eq 0) { throw 'No raw ECL dumps found for cross-check' }
$result = [ordered]@{
    schema = 'th20.cpp-thtk.crosscheck.v1'
    comparison = 'Independent C++ parser versus thtk raw decompilation: ordered names and opcode IDs'
    file_count = $rows.Count
    subroutine_count = ($rows | Measure-Object subroutine_count -Sum).Sum
    instruction_count = ($rows | Measure-Object instruction_count -Sum).Sum
    all_match = $true
    files = $rows
}
$result | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $ProjectRoot 'reports\cpp_thtk_crosscheck.json') -Encoding utf8
[pscustomobject]$result | Select-Object file_count,subroutine_count,instruction_count,all_match | Format-List
