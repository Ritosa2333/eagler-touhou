export const PC98_INITIAL_STOCK=Object.freeze({spares:3,bombs:2});
export const isPc98StockProduct=product=>product==='th02mp'||product==='th04mp';
export const validPc98InitialSpares=value=>Number.isInteger(value)&&value>=0&&value<=5;
export const validPc98InitialBombs=value=>Number.isInteger(value)&&value>=0&&value<=2;
