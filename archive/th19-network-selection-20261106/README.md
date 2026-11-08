# TH19 network character selection

Each seat operates the native character grid independently before the match. The network selector makes all nineteen characters available through a temporary availability predicate; the score file and single-player progress are unchanged. Room difficulty and match rules remain room settings. The launcher hides its TH19 character controls through the TH19-only inGameCharacterSelection policy.

Selection uses separate ordered packets and frame counters. Both peers compare final character/card choices before resetting the match with its shared seed. Recorder and spectator match streams begin after that reset, so selection input never enters .rpy files. A session closed during selection releases the native network state.

Two real browser/WASM instances exchanged selection messages, including independent navigation, confirmation and cancellation. Both selected [13,9], excluded all 156 selection frames, recorded 182 match frames and replayed those frames through the state-hash check. Native progress diagnostics were unchanged. Transport in this test is an in-page ordered message queue; public network latency is outside this test. Earlier selection integration also passed a 366-frame replay. The screenshot shows all nineteen character portraits enabled.

## Selection music and network touch

Selection explicitly restarts th19_01 after match initialization. Unlimited network touch now consumes pointer displacement once, separately from the target used by local play. Sampling future inputs against an unchanged or rolled-back player position no longer repeats the same displacement.

Native tests cover queued samples, position changes during rollback, cancellation and blocked movement. The browser test verifies the title music path with a silent WAV fixture and samples eight P2 inputs before simulation: one (32, 9.6) displacement followed by seven zero displacements. P2 moves once; checkpoint replay produces the same position and hash. The 41 JavaScript regression tests pass.

## Selection scheduling

The selection phase now samples input at 60 Hz into a latency-aware bounded queue instead of filling four fixed slots at once. In a six-second simulated 200 ms RTT run, selection progression increases from 35.33 to 58.83 frames/second. Catch-up advances every ANM/game tick and presents the final available frame once.

A browser/WASM comparison of 120 selection ticks yields identical selection state and network hashes with 120 versus 30 presentation calls. Wall-clock values in the headless SwiftShader run include asynchronous graphics backpressure and are not an FPS benchmark. All 45 JavaScript checks pass.
