# TH07 public network / TURN deployment handoff

## Read this first

This document is the **current authority for the public WebRTC/TURN deployment work** as of 2026-08-26.

Public runtime note as of 2026-08-28: the multiplayer difficulty/Extra/Phantasm and shared Ending-input update with gameplay ABI `2` is now deployed on host 43 together with the Launcher/App-Shell rearchitecture and the matching `th07.html`, `th07.js`, and `th07.wasm`. The relay was deployed first, preserving compatibility with the targeted WebSocket envelope before the new multiplayer Runtime became reachable.

## 2026-08-28 mobile rollback checkpoint optimization deployment

The TH07 multiplayer Runtime on host 43 now contains the corrected two-logical-frame **first-write checkpoint interval** optimization. This is not alternate-frame snapshot skipping: the journal remains open across both logical frames, so objects first touched or spawned on the second frame are still captured before mutation. A rollback targeting the second logical frame restores to the interval start and resimulates from that actual restored frame.

Deployment was built from the current shared `build-web-th07-netplay` source. Before cutover, host 43 was discovered to contain a newer multiplayer Runtime shell than the generated local `th07.html`; that public shell was deliberately preserved because it includes the later JS cache-busting hotfix. The generated `th07.js` already matched the public JS byte-for-byte, so the effective gameplay binary change is the multiplayer WASM plus the corresponding App Shell Workbox revision/cache id and deployment inventory.

Current public hashes after cutover:

```text
TH07MP HTML SHA-256 = b7277ad79fe4a32199db3eafd8774bf5c1633013fdfaa720f18842db8901b613
TH07MP JS SHA-256   = 0391c2c42f95b97350928fb6f91daca5c4338bb2a2229b367c3f3e94385e9fb0
TH07MP WASM SHA-256 = 32882022b2b2fa6bc71081590a59a3d2961f7fdbd0b4168462ed4214a805b366
App Shell SW SHA-256 = 8cb4759165f8a36555c5d546a22ca5e8de328b030f1ca52efc8daac0016ee5e2
```

Validation before/after deployment:

- full Ninja/Emscripten link build: PASS;
- native rollback-journal tests, including second-frame first-touch/restore behavior: PASS;
- `netplay-rollback-feature-rebase-test.py`: PASS;
- staging `verify-server-build.mjs`: PASS, 60 files;
- post-cutover live hashes: exact match to staged files;
- Nginx configuration: PASS;
- `nginx` and `eagler-netplay.service`: active.

The generic `verify-deployed-site.mjs` still reports `import-partial sparse Runtime publication mismatch`. This is a pre-existing publication-metadata inconsistency: the site was already marked `resourceMode=import-partial` while its release catalog remained empty and `deployment.json` had no `runtimeUpdates` field before this cutover. The deployment intentionally preserved that existing publication mode rather than mixing an unrelated catalog migration into the rollback optimization release.

Rollback tree retained on host 43:

`/var/www/eagler-touhou.rollback-pre-rbopt-20260828-1950`

## 2026-08-28 r713 rollback-performance deployment

The general rollback performance update is deployed to the App-owned TH07 multiplayer Runtime at `test.touhou.vip`. It contains logarithmic journal overlap lookup, recycled history byte buffers and final sbrik's two-logical-frame keyframe cadence. Only `runtime/th07/multiplayer/th07.html`, `.js` and `.wasm` changed; no DATA, OGG or other game resources were published.

The three Runtime files, their Workbox revisions, a new App Shell cache id `72329a46c30510eeb829`, and all 60 authoritative `deployment.json` inventory entries were staged and switched together. This also repaired four stale inventory entries left by earlier live hotfixes. The immediately preceding site tree is retained at `/var/www/eagler-touhou.rollback-pre-r713-20260828`.

Public verification after cutover:

```text
TH07MP HTML SHA-256 = aa5cb20fd04d09822f7f01911fcbed1b7719cb6be86265a68a7b9b5b2eceba71
TH07MP JS SHA-256   = d2db4c68aa79a24b4d31598c389701e89535fb269b90a4521a00233a2aa2ec1d
TH07MP WASM SHA-256 = 5734079f2e8e43ab1153bba78e1a0778fdc2c79fcb08899319971858de54e97b
resourceMode         = import-only
public inventory     = PASS, 60 files, 11,009,561 bytes
nginx                = active, configuration PASS
```

ADB device `192.168.31.177:5555` was connected for human acceptance and reverse mappings for local ports `8136` and `18142` were restored. Dense real-stage Bomb/non-Bomb 2P/3P behavior remains human acceptance rather than automated proof.

It supersedes the older public-network status paragraphs in:

- `docs/th07-sbrik-feature-rebase-handoff.md`
- `docs/th06-th07-multiplayer-feature-rebase-notes.md`

Those documents are still authoritative for gameplay rebase, rollback semantics, touch/interpolation invariants and TH06 reuse lessons. Do not discard them.

Workspace root:

`D:\workspace\eagler`

Relevant repositories:

- `th07-eagler`
- `eagler-touhou`
- later sister-port: `th06-eagler`

