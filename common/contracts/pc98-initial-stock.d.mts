export const PC98_INITIAL_STOCK: Readonly<{spares:number,bombs:number}>;
export function isPc98StockProduct(product:unknown): boolean;
export function validPc98InitialSpares(value:unknown): value is number;
export function validPc98InitialBombs(value:unknown): value is number;
