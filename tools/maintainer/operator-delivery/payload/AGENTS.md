# Eagler Touhou Operator Agent Entry

This is a generated operator deployment bundle, not a source checkout. Read `README.md` and
`services/README.md` before changing a host.

Do not invent the SSH target, web root, hostname, certificate ownership,
migration phase, Relay endpoint, TURN address or firewall policy. Ask the
operator when a required value is unknown.

- Verify `operator-checksums.txt` before using the payload.
- Keep the delivered `site/` pristine; configure a deployed copy.
- Resolve the exact web root before copying or replacing anything.
- Keep Relay/TURN secrets under `/etc/eagler-touhou/`, mode `0600`.
- Run `nginx -t` before reload and retain the previous working generation.
- During migration, keep HTTP `/migrate.html` reachable and do not enable HSTS.
- In final HTTPS mode, remove `shared.originMigration` before enabling HSTS.
- Do not claim browser/gameplay acceptance from file, process or HTTP checks.

Deployment is structurally complete only after checksums, `nginx -t` and the
applicable cutover verifier pass. Netplay additionally requires both Relay
probes and the verified WSS endpoint in the deployed Site Manifest.
