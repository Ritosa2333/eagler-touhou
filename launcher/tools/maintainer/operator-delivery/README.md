# Operator Delivery template

This tracked directory owns only the public capability profile and redacted
static handoff template used by `tools/maintainer/build-operator-delivery.mjs`.
It contains no deployment credentials, real host configuration, or game
resources.

It is not a public hosting API. Compatibility with older Operator archives is
not required. The important contract is that every archive is derived from one
verified formal Release and that the declared capability profile is checked
against the generated Hosted site before packaging.

```text
node tools/maintainer/build-operator-delivery.mjs \
  --release=D:\Releases\candidate \
  --output=D:\Releases\Eagler-Touhou-Operator-YYYYMMDD \
  --archive=D:\Releases\Eagler-Touhou-Operator-YYYYMMDD.zip
```

The output path and archive path must not already exist. The archive option is
optional. When present, its SHA-256 sidecar is written only after the archive is
closed and readable.

Generated output may contain the Hosted site's legally owned game resources.
Always write it outside the source repository, transfer it privately, and never
commit it.
