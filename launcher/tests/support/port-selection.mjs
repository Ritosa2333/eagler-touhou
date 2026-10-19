import {PRODUCT_GAMES} from '../../lib/contracts/product-catalog.mjs';

/** Explicit audit selection; ordinary test runs continue to cover every game. */
export function selectedPortGames(available = Object.keys(PRODUCT_GAMES), env = process.env) {
  const value = env.EAGLER_PORT_CHECK_GAMES;
  if (value === undefined) return [...available];
  const selected = JSON.parse(value);
  if (!Array.isArray(selected) || !selected.length || selected.some(game => !Object.hasOwn(PRODUCT_GAMES, game)))
    throw new Error('EAGLER_PORT_CHECK_GAMES must be a nonempty JSON array of registered games');
  return available.filter(game => selected.includes(game));
}
