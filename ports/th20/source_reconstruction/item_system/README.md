# Item controller reconstructed source

`ItemInf` is the actual `0x49c89c` controller stored at game Context `+0x0c`.
Its 1,536 inline `0xc4c` items use a 512-slot ordinary pool and a 1,024-slot
special pool. Each item contains two real `sprite::Animation` objects. The
controller shares the existing scheduler, Session, RNG, timer, ANM, effects and
audio source modules. Production does not load or call the original executable.

`item.hpp` supplies `controller`, `create_controller`, `spawn`, `spawn_many`,
`retire`, `update` and `draw`. `rewards.hpp` contains the recovered score/power/
life/bomb increments and complete item pickup dispatch. Bullet cancellation and
damage rewards both use these same item pools; `entry_adapter.cpp` supplies the
existing damage creation boundary.

The original integer overflow, signed division, clamping side effects, RNG order,
recursive bonus spawn, delay bands and list order are retained. In particular:

- The inherited disable method disables only the two base callbacks; enabling
  the Item controller enables all three callbacks.
- A full-power large power item adds 20,000 twice in the original score units.
- Life-fragment conversion reads its threshold once, even when successive
  extends change the threshold index.
- The offscreen secondary animation writes its vertical coordinate to 8 before
  applying opacity, matching the original implementation.

`cpu_validation.json` records **287,824 passed, 0 failed** selected-function
hardware comparisons, bound to the source hashes by `record_evidence.py`:

| Scope | Checks |
|---|---:|
| Constructors, whole pool reset, ordinary/special spawn | 24,656 |
| Whole frame and draw, including complete pool/list bytes | 16,384 |
| Player reward increments and observer event order | 196,608 |
| Four score/power pickup bodies and overlay meter | 28,672 |
| Multi-item random scatter | 1,280 |
| All 17 pickup dispatch values inside the original frame | 3,264 |
| Special-item selection, RNG and script binding | 16,384 |
| Three callback registration, enable/disable and destruction | 576 |

The oracle executes only selected original functions. ANM execution/binding,
effect creation, HUD notifications, floating score, player option refresh and
special phase transition use recorded boundaries where stated in the report.
The corresponding production ANM/effect/audio calls use recovered source.
No whole-game equivalence is claimed. Invalid original indexes, division by
zero, allocation failure and the original ordinary-pool exhaustion null
dereference are outside the validated domain.

The remaining required production dependencies are explicit functions in
`rewards.hpp` and `collect.hpp`: HUD `4b8650/4b8bf0/4b90e0`,
player option refresh `4faca0`, special owners `51b960/513dd0`, and
phase transition `534d00`. None has a fake implementation in this module. The score glyph call `510710`
now resolves to `small_score/entry_adapter.cpp` and its recovered controller.

Build the static source library with the parent `build_sources.ps1`. To run the
standalone Win32 oracle:

```powershell
& D:\cmake\bin\cmake.exe -S source_reconstruction/item_system/oracle -B source_reconstruction/item_system/oracle/build -G "Visual Studio 16 2019" -A Win32
& D:\cmake\bin\cmake.exe --build source_reconstruction/item_system/oracle/build --config Release
& source_reconstruction/item_system/oracle/build/Release/th20_item_cpu_compare.exe $originalExe source_reconstruction/item_system/cpu_validation.json
python source_reconstruction/item_system/record_evidence.py
```

The executable path supplied as `$originalExe` must have SHA-256
`a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`.
