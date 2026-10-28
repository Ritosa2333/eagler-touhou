/** Cross-language format-adapter coverage gate.
 * Product registration and retail artwork decoding are separate owners, but
 * every formal game must have an artwork adapter before Host assembly can be
 * considered complete. */
import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import { resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { HOST_SITE_ARTWORK_FILES } from "../lib/frontend-manifest.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const python = process.env.EAGLER_PYTHON || process.env.PYTHON || "python";
const result = spawnSync(python, ["scripts/prepare-host-artwork.py", "--list-games"], {
  cwd: project,
  encoding: "utf8",
  windowsHide: true,
});
assert.equal(result.error, undefined, `cannot start configured Python interpreter: ${python}`);
assert.equal(result.status, 0, result.stderr || result.stdout);
const report = JSON.parse(result.stdout);
assert.ok(Array.isArray(report.games));
assert.deepEqual([...report.games].sort(), Object.keys(PRODUCT_GAMES).sort(),
  "Host artwork format-adapter registry must cover every formal Product Catalog game");
for (const [game, product] of Object.entries(PRODUCT_GAMES)) {
  assert.ok(Array.isArray(report.files?.[game]), `${game}: artwork adapter did not report its outputs`);
  if (product.cardArtwork) {
    assert.ok(report.files[game].includes(product.cardArtwork),
      `${game}: artwork adapter outputs must include Product Catalog cardArtwork ${product.cardArtwork}`);
  }
}
for (const artwork of HOST_SITE_ARTWORK_FILES) {
  assert.ok(report.files.th06.includes(artwork),
    `TH06 host artwork adapter must produce global site artwork ${artwork}`);
}

// Synthetic TH13 ANM v8 regression: two title/title00a.png entries with the
// same name but different source_x must compose into the 640x480 card. This
// exercises the real Python format adapter without retail game data.
const synthetic = spawnSync(python, ["-c", String.raw`
import importlib.util, io, json, struct, sys
from PIL import Image
sys.path.insert(0, "scripts")
spec = importlib.util.spec_from_file_location("host_artwork", "scripts/prepare-host-artwork.py")
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)

def make_entry(source_x, width, height, pixel, sprite_w, sprite_h):
    sprites, scripts = 1, 0
    table_end = 64 + sprites * 4 + scripts * 8
    name = b"title/title00a.png\0"
    name_offset = table_end
    texture_offset = name_offset + len(name)
    sprite_offset = texture_offset + 16 + len(pixel) * width * height
    header = bytearray(64)
    struct.pack_into("<IHH", header, 0, 8, sprites, scripts)
    struct.pack_into("<HH", header, 10, width, height)
    struct.pack_into("<hh", header, 20, source_x, 0)
    struct.pack_into("<I", header, 16, name_offset)
    struct.pack_into("<I", header, 28, texture_offset)
    header[32] = 1
    sprite_table = struct.pack("<I", sprite_offset)
    sprite = struct.pack("<Iffff", 72, 0, 0, sprite_w, sprite_h)
    pixels = pixel * (width * height)
    thtx = struct.pack("<4s4H I", b"THTX", 0, 5, width, height, len(pixels)) + pixels
    return bytes(header) + sprite_table + name + thtx + sprite

red = bytes((0x00, 0xFF))
green = bytes((0xF0, 0xF0))
first = bytearray(make_entry(0, 512, 512, red, 512, 480))
struct.pack_into("<I", first, 36, len(first))
anm = bytes(first) + make_entry(512, 128, 512, green, 128, 480)
for broken in [anm[:63], anm[:len(first) - 1], anm[:-1], bytes(first[:36]) + struct.pack("<I", 1) + bytes(first[40:])]:
    try:
        module._compose_th13_title(broken)
    except ValueError:
        pass
    else:
        raise AssertionError("malformed TH13 ANM must fail closed")
png = module._compose_th13_title(anm)
with Image.open(io.BytesIO(png)) as image:
    rgba = image.convert("RGBA")
    print(json.dumps({"size": list(rgba.size), "left": list(rgba.getpixel((0, 0))),
                      "right": list(rgba.getpixel((512, 0)))}))
`], {cwd: project, encoding: "utf8", windowsHide: true});
assert.equal(synthetic.error, undefined, `cannot start configured Python interpreter: ${python}`);
assert.equal(synthetic.status, 0, synthetic.stderr || synthetic.stdout);
assert.deepEqual(JSON.parse(synthetic.stdout),
  {size: [640, 480], left: [255, 0, 0, 255], right: [0, 255, 0, 255]},
  "TH13 synthetic title.anm must compose both source_x layers");

console.log(JSON.stringify({ hostArtworkAdapterCoverage: "PASS", games: report.games,
  th13SyntheticCard: "PASS" }));
