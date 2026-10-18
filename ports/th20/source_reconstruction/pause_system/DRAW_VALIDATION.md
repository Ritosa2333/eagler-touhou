# Pause drawing CPU comparison

`draw_validation.json` records 81,920 passing comparisons from 8,192 cases.
The six complete original entries are 4e4d50, 4e5020, 4e5240, 4e5390,
4e54c0 and 4e5620; 4e67e0 is the complete text-state reset. Whole PauseInf,
Renderer, 25 Replay user headers, cached TextJob, PlayerTable and 64-animation
pool state are compared, including the main draw's actual integer return 1.

The test includes all replay rows, name editing, score ranking, background child
color, main-menu routing and forced stone-count hints. The forced hint found a
real selector mistake: original 464080 reads Context.current_player, which can
differ from PlayerTable.players[0]. `draw_first_failure.json` retains the 196
state mismatches before the source correction; the same cases now pass.

Original instructions are never patched. Both CRT date conversions run against
the same actual non-DST host timezone; original initialization data is supplied.
Positive valid timestamps are covered. DST and invalid-time CRT paths are not.
Text queues and a real existing zero-frame dynamic Job are compared; new GDI
jobs, asynchronous uploads, GPU pixels, file IO and complete owner lifecycle are
outside this drawing group. See the JSON for bound source hashes and reproducible
shared-oracle commands in `../card_system/CPU_VALIDATION.md`.
