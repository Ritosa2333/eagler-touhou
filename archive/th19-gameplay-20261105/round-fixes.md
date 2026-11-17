# TH19 round and dense-spirit verification

Build WASM SHA256: `fefa3c11e2d1adc555a72ecfb55590ba22822abb2636e9f6ae40dc0b321341de`.

Story and VS difficulty menus retain Lunatic after character-selection cancellation and clamp at Easy/Lunatic. The production round initializer reduces both EX/Boss ranks by two, clamped to stored rank zero (displayed Lv1). Evidence: native 0x518067–0x518109. Actual shield contact cancels C3/C4 charging without dispatching an attack or restoring the shield; native player update 0x4e79e0 skips release when control state is invalid.

C3/C4 release starts the original Ecl_EtBreakLv3/4 scripts at the player position. Original opcode dispatch maps 615 and 616 to 0x503d9e. The native C++ ECL regression cancels 3/5 probe bullets respectively, respects the original animation timing and retires the controller.

ANM delete and end-of-script return termination from native 0x4bd660 (cases 1/-1), unlike stop or visibility changes. The browser VM now retires terminated child trees after their independently living descendants complete. A 10,000-iteration emitter regression verifies reclamation, hidden stopped children, and children surviving a deleted parent.

The prior dense-spirit run exhausted the 1 GiB WASM limit. The same six-cycle run now completes twice with matching hashes at every 120-frame checkpoint, after adding 1,200 original-script enhanced spirits per field over 720 frames. Heap allocation stabilizes at 799,932,416 bytes. The test runs simulation without presenting every frame; timings are not rendered FPS measurements.

Two real Launcher RTC peers also pass continuous injection and 720-frame synchronization. Sampled populations reach 636 enemy objects and 1,316 bullets per field; peak sampled heap is 895,221,760 bytes. Common frame hashes agree and no page errors occur. This verifies this pressure range, not unlimited populations or long-duration performance.

Evidence: [menu/combat/simulation](round-fixes.json), [two-peer RTC](round-rtc-pressure.json), [original cancellation ECL](charge-clear-ecl.txt), [original ANM dispatch](anm-lifetime-native.c).
