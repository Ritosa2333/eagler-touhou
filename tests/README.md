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

## Host integration smoke tests

The browser/audio netplay smoke tests exercise the production Host-owned relay,
not a private server copy. Set:

```sh
TH_EAGLER_HOST_ROOT=/path/to/eagler-touhou
```

The checkout must contain `server/netplay-relay.mjs` and its Node dependencies.
The cross-game countdown audio test also requires:

```sh
TH_EAGLER_TH07_ROOT=/path/to/th07-eagler
```

pointing at a TH07 `eagler` checkout. These browser tests are integration
validation and are intentionally separate from the dependency-light core CI
suite above.