The worktrees are intentionally dirty. **Do not reset, clean, checkout over, or otherwise destroy existing changes.** Do not commit/push unless the user explicitly asks.

Global project preference: do not switch to Work mode / `local.handoff`.

If this is a new ChatGPT conversation, follow the project's history-session discipline: bootstrap exactly once with the user's verbatim first request, then use history search/read and checkpoint the same session. The latest project history is in `docs/history-session/24.md`.

## User intent for this work

The public multiplayer network should stay simple:

1. WebRTC direct first.
2. STUN/ICE for path discovery.
3. TURN/UDP as the preferred relay path when direct connectivity fails.
4. TURN/TCP may remain available as a compatibility fallback.
5. Existing WebSocket gameplay relay stays temporarily as an emergency fallback until real public TURN evidence is good enough to remove it.
6. Do **not** invent a room-JWT / custom TURN-auth stack unless real abuse requires it.
7. Long-lived TURN shared secrets remain server-side; browsers receive short-lived coturn REST credentials.
8. The user's low-bandwidth/low-latency VPS should primarily carry signaling and relay traffic, not large game downloads.
9. Large complete offline packages are distributed separately through the existing private Lanzou/OpenList administration workflow. Provider-specific Lanzou logic must not enter product code.

## Browser transport architecture already implemented

Production browser networking is in `src/netplay/BrowserPeerTransport.*`.

Normal gameplay route:

```text
Rollback / Netplay core
        |
        v
BrowserPeerTransport
        |
        v
RTCPeerConnection
   |             |
   |             +-- th07-control: reliable + ordered
   |
   +---------------- th07-input: ordered=false, maxRetransmits=0
        |
        v
ICE-selected path
direct / srflx / TURN relay
```

Important: gameplay code only sees route `rtc`; whether ICE selected direct or TURN is not a second gameplay transport.

Emergency route:

```text
BrowserPeerTransport -> preconnected WebSocket relay
```

This is whole-room fallback only. No v1 mid-game hot switching. 3P is full mesh and must be all-RTC or all-relay; no per-edge mixed topology.

The two DataChannels are intentional:

- `th07-control`: session/control packets that should be reliable and ordered.
- `th07-input`: latency-sensitive input/ACK traffic. The application already has 32-frame redundancy, ACK recovery, prediction and rollback, so transport retransmission should not create TCP-style head-of-line delay.

## Route-barrier race already fixed

Do not reintroduce this bug.

The server broadcasts `route=rtc` to all endpoints, but browser event tasks are not simultaneous. One peer can receive the route first and immediately send HELLO while another peer's DataChannels are already open but its local route is still unset. The old handler discarded that packet and could deadlock at frame 0.

Current `BrowserPeerTransport` buffers RTC packets while route is undecided, symmetric with the WebSocket pre-route buffer. A deterministic focused test delays one endpoint's `route=rtc` by 300 ms and passes.

## Signaling / TURN credential server

Current development/public server code is Host-owned and shared by TH06/TH07:

`eagler-touhou/server/netplay-relay.mjs`

It currently combines:

- lobby;
- signaling;
- per-run route barrier;
- short-lived TURN REST credential generation;
- temporary WebSocket emergency gameplay relay.

TURN environment variables include:

- `TH07_STUN_URLS`
- `TH07_TURN_URLS`
- `TH07_TURN_SHARED_SECRET`
- `TH07_TURN_TTL_SECONDS`

`iceServersFor()` generates coturn REST-style credentials:

```text
username = expiry:room-run-player
credential = HMAC-SHA1(shared-secret, username), base64
```

The shared secret never needs to be sent to the browser.

Security policy intentionally stays minimal and standard:

- short-lived credentials;
- server-only shared secret;
- coturn auth enabled;
- basic allocation/bandwidth quotas;
- simple monitoring;
- optional application-layer room membership checks only if future abuse justifies them.

Do not add a new room JWT/custom TURN token system merely for theoretical hardening.

## coturn deployment files in the repository

Current owners:

- `eagler-touhou/server/render-coturn-config.cjs`
- `eagler-touhou/server/coturn.env.example`
- `eagler-touhou/docs/SELF_HOSTING_REFERENCE.md`
- `tests/coturn-config-contract-test.cjs`
- `tests/turn-signaling-contract-test.cjs`

The generated config uses coturn's standard capabilities rather than project-specific authentication.

## Public VPS: current real state

Public IP:

`43.138.163.67`

OS / role:

- Ubuntu 22.04 x86_64
- ~8 GB RAM
- user describes it as a low-bandwidth (10 Mbps upload) but low-latency game server
- do not make it the large Runtime/DATA/OGG download origin if avoidable

The machine's system Node is old (`v12.22.9`). Do not replace it globally because existing services may depend on it.

An isolated modern Node runtime was installed at:

`/opt/eagler-netplay/runtime/node`

Observed version during deployment:

`v24.13.0`

Network backend:

`/opt/eagler-netplay/app`

Systemd service:

`eagler-netplay.service`

Important service properties:

- runs as `eaglernet`;
- listens only on `127.0.0.1:18142`;
- uses `/etc/eagler-netplay/netplay.env`;
- Node 18142 is **not intended to be publicly exposed**.

