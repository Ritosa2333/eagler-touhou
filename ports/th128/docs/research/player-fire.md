# Player firing and charge dispatcher prefixes

This component ports original `0x43f1b0` head and `0x43f2e3` charge prefixes
using explicit player timers, inputs, manager busy state and system mode.
The diagnostic context is not a complete native initializer. Original EXE
bytes are read-only; no Windows game process or invented call return is used.

## Implemented Boundaries

- The state1 gate, negative shot-cycle start, native timer initialization and
  current-versus-previous emission check retain their actual order. An emitter
  request stops at `0x43f25d -> 0x43f160`; cycle completion is separate.
- Holding Z increments the independent `+0x43910` timer. Sound31 is required
  at count15, sound70 at count45; neither is silently discarded.
- Recharge uses the existing proven `FreezeRules` values and executes before
  its threshold sounds. Explicit zero callbacks are not inferred from SHT IDs.
- A release with count>=45, power>=3000 and mode0 resets the timer to-40 before
  mandatory `0x4101d0`. Early releases reset to0 before sound-stop operation31.
- Negative recovery increments without baseline recharge or pretending an ice
  manager exists. The busy/mode gates retain their original fallback behavior.
- The held branch stops before the native DirectSound positional update at
  `0x43f3dc`. The common presentation branch at `0x43f48b`, formatted text,
  charge-ring ANM and completed return are not implemented by these helpers.

## Verification

The report `local/research/player-fire-fixtures.json` includes34 head and39
charge original-byte cases with full explicit-memory before/after data, code
hashes and executed traces. A missing portable module was observed failing;
the73 prefixes match in ordinary and UBSan WASM. Timer clock1, integral bounded
counts and power0..10000 are the supported contract. This is not complete
keyboard shooting, freezing or a playable first stage.

All work after the user's execution override runs exclusively in the main
thread. No new agent delegation is permitted.
