import { readFile, stat } from "node:fs/promises";
import { resolve } from "node:path";

import { validateCompletionReport } from "./completion-report.mjs";
import { verifyReleaseManifest } from "./release-manifest.mjs";
import { verifyRuntimeRelease } from "./runtime-release.mjs";
import { PRODUCT_GAMES } from "./contracts/product-catalog.mjs";

async function requireDirectory(path, label) {
  if (!(await stat(path)).isDirectory()) throw new Error(`release bundle directory missing: ${label}`);
}

async function requireFile(path, label) {
  if (!(await stat(path)).isFile()) throw new Error(`release bundle file missing: ${label}`);
}

export async function verifyReleaseBundle(root) {
  const target = resolve(root);
  const manifest = await verifyReleaseManifest(target);
  const deployment = JSON.parse(await readFile(resolve(target, "deployment.json"), "utf8"));
  if (deployment.format !== "eagler-touhou-release-bundle/2") throw new Error("invalid release bundle deployment manifest");
  const games = Object.keys(PRODUCT_GAMES);
  if (JSON.stringify(deployment.games) !== JSON.stringify(games)) {
    throw new Error("formal release bundle must contain every registered product in canonical order");
  }
  if (JSON.stringify(manifest.parameters?.games) !== JSON.stringify(games)) {
    throw new Error("Release Manifest game selection does not match release bundle");
  }
  for (const directory of ["hosted-site", "import-site", "runtime-release", "game-package", "offline-zip"]) {
    await requireDirectory(resolve(target, directory), directory);
  }
  await verifyRuntimeRelease(resolve(target, "runtime-release"));
  for (const file of ["input-manifest.json", "deployment.json", "verification-report.json", "checksums.txt", "release-manifest.json"]) {
    await requireFile(resolve(target, file), file);
  }
  const report = validateCompletionReport(JSON.parse(await readFile(resolve(target, "verification-report.json"), "utf8")));
  if (report.completion.RELEASED !== "no") throw new Error("local release candidate must not claim publication");
  for (const game of games) {
    await requireFile(resolve(target, "game-package", game, "package.json"), `${game} Package Descriptor`);
    await requireFile(resolve(target, "offline-zip", `${game}.zip`), `${game} offline ZIP`);
  }
  return { releaseId: manifest.releaseId, games, completion: report.completion };
}