At last verification:

```text
eagler-netplay.service = active
127.0.0.1:18142         = listening
```

## Nginx public WebSocket entry

The Eagler Nginx site is:

`/etc/nginx/sites-available/eagler-touhou`

It now contains:

```nginx
location /eagler-netplay/ {
    proxy_pass http://127.0.0.1:18142;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_read_timeout 3600s;
    proxy_send_timeout 3600s;
    proxy_buffering off;
}
```

`nginx -t` passed and Nginx was reloaded successfully.

Real external check from the development machine passed:

```text
ws://43.138.163.67/eagler-netplay/
-> lobby state received
-> WS_PUBLIC_PASS
```

Do not open public port 18142 just because old local URLs used it.

### Current public HTTPS origin

`test.touhou.vip` now resolves to `43.138.163.67` and has its own HTTPS Nginx vhost and certificate. The default public entry is:

```text
https://test.touhou.vip/eagler-touhou/
```

The domain root redirects to that path. `/eagler-netplay/` is proxied on the same HTTPS origin and therefore uses WSS. The separate `*.mcp.touhou.vip` vhost remains untouched.

## Launcher URL change: deployed

`eagler-touhou/app.js` has been changed so the default multiplayer URL is deployment-friendly:

- loopback (`127.0.0.1`, `localhost`, `::1`) keeps direct `:18142` for local development;
- non-loopback deployment uses same-origin `/eagler-netplay/`;
- HTTP page therefore uses `ws://.../eagler-netplay/`;
- HTTPS page therefore uses `wss://.../eagler-netplay/`.

This source change is now deployed on `test.touhou.vip`. The live Launcher and the public Nginx WebSocket endpoint both use the same-origin path; port 18142 remains private.

Local contract after this source edit:

```text
node eagler-touhou/scripts/test-local-launcher-contract.mjs
Local Launcher contract: PASS
```

## coturn on 43: current real state

coturn is installed and enabled through Ubuntu's `coturn.service`.

Config source:

`/etc/eagler-netplay/turnserver.conf`

`/etc/turnserver.conf` points to it.

One deployment bug was found and fixed: `/etc/eagler-netplay` had mode `0700 root`, so the `turnserver` service user could not traverse the directory and silently started with default discovery instead of the intended config. The directory was corrected to permit the `turnserver` group to read the config.

After restart, coturn listened only on the intended private address:

```text
10.1.20.6:3478 UDP
10.1.20.6:3478 TCP
```

The earlier unwanted listeners on loopback, Docker bridges and the local proxy/TUN interface disappeared. This is evidence that the custom config is actually being read.

External mapping:

```text
external-ip=43.138.163.67/10.1.20.6
```

Relay port range:

```text
49160-49559
```

Current conservative 10-Mbps-server defaults:

```text
user-quota=12
total-quota=256
max-bps=131072
bps-capacity=786432
```

Other current hardening includes:

- `use-auth-secret`
- `fingerprint`
- `no-cli`
- `no-multicast-peers`
- common private/reserved IPv4 `denied-peer-ip` ranges
- no TURN TLS yet because no dedicated TURN certificate was configured

Keep this simple unless measurement shows a real problem.

## Real TURN server-side relay test: PASS

This is no longer just a config-contract result.

On 43, `turnutils_uclient` was run using the configured TURN REST shared secret against the private listener. Actual relayed traffic crossed coturn:

```text
tot_send_msgs=12
tot_recv_msgs=12
Total lost packets 0
TURN_INTERNAL_PASS
```

Therefore these pieces are proven on the server itself:

- shared-secret auth;
- TURN allocation;
- relay port pool;
- actual relay packet forwarding.

That original server-side test alone did not prove public Internet TURN connectivity. The public browser tests below now do.

## Tencent Cloud Lighthouse firewall: PASS

The 43 host is a Tencent Cloud **Lighthouse** instance, not a CVM instance. This matters operationally: the relevant cloud control is the Lighthouse instance firewall (`lighthouse CreateFirewallRules`), not a CVM security group.

Verified instance identity:

```text
region       = ap-guangzhou
zone         = ap-guangzhou-6
instance     = lhins-km9ydq69
private IP   = 10.1.20.6
public IP    = 43.138.163.67
bandwidth    = 10 Mbps
```

The Lighthouse firewall was changed through official TCCLI on 2026-08-26. Firewall version advanced from 31 to 32 and the rule count from 40 to 43. The three added IPv4 ingress rules are:

```text
UDP 3478         0.0.0.0/0 ACCEPT
TCP 3478         0.0.0.0/0 ACCEPT
UDP 49160-49559  0.0.0.0/0 ACCEPT
```

Port 18142 was not opened; signaling remains behind Nginx `/eagler-netplay/`.

External development-machine checks after the change:

```text
TCP 3478 connect                  PASS
UDP 3478 STUN Binding Success    PASS (92-byte response)
```

The user supplied a Tencent Cloud API credential pair in chat to allow automation. **Do not copy those credential values into this file, repository, history, server, logs, or final replies.** The cloud change is complete; rotate/delete that long-lived pair.

A temporary helper script may exist at:

