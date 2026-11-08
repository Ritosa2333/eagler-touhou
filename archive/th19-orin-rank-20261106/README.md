# Orin EX attack rank source

Original executable SHA256: `8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9`.

`pl07.ecl: ExAttackFunc` reads -9960, the shared rank divided by 200 (0x4fcc9f, 0x40ef40). Below 0.1 its delay is 10000; otherwise the delay is int(120 - 60 * rank/200). It makes six attempts, each emitting five type-12/color-2 bullets if host Y < 330. The script snapshots rank at entry.

GlobalRank follows the difficulty/stage table at 0x5c5f90 and initialization at 0x518488. Controller 0x51afe0 advances the timer at +0x24 before checking its integer value at +0x28 modulo the interval. The VS late-game branch starts at 10800 ticks, changes the interval to 240, and uses the executable's actual cap branch. Both fields receive the same source; application checkpoints include its clock and values.

Validation: `tests/orin-rank.mjs <original pl07.ecl>` checks all initial difficulty modes, periodic growth, VS cap change, and executes the original firing script at ranks 0/19/20/30/50 at Y=329/330. The emission test supplies an empty animation registry to isolate ECL emissions; visual assets are exercised by the browser runtime. No original game process was started.

Local verification: 41 JavaScript tests passed; gameplay browser regression passed; the Orin actor preview produced identical network hashes after checkpoint restore (1374370215). Desktop normal/multiplayer publish WASM SHA256 d216b1455c57c7f7a7341ab651981e0db6ae9dfa178ad145001d2dc5a1b0eaa9. The preview screenshot is an isolated actor view, not proof of firing at every position.

