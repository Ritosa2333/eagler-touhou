import { resolve } from "node:path";
import { fileURLToPath } from "node:url";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));

// Host-private frontend resources are intentionally absent from Git. When
// present, local preview and server assembly publish them at stable URLs; when
// absent, the Launcher hides the corresponding optional control.
export const PRIVATE_FRONTEND_ASSETS = Object.freeze([
  Object.freeze({ source: "private-assets/donation.webp", target: "assets/donation.webp" }),
]);

export function privateFrontendAssetSource(target) {
  const entry = PRIVATE_FRONTEND_ASSETS.find(asset => asset.target === target);
  return entry ? resolve(project, entry.source) : null;
}