`archive/temporary/tencent_turn_sg_once.py`

It contains no intentionally persisted cloud secret. Its manual TC3 signer returned `AuthFailure.SignatureFailure`; the successful Lighthouse queries and firewall mutation used official TCCLI instead. Inspect before reuse; it is not product code.

### Public browser TURN acceptance: PASS

Public acceptance did not stop at port reachability. The current production `BrowserPeerTransport` harness was pointed at public `/eagler-netplay/` and Chromium was forced to `iceTransportPolicy=relay`.

```text
transport                  = rtc / rtc
__eaglerNetplayPath        = turn / turn
local candidateType        = relay / relay
remote candidateType       = relay / relay
selected protocol          = udp / udp
control/input binary data  = bidirectional PASS
```

The full local Launcher -> public signaling -> forced public TURN -> two independent TH07MP Runtime chain also passed:

```text
frames             = 135 / 135
transport          = rtc / rtc
path               = turn / turn
candidateType      = relay / relay
protocol           = udp / udp
```

The first full-game attempt selected the emergency WebSocket relay, while an immediate repeat selected TURN/UDP and passed. Keep the WebSocket emergency fallback; this small sample shows at least one transient RTC/TURN setup fallback and is not enough to estimate production success rate.

Still pending: a real PC<->phone/public-network TURN run and public beta path-rate telemetry.

## Latest browser transport evidence

Focused BrowserPeerTransport suite was rerun after the route-barrier changes:

```text
2P RTC direct                  PASS
forced WebSocket fallback     PASS
3P RTC full mesh              PASS
300 ms route skew             PASS
```

The recorded run printed:

```text
TH07MP WebRTC transport: PASS
rtc_modes=['rtc','rtc']
fallback_modes=['relay','relay']
mesh_modes=['rtc','rtc','rtc']
route_skew_modes=['rtc','rtc']
```

Same-host test candidates were host<->host UDP, as expected; this is not public NAT/TURN evidence.

Earlier full Launcher -> room -> TH07MP Runtime verification after the route-buffer fix reached roughly 130 logical frames on both peers with `rtc/direct`. That proves the production gameplay/session chain, but again not TURN.

## Large-resource distribution / Lanzou state

The user's intent is to keep large game packages off the 10 Mbps realtime VPS.

Current private admin workflow uses a temporary OpenList v4.2.5 Lanzou driver under:

`archive/temporary/tools/openlist-v4.2.5`

The real credential remains in the temporary OpenList SQLite/config and must not be copied into Git/docs/replies.

Fixed admin folder:

```text
/lanzou/TouhouEaglerData
folder id = 13866459
```

New complete offline packages were generated from the current production Package architecture and uploaded successfully:

```text
th06-offline-05d01e7bd26698c1.zip
62,689,469 bytes

th07-offline-d18342fca3d928e2.zip
98,009,902 bytes
```

After real import-only public multiplayer testing exposed a frame-zero startup deadlock, a corrected TH07 package was built, tested and uploaded:

```text
th07-offline-fe2462d63ac3c81d.zip
98,009,854 bytes
package revision = f9e37680da2294cb
sha256 = fe2462d63ac3c81d5887d39ec5ec3a949ffef7c8eb0bb3e4f39c20e6712b58f8
```

The deadlock had two linked causes:

1. Public ICE/DataChannel negotiation started only when Stage 1 took ownership, so a slower route could leave the deterministic frame-zero gate waiting before a transport existed.
2. With asymmetric Runtime loading, the later peer could receive the earlier peer's HELLO before its own Stage 1 gate began. It then sent READY without ever sending its own HELLO; the earlier peer correctly rejected READY-before-HELLO forever.

The low-coupling fix keeps transport ownership inside the player Runtime:

- begin production transport setup while the title/loading chain still presents frames;
- do not dispatch Stage 1 until the transport is actually open;
- while advancing to READY, retransmit HELLO first and then READY so asymmetric loading always completes the ordered session contract.

Do not replace this with a Launcher-owned RTC connection unless the project intentionally accepts a new cross-iframe packet proxy/ownership layer. `RTCPeerConnection` and `RTCDataChannel` cannot simply be transferred into the player iframe.

Real acceptance using two independent browser contexts, two independent IndexedDB stores, two fresh imports of the corrected ZIP, and the public HTTPS/WSS endpoint:

```text
TH07MP launch: PASS
frames = 120 / 124
transport = rtc / rtc
path = direct / direct
```

The public Lanzou folder was then opened in a fresh Chromium context through its password gate and visibly contained `th07-offline-fe2462d63ac3c81d.zip`.

Players who imported the earlier TH07 ZIP must import the corrected ZIP so their local multiplayer Runtime is replaced. The 43 server remains import-only and does not host this Runtime.

The TH07 complete package contains the current multiplayer Runtime as part of the shared TH07 generation.

Do not confuse these with the older game-data-only ZIPs.

The new files were visible in the Lanzou folder after upload. OpenList's Lanzou driver explicitly warns that directory-list file size/mtime are not reliable, so a small list-size mismatch was **not** treated as corruption evidence.

After the corrected TH07 package passed and the user explicitly authorized cleanup, these older complete packages were deleted from `TouhouEaglerData`:

