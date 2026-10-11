# React launcher maintainer lane

The default frontend remains `main`. Select `react` explicitly when serving or packaging the built rewrite. Both frontends use the same runtime protocols, package storage and multiplayer services.

## Development and verification

Use Node.js 24 with dependencies installed using `npm ci --ignore-scripts`. Run:

```sh
npm run typecheck:ui-rewrite
npm run typecheck:ui-rewrite:tools
npm run test:ui-rewrite
npm run build:ui-rewrite
EAGLER_FRONTEND=react npm start
```

On PowerShell, set `$env:EAGLER_FRONTEND='react'` before `npm start`. For a launcher checkout below `worktrees`, also set `EAGLER_WORKSPACE_ROOT` to the actual workspace root. Development serving uses a root-mounted artifact.

`EAGLER_REACT_BUILD_DIRECTORY` selects the build output directory, defaulting to `.cache/build/ui-rewrite`. `EAGLER_REACT_MOUNT_PATH` selects the build-time mount, defaulting to `/`. Build and host selection must use the same mount. A nested artifact belongs on a matching mounted static host. An existing built artifact is consumed without invoking the compiler during host assembly.

Offline support is an explicit deployment choice. Set both `EAGLER_REACT_APP_SHELL=isolated` and `EAGLER_REACT_APP_SHELL_ORIGIN` to the exact intended origin before building. Setting only a mount does not enable a worker. The built metadata binds the worker to that origin and mount, and rejects foreign or ancestor registrations. Rebuild to change this identity. `refresh:deployment-frontend` requires explicit React selection and a matching artifact.

## Components and return policy

`LibraryCards` renders the same card artwork, titles, selection motion and responsive dimensions in the launcher and directory. The directory adds its room list and selects a product filter before activating options. Page-specific card markup, sizing, typography and selection colors are not separate variants.

`useLibraryOptionsPresence` owns options entrance and exit in both contexts. The directory's native dialog provides focus and input isolation, follows the shared visual lifetime, and has no separate animation, image-decode wait or close timer. Both contexts use the same desktop entrance and the same full-height mobile slide with a 240 ms close lifetime.

`SettingsBody` and `OptionsPanel` are shared. Multiplayer adds room controls and visibility settings to the established single-player settings layout. Multiplayer restrictions leave the corresponding control visible and disabled with a reason. Common row spacing, scrolling and responsive header dimensions follow the single-player baseline. Room drawers move the existing header and settings carrier rather than cloning their state.

`page-history.ts` declares fixed page parents: launcher is the root, lobby returns to launcher, single-player options return to launcher, and multiplayer options and rooms return to lobby. Direct invitations seed those same browser history parents. Page buttons and browser Back use the same navigation owner and save/leave cleanup. History records retain bounded ancestor positions, not a separate route stack or inferred entry source.

`InformationalDialog` owns the native modal shell and `useMainDialog` lifecycle for Apple refresh help, donations and multiplayer rules. Callers supply body content and one explicit layout: standard, artwork-width or scrollable. All three use the same completion-driven Web Animations adapter, including interruption and reduced-motion handling. Native animation completion closes the dialog; there is no mirrored CSS duration, fallback close timer or separate presence owner. Unsupported/partial animation APIs degrade to immediate presentation and closure. Donation's immediate-close mode and unavailable-artwork action use that same lifecycle. The shared motion replaces the older per-dialog timings with one whole-dialog fade/translate; backdrop colors remain static until native close. This intentional visual change still needs browser review. Other numeric `useMainDialog` callers retain their established timing. The colocated stylesheet is React's sole shell styling owner; old Apple/guide selectors in `public/styles.css` remain unchanged for canonical main, and no React shell matches them.

Attached dialogs and the touch editor return to their opener. Gameplay has one Back layer over its options or room. Save failure retains the existing Retry/Leave/Stay decision. The persistent iframe follows main's validated `src` assignment/removal, rather than replacing its Window history. After save consent, Player close exits fullscreen and waits for the blank iframe document to commit before completing navigation. The host reattaches that same iframe node to discard the retired child browsing context and its history before another launch. A completed game replaces its active entry with its options or the same room, so it does not traverse an earlier game iframe snapshot. Room departure replaces the address with the selected game's lobby, including after refresh and direct invitations. An explicit native Return to Menu leaves membership after closing the Player. TH09's native title room remains an attached overlay of its retained runtime.

Synthetic mounted tests cover routing, native message boundaries and ownership. They do not establish actual game, mobile-device or durable browser-storage acceptance. Real browser and native runtime gates remain required before replacing the production frontend.

## Pre-merge record — 2026-10-11

Status: **HOLD**. Synchronization is committed, but this record does not authorize
merging into main, switching the default frontend, or publishing/deploying.

The source candidate is `ace43d76c3a019308ba102eb25927f59def231ca`.
Local main is `28bda8ca502b3d643eed3a5b705eb38d15769ad0`; fetched origin/main is
`edee9633e5e3ee79cd2e1aa334f84f6caf755090`. Both are ancestors of the candidate.
The local main commits added TH11 multiplayer and native Return-to-Menu room
departure. Merge `b5a42505f83b5cce10893402f5428c8d998bad34` retains both histories,
including origin/main's editable nicknames. The sole content conflict was the
rule renderer: its shared extraction was retained and the TH11 tab added there.
Main's catalog, room relay, spectator protocol, runtime/build configuration and
authored rule document are unchanged by the rewrite synchronization.

