# Product tests

This directory is for executable or behavior-oriented validation, including
the C++ netplay/input/rollback tests and browser smoke harnesses.

Static source-text checks are kept under `audit/source-contracts/`. They are
useful architecture/migration evidence but must not be reported as proof that
the corresponding gameplay behavior works.

The fast, dependency-light C++ core suite is the CI entry point:

```sh
bash scripts/run-core-tests.sh
```
