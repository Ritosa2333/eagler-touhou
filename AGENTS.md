# Agent rules

## Commits

Put one fix or one feature in each commit. Write the message in English using Conventional Commits, for example `fix(input): follow a touch drag at the original move speed`.

## Modules

Give each module one responsibility. Keep protocol handling, key ownership, touch movement, menu gestures, and save bridging apart.

Group modules in folders by that responsibility. Do not leave new modules loose in a shared root.

The browser entry bundles these modules into one `shell.mjs` with esbuild.
