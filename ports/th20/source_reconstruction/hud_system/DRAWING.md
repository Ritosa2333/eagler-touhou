The source drawing functions execute actual TextRenderer and Sprite operations:

| Original entry | Source | Verified behavior |
| --- | --- | --- |
| 4b4730 | `draw(FrontInf&)` | Score and resource rows; mutating range getters; spell time and encoded best time; popup retirement; Boss hint gates; complete queued text and render state; return 1 |
| 4f8840 | `draw_feedback(Feedback&)` | Timer restart and blink, signed hit count, hit/hits text, alpha, font, shadow and alignment |
| 4b5790 | `draw_player()` | Viewport 4 selection, camera matrices and Sprite offsets, then the actual feedback function; return 1 |

`draw_support.hpp` retains the exact TextRenderer field operations from 488940,
488a00, 488b00, 4b8600, 4b8620 and 4b8d90. The score formatter is the separately
recovered `TextRenderer::write_grouped_score` (46d020/453500), not a replacement
locale-formatting rule. The primary high-score call deliberately sign-extends
only the low 32 bits of the stored high score, as the original caller does.
Constants and original caller assembly are in `evidence/`.

`draw_validation.json` currently records 232,192 original-CPU checks. The original drawing group contains 58,112: 16,384
feedback checks; 20,480 primary numeric-row checks; 18,432 scene checks; and
2,816 player-wrapper/viewport checks. These include whole Feedback, FrontInf,
Player and Renderer storage, selected animation-pool state and return values.
The scene cases run the actual original CRT formatting functions without
patching original instructions. The viewport cases compare complete Graphics
state and COM matrix/viewport traces. This is queued text and state evidence,
not a GPU pixel comparison.

The original 4e15c0 getter clamps its index to 0..100, although its actual numeric
data span has 41 entries (31 normal and 10 adjacent Extra entries). The source
preserves all 41 values and raises `out_of_range` for a higher index instead of
reading unrelated adjacent native objects. Those invalid indices are not
claimed as equivalent. Spell/Pause/Enemy fixtures provide the actual accessed
storage fields; their full constructors and lifecycles are separate work.

Reproduce through `sprite_renderer/pool_test`: build the Win32 Release oracle,
run `th20_pool_cpu_compare.exe ORIGINAL.exe pool_cpu_validation.json`, then run
`bullet_system/record_pool_evidence.py`. The recorder refuses failures and any
source hash changed after compilation. Its CMake hash also records that the
production Session constructor/storage is compiled while its unused context
accessor is renamed for the shared oracle's mapped-context provider.

The additional 71,680 checks cover actual icon interrupts (16,384), score notifications with named-spawn and digit sprites (18,432), and HUD enable with callbacks, real pool allocation and mode/stage paths (36,864). `icons.cpp` is an unchanged extraction from `core.cpp`, permitting direct leaf linking without supplying a fake FrontInf destructor or resource lifecycle. Enable excludes Graphics+0xb18: the StoneMenu.open dependency throws if that excluded path is entered.

The frame group adds 36,864 checks over 4,096 calls to 4b2c90. It covers two real Enemy-list entries, health rings and marker geometry, countdown state, animation allocation/deletion, ticker state and the real SoundInf request queue. Active Dialogue remains excluded with explicit throwing fixture boundaries. The first run found and preserved the original marker scratch-vector quirk: 458fa0 only writes X/Y, so scratch Z accumulates the boss Z for each visible marker. `frame_first_failure.json` and its corrected rerun retain this evidence.

Another 65,536 checks over 32,768 calls to 4b4490 compare complete FrontInf and Session for score easing, unsigned 64-bit wrapping, signed-low-word comparisons and high-score flags. These are original integer-instruction comparisons, not a replacement score formula.
