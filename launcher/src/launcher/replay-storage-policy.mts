import {gameIdForProduct, isMultiplayerProductId, PRODUCT_GAMES, type ProductId} from '../contracts/product-catalog.mjs';
import {isValidReplayName} from './replay-files.mjs';

export function replayFilePolicy(product: ProductId) {
  const game = gameIdForProduct(product), mp = isMultiplayerProductId(product);
  const prefix = product === 'th15mp' ? 'th15mp' : PRODUCT_GAMES[game].replay.prefix;
  return {prefix, sharedExtensionSlot: ['th09', 'th10'].includes(game) || !mp && game === 'th08',
    validName: (name: string) => isValidReplayName(prefix, name) || product === 'th15mp' && isValidReplayName('th15', name)};
}

/** Compatibility with the shipped native IDBFS namespaces, not a new store. */
export function replayStorageLocation(product: ProductId, language = 'ja') {
  const game = gameIdForProduct(product), mp = isMultiplayerProductId(product);
  const database = PRODUCT_GAMES[game].storage.saveRoot + (mp ? (['th06', 'th07', 'th08', 'th10'].includes(game) ? '-multiplayer' : 'mp') : '');
  return {game, database, root: database + (game === 'th10' || game === 'th20' ? (language === 'lang_zh-hans' ? '/chs' : '/jp') : ''), ...replayFilePolicy(product)};
}
