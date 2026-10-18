# HitCtrlInf and collision source

This is genuine C++ source. `HitCtrlInf` is identified by the original diagnostic
string and has the exact 0xc460-byte x86 layout, with 256 inline 0xc4-byte regions.
Production code neither loads the original program nor embeds its instructions.

The module contains construction, registration, allocation, handle lookup,
rectangle/circle setup, expiry and motion, removal, shutdown, all five collision
shapes, damage aggregation, both player-shot hit callbacks, damage caps, score
addition and the counter that converts damage to item rewards. Bomb and Enemy
modules consume the same regions. Public APIs are `regions.hpp`, `geometry.hpp`
and `damage.hpp`. `state::Motion` and the process clock are shared source types.

The original shape policies are preserved, including sampled ellipse tests,
polygon edge iteration, mixed strict/inclusive boundaries, collinear segment
ordering, four independent damage groups and the original ordering of group
subtraction before duplicate-target rejection. Handle generation drops the
view-index bits after its first increment. Scheduler callbacks initially have
flags 1, meaning owned but disabled, and are enabled by their enclosing scene.

`cpu_validation.json` records **296,121 passing checks and zero differences**:

| Scope | Checks |
|---|---:|
| Region and controller layout, shape setup, allocation, retirement, motion, frame lists and heap fallback | 60,032 |
| Shared shape functions and both target types through original 4c1030 | 180,000 |
| 4,096 damage-aggregation scenarios: return, flags, full region pool, positions, enemy marks, score, reward counter and event order | 36,864 |
| Player-shot lookup and both hit callbacks, real pooled animation interruption and region retirement | 10,240 |
| Zero, signed zero, adjacent boundary floats, infinities and NaNs | 3,993 |
| Unsigned 64-bit score addition/division/clamp | 4,096 |
| Scheduler registration/node flags, actual callback, shutdown and list removal | 896 |

The test executable maps only selected original functions and does not call the
original entry point. It checks the original SHA before use. The aggregate
fixture substitutes the separate Item creation boundary and installs controlled
hit callbacks to compare event ordering. Separate tests execute both original
player-shot callbacks, including their custom callback ABI and default actual
animation/region behavior. The original Bomb path is tested with no active Bomb;
the production adapter calls the independently recovered Bomb implementation.
ANM interpreter execution is forbidden in this fixture; animation interrupt
state and pool lookup use their real recovered source implementations.

Pointer comparisons normalize only known relocation/allocation/vtable aliases.
No field values are normalized. Callback registration originally used the wrong
enabled flag in an intermediate implementation; the whole-node CPU comparison
caught it and the final implementation matches the original flags exactly.

The remaining declared game implementation is
`unrecovered::spawn_item_004c45b0`; its production definition now resides in `item_system/entry_adapter.cpp` and calls the recovered Item controller.
The production source implements reward accumulation and invokes that required
method; it does not substitute a successful result. Source for the full player
entity, player-shot creation and enclosing game scene is owned by other modules.
Full-game equivalence is not claimed. Invalid original domains, including
division by zero, invalid callback indices and heap-region use after retirement
inside the original damage-limit path, are excluded from validation. Allocation
failure behavior and arbitrary concurrent scheduling are also not compared.

Build the standalone static library with CMake and MSVC Win32. The isolated test:

```powershell
cmake -S source_reconstruction/damage_regions/oracle -B source_reconstruction/damage_regions/oracle/build -G 'Visual Studio 16 2019' -A Win32
cmake --build source_reconstruction/damage_regions/oracle/build --config Release
source_reconstruction/damage_regions/oracle/build/Release/th20_damage_cpu_compare.exe ORIGINAL.exe source_reconstruction/damage_regions/cpu_validation.json
python source_reconstruction/damage_regions/record_evidence.py
```
