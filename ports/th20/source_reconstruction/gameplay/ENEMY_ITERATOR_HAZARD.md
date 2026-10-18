# Enemy retirement and nested lookup hazard

The isolated original-CPU composition probe in `enemy_cpu_compare.cpp` produced
`enemy_iterator_hazard.json`: 13 of 20 scenarios wrote the observer word in a
retired Enemy slot. Source and original behavior matched in all 20 scenarios.
The complete refreshed Enemy report passes 1,254,134 checks with no failures.
Matching this hazard is evidence about the binary, not a memory-safety pass.

The probe calls unchanged original functions in this order:

1. `412280` constructs an outer iterator directly over the controller list.
2. `498a80` runs an inner Enemy ID lookup.
3. `411ce0` unlinks the outer iterator's current Enemy link.
4. `411c30` advances the outer iterator.
5. `411b00` destroys the iterator.

The inner lookup overwrites each visited link's single observer pointer and
clears it on advance/destruction. It does not restore the outer observer.
Unlinking the outer current link can therefore no longer clear that outer
iterator's `current` pointer. Advance writes zero to `current->iterator`.
`Link` starts at Enemy+0x74, so that write targets Enemy+0x84.

The probe retains the retired storage and puts `0xa5a5a5a5` in its observer
word after unlink. This makes the subsequent write directly observable while
avoiding a real use-after-free in the diagnostic itself. It checks four outer
positions against lookup identifiers 0, 1, 2, 4, and missing 99. ID 0 returns
without an inner iterator and provides a negative control. A search that ends
before reaching the outer current/next observers provides another control.

Actual original `4a5040` calls `4a2720` to destroy/free the Enemy before calling
`411c30`. `4a3b10` and its helper `4a55d0` unlink the controller link during
destruction. Consequently the tested composition becomes a freed-memory write
when that outer loop and an inner lookup occur together. No original function
or dependency endpoint was patched for this probe.

The ordinary source EXE's `diagnostics/third_run/debugger.log` reports a write
at heap-block offset 0x8c. The subsequent actual source-game ASAN report
`diagnostics/asan_interception_probe2/asan.25132` confirms the first invalid
WRITE4 at Enemy+0x84 in `Iterator::advance`, called by
`update_enemy_controller` after `retire_entity` frees that same Enemy.
The diagnostic runtime required explicit continuation of its announced
Windows interception failure. Its positive report establishes this bug;
the run is not a complete sanitizer validation.

Disassembly evidence for the above functions is saved in
`iterator_hazard_evidence/`, extracted from the SHA-256-verified original
`a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`.

Production `update_enemy_controller` and `clear_entities` now clear the outer
iterator's current pointer immediately before retirement. The cached next
link and visit/deletion order remain intact; the generic scheduler and its
original single-observer ABI are unchanged. This intentionally removes the
original freed-storage write, which has no defined C++ behavior.

`enemy_frame_tests.cpp` exercises both production loops with 256 deletion
masks, with and without nested queries during update and destruction (512
cases). Each test Entity's destructor unlinks it, then its test deallocator
marks the entire retained page PAGE_NOACCESS. Any subsequent iterator access
faults immediately, independently of heap reuse. All 7,682 frame assertions
pass, including visit/delete order and final cleanup. The fixture's entity
body/deallocator is an explicit test boundary, not a substitute in the game.
`frame_validation.json` records this scope and source hashes.

The repaired ASAN source executable (PID 27092) has run actual stage 1 and
stage 3 demos past the previous failure, with stage 3 frame 2613 observed.
Evidence is in `diagnostics/asan_after_enemy_fix/`; its same interception
limitation remains. This does not establish complete-game equivalence.
