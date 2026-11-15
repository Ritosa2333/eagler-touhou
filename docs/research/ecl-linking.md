# Bounded ECL linkage and typed game-global integration

The additions in `EclProgram` and `EclVm` extend the earlier independent control
VM without assuming TH12 game state. The actual 16 stage roots link their ECLI
closures from loaded, validated resource names; first A1-1 has three sources.

`Program::linkIncludes(vector<pair<string,Program>>,string&)` requires an
unlinked loaded root and providers, rejects case-ambiguous provider identifiers,
missing names, cycles and duplicate subroutine names, and publishes only after
all bounded parsing/link checks pass. It performs no file I/O. Resource bytes
remain owned, existing Program/VM snapshots remain immutable, and sourceCount /
sourceBytes report linked resource identity and retained source sizes. Aggregate
bytes/subroutines/instructions retain the reader's existing hard budgets.

Depth is the longest dependency path, not merely the current recursion stack.
An independent review reproduced a completed-node shortcut that accepted a
65-edge graph in one root order but rejected its reverse. Cached subtree depths
now reject both, accept the 64-edge boundary and leave previous state intact.

`Vm::setReferenceResolver(ReferenceRead,ReferenceWrite,error)` installs explicit
providers outside an executing tick. Readers receive the exact signed reference
identifier and requested float/integer type; writers receive the exact bits and
ValueType. Negative -1 retains native expression-stack treatment instead of
being passed to a provider. Unknown references, invalid values and provider
declines are sticky faults. Reentrant VM changes fail, and enabled exceptions
become HostFailure rather than escaping a tick. No unknown value silently
defaults to zero. Logical parameter mask indexing remains distinct from raw
word offsets after variable-length strings.

These interfaces allow a host to implement verified game globals, not arbitrary
unrestricted native memory. StageWorld currently supplies only selected proven
float position/RNG paths and explicit difficulty; additional variables remain
unsupported. The source getter evidence lives in `ecl_globals_probe.py` and
private `local/research/ecl-globals.json`, with 27 bounded original-byte samples.

Checks: `tests/native/ecl-link.cpp` plus the include-linkage Node test cover
transactional failure, cycles, collisions, exact aggregate source bytes,
immutable snapshots, provider values/stores/declines/exceptions/reentrancy,
order-independent graph-depth boundaries, and every original stage closure.
The same fixture runs under UBSan using `TH128_ECL_SANITIZE=1`.

Collision rejection is an intentionally strict supported subset. This does not
claim that native duplicate-subroutine override order or every root/include
scheduling case has been recovered. Current actual corpus has no collision in
the required linked sets; unsupported future collisions stop explicitly.
