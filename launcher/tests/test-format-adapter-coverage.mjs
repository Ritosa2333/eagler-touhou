import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import { resolve } from "node:path";

import { PRODUCT_CONTENT } from "../lib/content-definition.mjs";
import { THCRAP_RUNTIME_COMPILER_GAMES } from "../server/thcrap-compiler.mjs";

const root = resolve(import.meta.dirname, "..");

const verifiedOggGames = Object.entries(PRODUCT_CONTENT)
  .filter(([, content]) => content.hostPreparation?.ogg?.kind === "verified-converter")
  .map(([game]) => game)
  .sort();
const python = process.env.PYTHON || "python";
const listed = spawnSync(python, ["scripts/convert_bgm_ogg.py", "--list-games"], {
  cwd: root,
  encoding: "utf8",
});
assert.equal(listed.error, undefined, `unable to start OGG converter coverage probe with ${python}`);
assert.equal(listed.status, 0, listed.stderr || listed.stdout);
assert.deepEqual(JSON.parse(listed.stdout).sort(), verifiedOggGames,
  "every PRODUCT_CONTENT verified-converter declaration must be implemented by convert_bgm_ogg.py, and no undeclared game may leak into that format adapter");

const thcrapCompilerGames = Object.entries(PRODUCT_CONTENT)
  .filter(([, content]) => content.hostPreparation?.languagePack?.kind === "thcrap-runtime-compiler")
  .map(([game]) => game)
  .sort();
assert.deepEqual([...THCRAP_RUNTIME_COMPILER_GAMES].sort(), thcrapCompilerGames,
  "thcrap runtime compiler support must exactly match products that declare that language preparation adapter");

console.log(JSON.stringify({
  formatAdapterCoverage: "PASS",
  verifiedOggGames,
  thcrapCompilerGames,
}));
