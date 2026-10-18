# TH15 repair evidence (2026-10-06)

Source owner: `th15-eagler`, branch `eagler`, base d6ea682. Shared Launcher main was updated to f5c2855 with pre-existing TH20 edits preserved. `th15/main` is the untouched original tracking checkout. The former scratch repair worktree is a backup, not a Launcher source.

The user selected preserving original atlas glyph shapes. Physical raster alignment and integer-scale sampling improve clarity without inventing glyph detail. GPU capture destinations are promoted to the current render scale only when used; CPU font atlases retain their authored dimensions and dirty-region uploads.

Initial, game-entry, stage, manual and ending loading requests publish the original prayer artwork before resource work. Startup signature uses its authored resolution scaling once. Stage Clear completes its outgoing 120-frame display while player motion stays active before constructing the next stage.

Pointdevice compression uses bounded incremental LZSS work. The plain nine-section container and encoded bytes remain compatible. Save/scene-exit boundaries finish pending work; completed-run checkpoint deletion cancels pending work. No-music retry seek is a no-op.

Evidence collected using original private th15.dat and privately measured Windows GDI fonts:

- 220 original/incremental codec byte comparisons and round trips (sizes through 262144; chunk budgets 1, 3, 17, 4096, 131072).
- Chromium 1280x960 loading, Replay, Player Data and Stage Clear movement regression.
- All six ordinary stages: exact IDBFS cold-load bytes, native ContinuePrompt resume, retry metadata and subsequent gameplay. Pending checkpoint completion/deletion is also covered.
- Ordinary and Extra completion, ending/staff, results and complete Replay file writing. Controlled completion/protection isolate lifecycle rather than proving natural full runs.
- A protected 3000-tick real first-stage progression comparison measured desktop Chromium/SwiftShader CPU submission. Baseline peak 22.35 ms; incremental version peak 3.97 ms. This predates the final lazy capture-target allocation improvement. Median and percentile values and raw samples are in ignored artifacts. This is not a phone GPU, thermal, audio or stable-60-FPS certification.

Tests accept TH15_ORIGINAL_DAT. Private fonts are expected in assets/sdl-native/fonts. SDK: TH15_EMSDK=<workspace>/th08/tools/emsdk, pinned 6.0.9.

High-refresh is the final topic. It must preserve fixed 60 Hz simulation, input and Replay timing. High-refresh presentation does not imply Presentation Lab admission or motion interpolation support. Production packaging must reject development probes and keep retail data/music out of the Runtime.
