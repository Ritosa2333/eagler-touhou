# Source-contract audits

These scripts are structural migration/architecture audits. They read project
source text and check that particular ownership boundaries, build switches, or
ported code shapes still exist.

They are **not product behavior tests**. A PASS does not prove multiplayer,
rollback, replay, rendering, persistence, input, or browser behavior. Source
refactoring can also invalidate an audit without changing observable behavior.

Executable C++ tests, transport/browser harnesses, smoke tests, and actual
build/runtime validation belong under `tests/` and are the acceptance evidence
for product behavior.

Keep new source-string checks here only when a structural audit is genuinely
needed. Do not assert comments, whitespace, or incidental formatting.

## Maintainer entry points

The following scripts are static proof/audit tools, not behavior tests:

- `thprac-source-contract.mjs` and `thprac-upstream-hooks.mjs`
- `thcrap-source-contract.mjs` and `thcrap-proof-ledger.mjs`
- `eagler-time-stop-contract.mjs`
- `surface-copy-contract.mjs`

Run them only when the corresponding source ownership/porting contract is being audited. A green result is structural evidence, not gameplay acceptance.
