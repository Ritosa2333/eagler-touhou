#!/usr/bin/env bash
# th13-mp MP 工作流原生测试入口（M1/T1 测试基建）。
#
# CLI 与外层 scripts/mp_workflow_gate.py native() 完全一致（gate 以
# cwd=ports/th13-mp 调用本脚本）：
#   tests/multiplayer/run-native.sh --through M<n> --scope current|cumulative \
#       --source-digest <hex> --report <path> [--case <id> ...]
#
# 契约（外层 docs/plans/th13-mp-m1-m9-workflow.md「验证入口与输出契约」）：
#   - --case 只认 tests/multiplayer/workflow-cases.json 映射（无命名约定回退），
#     映射即唯一真值；条目 files 以 PORT=ports/th13-mp/ 为基（不带仓根前缀，
#     否则 gate/runner 的 (PORT/f).resolve() 前缀加倍判非文件）。
#   - 无 --case 时 current/cumulative 必需固定 case 未登记即非零退出并打印清单
#     （固定表 = gate:21-31 NATIVE 的只读镜像，见 lib/native-runner.mjs）。
#   - 报告 schema th13/mp-workflow-test/v1；case 只在真实编译运行并断言通过后
#     标 passed；任一失败整体非零。
#   - 退出码取直接子进程：本脚本 exec node lib/native-runner.mjs（同一 PID，
#     编译/链接/用例子进程失败原样上抛，不经管道筛选）。
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
