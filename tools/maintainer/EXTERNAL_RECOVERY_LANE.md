# Maintainer External recovery/validation lane

This is an operational recovery/validation lane, **not** the formal Release
entrypoint. Formal candidates remain owned exclusively by `npm run release` as
documented in [`../../docs/RELEASE.md`](../../docs/RELEASE.md).

Use this lane only when an already-published Hosted Package generation remains
the actual resource origin and a staging/test External site must be regenerated
around that existing generation. Do not build a reduced Hosted site merely to
change Relay or migration settings: DATA, music, language, feature, Package
pointer, and Package Descriptor identity remain tied to the Hosted generation.

```powershell
node tools/maintainer/recover-external-site.mjs `
  --source=D:\Releases\hosted-site `
  --runtime-release=D:\Releases\runtime-release `
  --output=D:\Validation\external-site `
  --profile=web-validation-external-recovery `
  --netplay-relay=wss://play.example.com/eagler-netplay/ `
  --origin-migration=none
```

Only `web-validation-*` profiles are accepted. Output must be a new directory;
the tool never replaces an existing directory. The result may be used for an
explicitly authorized staging/recovery deployment, but it is not a formal
Release candidate and must not be promoted as one.

The source must pass its own Release Manifest/checksum integrity. The Runtime
Release must satisfy the current Runtime Release contract and match every
selected DATA layout. The output retains the source generation's complete
Package metadata and descriptors, while deployment-owned Relay and migration
metadata may be explicitly overridden.

The final Release Manifest records an `externalRecovery` context containing the
source Hosted release ID, Runtime Release manifest SHA-256, selected games,
requested overrides, and effective deployment metadata. The wrapper passes only
the requested recovery overrides to `scripts/package-server.mjs`; the packager
independently verifies the Hosted Release and Runtime Release identities,
checks that the requested overrides match its actual packaging inputs, and
writes the provenance when it first creates `release-manifest.json`. The
wrapper verifies that provenance before publication. It does not rewrite a
generated manifest or checksum list afterward.

After an explicitly authorized staging/recovery upload, run the public
deployed-site verifier and retain an atomic rollback directory or hosting
revision. Never patch an active `site-manifest.json`, `release-catalog.json`, or
Package Descriptor in place.
