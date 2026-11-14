import {gameIdForProduct, isMultiplayerProductId, type ProductId} from '../../src/contracts/product-catalog.mts';
import type {SiteManifest} from '../../src/contracts/site-manifest.mts';

/** PC-98 multiplayer entries become available when the site publishes their runtime. */
export function isPc98MultiplayerCandidate(product: ProductId): boolean {
  return isMultiplayerProductId(product) && ['th01mp','th02mp','th03mp','th04mp'].includes(product);
}
export function publishedPc98Multiplayer(product: ProductId, manifest: SiteManifest | null): boolean {
  if (!isPc98MultiplayerCandidate(product)) return false;
  const runtime = manifest?.games[gameIdForProduct(product)]?.multiplayerRuntime;
  return typeof runtime === 'string' && runtime.length > 0;
}
