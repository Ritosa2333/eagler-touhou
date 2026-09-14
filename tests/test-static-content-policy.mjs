/** L3/module behavior. No filesystem fixtures. Proves the local static-server
 * MIME/compression/cache policy, including host-generated asset extensions.
 * Does not prove an HTTP server was started or that a concrete host contains
 * those assets. */
import assert from "node:assert/strict";
import {
  staticContentCacheControl,
  staticContentCompressible,
  staticContentType,
} from "../server/static-content-policy.mjs";

assert.equal(staticContentType("assets/th06.ico"), "image/x-icon");
assert.equal(staticContentType("assets/example.jpg"), "image/jpeg");
assert.equal(staticContentType("runtime/th07.wasm"), "application/wasm");
assert.equal(staticContentType("runtime/font.otf"), "font/otf");
assert.equal(staticContentType("site.webmanifest"), "application/manifest+json; charset=utf-8");
assert.equal(staticContentType("content/FIRST_USE_NOTICE.md"), "text/markdown; charset=utf-8");
assert.equal(staticContentType("unknown.bin"), "application/octet-stream");

assert.equal(staticContentCompressible("runtime/th07.data", 2048), true);
assert.equal(staticContentCompressible("runtime/th07.data", 512), false);
assert.equal(staticContentCompressible("assets/th06.ico", 4096), false);

assert.equal(staticContentCacheControl("packs/lang.0123456789abcdef01234567.zip"), "public, max-age=31536000, immutable");
assert.equal(staticContentCacheControl("packs/lang_en.zip"), "public, max-age=0, must-revalidate");
assert.equal(staticContentCacheControl("runtime/th07.wasm"), "public, max-age=0, must-revalidate");

console.log("Static content policy: PASS");
