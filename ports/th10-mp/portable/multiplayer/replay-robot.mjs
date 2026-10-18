// Diagnostic input producer only. All positions come from a read-only native
// probe; the output is ordinary keyboard bits committed through RollbackCore.
// This is never linked/packaged into a Runtime and never changes game state.
export function reachedNativeStage(status, lifecycle, stage) {
  // MSG advances game.stage before the native loading boundary, while the
  // old stage_frames is still large. Neither field alone proves arrival.
  return Number.isInteger(stage) && stage > 0 && status?.length === 44 &&
    lifecycle?.length === 12 && status[3] === 1 && status[4] === 0 && status[5] === 0 &&
    status[6] === stage && status[7] > 60 && lifecycle[1] === lifecycle[2];
}

export function probeState(words, controls = null) {
  if (!Array.isArray(words) || words[0] !== 1) return null;
  const count = words[1], bullets = words[2], lasers = words[3], items = words[12];
  if (![count, bullets, lasers, items].every(Number.isInteger) || count < 2 || count > 3 ||
      bullets < 0 || bullets > 2001 || lasers < 0 || lasers > 512 || items < 0 || items > 150 ||
      words.length !== 52 + bullets * 7 + lasers * 8 + items * 4) throw Error('Invalid native robot observation');
  if (controls !== null && (!Array.isArray(controls) || controls.length !== 16 || controls[0] !== 1 ||
      !controls.every(Number.isFinite) || ![0,1].includes(controls[1]) || ![0,1].includes(controls[5])))
    throw Error('Invalid native control observation');
  let at = 52;
  const take = (n, width) => Array.from({length:n}, () => {const row = words.slice(at, at + width); at += width; return row;});
  return {count, stage:words[4], stageFrames:words[5], result:words[6], selected:words[7],
    keyboard:words[8], nameLength:words[9], menuTime:words[10], cleared:words[11],
    frame:words[13], generation:words[14], cursor:words[15],
    dialogue:controls?.[1] === 1,
    boss:controls?.[5] === 1 ? {x:controls[6],y:controls[7],health:controls[8]} : null,
    players:Array.from({length:count}, (_, seat) => words.slice(16 + seat * 12, 28 + seat * 12)),
    bullets:take(bullets,7), lasers:take(lasers,8), items:take(items,4)};
}

function menuInput(state) {
  // Native input edge, not a direct menu cursor assignment. Leave key-up ticks
  // between each key-down, including when moving from gameplay into Results.
  if (state.menuTime < 12 || state.menuTime % 16 !== 0) return 0;
  if (state.result === 6 || state.result === 10 || state.result === 11 || state.result === 12) return 1;
  if (state.result === 8) return state.selected === 1 ? 1 : 0x20;
  return 0;
}

const clamp = (value, low, high) => Math.max(low, Math.min(high, value));
const directions = [[0,0,0],[-1,0,0x40],[1,0,0x80],[0,-1,0x10],[0,1,0x20],
  [-1,-1,0x50],[1,-1,0x90],[-1,1,0x60],[1,1,0xa0]];

export function keyboardInput(state, seat) {
  if (!state || seat < 0 || seat >= state.count) return 0;
  if (state.result >= 6) return seat === 0 ? menuInput(state) : 0;
  // The original message owner consumes a fresh Shoot edge or its native
  // Ctrl skip key. Holding Shoot forever only waits for every text timeout.
  // Seat zero owns dialogue; neither this observer nor another seat calls a
  // message opcode or completes a stage directly.
  if (state.dialogue) return seat === 0 ? 0x100 | (state.frame % 8 === 0 ? 1 : 0) : 0;
  const p = state.players[seat], [x,y,life,lives,power,slow,hx,invulnerability,,fast,hy] = p;
  if (life === 4 && power >= 20) return 1 | 4 | 2; // native deathbomb, with native debit
  if (life !== 1) return 1 | 4;
  let targetX = (seat - (state.count - 1)/2) * 28, targetY = 380, shoot = true;
  if (state.boss && state.boss.health > 0)
    targetX = clamp(state.boss.x + (seat - (state.count - 1)/2) * 16,-160,160);
  if (lives > 0) {
    const spirit = state.players.find((peer, index) => index !== seat && peer[2] === 3);
    if (spirit) {targetX = spirit[0];targetY = spirit[1];shoot = false;}
  }
  if (shoot && power < 100 && (invulnerability > 45 || state.bullets.length < 65)) {
    let best = Infinity;
    for (const item of state.items) {
      if (![1,4].includes(item[2]) || item[1] < 160 || item[1] > 440) continue;
      const distance = (item[0]-x)**2 + (item[1]-y)**2;
      if (distance < best) {best=distance;targetX=item[0];targetY=item[1];}
    }
  }
  // Only nearby or rapidly approaching projectiles matter to a short input
  // choice. The native game, not this robot, still determines every collision.
  const bullets = state.bullets.filter(b => Math.abs(b[0]-x) < 110 + Math.abs(b[2])*12 &&
                                             Math.abs(b[1]-y) < 110 + Math.abs(b[3])*12);
  let selected = 4 | (shoot?1:0), bestCost = Infinity;
  for (const focus of (shoot ? [true,false] : [true])) for (const [dx,dy,bits] of directions) {
    const speed = (focus?slow:fast) || (focus?2:4);
    const scale = dx && dy ? Math.SQRT1_2 : 1;
    let cost = 0;
    for (const time of [2,5,9,12]) {
      const px = clamp(x + dx*speed*scale*time,-181,181), py = clamp(y + dy*speed*scale*time,32,430);
      // Keep an achievable firing/rescue target. Excessive soft repulsion
      // pins a keyboard robot to a wall for whole Boss patterns; hard native
      // collision predictions still dominate every positional preference.
      cost += ((px-targetX)**2+(py-targetY)**2)*0.0002;
      if (invulnerability > time + 8) continue;
      for (const b of bullets) {
        const bx = b[0] + b[2]*time, by = b[1] + b[3]*time;
        const rx = Math.max(4,Math.abs(b[4])+hx+3), ry = Math.max(4,Math.abs(b[5])+hy+3);
        const norm = ((bx-px)/rx)**2 + ((by-py)/ry)**2;
        cost += norm < 1 ? 10000 * (2-norm) : 0.6 / (norm + 0.1);
      }
      for (const laser of state.lasers) {
        if (laser[5] === 1 || laser[3] <= 0 || laser[4] <= 0) continue;
        const lx = laser[0]+laser[6]*time, ly=laser[1]+laser[7]*time;
        const c=Math.cos(laser[2]), s=Math.sin(laser[2]);
        const along=clamp((px-lx)*c+(py-ly)*s,0,laser[3]);
        const distance=Math.hypot(px-lx-c*along,py-ly-s*along);
        const gap=distance-(laser[4]/2+Math.max(hx,hy)+4);
        cost += gap < 0 ? 10000-gap*100 : 2/(gap+1);
      }
    }
    if (Math.abs(x+dx*speed)>181 || y+dy*speed<32 || y+dy*speed>430) cost += 1;
    if (cost < bestCost) {bestCost=cost;selected=bits|(focus?4:0)|(shoot?1:0);}
  }
  // All keyboard trajectories intersect an observed hazard: press the native
  // Bomb key before impact. Its availability, power debit and invulnerability
  // are still decided exclusively by the original Player code.
  if(bestCost>=10000&&invulnerability<4&&power>=20)selected|=2;
  return selected;
}

export function allKeyboardInputs(words, controls = null) {
  const state = probeState(words, controls);
  return state ? state.players.map((_,seat)=>keyboardInput(state,seat)) : [];
}
