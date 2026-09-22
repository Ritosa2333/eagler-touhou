import { createHash, randomUUID } from "node:crypto";
import { cp, lstat, mkdir, readFile, readlink, realpath, rename, rm, symlink } from "node:fs/promises";
import { dirname, relative, resolve, sep } from "node:path";
import { execFile } from "node:child_process";
import { promisify } from "node:util";
import { publishRuntimeManifest, readRuntimeManifest } from "./runtime-generations.mjs";
import { verifyReleaseManifest } from "./release-manifest.mjs";
import { fileURLToPath } from "node:url";

const run = promisify(execFile);
const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
function inside(parent, child) {
  const path = relative(parent, child);
  return !path || (path !== ".." && !path.startsWith(".." + sep) && !path.startsWith(sep));
}
async function validate(site) {
  await verifyReleaseManifest(site);
  await run(process.execPath, [resolve(project, "scripts/verify-server-build.mjs"), site], { maxBuffer: 8 * 1024 * 1024 });
}
/** Local filesystem publication only. No SSH, guessed host configuration,
 * automatic deletion of old releases, or modifications to the live target.
 * An atomic symlink switch keeps the release's pointers and files coherent. */
export async function deployStaticSite({ source, releases, current }) {
  source = await realpath(source);
  releases = resolve(releases); current = resolve(current);
  if (inside(source, releases) || inside(releases, source) || inside(source, current) || inside(current, source) ||
      inside(current, releases) || inside(releases, current) || current === dirname(current)) {
    throw new Error("Source, releases, and current link must be distinct non-nested paths");
  }
  await mkdir(releases, { recursive: true });
  await mkdir(dirname(current), { recursive: true });
  // Serializes publishers on this deployment store. Do not silently remove a
  // possibly live lock; operator recovery is explicit after a process crash.
  const lock = resolve(releases, ".publication-lock");
  await mkdir(lock);
  const stage = resolve(releases, `.candidate-${randomUUID()}`);
  const link = resolve(dirname(current), `.${current.split(sep).at(-1)}-${randomUUID()}`);
  let previousTarget = null;
  try {
    try {
      const entry = await lstat(current);
      if (!entry.isSymbolicLink()) throw new Error("Current publication must be a symlink, not a directory; migrate the webroot explicitly first");
      previousTarget = await readlink(current);
    } catch (error) { if (error.code !== "ENOENT") throw error; }
    const previous = previousTarget === null ? null : await realpath(current);
    if (previous && !inside(await realpath(releases), previous)) throw new Error("Current release is outside the configured release store");
    await validate(source);
    await cp(source, stage, { recursive: true, dereference: false, errorOnExist: true, force: false });
    if (previous) {
      const manifest = await readRuntimeManifest(stage);
      // Keep every retained immutable generation from the live site, including
      // generations absent from the new pointer. Existing clients may still use
      // them. Default publication never prunes these server archives.
      await publishRuntimeManifest(stage, manifest.groups, { previousSite: previous });
      await run(process.execPath, [resolve(project, "scripts/refresh-deployment-app-shell.mjs"), stage],
        { maxBuffer: 8 * 1024 * 1024 });
    }
    await validate(stage);
    const id = createHash("sha256").update(await readFile(resolve(stage, "release-manifest.json"))).digest("hex");
    const destination = resolve(releases, id);
    let exists = false;
    try { await lstat(destination); exists = true; } catch (error) { if (error.code !== "ENOENT") throw error; }
    if (exists) {
      await validate(destination);
      if (!Buffer.from(await readFile(resolve(stage, "checksums.txt"))).equals(await readFile(resolve(destination, "checksums.txt")))) {
        throw new Error("Existing immutable release identity differs");
      }
    } else await rename(stage, destination);
    // Recheck immediately before commit, also detecting out-of-band switches.
    let observed = null;
    try { observed = await readlink(current); } catch (error) { if (error.code !== "ENOENT") throw error; }
    if (observed !== previousTarget) throw new Error("Current release changed while publishing; refusing to overwrite it");
    await symlink(relative(dirname(current), destination), link, "dir");
    await rename(link, current);
    return { published: true, current, release: destination, previous, retained: true };
  } finally {
    await rm(link, { force: true });
    await rm(stage, { force: true, recursive: true });
    await rm(lock, { recursive: true });
  }
}
