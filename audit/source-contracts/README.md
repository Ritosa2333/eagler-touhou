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
