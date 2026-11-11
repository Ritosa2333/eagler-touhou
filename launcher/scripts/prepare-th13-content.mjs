import { copyFile, mkdir, readFile, readdir, stat, writeFile } from "node:fs/promises";
import { existsSync, mkdtempSync, rmSync } from "node:fs";
import { join, resolve } from "node:path";
import { tmpdir } from "node:os";
import { createHash } from "node:crypto";
import { spawnSync } from "node:child_process";
import { TH13_DATA_MAGIC, TH13_DATA_SCHEMA, TH13_DATA_LAYOUT } from "../lib/th13-content-layout.mjs";

function argument(name, fallback) {
  const inline = process.argv.find(value => value.startsWith(`${name}=`));
  if (inline) return inline.slice(name.length + 1);
  const index = process.argv.indexOf(name);
  return index >= 0 ? process.argv[index + 1] ?? fallback : fallback;
}

const originalArgument = argument("--original", "");
const rawArgument = argument("--raw", "");
if (!originalArgument && !rawArgument) {
  throw new Error("Usage: node scripts/prepare-th13-content.mjs (--original=DIR | --raw=DIR) --output=DIR");
}

const original = originalArgument ? resolve(originalArgument) : "";
const raw = rawArgument ? resolve(rawArgument) : "";
const output = resolve(argument("--output", resolve("assets-ogg")));
const outputMusic = join(output, "bgm-ogg");
const provenancePath = join(output, "provenance.json");
const thdatExe = argument("--thdat", process.env.TH13_THDAT ?? process.env.THDAT ?? "thdat");

const hash = bytes => createHash("sha256").update(bytes).digest("hex");

let extractedDir = "";
let tempWorkDir = null;

if (raw && existsSync(raw)) {
  extractedDir = raw;
} else if (original && existsSync(join(original, "raw"))) {
  extractedDir = join(original, "raw");
} else if (original && existsSync(join(original, "th13.dat"))) {
  tempWorkDir = mkdtempSync(join(tmpdir(), "th13-extract-"));
  extractedDir = tempWorkDir;
  const datPath = join(original, "th13.dat");
  console.log(`Extracting ${datPath} using ${thdatExe}...`);
  const result = spawnSync(thdatExe, ["-x13", datPath], {
    cwd: extractedDir,
    encoding: "utf8",
    windowsHide: true,
  });
  if (result.error) {
    if (tempWorkDir) rmSync(tempWorkDir, { recursive: true, force: true });
    throw new Error(`thdat execution failed: ${result.error.message}`);
  }
  if (result.status !== 0) {
    if (tempWorkDir) rmSync(tempWorkDir, { recursive: true, force: true });
    throw new Error(`thdat failed with code ${result.status}: ${result.stderr || result.stdout}`);
  }
} else if (original && existsSync(original)) {
  // Original itself might be the extracted loose files directory
  extractedDir = original;
} else {
  throw new Error("Cannot locate th13 extracted assets or th13.dat archive");
}

