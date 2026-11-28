#!/usr/bin/env bash
set -u

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
mapping="$here/workflow-cases.json"
runner="$here/lib/native-runner.mjs"

[[ -f "$mapping" ]] || { echo "run-native.sh: missing rule-to-test case map: $mapping" >&2; exit 1; }
[[ -f "$runner" ]] || { echo "run-native.sh: missing runner: $runner" >&2; exit 1; }

args=()
while [[ $# -gt 0 ]]; do
    case "$1" in
        --through|--scope|--source-digest|--report|--case)
            [[ $# -ge 2 ]] || { echo "run-native.sh: $1 needs a value" >&2; exit 2; }
            args+=("$1" "$2")
            shift 2
            ;;
        *)
            echo "run-native.sh: unknown argument: $1" >&2
            exit 2
            ;;
    esac
done

exec node "$runner" "${args[@]}"
