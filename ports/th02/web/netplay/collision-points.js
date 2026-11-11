// Presentation-only marker for the local player; inputs remain shared.
export function collisionPoints({state,slot,raw,paused,preferences,inputs}){
  const player=state?.players[slot];
  if(!raw&&player&&!player.hit&&!player.out&&!paused&&
    (preferences.always||(preferences.focus&&(inputs[slot]&64)))){
    return [[player.x+16,player.y+21]];
  }
  return [];
}
