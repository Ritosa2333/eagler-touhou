import {gameIdForProduct, isMultiplayerProductId, PRODUCT_GAMES, type ProductId} from '../contracts/product-catalog.mjs';
import {isValidReplayName} from './replay-files.mjs';

export function replayFilePolicy(product: ProductId) {
  const game = gameIdForProduct(product), mp = isMultiplayerProductId(product);
  const prefix = product === 'th15mp' ? 'th15mp' : product === 'th12mp' ? 'th12_mp' : PRODUCT_GAMES[game].replay.prefix;
  return {prefix, sharedExtensionSlot: ['th09', 'th10'].includes(game) || !mp && game === 'th08',
    validName: (name: string) => product === 'th12mp' ? /^th12_mp_[a-z0-9_-]+\.rpy$/i.test(name)
      : isValidReplayName(prefix, name) || product === 'th15mp' && isValidReplayName('th15', name)};
}

/** Compatibility with the shipped native IDBFS namespaces, not a new store. */
export function replayStorageLocation(product: ProductId, language = 'ja') {
  const game = gameIdForProduct(product), mp = isMultiplayerProductId(product);
  const database = PRODUCT_GAMES[game].storage.saveRoot + (mp ? (['th06', 'th07', 'th08', 'th10'].includes(game) ? '-multiplayer' : 'mp') : '');
  const root = game === 'th12' ? PRODUCT_GAMES[game].storage.saveRoot : database;
  return {game, database, root: root + (game === 'th10' || game === 'th20' ? (language === 'lang_zh-hans' ? '/chs' : '/jp') : ''), ...replayFilePolicy(product)};
}
