# Eagler Touhou touch source attribution

Upstream repository: https://github.com/Ritosa2333/eagler-touhou
Fixed revision: 886952931e30e672253ea3abc8464d74ad662531.

`site/runtime/upstream-touch-input.mjs` is an exact copy of `ports/th12/src/touch-input.mjs` (SHA-256 d6d8a59e008e5ac015d1d71a82396b6a775854647bb7d7fe4612d093f95aaf7d). The TH12 directory's original MIT license is retained in `eagler-touhou-th12-MIT.txt`.

The launcher-derived `touch-layout-model.mjs`, `touch-layout-editor-state.mjs`, `touch-layout.mjs`, `touch-joystick.mjs`, `game-zoom.mjs`, and `full-touch.css` retain the upstream launcher's GNU GPL version 3 license in `eagler-touhou-launcher-GPL-3.0.txt`. The TH128 `full-touch.mjs` adapter combines these launcher modules. Original TypeScript types were erased, relative imports rebased, CSS restricted to touch rules and its player scope rebased; TH128 input and DOM bindings are adaptations. These launcher-derived modules are not represented as MIT code.

The corresponding adapted source is included in this repository under `site/runtime/`; original paths and source hashes are listed in `docs/TOUCH_UPSTREAM_REUSE.json`. Any replacement Runtime containing these modules must include this notice, both original license texts and corresponding adapted source. The original TH128 game resources are separate and were not imported from this repository or relicensed here.
