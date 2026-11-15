# Original replay compatibility evidence

run.mjs is the exact test source executed by case617 against immutable private100. That run FAILED at an early GameOver after4906 of23707 recorded inputs; it is not a passing regression or a complete compatibility claim. See ../../docs/original-replay617.md and its evidence JSON.

The test takes an explicit owned request containing the exact runtime, original resources, codec and authorized replay pins. Those external inputs, the user's replay, and local evidence are not distributed. The filename run.mjs is significant because the runner audits its own callable inventory.

Input verification only:

```text
node tests/original-replay-compatibility/run.mjs <owned-request.json> --verify-inputs
```

An actual run requires a fresh MAIN admission wrapper which verifies the real thread, complete manifest and exact command, obtains an exclusive once/ROOT lease, supplies --run --grant <grant.json>, captures all outputs, and releases the lease in finally. Do not point output at the frozen616/617 directories, rerun accepted403/444/560 merely for PASS, or rebuild successful cores.

The recorded input is used without invincibility, HP changes, RNG overrides or history edits. Audio output is synthetic journal ACK only. This script creates no browser, GPU, WebAudio device, IndexedDB store, normal-clear record or multiplayer proof.
