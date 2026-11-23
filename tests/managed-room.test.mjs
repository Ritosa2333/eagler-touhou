import assert from 'node:assert/strict';
import {test} from 'node:test';
import {buildMultiplayerGameplayRelayUrl} from '../../../launcher/src/launcher/multiplayer-relay-url.mts';
import {envelope,fakeWindow,flush} from '../../th128/tests/launcher-session/environment.mjs';
import {readTh128RoomOptions,mountManagedTh128Room} from '../runtime/managed-room.mjs';

const identity={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64),generation:'c'.repeat(64)};
function options(role={player:0},edit={}){
  return {netplayMode:'lan',netplayPlayer:role.player??0,netplayPlayerCount:2,netplaySeed:34772,netplayDifficulty:1,
    netplaySpectator:'spectator' in role,netplaySpectatorId:role.spectator??'',netplaySpectatorCount:1,netplayIceServers:[],
    netplayInputDelay:0,netplayAdonisMode:0,netplayPredictionLimit:2,netplayAllowWebsocketFallback:true,
    netplayLoadouts:[{character:0,shot:0},{character:0,shot:0}],
    netplayUrl:buildMultiplayerGameplayRelayUrl('wss://relay.example.test/netplay',{product:'th128mp',roomCode:'34772',runId:12,role,
      memberId:`member_${role.player??'observer'}`,runtimeGeneration:identity.generation}),...edit};
}
test('actual author gameplay URLs map two players and an independent viewer to the same rules',()=>{
  const a=readTh128RoomOptions(options(),identity),b=readTh128RoomOptions(options({player:1}),identity),
    v=readTh128RoomOptions(options({spectator:'observer12'}),identity);
  assert.deepEqual(a.rules,b.rules);assert.deepEqual(b.rules,v.rules);assert.equal(a.rules.matchId,'th128mp-34772-12');
  assert.deepEqual([a.role,b.role,v.role],['player','player','spectator']);assert.deepEqual([a.seat,b.seat,v.seat],[0,1,null]);
  assert.equal(v.spectatorId,'observer12');assert.equal(a.run,12);assert.equal(a.rules.seed,34772);assert.equal(a.rules.difficulty,1);
  assert.equal(a.rules.route,'A1-3');assert.equal(a.rules.rollbackWindow,2);
  assert.equal(a.transport.netplayAllowWebsocketFallback,true);assert.ok(Object.isFrozen(a));
});

test('an author URL cannot be rebound to a different seat, seed, game or served Runtime',()=>{
  for(const [values,build,error] of [
    [options({player:1},{netplayPlayer:0}),identity,/seat/],
    [options({player:0},{netplaySeed:0}),identity,/seed/],
    [options({player:0},{netplayDifficulty:2}),identity,/difficulty/],
    [options(),{...identity,generation:'d'.repeat(64)},/generation/],
    [options({spectator:'observer12'},{netplaySpectatorId:'different12'}),identity,/spectator/],
    [options({player:0},{netplayChallengeMode:true}),identity,/challenge/],
  ])assert.throws(()=>readTh128RoomOptions(values,build),error);
  const a=options();assert.throws(()=>readTh128RoomOptions({...a,netplayUrl:a.netplayUrl+'&player=0'},identity),/duplicate/);
  assert.throws(()=>readTh128RoomOptions({...a,netplayUrl:a.netplayUrl.replace('th128mp-','th12mp-')},identity),/room/);
  assert.equal(readTh128RoomOptions({...a,netplayAdonisMode:1,netplayInputDelay:3},identity).rules.rollbackWindow,0);
});

