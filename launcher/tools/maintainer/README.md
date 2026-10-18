# Maintainer-only tools and operational probes

This directory contains public, stateless maintainer tool source and
redacted templates. It must never contain credentials, private game resources,
real server configuration, generated delivery archives, logs, or acceptance
evidence.

This directory contains both maintainer-only release/bundle adapters and live
infrastructure probes. They intentionally live outside the portable
product/build CLI in `scripts/`, are not ordinary self-host APIs, and are not
formal Release entrypoints.

- `verify-public-relay-fallback.mjs` checks public WebSocket relay fallback.
- `verify-public-targeted-relay.mjs` checks targeted public relay routing.
- `verify-public-turn-quality.py` measures a caller-supplied public TURN target.
- `verify-public-first-install.mjs` starts a fresh Chromium profile against an
  explicitly supplied staging URL and requires Package generation commit,
  Runtime readiness, and a real first frame. Its timeout is a deadman; reported
  elapsed time is diagnostic, not a performance threshold.
- `verify-thcrap-real.mjs` checks the current workspace's real thcrap toolchain.
- `build-github-release-assets.ps1` is the sole public project-asset producer.
  It accepts a verified formal Release candidate and derives both the
  recommended Self-host ZIP and advanced Runtime ZIP from its Runtime identity.
- `recover-external-site.mjs` derives a validation/recovery External
  deployment from an existing Hosted Package generation while allowing only
  deployment-owned Relay/origin-migration overrides. See
  `EXTERNAL_RECOVERY_LANE.md`.

These tools intentionally have no npm aliases. Maintainers invoke the required
file directly with explicit inputs. Live probes must record target, time, and
release identity outside the source tree when the result is release evidence.
A passing probe proves only the target and conditions actually exercised; it is
not a repository-core, browser-device, or public-release acceptance result.

`operator-delivery/` likewise contains only the public template and capability
profile used to build an operator bundle. The generated bundle may contain a
Hosted site's legally owned game resources and must be written outside the
repository, transferred privately, and never committed.

`recover-external-site.mjs` is a maintainer recovery/validation lane,
not an ordinary self-host API or formal Release entrypoint. It exists so a
staging or recovery External deployment can reuse an already-published Hosted
Package generation without rebuilding a different MIDI/language capability set
merely to change environment-specific Relay or migration metadata. It keeps
DATA, music, language, feature, Package pointer, and Package Descriptor identity
tied to the chosen Hosted generation and emits only `web-validation-*`
authority.
