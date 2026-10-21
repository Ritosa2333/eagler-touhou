import assert from "node:assert/strict";

import { assertOggProductionBaseline, pinOggSerial } from "../lib/ogg-production-baseline.mjs";

// Known-answer Ogg page: the input has one zero-length segment and a zeroed
// checksum. With serial 0x12345678, polynomial long division of the big-endian
// header value multiplied by x^32 by 0x104c11db7 yields 0x88f2e75f. This is
// the Ogg CRC polynomial 0x04c11db7 with MSB-first bits, no reflection and an
// initial remainder of 0. Keep expected bytes and digest independent of
// pinOggSerial so a CRC regression cannot bless its own output.
const fixture = Buffer.alloc(28);
fixture.write("OggS", 0, "ascii");
fixture[4] = 0;
fixture[26] = 1;
fixture[27] = 0;

const expectedPage = Buffer.from([
  0x4f, 0x67, 0x67, 0x53, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x56,
  0x34, 0x12, 0x00, 0x00, 0x00, 0x00, 0x5f, 0xe7,
  0xf2, 0x88, 0x01, 0x00,
]);
const expected = {
  bytes: 28,
  sha256: "9639c663bffa910f46e70327c4ea0909e590c4723e2ca517806249a22aefa2de",
};

const pinned = pinOggSerial(fixture, 0x12345678);
assert.deepEqual(pinned, expectedPage);
assert.equal(pinned.readUInt32LE(14), 0x12345678);
assert.equal(pinned.readUInt32LE(22), 0x88f2e75f);
assert.deepEqual(assertOggProductionBaseline(pinned, expected), expected);
assert.throws(
  () => assertOggProductionBaseline(pinned, { ...expected, sha256: "0".repeat(64) }),
  /does not match/,
);

console.log("OGG stream serial and production baseline known-answer: PASS");
