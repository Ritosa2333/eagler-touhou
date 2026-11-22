#!/usr/bin/env node
// Point ports/thsss/build-eagler at a staged THSSS-WASM runtime directory.
import { existsSync, lstatSync, mkdirSync, readFileSync, rmSync, symlinkSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const portRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const target = resolve(process.argv[2] || resolve(portRoot, "../../../THSSS-WASM/build-eagler"));
const link = resolve(portRoot, "build-eagler");

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

mkdirSync(portRoot, { recursive: true });
if (existsSync(link) || lstatSync(link, { throwIfNoEntry: false })?.isSymbolicLink()) {
  rmSync(link, { recursive: true, force: true });
}
symlinkSync(target, link, "dir");
console.log(`linked ${link} -> ${target}`);
