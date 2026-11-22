#!/usr/bin/env node
// Point ports/thsss/build-eagler at a staged THSSS-WASM runtime directory, and
// expose the staged Launcher card for local host-artwork serving.
import { existsSync, lstatSync, mkdirSync, readFileSync, rmSync, symlinkSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const portRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const repoRoot = resolve(portRoot, "../..");
const target = resolve(process.argv[2] || resolve(portRoot, "../../../THSSS-WASM/build-eagler"));
const link = resolve(portRoot, "build-eagler");
const cardName = "thsss-card.webp";
const stagedCard = resolve(target, "artwork", cardName);

if (!existsSync(target)) {
  throw new Error(`staged runtime missing: ${target}`);
}
const manifest = JSON.parse(readFileSync(resolve(target, "manifest.json"), "utf8"));
if (manifest.game !== "thsss" || manifest.entry !== "thsss.html") {
  throw new Error(`${target}/manifest.json is not a thsss directory runtime`);
}
if (!existsSync(resolve(target, "shell.mjs")) || !existsSync(resolve(target, "THSSS.Browser.wasm"))) {
  throw new Error(`${target} is missing shell.mjs or THSSS.Browser.wasm`);
}
if (!existsSync(stagedCard)) {
  throw new Error(`${target}/artwork/${cardName} is missing; re-run npm run stage:eagler in THSSS-WASM`);
}

function replaceSymlink(path, destination, type) {
  if (existsSync(path) || lstatSync(path, { throwIfNoEntry: false })?.isSymbolicLink()) {
    rmSync(path, { recursive: true, force: true });
  }
  symlinkSync(destination, path, type);
}

mkdirSync(portRoot, { recursive: true });
replaceSymlink(link, target, "dir");
console.log(`linked ${link} -> ${target}`);

const contentArtwork = resolve(portRoot, "content", "artwork");
mkdirSync(contentArtwork, { recursive: true });
replaceSymlink(resolve(contentArtwork, cardName), stagedCard, "file");
console.log(`linked content/artwork/${cardName}`);

const hostArtwork = resolve(repoRoot, "games", "host-artwork");
mkdirSync(hostArtwork, { recursive: true });
replaceSymlink(resolve(hostArtwork, cardName), stagedCard, "file");
console.log(`linked games/host-artwork/${cardName}`);
