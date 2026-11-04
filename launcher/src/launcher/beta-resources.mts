import type { SiteManifest, SiteLanguageOption } from '../contracts/site-manifest.mjs';
import type { ReleaseCatalog } from '../contracts/release-catalog.mjs';
import type { GameId } from '../contracts/product-catalog.mjs';

/** Reuse stable content URLs only when the published identities agree. */
export function shareBetaResources(
  site: SiteManifest,
  catalog: ReleaseCatalog,
  stable: SiteManifest,
  stableCatalog: ReleaseCatalog,
  stableBase: string,
) {
  const next = structuredClone(site),
    releases = structuredClone(catalog);
  for (const key of ['vanillaFont', 'unicodeFont'] as const) {
    const source = stable.shared[key];
    if (source && site.shared[key] === source) next.shared[key] = new URL(source, stableBase).href;
  }
  for (const game of Object.keys(next.games) as GameId[]) {
    const entry = next.games[game],
      original = stable.games[game];
    if (!entry) continue;
    const release = stableCatalog.games[game];
    if (release && release.revision === catalog.games[game]?.revision)
      releases.games[game] = {
        ...release,
        descriptor: new URL(release.descriptor, stableBase).pathname + new URL(release.descriptor, stableBase).search,
      };
    if (!original) continue;
    if (
      entry.music.ogg &&
      original.music.ogg &&
      JSON.stringify(entry.music.ogg.sha256) === JSON.stringify(original.music.ogg.sha256) &&
      JSON.stringify(entry.music.ogg.files) === JSON.stringify(original.music.ogg.files)
    ) {
      entry.music.ogg = {
        ...original.music.ogg,
        base: new URL(String(original.music.ogg.base ?? './'), stableBase).href,
      };
    }
    for (const name of ['languageOptions', 'languages'] as const) {
      const languages = entry[name];
      if (!languages) continue;
      entry[name] = languages.map((language: SiteLanguageOption) => {
        const source = original[name]?.find((item) => item.id === language.id);
        return language.pack && source?.pack && language.pack.sha256 === source.pack.sha256
          ? { ...language, pack: { ...language.pack, url: new URL(source.pack.url, stableBase).href } }
          : language;
      });
    }
  }
  return { siteManifest: next, releaseCatalog: releases };
}
