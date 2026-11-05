// TH19 native indices: time, rounds, ability, P1/P2 life, P1/P2 barrier, P1/P2 card selection.
export const TH19_VERSUS_RULES = Object.freeze([4,1,1,1,1,1,1,0,0]);
export const TH19_RULE_COUNTS = Object.freeze([7,3,4,5,5,4,4,39,39]);
export function validTh19VersusRules(value) {
  return Array.isArray(value) && value.length===9 && value.every((n,i)=>Number.isInteger(n)&&n>=0&&n<TH19_RULE_COUNTS[i]) && (value[2]!==2 || (value[7]<2 && value[8]<2));
}
