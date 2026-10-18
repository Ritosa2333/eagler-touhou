# Deployment examples

These files illustrate the HTTP behavior expected by a generated Eagler Touhou
site. They are examples, not installers or an infrastructure API.

The project builds and verifies a complete static site. Operators remain
responsible for choosing, installing, and configuring their Web server, TLS,
CDN, service manager, firewall, and rollback strategy.

- `nginx.conf` shows the required MIME, cache, root-redirect, legacy mount, and
  optional WebSocket proxy behavior for nginx.
- `nginx-external-pair.conf` shows the Hosted resource origin and its derived
  External user site, including redirects, CORS and byte-range-compatible
  static delivery.
- `npm run verify:deployed -- https://example.com/` validates the resulting
  public behavior independently of the chosen infrastructure.
