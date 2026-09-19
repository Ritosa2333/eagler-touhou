# Product tests

For the 50 ms input-buffer and bounded touch-prediction investigation, see
`../docs/th07-mp-buffer-prediction-2026-09-16.md`. The focused browser runner
checks that requested delay/prediction settings actually reached the Runtime;
a canonical-state PASS is not a guarantee of smooth physical-phone play.

This directory is for executable or behavior-oriented validation: C++ netplay
tests, transport/relay harnesses, browser smoke tests, replay tests, and other
checks whose result is tied to observable program behavior.

Historical `*-feature-rebase-test.py` source-text checks are kept separately in
`audit/feature-rebase/`. They remain useful migration evidence, but they must
not be reported as product behavior coverage.

## MP performance and correctness

`python tests/run-netplay-performance-tests.py` runs native and WASM SAFE_HEAP
journal/core/window/budget tests. Assertions remain enabled.

`benchmark-rollback-journal.py` compiles a chosen Git baseline and current
journal with identical flags; `build-netplay-journal-baseline.py` creates a
separate Web A/B lane without checking out source or replacing the candidate.

`netplay-performance-browser.py` covers 2P/3P confirmed gameplay, analog input,
relay delay/jitter, RTC and single-endpoint CPU throttling. It records RAF and
observable simulation cadence separately and rejects canonical disagreement.
PASS means correctness/completion, not phone smoothness acceptance.

Commands, evidence and unresolved failures:
`../docs/th07-mp-performance-2026-09-16.md`.

Static source-contract checks that are not tied to the historical feature rebase live in `audit/source-contracts/`.

The fast, dependency-light C++ core suite is the CI entry point:

```sh
bash scripts/run-core-tests.sh
```

## Host integration dependencies

Relay/coturn/browser integration tests execute the canonical Host implementation
rather than a repository-private server copy. Set:

```sh
TH_EAGLER_HOST_ROOT=/path/to/eagler-touhou
```

The Host checkout must contain `server/netplay-relay.mjs`; Node-based relay
tests also require the Host Node dependencies to be installed. Cross-game
spectator smoke that exercises TH06 additionally requires:

```sh
TH_EAGLER_TH06_ROOT=/path/to/th06-eagler
```

No integration test should discover sibling checkouts from a fixed workspace
layout. These environment variables make external ownership explicit.
Cross-repository source-contract audits use the same variables.