test('real Launcher message bridge keeps Configure, title presentation, launch, input and Return separate',async()=>{
  const env=fakeWindow(),calls=[],runs=[];
  const session=mountManagedTh128Room({window:env.win,identity,
    async verifyMounts(files){calls.push(['mount',files.length]);},
    async presentTitle({config}){assert.equal(config.options?.netplayMode,undefined);calls.push(['title']);return true;},
    async launchRoom({room,signal,present}){
      runs.push({room,signal,present});calls.push(['launch',room.role]);
      return {close(){calls.push(['close',room.role]);},key(value){calls.push(['key',value.code]);},clearInput(){calls.push(['clear']);},
        spectatorPlayback(value){calls.push(['spectator',value.run,value.action,value.frame]);}};
    },
    disposeResources(){calls.push(['dispose']);},
  });
  const send=(command,id,fields)=>env.send(envelope(command,id,fields));
  const reply=id=>env.messages.find(row=>row.data.request===id)?.data;
  const events=kind=>env.messages.filter(row=>row.data.event===kind).map(row=>row.data);
  assert.equal(session.markReady(),true);assert.equal(events('ready')[0].roomPreview,true);assert.equal(events('ready')[0].roomReturn,true);
  send('configure','menu',{music:'ogg',resources:[{path:'/th128.dat'}]});await flush();assert.equal(reply('menu').ok,true);
  send('prepare-room','prepare');await flush();assert.equal(reply('prepare').presented,true);
  send('configure','P1',{music:'ogg',options:options()});await flush();assert.equal(reply('P1').ok,true);
  assert.equal(runs.length,0,'configuring a room must not connect or create a running World');
  send('launch','l1',{launchSerial:9});await flush();assert.equal(reply('l1').ok,true);assert.equal(events('first-frame').length,0);
  assert.equal(runs[0].present(false,'rtc'),false);assert.equal(runs[0].present(true,'spectator-relay'),false);
  assert.equal(runs[0].present(true,'rtc'),true);
  assert.deepEqual(events('first-frame').map(({launchSerial,th128RoomPath})=>[launchSerial,th128RoomPath]),[[9,'rtc']]);
  send('spectator-playback','not-viewer',{run:'th128-1-1',action:'pause'});await flush();assert.equal(reply('not-viewer').ok,false);
  send('sync','sync');await flush();assert.equal(reply('sync').ok,true);assert.equal(reply('sync').changed,false);
  send('write','player-write',{path:'replay/th128_01.rpy'});await flush();assert.equal(reply('player-write').ok,false);
  send('keyboard','key1',{code:'ArrowLeft',down:true});await flush();assert.ok(calls.some(row=>row.join(',')==='key,ArrowLeft'));
  send('return-room','rt');await flush();assert.equal(reply('rt').presented,true);assert.equal(runs[0].signal.aborted,true);
  assert.equal(runs[0].present(true),false);assert.equal(events('room-menu')[0].launchSerial,9);
  send('configure','menu2',{music:'ogg'});await flush();assert.equal(reply('menu2').ok,true);
  send('configure','view',{music:'none',options:options({spectator:'observer12'})});await flush();assert.equal(reply('view').ok,true);
  send('launch','l2',{launchSerial:10});await flush();assert.equal(reply('l2').ok,true);
  send('keyboard','view-key',{code:'ArrowLeft',down:true});await flush();assert.equal(reply('view-key').ok,false);
  assert.equal(runs[1].present(true,'relay'),false);assert.equal(runs[1].present(true,'spectator-relay'),true);
  assert.deepEqual(events('first-frame').map(({launchSerial,th128RoomPath})=>[launchSerial,th128RoomPath]),[[9,'rtc'],[10,'spectator-relay']]);
  send('spectator-playback','pause',{run:'th128-1-1',action:'pause'});await flush();assert.equal(reply('pause').ok,true);
  send('spectator-playback','seek',{run:'th128-1-1',action:'seek',frame:9});await flush();assert.equal(reply('seek').ok,true);
  send('spectator-playback','invalid-seek',{run:'th128-1-1',action:'seek',frame:-1});await flush();assert.equal(reply('invalid-seek').ok,false);
  assert.deepEqual(calls.filter(row=>row[0]==='spectator'),[['spectator','th128-1-1','pause',undefined],['spectator','th128-1-1','seek',9]]);
  send('exit','quit');await flush();assert.equal(reply('quit').ok,true);assert.equal(runs[1].signal.aborted,true);
  assert.deepEqual(calls.filter(row=>['launch','close','title','dispose'].includes(row[0])),[
    ['title'],['launch','player'],['close','player'],['title'],['launch','spectator'],['close','spectator'],['dispose'],
  ]);
});

