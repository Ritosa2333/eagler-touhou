# TH15 deferred GPU commands

The multiplayer rollback owner alone enables this experimental queue. Ordinary,
pure-delay and read-only playback retain immediate terminal submission.
The queue stores only already-authored GPU commands. Native
Calculation and Draw callbacks still execute in full, including VM, parent and
RNG mutations. The queue owns vertex/index/instance bytes and numeric resource
handles; it retains no game pointers. Submitted byte storage is bounded to eight
MiB and 4,096 commands before a forced drain.

A backward dependency pass can remove a draw only when every possible color
and depth write lies inside a later clear, before any retained texture, blend or
depth read. Partial clears are not promoted to full-surface coverage. Texture
reads conservatively cover the complete source. Unknown or incompatible inputs
fall back to immediate execution. Clear operations themselves are never removed.

First/stale texture uploads and dither-state transitions remain immediate.
Copies, readback, resource retirement, utility rendering, ImGui and host
presentation drain queued work. Multiplayer undo drains submitted work after
canceling the unsubmitted CPU batch and before restoring game-owned texture
bytes. Canceling presentation alone does not invent a GPU rollback.

The model tests compile the production queue and table from both ports and use
independent terminal state-grid semantics. They cover partial rectangles,
texture/depth dependencies, first uploads, CPU batch cancellation, command
limits, resource scale and dither dependencies. They do not implement GLES and
are not framebuffer, whole-game performance, or 60 Hz evidence.

The separate frozen-b7 browser reference remains mandatory: it compares the
candidate to an independent old-renderer Wasm, retaining exact framebuffer,
trajectory and Replay checks across all seven cases. The fixed twelve-run
performance matrix and all input/persistence proof requirements are unchanged.
No software-rendered result is Intel UHD hardware acceptance.
