import { extname } from "node:path";

const MIME_BY_EXTENSION = new Map([
  [".html", "text/html; charset=utf-8"],
  [".js", "text/javascript; charset=utf-8"],
  [".mjs", "text/javascript; charset=utf-8"],
  [".json", "application/json; charset=utf-8"],
  [".webmanifest", "application/manifest+json; charset=utf-8"],
  [".css", "text/css; charset=utf-8"],
  [".txt", "text/plain; charset=utf-8"],
  [".md", "text/markdown; charset=utf-8"],
  [".wasm", "application/wasm"],
  [".data", "application/octet-stream"],
  [".zip", "application/zip"],
  [".ttc", "font/collection"],
  [".otf", "font/otf"],
  [".woff2", "font/woff2"],
  [".ogg", "audio/ogg"],
  [".wav", "audio/wav"],
  [".png", "image/png"],
  [".jpg", "image/jpeg"],
  [".jpeg", "image/jpeg"],
  [".webp", "image/webp"],
  [".svg", "image/svg+xml"],
  [".ico", "image/x-icon"],
]);

const COMPRESSIBLE = new Set([
  ".html", ".js", ".mjs", ".json", ".css", ".txt", ".md", ".wasm", ".data", ".ttc", ".otf", ".svg",
]);

export function staticContentType(path) {
  return MIME_BY_EXTENSION.get(extname(path).toLowerCase()) || "application/octet-stream";
}

export function staticContentCompressible(path, bytes) {
  return Number.isFinite(bytes) && bytes >= 1024 && COMPRESSIBLE.has(extname(path).toLowerCase());
}

export function staticContentCacheControl(path) {
  const extension = extname(path).toLowerCase();
  const immutableRuntime = /(?:^|\/)runtime\/(?:[A-Za-z0-9_-]+\/)+[a-f0-9]{64}\/[A-Za-z0-9][A-Za-z0-9._/-]*$/.test(path) &&
    !path.split("/").some(part => part === ".." || part === ".");
  const contentAddressedPack = extension === ".zip" && /[a-f0-9]{24}\.zip$/i.test(path);
  return immutableRuntime || contentAddressedPack
    ? "public, max-age=31536000, immutable"
    : "public, max-age=0, must-revalidate";
}
