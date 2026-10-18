# Feature-rebase structural audits

The scripts in this directory are historical migration audits from the TH07
multiplayer feature-rebase work. They intentionally inspect source text and
frozen upstream diffs to answer questions such as:

- was every upstream hunk classified during the rebase;
- did a required portable equivalent exist at that point in the migration;
- were native-only or unsupported slices deliberately excluded.

They are **not product behavior tests**. A PASS here does not prove gameplay,
rollback, rendering, audio, persistence, input, or browser behavior. Refactors
may legitimately invalidate their source-text expectations without causing a
product regression.

Current product acceptance belongs in executable C++ tests, protocol/relay
harnesses, browser smoke tests, and real build/runtime validation under
`tests/`. Do not add new implementation-string assertions here unless they are
needed to preserve a specific historical migration audit.

The original evidence and interpretation remain documented in
`docs/th07-sbrik-feature-rebase-ledger.md`.
