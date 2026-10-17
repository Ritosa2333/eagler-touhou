# EndingInf source reconstruction

The Win32 C++ module restores EndingInf at `5c49e8` and its message VM. This is
the ending scene dispatched by `4a1010`, previously mislabelled as a replay
scene in the platform boundary declarations. It contains no executable image
loader or original machine code.

`EndingInf` has its original 0x28-byte prefix. `Script` retains the entire
0xf0-byte layout, three independent timers, ten text/ruby ANM handles, four
loaded files, sixteen scene handles and the worker. `49f3d0` implements all
recognized opcodes 3..17, including timed/interactive waits, staff-file switching,
difficulty gates, text colors, music, screen fades and asynchronous ANM loading.
The original unknown/end opcode returns -1. Allocation, malformed resources and
invalid indices are outside the equivalence domain; checked array accesses can
throw instead of reproducing original out-of-bounds access.

Text tasks retain references to the real Script and captured Animation. Ruby
placement uses the original `|x,spacing,text` syntax. Completion runs interrupt 2
directly on that captured ANM, while line activation uses the manager wrapper.
Resource workers resolve the active ending controller when they execute.

`4a0600` handles gallery selection, original save metadata, all 23 file names and
the first-view flags. `4a0440` preserves the special return value 6 for credits
fast-forward, including the physical button slot 2 and twelve-frame cadence.

The source library compiles in Release/Win32. `pool_validation.json` records
**200,618 passed, zero failed** original CPU checks: 256 owner/Script constructor
and ten-ANM destructor cases; 12,288 complete VM/frame calls with real input,
timers, music commands and scene selection; 8,192 held-input cases; 1,024 pairs
of normal/ruby text tasks with GDI rasterization, exact upload pixels and real
ANM interrupt2 completion. Separate text uploads deliberately change Script
colors between scheduling and execution to validate captured references.
The recording-texture fixture excludes real GPU behavior, opcode7 asynchronous
resource IO, opcode12 credits-file replacement and complete initialization/file
unloading. These boundaries throw if entered by the fixture. This is scoped
module evidence, not full-game equivalence.
`extract_data.py` checks the specimen SHA-256 before regenerating the tables and
`evidence/data.json`. The original EXE is read only during evidence extraction.
