import {PRODUCT_GAMES, gameIdForProduct, isMultiplayerProductId, type ProductId, type GameId} from '../../../src/contracts/product-catalog.mts';
const shortTitles:Record<GameId,string>={hsob:'珠',th01:'灵',th02:'封',th03:'梦',th04:'幻',th05:'怪',th06:'红',th07:'妖',th075:'萃',th08:'永',th09:'花',th10:'风',th11:'地',th12:'星',th13:'神',th14:'针',th15:'绀',th17:'兽',th19:'园',th20:'锦'};
export const bookGameShortTitle=(game:GameId)=>shortTitles[game];

/** Host/publication decides membership and URLs before constructing this view data. */
export interface LibraryProduct {
  id: ProductId;
  gameId: GameId;
  title: string;
  subtitle: string;
  shortTitle: string;
  number: string;
  multiplayer: boolean;
  artwork: string | null;
  href: string;
  hidden?: boolean;
  presentation?: {positionPercent: number; artBrightness: number; artSaturation: number; glowBrightness: number; glowSaturation: number};
  adaptationNotice: boolean;
  sourceRepository: string;
  credit?: {name: string; url: string};
}

export function isPc98Product(product: Pick<LibraryProduct, 'number'>): boolean {
  const number = Number(product.number);
  return Number.isInteger(number) && number <= 5;
}

export type MultiplayerGroup = 'versus' | 'original' | 'fighting' | 'pc98';
export function multiplayerGroup(product: Pick<LibraryProduct, 'gameId'>): MultiplayerGroup {
  const game = PRODUCT_GAMES[product.gameId];
  if (isPc98Product(game)) return 'pc98';
  if ('inputProfile' in game && game.inputProfile === 'fighting') return 'fighting';
  if ('multiplayer' in game && game.multiplayer.gameplay === 'versus') return 'versus';
  return 'original';
}

/** Same catalog fields as lib/frontend-assets.mjs; no copied product list. */
export function createLibraryProducts(ids: readonly ProductId[], assetUrl: (path: string) => string, hrefForProduct: (id: ProductId) => string): LibraryProduct[] {
  return ids.map(id => {
    const gameId = gameIdForProduct(id), game = PRODUCT_GAMES[gameId], support = game.support;
    return {
      id, gameId, title: game.title, subtitle: game.subtitle, shortTitle:shortTitles[gameId], number: game.number,
      multiplayer: isMultiplayerProductId(id),
      artwork: 'cardArtwork' in game && game.cardArtwork ? assetUrl(`assets/${game.cardArtwork}`) : null,
      href: hrefForProduct(id),
      presentation: 'cardPresentation' in game ? game.cardPresentation : undefined,
      adaptationNotice: 'adaptationNotice' in support && support.adaptationNotice === 'early-test',
      sourceRepository: support.sourceRepository,
      credit: 'credit' in support ? support.credit : undefined,
    };
  });
}
