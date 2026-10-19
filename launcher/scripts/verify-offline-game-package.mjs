#!/usr/bin/env node
import { createHash } from "node:crypto";
import { readFile } from "node:fs/promises";
import { resolve } from "node:path";
import { parsePackageZip } from "../package/package-zip.mjs";

const input = process.argv[2] ? resolve(process.argv[2]) : null;
const expectedGame = String(process.argv[3] || "").toLowerCase();
if (!input || !/^th\d{2}$/.test(expectedGame)) {
  throw new Error("usage: node scripts/verify-offline-game-package.mjs <package.zip> thXX");
}

const bytes = await readFile(input);
const parsed = await parsePackageZip(new Blob([bytes]));
if (parsed.descriptor.game !== expectedGame) {
  throw new Error(`${expectedGame}: package game mismatch: ${parsed.descriptor.game}`);
}
for (const [fileId, declaration] of Object.entries(parsed.descriptor.files)) {
  const file = parsed.files.get(fileId);
  if (!file) throw new Error(`${expectedGame}: package is missing declared file ${fileId}`);
  if (file.bytes !== declaration.bytes) throw new Error(`${expectedGame}: byte length mismatch for ${fileId}`);
  const payload = Buffer.from(await file.blob.arrayBuffer());
  const sha256 = createHash("sha256").update(payload).digest("hex");
  if (sha256 !== declaration.sha256) throw new Error(`${expectedGame}: sha256 mismatch for ${fileId}`);
}
console.log(JSON.stringify({
  game: expectedGame,
  revision: parsed.descriptor.revision,
  files: parsed.files.size,
  archiveBytes: bytes.length,
  verified: true,
}));
