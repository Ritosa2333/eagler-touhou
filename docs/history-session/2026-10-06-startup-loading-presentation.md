# Startup loading presentation

Requested behavior: show each game's original loading artwork during actual
resource preparation and enter Title as soon as preparation completes. Do not
add a minimum splash duration. The user subsequently applied this rule to TH10
and explicitly requested removal of its original five-second startup wait.

## Changes

- TH06/TH07 keep the original logo surface and split Supervisor startup at the
  browser paint boundary. One RAF presents the logo before the remaining sound,
  text and menu resources are initialized. The first-frame event observes that
  completed swap once. The ordinary simulation clock starts afterwards without
  accumulated startup time. Both normal and multiplayer binaries were rebuilt.
- TH08 prepares its logo and `nowloading.anm` before the existing incremental
  preload. Loading animation updates are display-budgeted while resource work
  continues. Initialization reuses those resources and the existing backbuffer.
- TH09 presents its logo and original loading ANMs before font/audio/title
  preparation. Startup ANMs use a separate RNG/owner and are released afterwards.
  Both managed and standalone shells use this path. Normal and multiplayer
  directory Runtimes were assembled.
- TH11/TH15 present `sig.anm` through isolated animation owners before the rest
  of initialization. The first animation update publishes the draw lists.
  TH15's signature uses the authored double-resolution scaling. Startup graphics
  are retired after initialization. TH15's existing configurable SDK path now
  accepts both upstream and install SDK layouts.
- TH10 keeps the native Title loader's 300-frame policy as the default environment
  behavior. The browser environment completes startup after its first drawable
  opening frame, then enables Title immediately when its resources are ready.
  The opening bank is retained through VM creation and presentation before it
  is retired. The existing practice-enabled build configuration was preserved.
- TH20 separates signature/text preparation from the inline worker's remaining
  resource work. Its existing callbacks present the startup artwork before
  shared/game preparation resumes. Failure keeps the existing exit state.

## Evidence

Local Chromium with software WebGL, prepared private DATA and fresh browser
contexts. This is browser evidence, not mobile/device or deployment acceptance.

- TH06/TH07/TH08/TH09/TH10/TH11/TH15: compositor screenshots captured the original
  loading artwork, then Title and gameplay. No page or Runtime errors in the
  completed runs. TH15 was checked after its signature scaling correction.
- TH06/TH07/TH09 multiplayer variants: without-room startup and gameplay smoke
  passed. This does not establish peer, rollback or public-network acceptance.
- TH10: with OGG installed, Title became ready approximately 361 ms after launch
  began, following resource delivery/configuration. Stage playback had nonzero
  audio RMS. Sync/list returned `th10.cfg` and `scoreth10.dat`.
- TH10 presentation-purity check passed. All ten shared host tests passed.
  TH06/TH07 shell protocol execution passed with canonical shell-path overrides.
  Whitespace checks passed for all eight adapters with CRLF handled explicitly.
- TH20: build/package passed. Its original DATA is unavailable locally, so its
  loading artwork and gameplay remain browser-unverified.
- The broad workspace Runtime protocol checker could not run against the
  existing Launcher workspace configuration, which still resolves TH06 source
  shell paths under `th06/resources`. That unrelated configuration was preserved.

Reproducible private reports/screenshots are under
`artifacts/validation/startup-loading/`. The focused harness serves one Runtime
page at a time and records the compositor frames. No retail DATA was added to
source or Runtime packages. No publication or deployment was performed.

| Runtime | Final WASM SHA-256 |
| --- | --- |
| TH06 normal | `60d7c8f75189a1a2e02f84ab0f170b5b2c1e4908f646587fb4c0b19a775a8d34` |
| TH07 normal | `00cb604aba360a08febccc9a3c452c70df1f7cb873639c9e9c0b22c5c7961f4f` |
| TH08 | `2fd7897e332be7dd5193f833f0a2c4ff7f5a5cff226d93d081348cb7714145ca` |
| TH09 | `a5c1c9a799376fdaa60c14635a1f2aee42016c751a98a60400d7c6a489bc0116` |
| TH10 | `d4b8a1a65022ccfd9f35835a392079371ef37289482ca8ded60a095e26da4bd6` |
| TH11 | `d3a15e0bf7eb61ecb5c00f48bdb75952a4d7ca123f0a9a35592af7111d29cb5b` |
| TH15 | `d0314ac01997f7bb0a6a6ef01ff0b6b693b1ef50e1ca1e88dbc6d72d8b311d41` |
| TH20 | `857fe8797b9638c8801c6260ee16039b8e7e7af85104af324a40f39343deab0f` |
