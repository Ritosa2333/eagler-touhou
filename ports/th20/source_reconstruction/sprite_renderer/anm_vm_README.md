# ANM execution source

`anm_vm.cpp` restores the 161 explicit cases of original `0x42b5d0`, plus the no-op / interrupt-label cases routed through its default branch. The original invalid-opcode branch advances by the encoded instruction size; no known opcode is replaced with an empty handler. Return values are recovered from assembly: 1 requests deletion, 0 continues or suspends.

The same source implements variable reads/destinations, timers, wait/jump/interrupt control, all arithmetic and render-state opcodes, interpolation setup, six interpolation samplers, motion, parent position/rotation/offset propagation, and all seven procedural vertex generators from `0x435c80`. Child creation, effect creation, sprite binding, camera state and allocation calls remain explicit `anm_environment` declarations. Their call sites are recovered; this module is not an independently linked game.

`anm_vm_cpu_validation.json` binds checks to compiled source hashes. The isolated oracle executes original CPU code only in its test executable. It compares whole animation objects, mutable script bytes, RNG state, return values, interpolation storage/results and generated vertex bytes. It also checks repeated frames, interrupt labels and parent chains. It does not validate all CRT exceptional paths, every camera/domain integration, or full rendered gameplay.

Reproduce with MSVC Win32:

```powershell
cmake -S source_reconstruction/sprite_renderer/anm_vm_test -B source_reconstruction/sprite_renderer/anm_vm_test/build -G "Visual Studio 16 2019" -A Win32
cmake --build source_reconstruction/sprite_renderer/anm_vm_test/build --config Release
source_reconstruction/sprite_renderer/anm_vm_test/build/Release/th20_anm_vm_cpu_compare.exe VERIFIED_TH20.exe source_reconstruction/sprite_renderer/anm_vm_cpu_validation.json
python source_reconstruction/sprite_renderer/anm_vm_evidence.py
```

`anm_vm_opcode_table.json` records each original case address, actual recovered ANM script occurrences, direct CPU coverage and remaining domain calls. Occurrences measure static resource use, not successful gameplay execution.
