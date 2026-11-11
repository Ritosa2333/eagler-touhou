# 2026-11-07 Replay / result / staff feedback fixes

## Original behavior and fixes

- Replay: 00447cfd registers 00448e50 at Update 30. Physical Ctrl or Z requests an Update-chain restart until ReplayInf+0x210 is divisible by eight. The port now repeats its simulation pass before late ANM Update 34, including its application-owned stage-transition clocks; recorded gameplay input remains independent of physical menu input. Loading, pause, EOF and scene handoffs stop restarts. Physical pause edges are submitted once, including resume while Ctrl stays held.
- Staff: 00416190 registers the ending thunk at Update 31. 00416610 increments the frame counter before testing its modulo-12 restart cadence. The staff/reload flag, loading flag, first-good-ending gate and Z repeat-age >=20 gate remain separate from individual text waits. First viewing is not converted into an unconditional instant skip.
- Result preview: 00454050 sets stage 8 for its current score insertion. The preview now applies that mapping only in the clear-result title context and shares its slowdown calculation with the persisted score. 0043d7fc explicitly rounds the ratio to float before multiplication by 100. The sampling mask is 0x14 (00424990), not the unrelated player-update mask 0x70.
- The displayed percentage is **slowdown, not completion**. Full-speed play correctly reports **0.0%**. The original score-ranking stage table calls stage 8 **Clear**; the separate Replay route table calls it **All**. No completion percentage is fabricated.
- The read-only result diagnostic consumes `result_screen_data`, the production renderer's data source. The existing debug ending entry now retires a title scene when invoked from that scene; ordinary gameplay handoff is unchanged.

## Validation on the delivered source

All commands use the outer container entry points (`make exec` / `make shot`).

- Web/SDL/Emscripten build: exit 0.
- Complete UI suite, including new Replay fast-forward, ending fast-forward and result-record tests: exit 0.
- Combat suite: exit 0 with the asset-provider working directory explicitly set.
- ANM suite: exit 0.
- Replay suite: exit 0.
- Managed shell tests: 5/5 after removing a direct-touch command's dependency on a cached rectangle helper. The live command uses the actual canvas rectangle; no test expectation was changed.
- Real `th13.html` / production wasm / SwiftShader regression: 5,005 displayed ticks, exit 0. Ctrl advances Replay by eight after phase alignment; release returns to one; Z also accelerates; pause freezes input and resumes while Ctrl remains held. First ending viewing and the second staff sequence complete. AfterEnding reaches the result title screen; observed stage=8 and slowdown=0 for actual/budget=360/360.
- Independent review: result-context overmarking and resume-key-edge reuse were raised, repaired, and re-reviewed against the final diff and browser report.

## Evidence and limits

Private ignored outer artifacts retain the binary disassembly (`fast-forward-disasm-20261107.txt`), build/test logs and `feedback-browser-final-20261107/report.json`. Original assets, disassembly dumps, Replay files, screenshots and compiled binaries are not part of this change.

This is targeted behavior/production-browser regression evidence, **not full-game or independent framebuffer fidelity certification**. The browser ending coverage uses the existing diagnostic scene entry, not a newly played complete six-stage run. The original disassembly provides the cadence/field semantics; the browser run does not pretend to be an independent original pixel comparison. Shared adaptation checks could not all run locally because the development container lacks the required Bun 1.4.4. Existing upstream th19 artwork test failures are outside this port's ownership and are not relabeled as passing.
