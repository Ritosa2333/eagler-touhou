# SmallScoreInf reconstructed source

The original diagnostic identifies `SmallScoreInf`; allocation `50fd00` is
`0xf98` bytes and the game Context stores it at `+0x1c`. It contains one real
Animation and two arrays of eighteen `0x44` entries. Update/draw process the
first array as thirteen numeric entries and five BONUS messages. `510710`
cycles through its first ten numeric entries.

`score.hpp` exposes the actual layout, `create_controller`, `controller`,
`spawn`, `update` and `draw`. The production environment uses the recovered
TextRenderer, SpriteController, scheduler and graphics fog controls. The two
entry adapters connect gameplay creation and Item floating-score generation.
This module has no unimplemented gameplay dependency of its own.

The score digits, negative-value glyph, residual entry bytes, rising motion,
strict lifetime threshold, BONUS alpha decay, near-player opacity and staged
digit sprites follow the original. `draw` retains the original `8 / time`
spacing expression, including its zero-time floating-point behavior.

The standalone Win32 oracle passed **46,208 checks, zero failures**:

- 4,096 entry constructors and 128 complete controller constructors, preserving
  the original untouched padding.
- 4,096 numeric spawn cases with complete controller comparisons.
- 4,096 update/draw frames: object bytes, return values, every glyph and BONUS
  text argument, and all text renderer setting changes.
- 128 initialization, callback registration and destruction cases.

Sprite initialization/selection, descriptor lookup, drawing, text commit and
fog are observed boundaries in this module test. Their production implementations
are existing source modules. This report does not claim rendered whole-game
equivalence or test corrupt indexes and allocation failure. The oracle never
starts the original program.

`record_evidence.py` binds the 17 original function ranges and source hashes to
`cpu_validation.json` and `module_status.json`. Build production with the parent
`build_sources.ps1`, or run the standalone oracle:

```powershell
& D:\cmake\bin\cmake.exe -S source_reconstruction/small_score/oracle -B source_reconstruction/small_score/oracle/build -G "Visual Studio 16 2019" -A Win32
& D:\cmake\bin\cmake.exe --build source_reconstruction/small_score/oracle/build --config Release
& source_reconstruction/small_score/oracle/build/Release/th20_small_score_cpu_compare.exe $originalExe source_reconstruction/small_score/cpu_validation.json
python source_reconstruction/small_score/record_evidence.py
```
