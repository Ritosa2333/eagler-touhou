import {TH128_MULTIPLAYER_PREVIEW_PROFILE, th128MultiplayerPreviewGeneration, validateSiteManifest, type SiteManifest} from '../../src/contracts/site-manifest.mts';
import {RESOURCE_MODE_IMPORT} from '../../src/contracts/resource-mode.mts';
import {RUNTIME_MANIFEST_FILE} from '../../src/contracts/runtime-generations.mts';
import {createLocalProductManifest, type GameId} from '../../src/contracts/product-catalog.mts';

function loopback(value: string, protocols: readonly string[]): boolean {
  try {
    const url = new URL(value);
    return protocols.includes(url.protocol) && ['127.0.0.1', '[::1]', 'localhost'].includes(url.hostname) &&
      !url.username && !url.password && !url.hash;
  } catch {return false;}
}

/** UI disclosure requires an explicit, already validated Host and its local Relay. */
export function th128LocalRoomAvailable(host: SiteManifest | null | undefined, launcherUrl: string): boolean {
  return !!host && host.profile === TH128_MULTIPLAYER_PREVIEW_PROFILE && host.shared.testBuild === true &&
    host.shared.resourceMode === RESOURCE_MODE_IMPORT && host.shared.runtimeManifest === RUNTIME_MANIFEST_FILE &&
    !!th128MultiplayerPreviewGeneration(host.games.th128?.multiplayerRuntime) &&
    loopback(launcherUrl, ['http:', 'https:']) && !!host.shared.netplayRelay &&
    [host.shared.netplayRelay, ...(host.shared.netplayRelayFallbacks ?? [])].every(url => loopback(url, ['ws:', 'wss:']));
}
/** Main5639–5655 uses immutable code only when the actual Host advertises it.
 * Otherwise the validated Host or unchanged built-in catalog supplies the live
 * same-origin entry. Installed DATA is still independently generation-pinned. */
export function liveRuntimeEntryAuthorized(plan: {
  game: GameId; runtimeVariant: 'normal' | 'multiplayer'; entry: string;
  runtimeHost?: SiteManifest; developmentRuntimeHost?: SiteManifest;
  localCatalogRuntime?: boolean;
}, baseUrl: string): boolean {
  try {
    const supplied = plan.runtimeHost ?? plan.developmentRuntimeHost;
    const manifest = supplied ? validateSiteManifest(supplied) : plan.localCatalogRuntime ? createLocalProductManifest() : null;
    if (!manifest || ('runtimeManifest' in manifest.shared && manifest.shared.runtimeManifest)) return false;
    const game = manifest.games[plan.game];
    if (!game) return false;
    const declared = plan.runtimeVariant === 'multiplayer' ? ('multiplayerRuntime' in game ? game.multiplayerRuntime : undefined) : game.runtime;
    if (typeof declared !== 'string' || !declared) return false;
    const entry = new URL(plan.entry, baseUrl), base = new URL(baseUrl);
    return entry.href === new URL(declared, baseUrl).href && entry.origin === base.origin &&
      ['https:', 'http:'].includes(entry.protocol) && !entry.username && !entry.password && !entry.hash;
  } catch {return false;}
}

export function th075SplitAssetUrl(plan: {
  game: GameId; runtimeVariant: 'normal' | 'multiplayer'; entry: string;
  runtimeHost?: SiteManifest; publishedRuntime: boolean;
}, baseUrl: string) {
  const host = validateSiteManifest(plan.runtimeHost);
  const game = host.games.th075;
  const split = game?.th075MultiplayerAssets;
  const entry = new URL(plan.entry, baseUrl);
  if (plan.game !== 'th075' || plan.runtimeVariant !== 'multiplayer' || !game?.multiplayerRuntime || !split ||
      plan.publishedRuntime !== !!host.shared.runtimeManifest ||
      new URL(game.multiplayerRuntime, baseUrl).href !== entry.href ||
      entry.origin !== new URL(baseUrl).origin || !/^https?:$/.test(entry.protocol) || entry.username || entry.password || entry.hash)
    throw new Error('Host does not authorize TH075 multiplayer assets');
  return {url: new URL(split.path, baseUrl), sha256: split.sha256};
}

export function requireTh128MultiplayerPreview(plan: {
  entry: string; fileOnly?: boolean; publishedRuntime: boolean; runtimeHost?: SiteManifest;
}, baseUrl: string): string {
  const denied = () => new Error('Host does not authorize TH128 multiplayer preview');
  const base = new URL(baseUrl), entry = new URL(plan.entry, base);
  if (plan.fileOnly || !plan.publishedRuntime || !['http:', 'https:'].includes(base.protocol) ||
      !['127.0.0.1', '[::1]', 'localhost'].includes(base.hostname) || base.username || base.password) throw denied();
  let host: SiteManifest;
  try {host = validateSiteManifest(plan.runtimeHost);} catch {throw denied();}
  const declared = host.games.th128?.multiplayerRuntime;
  const generation = th128MultiplayerPreviewGeneration(declared);
  if (host.profile !== TH128_MULTIPLAYER_PREVIEW_PROFILE || host.shared.testBuild !== true ||
      host.shared.resourceMode !== RESOURCE_MODE_IMPORT || host.shared.runtimeManifest !== RUNTIME_MANIFEST_FILE ||
      !generation || !declared ||
      entry.href !== new URL(declared, base).href) throw denied();
  return generation;
}

export function requireTh128PreviewRoomGeneration(roomUrl: string | undefined, generation: string): void {
  if (roomUrl && roomUrl.length <= 2048) {
    try {
      const url = new URL(roomUrl), ids = url.searchParams.getAll('runtimeGeneration');
      if (loopback(roomUrl, ['ws:', 'wss:']) && ids.length === 1 && ids[0] === generation) return;
    } catch { /* Invalid endpoints are rejected with the same room error. */ }
  }
  throw new Error('TH128 multiplayer room Runtime generation does not match the preview');
}
