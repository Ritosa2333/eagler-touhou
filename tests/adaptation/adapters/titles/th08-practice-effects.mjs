import assert from 'node:assert/strict';

export function decodeTh08PracticeEffects(raw){
  const w=Array.isArray(raw)?raw:raw?.available===true?raw.words:null;
  assert.ok(Array.isArray(w)&&w.length===48&&w.every(Number.isFinite),'Invalid th08 trainer observation');
  assert.equal(w[0],1);assert.equal(w[1],1,'No active trainer world');
  for(const i of [15,19,41,42,43,44])assert.ok(w[i]===0||w[i]===1);
  return{frame:w[2],stage:w[3],cheats:w[4],lives:w[5],bombs:w[6],power:w[7],deaths:w[8],bombsUsed:w[9],
    dissolve:w[10],spellCaptures:w[11],lastSpellCaptures:w[12],life:w[13],window:w[14],
    bombActive:!!w[15],bombType:w[16],bombCost:w[17],consumerSequence:w[18],consumerValid:!!w[19],
    hit:{sequence:w[20],before:w[21],after:w[22],result:w[23],count:w[24]},
    bosses:[25,33].map(i=>({present:!!w[i],life:w[i+1],timeout:w[i+2],lifetime:w[i+3],damage:w[i+4],flags:w[i+5],owner:w[i+6]})),
    active:!!w[41],replay:!!w[42],accepted:!!w[43],enabled:!!w[44],flags:w[45]};
}

// Expectations follow PlayerLife::resolve_death and PlayerBomb's live consumers.
export const TH08_PRACTICE_TRAINER=Object.freeze({
  snapshot:{export:'practice_effects_snapshot_v1',wordCount:48,wordType:'f64',app:true,decode:decodeTh08PracticeEffects},
  tracker:{export:'practice_tracker_status',wordCount:7,wordType:'i32',app:true},
  source:['ports/th08/web/cpp/game/PlayerLife.cpp | resolve_death','ports/th08/web/cpp/game/PlayerBomb.cpp | update_player_bomb',
    'ports/th08/web/cpp/game/PracticeRuntime.cpp | update_practice','ports/th08/web/cpp/game/PlayerScene.hpp | dissolve',
    'ports/th08/web/cpp/game/PlayerFrame.cpp | update_bomb precedes resolve_death: first post-hit tick uses regular Bomb'],
  alive:0,deathWindow:2,respawning:1,deathPowerCost:16,bombCost:1,deathbombCost:2,deathbombTypes:[2,3],autoBombCost:1,autoBombTypes:[0,1],bombKey:'KeyX',bombVirtualKey:88,
  cases:[
    {id:'miss-control',keys:[],mask:0,stimulus:'hit',kind:'miss',keepLife:false,keepPower:false},
    {id:'invincibility',keys:['F1'],mask:1,stimulus:'hit',kind:'invincible'},
    {id:'infinite-lives',keys:['F2'],mask:2,stimulus:'hit',kind:'miss',keepLife:true,keepPower:false},
    {id:'keep-power',keys:['F4'],mask:8,stimulus:'hit',kind:'miss',keepLife:false,keepPower:true},
    {id:'bomb-control',keys:[],mask:0,stimulus:'bomb',kind:'bomb',keepBombs:false},
    {id:'infinite-bombs',keys:['F3'],mask:4,stimulus:'bomb',kind:'bomb',keepBombs:true},
    {id:'immediate-hit-bomb-control',keys:[],mask:0,stimulus:'hit',kind:'hit-bomb'},
    {id:'delayed-hit-bomb-control',keys:[],mask:0,stimulus:'hit',kind:'late-hit-bomb'},
    {id:'automatic-bomb',keys:['F6'],mask:32,stimulus:'hit',kind:'auto-bomb'},
  ],
});