```text
th06-offline-9c2dadc6e30ef69a.zip
th07-offline-9e1ac5ed8aafcbb6.zip
th07-offline-d18342fca3d928e2.zip
```

The retained current package pair is:

```text
th06-offline-05d01e7bd26698c1.zip
th07-offline-fe2462d63ac3c81d.zip
```

Keep the safe update rule for future releases:

```text
upload new -> verify externally -> only then delete old
```

Provider-specific Lanzou handling belongs only to private deployment/admin tooling. Product code should only know the existing generic optional deployment field:

```text
gameDataFallback = { url, hint? }
```

Do not add Lanzou-specific product code.

## 43 import-only deployment: PASS

The live `test.touhou.vip` Launcher is now deployed in `import-only` mode. The 10 Mbps origin serves only the lightweight website/catalog/import UI and does not contain the game Runtime, WASM, data or OGG payloads.

Verified deployment evidence on 2026-08-26:

```text
verify-server-build.mjs: valid=true, resourceMode=import-only
live /eagler-touhou/games/th07/th07.wasm: HTTP 404
Nginx, eagler-netplay and coturn: active
```

The generic `gameDataFallback={url,hint?}` points players to the externally distributed complete offline packages. Product code remains provider-neutral.

The deployment was atomic. The immediately preceding site tree is retained as `/var/www/eagler-touhou.rollback-v5-20260826`.

### Mobile stale-cache incident

One Android Chrome profile kept reporting `EAGLER-BOOT/1 kind=script-load` for `app.js` while a no-cache Edge profile worked. An attempted rescue-page/Service-Worker-disable workaround did not fix that profile; the user then cleared the browser cache and confirmed the site loaded normally.

The ineffective rescue page and dynamic module loader were reverted. The live site keeps only the real delivery fixes:

- explicit JavaScript MIME handling for `.mjs`;
- versioned `app.js` and module URLs;
- HTML and Service Worker revalidation headers.

A fresh Chromium context subsequently reached `window.__eaglerBoot.done=true`, showed no emergency panel and reported no failed requests.

## Local development state worth preserving

Local Launcher contract after the URL change: PASS.

Focused WebRTC test command:

```powershell
python eagler-touhou/scripts/test-th07mp-webrtc.py http://127.0.0.1:8136/eagler-touhou
```

At last run it passed all four groups (2P RTC, forced relay fallback, 3P mesh, 300ms route skew).

Local test processes may not survive handoff. Check ports rather than assuming:

- Launcher dev server: 8136
- local relay/signaling: 18142
- temporary OpenList: 5244

Do not kill unrelated existing services on 43. Nginx 80/443 and other Node services predated this work.

## What the next agent should do first

1. Read this file completely.
2. Read `docs/th07-sbrik-feature-rebase-handoff.md` and `docs/th06-th07-multiplayer-feature-rebase-notes.md` for gameplay/rollback invariants.
3. Inspect current worktree status; preserve all user changes.
4. Verify 43 services are still active:
   - `eagler-netplay.service`
   - `coturn.service`
   - Nginx `/eagler-netplay/`
5. Preserve the now-PASS Lighthouse firewall rules and do not open 18142 publicly.
6. Rotate/delete the long-lived Tencent Cloud credential pair exposed in chat.
7. Run a real PC<->phone/public-network TURN check when practical and collect beta direct/turn/relay rates; do not infer reliability from the two-run local sample.
8. Decide whether any TURN quota needs tuning only from real usage. Do not tune by guess.
9. Verify the external offline packages from a real player download/import flow before deleting any old Lanzou packages.
10. Keep WebSocket gameplay relay as emergency fallback until public beta data justifies removing it.

## Explicit non-PASS / do-not-overclaim list

As of this handoff, do **not** claim any of the following:

- PC<->phone TURN has passed.
- Public-NAT/CGNAT direct-connect rate is known.

## Security / credential note

The user pasted a Tencent Cloud API credential pair in the conversation. Never reproduce it in any handoff, file, command log or answer. Treat it as needing rotation after the cloud-side task. The TURN shared secret is separately stored server-side and likewise must never be copied into source/history/replies.

---

## URGENT ADDENDUM — latest regression investigation before conversation rollover

This section is newer than all status above. Read it first when resuming.

### Current user-visible regression

After another agent continued the deployment/network work, the user reported two related startup failures:

1. clicking Start Game can sometimes launch TH07 but land in the **normal main menu** instead of entering Stage 1;
2. even when Stage 1 is entered, it can **sometimes freeze on the first Stage-1 frame**.

Treat these as current unresolved regressions. Do not tell the user the startup path is fixed merely because one automated run passes.

The second symptom is especially important. It strongly resembles the historical frame-zero/session-gate startup races (HELLO/READY/route timing), but the exact current cause has **not yet been proven**. Do not blindly revert the old route-buffer fix or invent a gameplay patch.

### Fresh evidence collected immediately before rollover

The current local legacy/publication-backed Launcher path still passed once:

