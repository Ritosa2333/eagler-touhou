#!/usr/bin/env node
import { readFile } from "node:fs/promises";
import { assertReactFrontendGameCoverage, readReactFrontendArtifact } from "../lib/react-frontend-artifact.mjs";
import { validateSiteManifest } from "../lib/contracts/site-manifest.mjs";

const [directory, manifestPath] = process.argv.slice(2);
if (!directory || !manifestPath || process.argv.length !== 4) {
  throw new Error("usage: node scripts/verify-frontend-game-catalog.mjs <frontend-client-directory> <site-manifest.json>");
}
const site = validateSiteManifest(JSON.parse(await readFile(manifestPath, "utf8")));
const artifact = await readReactFrontendArtifact({directory});
const gameIds = Object.keys(site.games);
assertReactFrontendGameCoverage(artifact, gameIds);
console.log(`React frontend catalog covers all ${gameIds.length} Site Manifest games`);
