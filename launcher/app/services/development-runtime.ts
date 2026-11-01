/** An unpublished Runtime is allowed only by explicit validated development
 * Host authority, never merely because a published code manifest is missing. */

import { type GameId, productEnabledForBuild } from '../../src/contracts/product-catalog.mts';
import { type SiteManifest, validateSiteManifest } from '../../src/contracts/site-manifest.mts';
export function isValidatedDevelopmentRuntime(
  plan: {
    game: GameId;
    runtimeVariant: 'normal' | 'multiplayer';
    entry: string;
    publishedRuntime: boolean;
    developmentRuntimeHost?: SiteManifest;
  },
  baseUrl: string,
): boolean {
  if (plan.publishedRuntime || !plan.developmentRuntimeHost) return false;
  try {
    const host = validateSiteManifest(plan.developmentRuntimeHost),
      game = host.games[plan.game];
    if (
      host.profile !== 'web-development' ||
      host.shared.testBuild !== true ||
      host.shared.resourceMode !== 'hosted' ||
      host.shared.runtimeManifest != null ||
      !productEnabledForBuild(plan.game, true) ||
      !game
    )
      return false;
    const declared = plan.runtimeVariant === 'multiplayer' ? game.multiplayerRuntime : game.runtime;
    if (!declared) return false;
    const entry = new URL(plan.entry, baseUrl),
      base = new URL(baseUrl);
    return (
      entry.href === new URL(declared, baseUrl).href &&
      entry.origin === base.origin &&
      entry.pathname.startsWith(base.pathname) &&
      !entry.username &&
      !entry.password &&
      !entry.hash
    );
  } catch {
    return false;
  }
}
