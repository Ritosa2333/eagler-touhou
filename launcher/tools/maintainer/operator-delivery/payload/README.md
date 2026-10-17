# Eagler Touhou Operator Delivery

This is an operator deployment bundle generated from one verified formal Release.
Automated operators and coding agents must read `AGENTS.md` before acting.

- Deploy `site/` as the static web root.
- TH06 and TH07 contain Japanese, Simplified Chinese and English.
- TH08 currently contains Japanese only.
- DATA, MIDI and OGG are included in the Hosted site.
- Netplay is disabled until the operator deploys and configures its own Relay.
- The external import fallback and HTTP-to-HTTPS migration capability are
  intentionally retained by the current internal profile.
- `verification-report.json` records the evidence level inherited from the
  formal Release. Build/structure PASS is not browser or gameplay acceptance.

Before deployment, verify the untouched delivery from its root:

```sh
sha256sum --check operator-checksums.txt
```

Use `services/nginx/nginx.migration-window.conf.example` during the migration
window. Exact HTTP `/migrate.html` must remain reachable and HSTS must remain
disabled. After migration is retired, remove the capability with
`services/configure-deployed-site.mjs`, switch to the final HTTPS example and
enable one-year HSTS.

Import-mode files and offline game ZIPs are intentionally not included in this
Operator delivery; they remain outputs of the source formal Release.
