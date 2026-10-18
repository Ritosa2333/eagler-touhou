# TH09 selection menu rollback

The shared difficulty and stage menus combine P1 and P2 button input. Character
navigation and confirmation consume the two independent player input histories.

Menu animation and cursor changes use the ordinary rollback window. Menu
checkpoints include settings, RNG, input edges, menu state, animations, stage
choices and the animation executor state. Initial resource setup, match creation
and transport departure still require reconciled input. This prevents prediction
from replacing resources referenced by an earlier checkpoint.

Validation: `tests/multiplayer/in-game-selection-browser.mjs --relay --selection-only`
with `MENU_PACKET_DELAY_MS=60` runs actual Release worlds with delayed binary
WebSocket sends. It checks menu cadence, both shared controls, independent
characters, matching confirmed hashes, real corrections and launch from P2.
This focused mode does not claim natural match completion, public-network or
physical-device coverage. Keep its report separate from the full lifecycle run.