Preparatory changes and their purposes:

- `28025f7`: checkpoint the previously browser-verified runtime retirement,
  fullscreen/save order, fixed room/lobby returns and native browser API bindings.
- `b5a4250`: port main's no-rollback declaration into React's room control,
  mutation guard, start packet, snapshot validation and runtime options. Zero
  prediction remains zero. Port native stopped-game diagnostics and an actionable
  return for players and spectators. Rebase parity fixtures on committed main
  inputs while preserving the separately accepted editable-name behavior.
- `cd6d2e4`: give the new TH11 browser test an explicit package command so it is
  owned by a discoverable acceptance lane.
- `4d6febd`: fetch both committed parity authorities in shallow CI checkouts.
- `225ca47`: wait for all lobby close handshakes before asserting reuse of room
  quota. Hiding a directory row does not establish that the room was deleted.
- `ace43d7`: extend notice/controller and keyboard-tab parity to main's TH11 rules.

| Gate | Evidence and result |
| --- | --- |
| UI and tools type checks | PASS, Node 24.13 on Windows. |
| Main module compilation and generated content | PASS, 69 authored TypeScript modules and content freshness check. |
| React production build | PASS. Default frontend remains main; React and isolated App Shell remain explicit selections. |
| Full React synthetic suite | 1203 cases executed: 1200 passed, 3 stale four-tab oracle assertions failed. Those fixtures were updated and the complete affected notice/guide set passed (97 cases). A fresh green whole-suite CI result is still required. |
| Notice/guide final regression | PASS, 97 tests after updating the committed main oracle and five-tab traversal. |
| Site notice | PASS, 36 cases against its unchanged origin/main authority. |
| Repository core | 120 scripts executed. Initial run: 118 passed, room-quota race and Windows symlink failure. The quota fixture was corrected and passed; the other 118 scripts were unchanged. Atomic deployment remains blocked by EPERM, not skipped. |
| Publication audit | PASS for tracked source candidates. This does not verify a public deployment. |
| Linux CI | NOT RUN for this unpushed candidate. Must pass the normal core and rewrite jobs before promotion. |

Local logs and exact changed-file inventory are retained under `.cache/premerge-*`.
They are diagnostic evidence, not distributable release artifacts. The unrelated
pre-existing `design-qa.md` edit is preserved and excluded from these commits.

Browser evidence uses Windows Chrome 155 on two localhost origins, separate
members, a private room, the actual game Runtime and the shared public signaling
relay. TH11 entered Stage 1 with streaming OGG, advancing confirmed input and
RTC direct channels. The real Module received mode 1, prediction 0, and P2
`{character:1, shot:1}` for Marisa B. The native pause menu contained four entries.
The same page launched successive Runtime epochs 1 and 4. Menu Restart reached
fresh calibration and reset the confirmed-frame counter; complete gameplay-state
reset is not established by that observation. Manual error recovery returned to
the same room, and room departure returned to `lobby.html?game=th11mp` on both
clients. Test memberships were released and temporary tabs/services closed.

The frozen TH11 Runtime generation used for this test is
`4c629ac48793a981cc74c6cc8ba2ca20496982ee7f1b2fedd656b2cb99cc2d6c`.
Observed native pause Return-to-Title behavior: P2 retired into the room, while
P1 stopped on a control-channel error and could use the recovery button to return
to that room. On 2026-10-11 the user clarified that one side returning and the
other side stopping is intended. The prior requirement that both clients exit
automatically was incorrect and is withdrawn. This behavior is not a merge
blocker. The missing `returnToMenu` marker remains an observed protocol fact,
not a requirement to force both players out of the room.

The synchronized candidate initially preserved main's outdated TH11 rule text.
On 2026-10-11 the user specified the final three TH11-specific player-facing
rules: ordinary P drops scale 2x/3x (not Full Power), Bomb invulnerability is
shared except for Marisa A (8.00 maximum Power), and shared life fragments drop
at the original 1x rate. The authored `content/MULTIPLAYER.md` and generated
HTML now use those rules; the generic P-drop exception no longer includes TH11.
All-seat/ghost dialogue advancement, P1-owned story selection, and transfers
during dialogue are multiplayer implementation invariants, not extra
player-facing rules. UI layout and the optional Always Hitbox are likewise
presentation requirements, not game-rule bullets.

TH11MP native presentation verification passed 807 checks / 376 native
logic-and-draw ticks: original life/Power atlas elements, local Graze below
the seat groups, local-only view selection, and Always Hitbox drawn only for
the local seat when enabled, without changing combat RNG, Focus, motion or
authoritative state. The original communication/point display remains in
the native lower-left location. Launcher content freshness, TH11 policy,
shared guide, and dialog regression checks also passed. These focused
checks do not replace a full browser/device run or the pending Linux CI.

Earlier all-title browser evidence is retained separately in
`.cache/ui-acceptance/report.json` and is not relabeled as acceptance of this
candidate. No new 3P/Extra/spectator complete run, mobile-device, forced TURN,
long-duration Replay, production cutover or formal release is claimed here.

Before promotion: obtain green full-suite Linux CI including the atomic-deployment
gate, then complete formal browser/release acceptance. The TH11 rule wording is
reviewed and synchronized locally, not yet published. One-sided return stopping
its peer is accepted behavior, not an outstanding terminal-flow repair.
