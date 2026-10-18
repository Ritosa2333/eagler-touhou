import { basename, extname } from "node:path/posix";

export const FORBIDDEN_PUBLIC_TOP_LEVEL = Object.freeze(new Set([
  "android-webview-lab",
  "archive",
  "artifacts",
  "design",
  "screenshots",
]));

const FORBIDDEN_EPHEMERAL_EXTENSIONS = Object.freeze(new Set([
  ".log",
  ".pid",
  ".tmp",
]));

export function publicTreePolicyViolation(input) {
  const path = String(input).replaceAll("\\", "/").replace(/^\.\//, "");
  const segments = path.split("/").filter(Boolean);
  if (!segments.length) return "empty public path";
  if (FORBIDDEN_PUBLIC_TOP_LEVEL.has(segments[0])) {
    return `investigation/local-output directory is not public source: ${segments[0]}`;
  }
  const name = basename(path);
  if (name === ".DS_Store" || name === "Thumbs.db" || FORBIDDEN_EPHEMERAL_EXTENSIONS.has(extname(name).toLowerCase())) {
    return `ephemeral local file is not public source: ${name}`;
  }
  if (/^host\/config\/site-features\.(?!default\.json$).+\.json$/i.test(path)) {
    return `site-specific self-host configuration is not public source: ${path}`;
  }
  if (/^ref-[^/]+\.(?:png|jpe?g|webp)$/i.test(name)) {
    return `investigation reference image is not public source: ${name}`;
  }
  return "";
}
