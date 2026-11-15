import {test} from 'vitest';
import assert from "node:assert/strict";
import {
  staticContentCacheControl,
  staticContentCompressible,
  staticContentType,
} from "../server/static-content-policy.mjs";

test("static content policy", async () => {
/** L3/module behavior. No filesystem fixtures. Proves the local static-server
 * MIME/compression/cache policy, including host-generated asset extensions.
 * Does not prove an HTTP server was started or that a concrete host contains
 * those assets. */



assert.equal(staticContentType("assets/favicon.ico"), "image/x-icon");
assert.equal(staticContentType("assets/example.jpg"), "image/jpeg");
assert.equal(staticContentType("runtime/th07.wasm"), "application/wasm");
assert.equal(staticContentType("runtime/font.otf"), "font/otf");
assert.equal(staticContentType("site.webmanifest"), "application/manifest+json; charset=utf-8");
assert.equal(staticContentType("content/FIRST_USE_NOTICE.md"), "text/markdown; charset=utf-8");
assert.equal(staticContentType("unknown.bin"), "application/octet-stream");

assert.equal(staticContentCompressible("runtime/th07.data", 2048), true);
assert.equal(staticContentCompressible("runtime/th07.data", 512), false);
assert.equal(staticContentCompressible("assets/favicon.ico", 4096), false);

assert.equal(staticContentCacheControl("packs/lang.0123456789abcdef01234567.zip"), "public, max-age=31536000, immutable");
assert.equal(staticContentCacheControl("packs/lang_en.zip"), "public, max-age=0, must-revalidate");
assert.equal(staticContentCacheControl("runtime/th07.wasm"), "public, max-age=0, must-revalidate");
const generation = 'a'.repeat(64);
for (const name of ['th14.html', 'shell.mjs', 'th14-sdl.wasm', 'fonts/cp932.bin']) {
  assert.equal(staticContentCacheControl(`runtime/th14/${generation}/${name}`),
    'public, max-age=31536000, immutable, no-transform',
    'CDNs must preserve every byte of the hash-verified Runtime generation');
}

console.log("Static content policy: PASS");
});
