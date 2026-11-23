// Prepare the TH16 host content directory: th16.dat → th16.data and the
// /bgm-ogg OGG set. OGG tracks are taken from an existing --original/bgm-ogg
// directory when present; otherwise they are encoded from retail thbgm.dat with
// ffmpeg (libvorbis q5) following the canonical thbgm.fmt layout. Each OGG
// must decode to exactly the PCM frame count of its thbgm.fmt record; the
// Runtime mixer rejects tracks whose length differs.
import { copyFile, mkdir, readFile, rename, stat, writeFile } from "node:fs/promises";
import { existsSync } from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { createHash } from "node:crypto";
import { spawnSync } from "node:child_process";
import { TH16_MUSIC_FILES, TH16_MUSIC_LAYOUT } from "../lib/th16-content-layout.mjs";

const PCM_BYTES_PER_FRAME = 4;

function argument(name, fallback) {
  const inline = process.argv.find(value => value.startsWith(`${name}=`));
  if (inline) return inline.slice(name.length + 1);
  const index = process.argv.indexOf(name);
  return index >= 0 ? process.argv[index + 1] ?? fallback : fallback;
}

const originalArgument = argument("--original", "");
if (!originalArgument) throw new Error("Usage: node scripts/prepare-th16-content.mjs --original=DIR --output=DIR");
const original = resolve(originalArgument);
const output = resolve(argument("--output", resolve("assets-ogg")));
const sourceArchive = join(original, "th16.dat");
const sourceMusic = join(original, "thbgm.dat");
const sourceOgg = join(original, "bgm-ogg");
const outputArchive = join(output, "th16.data");
const outputMusic = join(output, "bgm-ogg");
const digest = bytes => createHash("sha256").update(bytes).digest("hex");

if (TH16_MUSIC_LAYOUT.length !== 18 || TH16_MUSIC_FILES.length !== TH16_MUSIC_LAYOUT.length) {
  throw new Error("Invalid canonical TH16 music layout");
}
const archiveInfo = await stat(sourceArchive);
if (!archiveInfo.isFile() || archiveInfo.size === 0) throw new Error("Missing TH16 resource: th16.dat");

function ffmpegVersion() {
  const result = spawnSync("ffmpeg", ["-hide_banner", "-version"], { encoding: "utf8", windowsHide: true });
  if (result.error) throw new Error(`ffmpeg is required on PATH to encode thbgm.dat: ${result.error.message}`);
  if (result.status !== 0) throw new Error(`ffmpeg -version failed: ${result.stderr || result.stdout}`);
  return (result.stdout ?? "").split(/\r?\n/, 1)[0].trim();
}

function encodeOgg(pcm, name) {
  const result = spawnSync("ffmpeg", [
    "-hide_banner", "-loglevel", "error",
    "-f", "s16le", "-ar", "44100", "-ac", "2", "-i", "pipe:0",
    "-c:a", "libvorbis", "-q:a", "5", "-f", "ogg", "pipe:1",
  ], { input: pcm, windowsHide: true, maxBuffer: 64 * 1024 * 1024 });
  if (result.error) throw new Error(`ffmpeg OGG encode failed for ${name}: ${result.error.message}`);
  if (result.status !== 0) throw new Error(`ffmpeg OGG encode failed for ${name}: ${result.stderr || result.stdout}`);
  if (!result.stdout?.length) throw new Error(`ffmpeg emitted empty OGG for ${name}`);
  return new Uint8Array(result.stdout);
}

const useExistingOgg = TH16_MUSIC_FILES.every(name => existsSync(join(sourceOgg, name)));
let musicBytes = null, ffmpeg = null;
if (!useExistingOgg) {
  if (!existsSync(sourceMusic)) throw new Error("Missing TH16 music: provide --original/bgm-ogg/*.ogg or --original/thbgm.dat");
  musicBytes = await readFile(sourceMusic);
  if (TH16_MUSIC_LAYOUT.some(row => row.offset < 16 || row.length <= 0 || row.loop < 0 || row.loop >= row.length ||
      row.offset + row.length > musicBytes.length || row.offset % PCM_BYTES_PER_FRAME ||
      row.loop % PCM_BYTES_PER_FRAME || row.length % PCM_BYTES_PER_FRAME)) {
    throw new Error("Canonical TH16 music layout does not match the supplied retail thbgm.dat");
  }
  ffmpeg = ffmpegVersion();
}

await mkdir(outputMusic, { recursive: true });
const archiveBytes = await readFile(sourceArchive);
if (!existsSync(outputArchive) || digest(await readFile(outputArchive)) !== digest(archiveBytes)) {
  await copyFile(sourceArchive, outputArchive);
}
const files = { "th16.data": { bytes: archiveBytes.length, sha256: digest(archiveBytes) } };
const tracks = [];
for (const [index, name] of TH16_MUSIC_FILES.entries()) {
  const row = TH16_MUSIC_LAYOUT[index];
  const target = join(outputMusic, name);
  let bytes, action;
  if (useExistingOgg) {
    bytes = await readFile(join(sourceOgg, name));
    action = "copied";
    if (resolve(sourceOgg, name) !== target) await writeFile(target, bytes);
  } else if (existsSync(target)) {
    bytes = await readFile(target);
    action = "reused";
  } else {
    bytes = encodeOgg(musicBytes.subarray(row.offset, row.offset + row.length), name);
    action = "encoded";
    const temporary = `${target}.tmp-${process.pid}`;
    await writeFile(temporary, bytes);
    await rename(temporary, target);
  }
  if (Buffer.from(bytes.subarray(0, 4)).toString() !== "OggS") throw new Error(`Invalid OGG resource: ${name}`);
  files[`bgm-ogg/${name}`] = { bytes: bytes.length, sha256: digest(bytes) };
  tracks.push({ filename: name.replace(/\.ogg$/i, ".wav"), oggPath: `bgm-ogg/${name}`, offset: row.offset, loop: row.loop, length: row.length,
    frames: row.length / PCM_BYTES_PER_FRAME, loopFrame: row.loop / PCM_BYTES_PER_FRAME, action });
}
await mkdir(dirname(join(output, "provenance.json")), { recursive: true });
await writeFile(join(output, "provenance.json"), JSON.stringify({
  schema: "eagler-touhou/prepared-content/1", game: "th16", files,
  source: { archive: { name: basename(sourceArchive), ...files["th16.data"] }, music: useExistingOgg ? { kind: "ogg-directory" } : { kind: "thbgm.dat", ffmpeg, encoder: { codec: "libvorbis", quality: 5 } } },
  tracks,
}, null, 2) + "\n");
console.log(JSON.stringify({ game: "th16", output, files: Object.keys(files).length, music: useExistingOgg ? "copied" : "encoded" }));
