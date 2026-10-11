export {HOST_SITE_ARTWORK_FILES} from "./frontend-static-manifest.mjs";
export {hostArtworkFiles} from "./host-artwork.mjs";
import { resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {frontendSelection, reactFrontendArtifactDirectory, readReactFrontendArtifact} from "./react-frontend-artifact.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
export const FRONTEND_SELECTION = frontendSelection();
// React owns all current browser UI. Historical compatibility readers live
// inside the sealed React artifact, never in a separately built Launcher.
export const REACT_FRONTEND_ARTIFACT = await readReactFrontendArtifact({
  directory: reactFrontendArtifactDirectory({project}),
  expectedMountPath: process.env.EAGLER_REACT_MOUNT_PATH,
});

// Browser entrypoints are the only JavaScript URLs owned manually. Their full
// static ESM dependency closure is derived from source, so adding a module
// cannot accidentally publish an online-only launcher or omit it from precache.
export const BROWSER_MODULE_ENTRYPOINTS = REACT_FRONTEND_ARTIFACT.browserEntrypoints;
export const BROWSER_MODULE_FILES = REACT_FRONTEND_ARTIFACT.browserModules;

export const FRONTEND_PACKAGE_FILES = REACT_FRONTEND_ARTIFACT.packageFiles;
export const APP_SHELL_FILES = REACT_FRONTEND_ARTIFACT.shellFiles;
export const PUBLIC_ASSET_FILES = Object.freeze(FRONTEND_PACKAGE_FILES.filter(path => path.startsWith("assets/")));

export function resolveFrontendPackageSource(path) {
  if (!FRONTEND_PACKAGE_FILES.includes(path)) throw new Error(`unknown frontend package file: ${path}`);
  return REACT_FRONTEND_ARTIFACT.resolveSource(path);
}
