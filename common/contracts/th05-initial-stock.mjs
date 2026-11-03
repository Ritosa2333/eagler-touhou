// PR #143: spares exclude the current life; settings apply to the whole team.
export const TH05_INITIAL_STOCK = Object.freeze({spares:2,bombs:3});
export function validTh05InitialSpares(value) {
 return typeof value==='number'&&Number.isInteger(value)&&value>=0&&value<=5;
}
export function validTh05InitialBombs(value) {
 return typeof value==='number'&&Number.isInteger(value)&&value>=0&&value<=3;
}
