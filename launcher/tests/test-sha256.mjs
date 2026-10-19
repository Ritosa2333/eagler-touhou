import assert from "node:assert/strict";
import { createHash, webcrypto } from "node:crypto";

import { sha256Hex } from "../.cache/build/browser/assets/launcher/sha256.mjs";

const encoder = new TextEncoder();
const vectors = [
  [new Uint8Array(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"],
  [encoder.encode("abc"), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"],
  [encoder.encode("The quick brown fox jumps over the lazy dog"), "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"],
  [Uint8Array.from({ length: 1025 }, (_, index) => index & 0xff), null],
];

for (const [bytes, known] of vectors) {
  const expected = known || createHash("sha256").update(bytes).digest("hex");
  assert.equal(await sha256Hex(bytes, webcrypto.subtle), expected, "WebCrypto SHA-256 mismatch");
  assert.equal(await sha256Hex(bytes, null), expected, "pure-JS SHA-256 fallback mismatch");
}

console.log(JSON.stringify({ sha256: "PASS", paths: ["webcrypto", "fallback"], vectors: vectors.length }));
