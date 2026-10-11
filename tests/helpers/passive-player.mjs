// These isolated Enemy/Bullet/Laser/ANM samples do not execute the original
// Player16 entry-clear callback. Declare the same omission in the candidate.
// Original replay comparisons never call this helper; their fixture flags0
// retain collision, damage, Bomb and the complete entry-clear sequence.
export function omitPlayerEntryClear(core, otherFlags=0){
  core._th12_oracle_fixture(otherFlags|8);
}