try {
  // Required core assets check
  for (const requiredName of ["default.ecl", "pl00.anm", "title.anm", "ascii.anm", "thbgm.fmt"]) {
    if (!existsSync(join(extractedDir, requiredName))) {
      throw new Error(`Missing required TH13 core asset: ${requiredName}`);
    }
  }

  // Find original Vorbis stream (thbgm.ogg) - fail loudly if missing
  let sourceOgg = "";
  const oggCandidates = [
    join(extractedDir, "thbgm.ogg"),
    original ? join(original, "thbgmogg.dat") : "",
    original ? join(original, "bgm-ogg", "thbgm.ogg") : "",
    original ? join(original, "thbgm.ogg") : "",
    original ? join(original, "assets", "thbgm.ogg") : "",
  ].filter(Boolean);

  for (const candidate of oggCandidates) {
    if (existsSync(candidate)) {
      const candidateStat = await stat(candidate);
      if (candidateStat.isFile() && candidateStat.size > 0) {
        sourceOgg = candidate;
        break;
      }
    }
  }

  if (!sourceOgg) {
    throw new Error(
      "Missing required original Vorbis stream thbgm.ogg. " +
      "Must provide prepared bgm-ogg/thbgm.ogg or thbgm.ogg in original/raw directory."
    );
  }

  // 1. Pack extracted assets into th13.data using TH13PK01 format
  // Include all runtime assets AND thbgm.ogg so /assets/thbgm.ogg is always available
  // even under Launcher music=none mode (satisfies AudioDevice::initialize unconditionally).
  const dirEntries = await readdir(extractedDir);
  const assetNames = dirEntries.filter(name =>
    /^[a-zA-Z0-9_-]+\.(?:png|anm|ecl|std|sht|msg|wav|rpy|fmt|txt|ver)$/.test(name)
  ).sort();

  const files = [];
  const payloads = [];
  let currentOffset = 0;

  for (const name of assetNames) {
    const filePath = join(extractedDir, name);
    const fileStat = await stat(filePath);
    if (!fileStat.isFile() || fileStat.size === 0) continue;
    const fileBytes = await readFile(filePath);
    const sha256 = hash(fileBytes);
    files.push({
      path: `assets/${name}`,
      offset: currentOffset,
      bytes: fileBytes.length,
      sha256,
    });
    payloads.push(fileBytes);
    currentOffset += fileBytes.length;
  }

  // Include thbgm.ogg inside th13.data
  const oggBytes = await readFile(sourceOgg);
  if (oggBytes.length < 4 || oggBytes.toString('ascii', 0, 4) !== 'OggS')
    throw new Error('Music source must be a Vorbis OGG stream, not retail PCM thbgm.dat');
  const oggHash = hash(oggBytes);
  files.push({
    path: "assets/thbgm.ogg",
    offset: currentOffset,
    bytes: oggBytes.length,
    sha256: oggHash,
  });
  payloads.push(oggBytes);
  currentOffset += oggBytes.length;

  const manifestObject = {
    schema: TH13_DATA_SCHEMA,
    game: "th13",
    layout: TH13_DATA_LAYOUT,
    files,
  };

  const manifestBuffer = Buffer.from(JSON.stringify(manifestObject), "utf8");
  const headerBuffer = Buffer.alloc(12);
  headerBuffer.write(TH13_DATA_MAGIC, 0, 8, "ascii");
  headerBuffer.writeUInt32LE(manifestBuffer.length, 8);

  const dataContainer = Buffer.concat([headerBuffer, manifestBuffer, ...payloads]);

  await mkdir(output, { recursive: true });
  await mkdir(outputMusic, { recursive: true });

  const outputDataFile = join(output, "th13.data");
  await writeFile(outputDataFile, dataContainer);

  // 2. Also write separate music stream bgm-ogg/thbgm.ogg for launcher package compatibility
  const destinationOgg = join(outputMusic, "thbgm.ogg");
  await copyFile(sourceOgg, destinationOgg);

  // 3. Write provenance.json
  const provenance = {
    schema: "th13-prepared-content-provenance/1",
    game: "th13",
    dataLayout: TH13_DATA_LAYOUT,
    gameData: {
      path: "th13.data",
      bytes: dataContainer.length,
      sha256: hash(dataContainer),
    },
    music: {
      "bgm-ogg/thbgm.ogg": {
        bytes: oggBytes.length,
        sha256: oggHash,
      },
    },
    files,
  };

  await writeFile(provenancePath, JSON.stringify(provenance, null, 2) + "\n", "utf8");
  console.log(JSON.stringify({
    game: "th13",
    output,
    dataBytes: dataContainer.length,
    files: files.length,
    musicOggBytes: oggBytes.length,
  }));
} finally {
  if (tempWorkDir && existsSync(tempWorkDir)) {
    rmSync(tempWorkDir, { recursive: true, force: true });
  }
}