test('a title acknowledgement does not replace launch and a failed first draw cannot become a presented run',async()=>{
  const env=fakeWindow(),tries=[];
  const session=mountManagedTh128Room({window:env.win,identity,verifyMounts(){},disposeResources(){},presentTitle(){return false;},
    async launchRoom({present,signal}){
      assert.equal(present(true,'relay'),true);assert.equal(present(true,'relay'),false);tries.push({present,signal});
      if(tries.length===1)throw Error('Native attach failed after draw');return {close(){}};
    }});
  const send=(command,id,fields)=>env.send(envelope(command,id,fields));
  const reply=id=>env.messages.find(row=>row.data.request===id)?.data;
  const presentations=()=>env.messages.filter(row=>row.data.event==='first-frame');
  session.markReady();send('configure','menu',{music:'ogg'});await flush();send('prepare-room','false-title');await flush();
  assert.equal(reply('false-title').ok,false);assert.equal(tries.length,0);
  send('configure','room',{music:'ogg',options:options()});await flush();send('launch','bad',{launchSerial:7});await flush();
  assert.equal(reply('bad').ok,false);assert.equal(tries[0].signal.aborted,true);assert.equal(tries[0].present(true),false);
  assert.equal(presentations().length,0);
  send('launch','retry',{launchSerial:8});await flush();assert.equal(reply('retry').ok,true);
  assert.deepEqual(presentations().map(row=>row.data.launchSerial),[8]);
  assert.equal(presentations()[0].data.th128RoomPath,'relay');
  assert.ok(env.messages.findIndex(row=>row.data===reply('retry'))<env.messages.indexOf(presentations()[0]));
  await session.dispose();
});

test('only the cooperative room accepts its shared boolean Cirno palette option',()=>{
  const build={...identity,gameplay:'same-stage-coop'};
  const enabled=options({player:0},{netplayRandomCirnoPalette:true});
  assert.equal(readTh128RoomOptions(enabled,build).rules.randomCirnoPalette,true);
  assert.equal(readTh128RoomOptions(enabled,build).rules.cirnoPaletteSet,'remastered-6');
  assert.equal(readTh128RoomOptions(options(),build).rules.randomCirnoPalette,false);
  assert.equal(readTh128RoomOptions(options(),build).rules.cirnoPaletteSet,undefined);
  assert.throws(()=>readTh128RoomOptions({...enabled,netplayRandomCirnoPalette:1},build),/palette/i);
  assert.throws(()=>readTh128RoomOptions(enabled,identity),/palette/i);
});

test('TH128 freeze motivation is the same original multiplier for all players and spectators',()=>{
  const build={...identity,gameplay:'same-stage-coop'};
  assert.equal(readTh128RoomOptions(options(),build).rules.freezeMotivationMultiplier,1);
  for(const multiplier of [2.5,2,1,0.75,0.5])for(const players of [2,3]){
    const overrides={netplayPlayerCount:players,netplayLoadouts:Array.from({length:players},()=>({character:0,shot:0})),netplayFreezeMotivationMultiplier:multiplier};
    const owner=readTh128RoomOptions(options({player:0},overrides),build);
    const guest=readTh128RoomOptions(options({player:players-1},overrides),build);
    const viewer=readTh128RoomOptions(options({spectator:'observer12'},overrides),build);
    assert.equal(owner.rules.freezeMotivationMultiplier,multiplier);
    assert.deepEqual(owner.rules,guest.rules);assert.deepEqual(guest.rules,viewer.rules);
  }
  for(const multiplier of [0,4,5,'2.5',true,null])assert.throws(()=>readTh128RoomOptions(options({player:0},{netplayFreezeMotivationMultiplier:multiplier}),build),/motivation/i);
  assert.throws(()=>readTh128RoomOptions(options({player:0},{netplayFreezeMotivationMultiplier:2.5}),identity),/motivation/i);
  assert.throws(()=>readTh128RoomOptions(options({player:0},{netplayFreezeMotivationArea:2.5}),build),/motivation/i);
});