```text
python eagler-touhou/scripts/test-th07mp-launch.py http://127.0.0.1:8136/eagler-touhou/

TH07MP launch: PASS
frames=[135,138]
transports=['rtc','rtc']
paths=['direct','direct']
```

Therefore the multiplayer Runtime itself is not simply always broken.

The current 43 deployment is import-only and its live catalog was observed as:

```json
{
  "schema": "eagler-touhou/release-catalog/1",
  "games": {}
}
```

That means public TH07MP launch depends entirely on the browser's installed Package generation having `descriptor.runtimes.multiplayer`. There is no remote `multiplayerRuntime` catalog fallback on 43.

A fresh local import-only-equivalent test using the current complete TH07 Package proved that the Package architecture itself can work correctly:

```text
player.html?...runtime=multiplayer
Module.eaglerOptions.netplayMode = 'lan'
transport = rtc
logical frames observed = 417 / 439
```

So when the installed Package really contains the multiplayer Runtime, the import-only path can enter Stage 1 and advance normally.

### Important Launcher compatibility hole to audit

`ensureInstalledPackageRuntime()` correctly refuses an old installed Package generation that lacks `descriptor.runtimes.multiplayer` when `state.runtimeVariant === 'multiplayer'`.

However, the later legacy/offline compatibility branch still has an older single-runtime `offline.runtime` path. Audit that path carefully: a multiplayer room must **never silently fall back to a normal TH07 Runtime**. If an old package lacks a multiplayer Runtime, the correct behavior is an explicit "please import/update the multiplayer-capable package" error, not launching the normal main menu.

Also audit `render()` around the current logic:

```js
const multiplayerAvailable = state.game === "th07" && (
  !!installedGeneration?.descriptor?.runtimes?.multiplayer ||
  typeof game().multiplayerRuntime === "string"
);
if (!multiplayerAvailable && state.runtimeVariant === "multiplayer")
  state.runtimeVariant = "normal";
```

This can be dangerous if `installedPackageSnapshots` is temporarily stale or absent while a room-start flow is already requesting multiplayer. Do not allow UI rendering to silently downgrade an in-flight room launch to `normal`.

### Fresh production-delivery regression on 43

A clean Chromium context against the current public 43 site failed to complete `window.__eaglerBoot.done` within 30 seconds.

Direct HTTP checks then showed that the live 43 Nginx was serving `.mjs` files as:

```text
Content-Type: application/octet-stream
```

Examples included:

- `package-zip.mjs`
- `package-installer.mjs`
- `package-launcher.mjs`
- `package-generation.mjs`
- `package-store.mjs`
- `network-activity.mjs`
- `product-catalog.mjs`
- `release-catalog.mjs`
- `package-runtime-access.mjs`

This is invalid for browser ES modules and can break a fresh Launcher boot while cached profiles appear to work.

The local deployment template has already been patched to add an explicit `.mjs` location with:

```nginx
default_type application/javascript;
```

File changed:

`eagler-touhou/deploy/nginx-eagler-touhou.conf`

But the remote `/etc/nginx/sites-available/eagler-touhou` had **not yet been updated** during the latest investigation because the current MCP execution policy rejected a remote `/etc` write through the SSH subprocess. Do not assume production MIME is fixed. Re-check the live response headers first.

### Runtime changes that need first-priority audit for the first-frame freeze

The files modified by the later agent around the regression window were observed with timestamps around 17:23-17:27:

- `th07-eagler/src/main.cpp`
- `th07-eagler/src/netplay/Th07LanStageProbe.cpp`
- rebuilt `th07-eagler/build-web-th07-netplay/th07.html`

`BrowserPeerTransport.cpp` itself still had the older stable timestamp, so start by auditing the newer **preconnect / Stage-1 dispatch / session-gate** changes rather than rewriting RTC transport.

Current `main.cpp` includes this Stage-1 dispatch gate:

```cpp
if (g_NetplayStage1Harness && !g_NetplayStage1HarnessDispatched &&
    g_MainMenuForDebug && g_MainMenuForDebug->calcChain &&
    (!g_NetplayLanProduction ||
     Netplay::Th07LanStageProbe::TransportReady()))
```

Current `Th07LanStageProbe.cpp` has `StartProductionTransportEarly()` and `TransportReady()` is effectively:

```cpp
return !ProductionLanMode() ||
       (g_ProductionTransportStarted && TransportIsOpen());
```

The intent was to negotiate ICE/DataChannels while title/loading screens still advance, then enter Stage 1 only after transport is open. That intent is reasonable, but the user now reports an intermittent first-frame freeze, so verify the **actual ordered startup contract** rather than assuming this new preconnect gate is correct.

In particular, capture a failing run and inspect, per peer:

- when `PRECONNECT` occurs;
- when route becomes `rtc`/`relay`;
- HELLO send/receive counts;
- READY send/receive counts;
- whether READY is observed before the peer has observed HELLO;
- frame-zero exact-input send/receive;
- `FRAME0 WAIT` versus `FRAME0 SIM`;
- whether a peer begins Stage 1 significantly before the other;
- whether any pre-Stage-1 control packet is consumed/dropped before `SessionGate::Reset` ownership is established.

