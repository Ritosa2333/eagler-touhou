# TH128 native stage resources

The pinned Japanese EXE stores a 40-byte stage record table at `0x4b1a30`.
`tools/research/stage_table.py` reads its actual pointers, validates each public
stage ID 1–16, retains raw words and requires every ANM/ECL/MSG/STD reference
to exist in the prepared archive. The generated private report lives at
`local/research/stage-table.json`.

This prevents an incorrect filename-based mapping: `st_a1_2.ecl` actually uses
`stage_b2.std`; `st_a1_3.ecl` uses `stage_c3.std`. The resource prefixes are not
interchangeable with route labels. The two music references in each record
also come from original pointers, not a guessed track order. Index zero is a
debug record with resources from a different title and is not a playable
TH128 stage; it is excluded.

Absolute references to the table base are present at `0x4247da`, `0x4259f3`,
`0x427408`, `0x435e5d`, `0x445840`, `0x44a37a`, and `0x44bc2e`. These are
static evidence, not a recovered complete call graph. Three trailing words
remain uninterpreted, and this does not establish actual route-branch logic,
dialogue sequencing, music command timing or complete gameplay.
