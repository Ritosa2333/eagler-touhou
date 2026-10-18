import { readFile, stat } from "node:fs/promises";
import { resolve } from "node:path";

import { validateCompletionReport } from "./completion-report.mjs";
import { verifyReleaseManifest } from "./release-manifest.mjs";
import { verifyRuntimeRelease } from "./runtime-release.mjs";
import { RESOURCE_MODE_EXTERNAL } from "./contracts/resource-mode.mjs";
import { validateReleaseCatalog } from "./contracts/release-catalog.mjs";
import { FORMAL_RELEASE_GAMES } from "./release-plan.mjs";

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
  const games = [...FORMAL_RELEASE_GAMES];
  if (JSON.stringify(deployment.games) !== JSON.stringify(games)) {
    throw new Error("formal release bundle must contain every non-test product in canonical order");
  }
  if (JSON.stringify(manifest.parameters?.games) !== JSON.stringify(games)) {
    throw new Error("Release Manifest game selection does not match release bundle");
  }
  for (const directory of ["hosted-site", "external-site", "import-site", "runtime-release", "game-package", "offline-zip"]) {
    await requireDirectory(resolve(target, directory), directory);
  }
  const externalRoot = resolve(target, "external-site");
  const externalDeployment = JSON.parse(await readFile(resolve(externalRoot, "deployment.json"), "utf8"));
  if (externalDeployment.resourceMode !== RESOURCE_MODE_EXTERNAL) throw new Error("release bundle external-site has the wrong resource mode");
  if (!Array.isArray(externalDeployment.files) || externalDeployment.files.some(file =>
    file.path?.startsWith("games/") || file.path?.startsWith("shared/"))) {
    throw new Error("release bundle external-site contains game/shared payloads");
  }
  const externalCatalog = validateReleaseCatalog(JSON.parse(await readFile(resolve(externalRoot, "release-catalog.json"), "utf8")));
  if (JSON.stringify(Object.keys(externalCatalog.games)) !== JSON.stringify(games)) {
    throw new Error("release bundle external-site must publish every formal-release Package Descriptor");
  }
  for (const [game, entry] of Object.entries(externalCatalog.games)) {
    await requireFile(resolve(externalRoot, entry.descriptor), `${game} external Package Descriptor`);
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