Do **not** claim the old 300 ms route-skew test covers this new asymmetric Runtime-loading/preconnect/session-reset case. They are not necessarily the same race.

### Very next action in the new conversation

1. Bootstrap history exactly once with the user's first new-chat message.
2. Read this addendum and the immediately preceding sections of this file.
3. Check current worktree/diff without reset/restore/clean.
4. Fix/redeploy `.mjs` MIME on 43 first so fresh-browser tests are trustworthy.
5. Reproduce TH07MP startup **repeatedly**, not once. Prefer 10-20 room starts or until a first-frame freeze is captured.
6. Preserve complete per-peer console/session diagnostics for the failing run.
7. Only then make the smallest session/preconnect fix.
8. Add a deterministic regression for the exact captured race before calling it fixed.

The user explicitly said the conversation is ending and the next assistant will lose conversational memory. This addendum is the authoritative continuation point for that next assistant.

## 2026-08-27 local keyboard-prediction and 3P timing update

This update was built and locally verified on 2026-08-27 and was **deployed to host 43 on 2026-08-28**. See the deployment verification section immediately below.

- Keyboard direction prediction now continues for at most three missing frames. The twelve-frame rollback/recovery window, safe held controls and existing direct-touch behavior remain unchanged.
- Production RTC input packets are built and sent per remote endpoint, restoring peer-relative ACK shrink and frame advantage in 3P.
- WebSocket fallback supports the same peer-relative packets through a two-byte transport envelope; `tools/netplay/lan-relay.cjs` strips the envelope before delivering the original game packet.
- Time synchronization now retains a trimmed sample window for every remote endpoint and uses the largest local lead as the room pacing recommendation, matching the GGPO/GGRS multi-endpoint policy instead of averaging links.
- Runtime diagnostics expose per-player confirmation gap, current prediction depth and rollback-source count.
- Short verification passed: C++ core, structural contracts, targeted relay delivery, RTC/fallback/3P transport, Emscripten link and a real three-browser 120-frame Launcher run. The three-browser run also asserted that the published room lead equals the maximum ready per-peer lead.

Deployment order matters because the new Runtime uses targeted relay envelopes: deploy the backward-compatible `lan-relay.cjs` service first, then the Launcher/App Shell and sparse TH07 multiplayer Runtime. Do not deploy the Runtime first. Real-person keyboard visual acceptance remains unclaimed until the deployed 2P extreme-jitter and normal 3P cases are observed.

## 2026-08-28 public deployment after Launcher/Runtime rearchitecture

The public `test.touhou.vip` deployment is now on the rearchitected App-owned Runtime path.

Deployment order actually used:

1. replace `/opt/eagler-netplay/app/lan-relay.cjs` and restart `eagler-netplay.service`;
2. fix the live Nginx `.mjs` MIME mapping while preserving `/eagler-netplay/`;
3. build a complete `import-only` publication containing Launcher/App Shell plus TH06/TH07/TH07MP App-owned Runtime files;
4. upload into a separate staging tree;
5. atomically rename the staged site into `/var/www/eagler-touhou`.

The live site remains intentionally `import-only`:

```text
games.json -> { schema: "eagler-touhou/release-catalog/1", games: {} }
```

The 10 Mbps host still does not publish DATA/OGG game-content payloads. Runtime HTML/JS/WASM are App Shell resources and are precached by the publication Workbox manifest.

Live service/file evidence after cutover:

```text
eagler-netplay.service = active
nginx = active
127.0.0.1:18142 = listening

relay SHA-256:
f81a8128cf21be71d68dfa1dbfd6c33d6714a073ff4d88d5c4aaaa56ba570400

Launcher app.js SHA-256:
d850eb3058b8630f070bccc4683e491464de8e2c81a4a9e6b60967ea9e0ef483

TH07 multiplayer WASM SHA-256:
0c1fc1d7de5ec5c8dcbff02189667dc50baa87958f3cf1665d1942c39ab58e4a

.mjs Content-Type:
application/javascript
```

Rollback copies retained on host 43:

```text
/var/www/eagler-touhou.rollback-pre-r711-20260828
/opt/eagler-netplay/app/lan-relay.cjs.bak-20260828-r711
```

Public browser verification after deployment:

- fresh WebKit-style TH06 import-only Package -> App-managed Runtime -> managed DATA -> first-frame: PASS;
- fresh WebKit-style TH07 import-only Package -> App-managed Runtime -> managed DATA -> first-frame: PASS;
- TH07MP public 2P room: PASS past 120 logical frames, RTC direct/UDP, peer readiness 1/1;
- TH07MP public 3P room: PASS past 120 logical frames on all endpoints, RTC full mesh direct/UDP, peer readiness 2/2;
- public forced route fallback with missing signaling peer: PASS, whole room selected `relay` in about 4.6 s;
- public targeted relay envelope: PASS; a P1 packet targeted at P3 was delivered only to P3 and the relay stripped the two-byte `0xe7,target` transport envelope before delivery.

One apparent post-deployment multiplayer startup failure was traced to the acceptance harness, not product code: the harness installed the Package through the generic manual-import action, which transiently launched normal TH07 and then killed it while IDBFS/WASM initialization was still in flight before immediately launching TH07MP. Using the real `import-only` primary acquisition path (which installs the Package without transiently launching normal TH07) made the same public 2P/3P runs pass. Do not reintroduce a product workaround for that harness artifact.

