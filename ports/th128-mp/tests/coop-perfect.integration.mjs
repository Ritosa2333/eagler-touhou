// Offline integration with the owned original DAT, native ANM/SHT and compiled MP core.
// Usage: node coop-world.integration.mjs <world-input.json> <MP-core.mjs> <MP-core.wasm>
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
const [pin,core,wasmPath]=process.argv.slice(2),config=JSON.parse(await readFile(pin,'utf8'));
const sha=b=>createHash('sha256').update(b).digest('hex');
const read=async p=>new Uint8Array(await readFile(p));
const dat=await read(config.dat),wasm=await read(wasmPath); assert.equal(sha(dat),config.datSHA256);
const paths={...config,bullet9:config.type9,bullet32:config.type32,...Object.fromEntries(Object.entries(config.bullets).map(([k,v])=>['bullet'+k,v]))};
const createModule=(await import(pathToFileURL(resolve(core)).href)).default;
let native;const factory=async args=>native=await createModule(args);
const iceDamage=()=>JSON.parse(native.UTF8ToString(native._th128_world_state())).damageStats[0];
const resources={build:{coreSHA256:sha(wasm),resourceSHA256:sha(dat)},read:k=>k==='dat'?dat:k==='wasm'?wasm:read(paths[k])};
const freezeMotivationMultiplier=process.env.TH128_TEST_FREEZE_MULTIPLIER?Number(process.env.TH128_TEST_FREEZE_MULTIPLIER):1;
const counts={instances:0,starts:0,ticks:0};const prepared=await prepareA13Preview({resources,factory,onFactory(){counts.instances++},onWorldStart(){counts.starts++},onTick(){counts.ticks++}});
assert.equal(typeof prepared.openCoopStage,'function','real shared-stage opener is missing');
// The solo fixture trajectory settles a large X reliably in 2P; 3P without
// an override still verifies real stock spend and animation. All presets are
// also exercised for both seat counts by coop-freeze-threshold.
for(const players of freezeMotivationMultiplier===1?[2,3]:[2]){
 const rules={players,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:false,freezeMotivationMultiplier,...resources.build};
 const g=await prepared.openCoopStage(rules);let before,after,spent,returned=false,award=null,pending=0;const casting=players-1,trace=[];
 try{for(let frame=0;frame<9500;frame++){
  before=g.state();const track=spent&&freezeMotivationMultiplier!==1&&frame-spent.frame>=160;const beforeIce=track?iceDamage():0;const caster=before.seats[casting];const use=!spent&&caster.charge>=10000&&caster.state===1;
  const dangers=g.stageState().bullets.map(b=>[b.x,b.y]);const packet={frame,inputs:before.seats.map((s,player)=>{const x=s.rawX/128,y=s.rawY/128,threat=dangers.filter(([bx,by])=>Math.abs(x-bx)<110&&Math.abs(y-by)<150);let best=-Infinity,move=0;
    for(const [dx,dy,keys] of [[0,0,0],[5,0,128],[-5,0,64],[0,-5,16],[0,5,32],[-3,-3,80],[3,-3,144],[-3,3,96],[3,3,160]]){const tx=Math.min(180,Math.max(-180,x+5*dx)),ty=Math.min(426,Math.max(240,y+5*dy));let danger=0;for(const [bx,by] of threat)danger+=2000/(14+(tx-bx)**2+(ty-by-8)**2);const value=-danger-.00004*((tx-(-95+player*43))**2+(ty-338)**2);if(value>best){best=value;move=keys;}}
    // Give the target real starting spares, not test-only stock. Keep non-donors
    // out of the 20px tie-break until each life has been accepted.
    if(players===3&&frame<80){
      if(player===2)return {player,held:frame<5?64:0};
      if(player===0&&frame<23)return {player,held:frame<5?64:0};
      if(player===1&&(s.transfers||frame>=23))return {player,held:128};
      if(!s.transfers){const target=before.seats[casting];return {player,held:(target.rawX>s.rawX+256?128:s.rawX>target.rawX+256?64:0)|(frame%2===0?8:0)};}
    }
    if(players===2&&player===casting-1&&!s.transfers&&frame<90){
      const target=before.seats[casting],tx=target.rawX/128,ty=target.rawY/128;
      const toward=(tx>x+2?128:tx<x-2?64:0)|(ty>y+2?32:ty<y-2?16:0);
      return {player,held:toward|(frame%2===0?8:0)};
    }
    if(spent&&freezeMotivationMultiplier!==1)return {player,held:move};
    const phase=(frame+player*57)%170;return {player,held:move|(player===casting&&use?2:phase<70?1:512)};})};
  let checkpoint,without;if(use){checkpoint=g.checkpoint();const control={frame,inputs:packet.inputs.map(o=>o.player===casting?{...o,held:o.held&~2}:o)};g.step(control);without=g.state();g.restore(checkpoint);}g.step(packet);after=g.state();if(use){if(after.seats[casting].perfectActive){
    assert.equal(after.seats[casting].charge,without.seats[casting].charge-10000);assert.deepEqual(after.seats.filter(s=>s.seat!==casting).map(s=>s.charge),without.seats.filter(s=>s.seat!==casting).map(s=>s.charge));
    g.restore(checkpoint);g.step(packet);assert.deepEqual(g.state(),after);spent={frame,after:after.seats.map(s=>s.charge)};}g.release(checkpoint);}
  if(track&&!award){
    const ice=(iceDamage()-beforeIce)|0;
    const finish=after.seats[casting].freezeCount>before.seats[casting].freezeCount&&after.seats[casting].lastChargeReward===0;
    const pixels=after.seats[casting].lastPixels,amount=ice>0?Math.floor(Math.floor(Math.min(ice,1700)/2)*freezeMotivationMultiplier):0;
    if(ice>0||finish){
      for(const s of after.seats){const old=before.seats[s.seat];assert.equal(s.deaths,old.deaths);
        const expected=s.seat===casting?amount:finish&&pixels>=8602?pending+amount:0;
        assert.equal(s.motivation-old.motivation-(s.starCredits-old.starCredits),expected,'Perfect Freeze uses ice damage, not area conversion');}
      pending+=amount;
      if(finish&&pixels>=8602)award={pixels,gain:pending,after:after.frame};
    }
  }
  if(spent&&[0,119,121,181].includes(frame-spent.frame))trace.push([frame,after.seats[casting].perfectActive,after.seats[casting].perfectPhase]);
  if(spent&&frame-spent.frame>181&&!after.seats[casting].perfectActive){returned=true;if(freezeMotivationMultiplier===1||award)break;}
 }assert.ok(spent,'earn/spend real personal X stock through unassisted battle: '+JSON.stringify(after));assert.ok(returned);if(freezeMotivationMultiplier!==1)assert.ok(award,'settle a real Perfect Freeze under the selected preset');console.log(JSON.stringify({players,spent,trace,award,final:after.frame}));}finally{g.close();}
}
