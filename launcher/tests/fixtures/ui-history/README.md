# Fixed UI history fixtures

These files are immutable inputs for source-parity tests. They come from the two exact repository revisions listed in `manifest.json`; each entry records the source URL, byte length and SHA-256 digest. The tests compare current behavior with these historical inputs so a fresh clone does not need to contain the original Git objects.

Keep the fixture bytes unchanged. To add one, fetch only its named source revision, copy the requested path into `<revision>/<path>`, record the upstream blob's bytes and SHA-256 in the manifest, then run `node --test tests/support/ui-history.test.mjs`. Do not replace an oracle with the current implementation or update a fixture to make a test pass.

The root `.gitattributes` preserves these files byte for byte across platforms. Historical trailing spaces remain part of the recorded input and are excluded from whitespace cleanup.
