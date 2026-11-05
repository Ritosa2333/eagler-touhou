import assert from 'node:assert/strict';
import {validateMultiplayerOptions} from '../../web/sdl-runtime/multiplayer.mjs';
globalThis.location={href:'https://example.test/runtime/th075.html'};
const base={netplayUrl:'wss://example.test/netplay?room=th075mp-1234&run=1&player=0',
  netplayMode:'lan',netplayPlayerCount:2,netplayPlayer:0,netplaySeed:1947,netplayDifficulty:1,
  netplayAdonisMode:1,netplayPredictionLimit:0,netplayInputDelay:2,netplayInputDelayAuto:false,
  netplayLoadouts:[{character:0,shot:0},{character:10,shot:8}]};
assert.equal(validateMultiplayerOptions(base).spectator,false);
for(const patch of [{netplayPlayerCount:3},{netplayAdonisMode:0},{netplayAdonisMode:2},
  {netplayPredictionLimit:1},{netplayInputDelay:10},{netplaySeed:65536},
  {netplayDifficulty:4},{netplayLoadouts:[{character:11,shot:0},{character:0,shot:0}]},
  {netplayUrl:base.netplayUrl.replace('&player=0','')}])assert.throws(()=>validateMultiplayerOptions({...base,...patch}));
assert.equal(validateMultiplayerOptions({...base,netplayAdonisMode:0,netplayPredictionLimit:8,netplayInputDelay:0}).spectator,false);
for(const patch of [{netplayPredictionLimit:0},{netplayPredictionLimit:9},{netplayInputDelay:1},{netplayInputDelayAuto:true}])assert.throws(()=>validateMultiplayerOptions({...base,netplayAdonisMode:0,netplayPredictionLimit:8,netplayInputDelay:0,...patch}));
const viewer={...base,netplayPlayer:null,netplaySpectator:true,netplaySpectatorId:'viewer_1234',
  netplayUrl:base.netplayUrl.replace('player=0','spectator=viewer_1234')};
assert.equal(validateMultiplayerOptions(viewer).spectator,true);
assert.throws(()=>validateMultiplayerOptions({...viewer,netplayUrl:viewer.netplayUrl+'&player=0'}));
assert.throws(()=>validateMultiplayerOptions({...viewer,netplaySpectatorId:'another_1234'}));
