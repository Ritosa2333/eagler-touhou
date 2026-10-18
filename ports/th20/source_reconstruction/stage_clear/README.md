# StageClearInf

The actual `0x98` StageClearInf (`510980`) has source construction/destruction,
factory (`5115c0`), initialization (`511270`), frame (`510b60`) and draw (`510e80`)
implementations. Its only global storage is the recovered `5c6114` owner shared
with startup and SpecialState; no duplicate scene pointer is invented.

`bonus.cpp` preserves the eight mutating level/phase getters, the resource getter,
32-bit multiplication/addition overflow, signed remainder and zero extension into
the score adder. The original frame transitions, input thresholds, ten-frame
completion point, ANM deletion, music fade and callback priorities are retained.
The draw path uses the real text renderer and original CP932 strings/constants,
recorded by `extract_constants.py` after checking the original EXE hash.

The Win32 source library compiles. `pool_validation.json` now records 95,232 original-CPU checks: 1,024 constructor/destructor checks, 16,384 direct getter checks, 49,152 full frame checks and 28,672 draw checks. Source hashes are bound to the exact compiled shared oracle.

The frame cases compare actual bonus/score updates, game flags, registered ANM, input, timers and audio commands. The draw cases execute actual CP932 formatting and TextRenderer dynamic text against six existing zero-frame jobs. This proves that cache-hit path and whole-job state; it does not prove new-job GDI creation, asynchronous rendering or GPU pixels.

File loading/factory scheduling, owned-node teardown and the state-6 timer-10 completion/self-delete point are excluded. The latter has an explicit throwing fixture boundary; no fake success implementation is added to production. The complete game does not yet link independently.

Reproduce using `sprite_renderer/pool_test`, then `bullet_system/record_pool_evidence.py`. The recorder rejects any failed comparison or changed compiled input. Earlier fixture failures are retained: audio mutex slot 11 initially lacked its isolated native initialization, and the ANM environment initially returned false instead of reading the actual GameController freeze flag. Both setup issues were corrected without changing original instructions or expected results.
