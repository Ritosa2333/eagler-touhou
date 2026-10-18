# FrontInf and dialogue source reconstruction

This is editable Win32 C++ source. The production library does not call, map or
embed original executable instructions. It reads the original ANM/message assets.
It is not yet an independently linked game.

Implemented source includes:

* The actual `FrontInf` layout (`0x2d8`), construction, lifetime, resource loading,
  transition cleanup, callback activation, lives/bombs and notification animations.
* `4b2c90` frame processing: countdown, boss health rings and markers, pointer,
  proximity fading, dialogue updates and reward timers.
* `4b4730`, `4b5790` and `4f8840` HUD/Player feedback drawing, plus `4b4490` score
  interpolation. Integer operations preserve the original low-word/sign-extension
  choices rather than replacing them with a mathematical approximation.
* The nonvirtual `0x140` Dialogue, `4af310` construction and `4afc00` destruction,
  `4b0720` message VM, `4b9740` entry, text decode, text box geometry, portraits,
  queued text/ruby drawing, skip/wait input, music and stage-clear dispatch.
* `4a4a20` dialogue enemy cleanup and existing source Bullet/Laser cancellation.

Original message opcodes `3` and `30`, and values beyond `36`, take the original
default branch. Text clear completions were traced through the real std::function
vtables to `46ebb0 -> 46a4b0 -> 40e5e0`; their empty bodies are original behavior.
The text clear code enqueues real raster work. Text jobs retain the original
capture timing: `15/16` read the current script at worker execution; `17` owns a
decoded string snapshot.

`DRAWING.md` and `draw_validation.json` record actual CPU-comparison domains and
source hashes. Newly added dialogue, resource lifecycle and scoring code are not
automatically covered by those earlier reports. Dialogue clear still requires the
pending real Enemy defeat operation; stage completion and spell ending still
require the pending game/Pause paths. StageClearInf is provided by `stage_clear/`.

Invalid resource indices and text exceeding the original 256-byte decode buffer
are explicit errors. Such out-of-bounds behavior is outside the claimed valid
asset domain. Whole-game input, rendering, sound and replay equivalence remain
unverified until the source game actually links and runs.
