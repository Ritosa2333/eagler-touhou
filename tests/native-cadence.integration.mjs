import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile,writeFile} from 'node:fs/promises';
import {pathToFileURL} from 'node:url';
import {setTimeout as sleep} from 'node:timers/promises';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {openPinnedPreviewResources} from '../runtime/preview-resources.mjs';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
import {createMatchClock} from '../runtime/match-clock.mjs';
import {createPlayerMatch} from '../runtime/player-match.mjs';
import {openRelayLink} from '../runtime/relay-link.mjs';
import {readMultiplayerReplay} from '../runtime/confirmed-history.mjs';

const bytes=async path=>new Uint8Array(await readFile(path));
const hash=value=>createHash('sha256').update(value).digest('hex');
const total=240,delayFrame=120;
const stages=[
  frame=>frame<48?64:120<=frame&&frame<148?128:0,
  frame=>40<=frame&&frame<85?128:165<=frame&&frame<192?64:0,
];

// Independent local native pairs on a real loopback Relay. This host feeds
// real monotonic callbacks, not a measured browser/display clock. The supplied
// button trace is explicit test input; no synthetic native state is installed.
export async function verifyNativeCadence({routes,files,port,output,onFactory,onWorldStart,onTick,signal}){
  const locations=new Map(routes.map(row=>[row.url,row.path]));
  const resources=await openPinnedPreviewResources(files,async url=>{
    const file=locations.get(url);assert.ok(file,`missing pinned route ${url}`);return bytes(file);
  });
  const {default:factory}=await import(pathToFileURL(locations.get(resources.url('core'))).href);
  const preview=await prepareA13Preview({resources,factory,onFactory,onWorldStart,onTick,signal});
  const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'native-clock-relay',epoch:1,mode:'parallel-stage',
    route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:2,...resources.build});
  const links=[],players=[],pairs=[new Map(),new Map()],samples=[0,0],counts=[0,0],net=[0,0],errors=[],clocks=[];
  let live=false,heldBack=null,releaseAt=0,delaysSent=0,playback=null;
  const requireHealthy=()=>{signal.throwIfAborted();assert.deepEqual(errors,[]);};
  async function until(test,label,ms=3000){
    const deadline=performance.now()+ms;
    do{requireHealthy();const value=test();if(value)return value;await sleep(4);}while(performance.now()<deadline);
    throw new Error(`${label} exceeded work budget`);
  }
  try{
    for(const seat of [0,1]){
      const match=await createPlayerMatch({rules,localPlayer:seat,allowedMask:763,maxFrames:total,signal,
        async openEngine(selected,signal){const pair=await preview.openPair(selected,signal);pairs[seat].set(selected.epoch,pair);return pair;},
        send(message){
          if(seat===0&&live&&message.epoch===1&&message.frame===delayFrame){
            assert.equal(heldBack,null);heldBack=message;releaseAt=performance.now()+60;return;
          }
          links[seat].send(message);
        }});
      players.push(match);
      clocks.push(createMatchClock({stepFrame:options=>{
        const outcome=match.tick(()=>stages[seat](samples[seat]++),options);
        if(outcome.advanced)counts[seat]++;return outcome;
      }}));
    }
    for(const seat of [0,1]){
      const url=`ws://127.0.0.1:${port}/?room=th128mp-9157&run=1&players=2&player=${seat}`;
      links.push(openRelayLink({url,localPlayer:seat,async onMessage(message,from){
        net[seat]++;await players[seat].receive(message,from);
        if(live&&Number.isInteger(message.frame))clocks[seat].input();
      },onError:error=>errors.push(error.message)}));
    }
    await Promise.all(links.map(link=>link.ready));for(const player of players)player.announceStart();
    await until(()=>players.every(p=>p.status().startup.canStart),'Native READY exchange');
    const start=performance.now();live=true;
    await until(()=>{
      const now=performance.now();
      if(heldBack&&now>=releaseAt){links[0].send(heldBack);heldBack=null;delaysSent++;}
      for(const clock of clocks)clock.advance(now,true);
      return players.every(p=>p.status().history.nextFrame===total);
    },'Real native 240-frame horizon',10000);
    const stageMs=Math.round(performance.now()-start);live=false;
    assert.deepEqual(samples,[total,total]);assert.deepEqual(counts,[total,total]);assert.equal(delaysSent,1);
    const corrected=players.map(p=>p.status().round.replayedFrames);
    assert.ok(corrected[1]>=1,'the late actual remote input must replay native frames');
    const states=[0,1].map(seat=>pairs[0].get(1).stateText(seat)),draws=[0,1].map(seat=>JSON.stringify(pairs[0].get(1).draws(seat)));
    for(const seat of [0,1]){
      assert.equal(states[seat],pairs[1].get(1).stateText(seat));assert.equal(draws[seat],JSON.stringify(pairs[1].get(1).draws(seat)));
      const state=JSON.parse(states[seat]);assert.equal(state.frame,total);assert.equal(state.normalRecord.bound,false);
    }
    players.forEach(p=>p.requestRestart());
    await until(()=>{players.forEach(p=>p.flush());return players.every(p=>p.status().epoch===2);},'Retire both real native Worlds');
    assert.deepEqual(players.map(p=>p.status().lastRestartFrameCount),[total,total]);
    const encoded=players.map(p=>p.previousReplay());assert.deepEqual(encoded[0],encoded[1]);
    await writeFile(output,encoded[0],{flag:'wx'});
    for(const p of players)p.close();for(const link of links)link.close();
    const reopened=await bytes(output);assert.deepEqual(reopened,encoded[0]);
    const replay=await readMultiplayerReplay(reopened,resources.build);assert.deepEqual(replay.status(),{nextFrame:total,end:'restart'});
    playback=await preview.openPair(createMultiplayerRules(replay.header.rules),signal);
    for(let frame=0;frame<total;frame++){signal.throwIfAborted();playback.step(replay.read(frame));}
    for(const seat of [0,1]){
      assert.equal(playback.stateText(seat),states[seat]);assert.equal(JSON.stringify(playback.draws(seat)),draws[seat]);
    }
    return {stageMs,confirmed:total,samples,corrected,net,delaysSent,stateSHA256:states.map(hash),drawSHA256:draws.map(hash),
      replay:{bytes:reopened.length,sha256:hash(reopened),frames:total,end:'restart'},browser:0,audioDevices:0,backgroundFactories:0};
  }finally{
    playback?.close();for(const p of players)p.close();for(const link of links)link.close();
  }
}
