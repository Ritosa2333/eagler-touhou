# Game frame and activation

`frame.cpp` implements `4ba4a0` and `4ba870`. `activation.cpp` implements `4ba940`
and `4bbf90` using source-owned subsystems. This includes first-frame and frame30
activation order, retained-background fading, initial enemy script spawn, source
ANM updates, score smoothing, demo timing, delayed results and source music calls.
`4ba940` returns1 on the loading-error path and0 normally; these returns are
present in machine code but omitted by the decompiler's void declaration.

`entry_adapter.cpp` now dispatches all14 named cleanup operations to their real
source owners, with source `CallbackOwner` virtual destruction for the Context
SmallScore/Spell slots. The Spell factory and Pause/Replay owners are still
required; these calls do not supply replacement objects for missing factories.
Known HUD, StoneMenu and StageClear owners are read from their actual storage.

`bullet_system/stage_reset.cpp` provides `483ad0`, including original raw pool
zeroing and free-list reconstruction. The original operation zeroes shared_ptr
storage without releasing an old reference. It is not replaced by ordinary
entity destruction, which would introduce different callbacks/side effects.
The supported source ABI is MSVC Win32, where the null representation is verified
by the existing Bullet storage assertions; live-metadata leaks and this reset's
full CPU behavior still require dedicated characterization.

The new source files compile, but these frame paths have not yet received a full
original-CPU or end-to-end run. Player `4fb450` and replay-finish `4e5f30` remain
explicit dependencies while their owners are being reconstructed. The diagnostic
`link_probe` target reports unresolved production symbols without supplying stubs.