The public site therefore has the new relay, Launcher/App-Shell architecture and matching multiplayer Runtime. **Do not claim real-person keyboard visual acceptance yet.** The deployed 2P extreme-jitter visual behavior and normal 3P visual behavior still require actual human observation.

## 2026-08-28 r712 public deployment after Runtime rearchitecture

This section supersedes the older "not deployed" status above for the keyboard-prediction / 3P timing update and records the post-rearchitecture public deployment.

Deployment completed on host `43.138.163.67` in compatibility-safe order:

1. `tools/netplay/lan-relay.cjs` was uploaded first and `eagler-netplay.service` restarted.
2. The public Nginx site was fixed so `.mjs` is served as `application/javascript`; the existing `/eagler-netplay/` WebSocket proxy was preserved.
3. The rearchitected Launcher/App Shell and App-owned TH06 / TH07 normal / TH07 multiplayer Runtime were staged together and switched atomically. The public site remains `import-only`; DATA/OGG are not hosted on the 10 Mbps VPS.

Current relay evidence:

```text
eagler-netplay.service = active
127.0.0.1:18142 = listening
lan-relay.cjs SHA-256 = f81a8128cf21be71d68dfa1dbfd6c33d6714a073ff4d88d5c4aaaa56ba570400
```

Current public Runtime evidence:

```text
TH07MP HTML SHA-256 = 8427f502a5a2b62372e3f9e5d8cc506b4cedbaa31c9cf4340c5ed536afb5b62d
TH07MP WASM SHA-256 = 0c1fc1d7de5ec5c8dcbff02189667dc50baa87958f3cf1665d1942c39ab58e4a
package-store.mjs Content-Type = application/javascript
games.json = { schema: "eagler-touhou/release-catalog/1", games: {} }
```

Two site rollback trees are retained:

```text
/var/www/eagler-touhou.rollback-pre-r711-20260828
/var/www/eagler-touhou.rollback-pre-r712-20260828
```

### r711 public failure and root cause

The first post-rearchitecture public cutover (r711) exposed a deployment/build artifact problem rather than an RTC/TURN bug. The TH07 multiplayer binary had the new gameplay/network code but an old generated `th07.html` shell. That stale shell still contained the retired Package Runtime bridge and did not contain the new Launcher-managed DATA preload hook.

On local hosted tests the defect was hidden because the stale Runtime could still fetch `th07.data` from the local hosted tree. On the real `import-only` 43 deployment the multiplayer iframe therefore requested an unavailable relative DATA file, received HTTP 404 and never reached `runtime-ready`. The Launcher remained at `正在准备本地 Runtime…`.

The multiplayer Runtime was relinked from the current `resources/shell.html`. The resulting shell now contains:

```text
managedData=1
-> window.parent.__eaglerPrepareManagedRuntimeDataV1(...)
-> Module.getPreloadedPackage(...)
-> generated Emscripten DATA loader
```

and no longer contains `packageBridge`, `package-bootstrap` or `__eaglerPackageBootstrapState`.

`eagler-touhou/scripts/package-server.mjs` now rejects any normal or multiplayer Runtime build whose HTML lacks the App-managed DATA preload hook or still contains the retired Package Runtime bridge. This prevents a stale hosted-only shell from being published to an `import-only` deployment again.

### Post-r712 public verification

Fresh public WebKit-style Launcher runs against `https://test.touhou.vip/eagler-touhou/` passed for both TH06 and TH07 normal after importing local content Packages. Both used App-managed Runtime + Package Store DATA and reached running/first-frame.

TH07MP public 2P after fresh per-endpoint Package import:

```text
room=5039
frames=[121,122]
transports=['rtc','rtc']
paths=['direct','direct']
peers=1/1 ready
PASS
```

TH07MP public 3P full mesh after fresh per-endpoint Package import:

```text
room=9523
frames=[139,140,143]
transports=['rtc','rtc','rtc']
paths=['direct','direct','direct']
peers=2/2 ready
PASS
```

The 3P timing diagnostic still obeyed the intended max-ready-per-peer policy: each endpoint's published room lead matched the maximum of its ready per-peer lead samples.

Public forced WebSocket fallback also passed:

```text
modes=['relay','relay']
elapsedMs=4650
PASS
```

The targeted relay envelope was verified directly against the deployed 43 service with a forced 3P relay room. P1 sent an envelope targeting P3; the relay stripped the two-byte transport header and delivered only the original payload `[17,34,51,68]` to P3, while P2 received no binary payload:

```text
modes=['relay','relay','relay']
target=2
payload=[17,34,51,68]
nonTargetReceived=false
PASS
```

Automated/public transport acceptance is therefore PASS for the deployed 2P RTC path, 3P full mesh, whole-room WebSocket fallback and targeted relay delivery. **Real-person keyboard visual acceptance is still not claimed.** The user still needs to observe the deployed 2P extreme-jitter case and a normal 3P case before that visual issue can be marked accepted.
