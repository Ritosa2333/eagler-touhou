# Monorepo workspace

- `launcher/`: frontend and Runtime/content build recipes, imported from original main.
- `ports/`: title sources, preserving existing ordinary and MP ownership.
- `common/`: the single default shared runtime source. Neutral client/service definitions live in `common/contracts/` with their own inherited license.
- `services/relay/`: independent room, signaling, fallback and spectator service.
- `services/turn/`: coturn configuration and operations support.
- `services/telemetry/`: independent bounded telemetry service.
- `tools/`: shared build and CI tooling. Root `.github/workflows/` is authoritative.

Read `launcher/config/workspace.json` for directory owners and `launcher/config/runtime-builds.json` for build recipes. Old imported branch names are historical identities, not selectors for monorepo development branches.

Sources were committed, pushed and refreshed before the move. Original repositories and complete histories remain. Final cutover depends on the [post-move acceptance](docs/migration/move-acceptance-20261018.md), including explicit failed and unproven behaviors.
