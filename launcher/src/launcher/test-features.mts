/** A local visibility preference for early-access games and features. */
export const TEST_FEATURES_KEY = 'eagler-touhou-beta-opt-in-v1';

export function testFeaturesEnabled(owner: Pick<Window, 'localStorage'>): boolean {
  try { return owner.localStorage.getItem(TEST_FEATURES_KEY) === '1'; }
  catch { return false; }
}

export function reloadTestFeatures(owner: Pick<Window, 'localStorage' | 'location'>, enabled: boolean): void {
  if (enabled) owner.localStorage.setItem(TEST_FEATURES_KEY, '1');
  else owner.localStorage.removeItem(TEST_FEATURES_KEY);
  owner.location.reload();
}
