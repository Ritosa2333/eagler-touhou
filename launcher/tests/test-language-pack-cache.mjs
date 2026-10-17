import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import { createHash } from "node:crypto";
import { mkdtemp, mkdir, readFile, rm, stat, writeFile } from "node:fs/promises";
import { basename, resolve } from "node:path";
import { tmpdir } from "node:os";
import { fileURLToPath } from "node:url";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const temporary = await mkdtemp(resolve(tmpdir(), "eagler-language-cache-test-"));

try {
  const archive = resolve(temporary, "game.dat");
  const font = resolve(temporary, "font.otf");
  const output = resolve(temporary, "cache");
  const packUrl = "language/lang_en.zip";
  const packPath = resolve(output, ...packUrl.split("/"));
  const packBytes = Buffer.from("cached-language-pack-test");
  await writeFile(archive, "fixture");
  await writeFile(font, "font-fixture");
  await mkdir(resolve(packPath, ".."), { recursive: true });
  await writeFile(packPath, packBytes);

  const fileInfo = await stat(archive);
  const fontInfo = await stat(font);
  const builderFiles = [
    resolve(project, "scripts/prepare-th06-language-pack.mjs"),
    resolve(project, "scripts/subset-font.py"),
    resolve(project, "integrations/thcrap.mjs"),
    resolve(project, "server/thcrap-compiler.mjs"),
    resolve(project, "server/thcrap-ascii-contract.mjs"),
    resolve(project, "server/thcrap-string-contract.mjs"),
    resolve(project, "server/thcrap-static-pack.mjs"),
    resolve(project, "server/thtk-runner.mjs"),
    resolve(project, "lib/content-definition.mjs"),
  ];
  const builders = [];
  for (const path of builderFiles) {
    const info = await stat(path);
    builders.push({ name: basename(path), bytes: info.size, mtimeMs: Math.trunc(info.mtimeMs) });
  }
  const fingerprint = createHash("sha256").update(JSON.stringify({
    schema: "eagler-touhou/language-pack-cache-input/1",
    game: "th06",
    language: "lang_en",
    repository: null,
    fontName: "Unifont",
    archives: [{ name: basename(archive), bytes: fileInfo.size, mtimeMs: Math.trunc(fileInfo.mtimeMs) }],
    font: { name: basename(font), bytes: fontInfo.size, mtimeMs: Math.trunc(fontInfo.mtimeMs) },
    builders,
  })).digest("hex");
  const digest = createHash("sha256").update(packBytes).digest("hex");
  await writeFile(resolve(output, "catalog.json"), `${JSON.stringify({
    schema: "eagler-touhou/thcrap-static-catalog/1",
    game: "th06",
    runtimeVersion: "0123456789abcdef",
    languages: [{ id: "lang_en", title: "English", pack: { url: packUrl, bytes: packBytes.length, sha256: digest, files: 1 } }],
  }, null, 2)}\n`);
  await writeFile(resolve(output, "lang_en.cache.json"), `${JSON.stringify({
    schema: "eagler-touhou/language-pack-cache/1",
    fingerprint,
    pack: packUrl,
    sha256: digest,
    bytes: packBytes.length,
  }, null, 2)}\n`);

  const result = spawnSync(process.execPath, [
    resolve(project, "scripts/prepare-th06-language-pack.mjs"),
    "--game", "th06",
    "--language", "lang_en",
    "--archive", archive,
    "--output", output,
    "--font-file", font,
    "--thdat", resolve(temporary, "missing-thdat"),
    "--thmsg", resolve(temporary, "missing-thmsg"),
  ], { encoding: "utf8", windowsHide: true });
  assert.equal(result.status, 0, result.stderr || result.stdout);
  const payload = JSON.parse(result.stdout.trim());
  assert.equal(payload.cached, true);
  assert.equal(payload.sha256, digest);
  assert.equal(await readFile(packPath, "utf8"), packBytes.toString("utf8"));
  console.log("Language pack persistent cache: PASS");
} finally {
  await rm(temporary, { recursive: true, force: true });
}
