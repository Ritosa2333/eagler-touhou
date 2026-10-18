/** Stable read URLs used by released packages. They all resolve to this one source owner. */
const names=new Set(['legacy-game-pack.mjs','legacy-import-storage.mjs','legacy-package-adapter.mjs','runtime-generation-reader.js']);
export function compatibilitySourcePath(path) {
  if (!path.startsWith('legacy/')) return null;
  const name=path.slice('legacy/'.length);
  return names.has(name)?`compat/${name}`:null;
}
