import {test, expect, vi, afterEach} from 'vitest';
import {createRoomMusic, LAST_MUSIC_KEY} from '../../app/services/room-music';
import {createTuneWeaveProvider,musicPlaylistId} from '../../app/services/music-provider';
const track={platform:'netease',id:'123',name:'Song',artist:'Artist'};
const room={product:'th09mp',room:{code:'1234',seats:[{clientId:'local'},{clientId:'other'}]},seat:0,connected:true,listenTogetherSupported:true,localClientId:'local'};
const session={type:'listen-together',session:'test-session',revision:2,participants:['local','other'],track,startedAt:1000,serverTime:5000};
class Audio extends EventTarget {
  src='';paused=true;volume=1;currentTime=0;ended=false;
  load(){if(this.src)queueMicrotask(()=>this.dispatchEvent(new Event('loadedmetadata')));}
  pause(){this.paused=true;}
  play(){this.paused=false;return Promise.resolve();}
  removeAttribute(){this.src='';}
}
afterEach(()=>vi.useRealTimers());
function fixture(stream=()=>Promise.resolve({urls:['https://music.example/track.mp3'],duration:120})){
  const storage=new Map(),sent=[],audio=new Audio();
  const factory=vi.fn(()=>audio);
  const provider={stream,track:async()=>track};
  const model=createRoomMusic({provider,storage:{getItem:k=>storage.get(k),setItem:(k,v)=>storage.set(k,v)},send:m=>{sent.push(m);return true;},createAudio:factory,clock:()=>0});
  model.update(room);return{model,sent,audio,factory,storage};
}
const settle=async()=>{for(let i=0;i<12;i++)await Promise.resolve();};
test('idle room creates no player or timer and spectators can only play privately',async()=>{
  vi.useFakeTimers();const f=fixture();expect(f.factory).not.toHaveBeenCalled();expect(vi.getTimerCount()).toBe(0);
  f.model.update({...room,seat:null});f.model.choose(track);await settle();f.model.receive(session);f.model.requestSync();
  expect(f.sent).toEqual([]);expect(f.model.getSnapshot().syncState).toBe('solo');
  expect(f.model.getSnapshot().status).toBe('playing');f.model.dispose();
});
test('leaving cancels a late media response and suppresses old membership',async()=>{
  let resolve;const f=fixture(()=>new Promise(r=>{resolve=r}));
  f.model.receive(session);f.model.leave();resolve({urls:['https://music.example/a.mp3'],duration:120});await settle();
  expect(f.audio.src).toBe('');expect(f.model.getSnapshot().joined).toBe(false);
  f.model.receive(session);await settle();expect(f.audio.src).toBe('');f.model.dispose();
});
test('playback follows room time, only own selection is persisted, disconnect releases timer',async()=>{
  vi.useFakeTimers();const f=fixture();f.model.choose(track);
  expect(JSON.parse(f.storage.get(LAST_MUSIC_KEY))).toEqual(track);
  f.model.receive({...session,track:{platform:'qq',id:'other'}});await settle();await vi.advanceTimersByTimeAsync(0);
  expect(f.audio.currentTime).toBe(4);expect(f.audio.paused).toBe(false);
  expect(JSON.parse(f.storage.get(LAST_MUSIC_KEY))).toEqual(track);
  f.model.update({...room,connected:false});await settle();
  expect(f.audio.paused).toBe(true);expect(f.model.getSnapshot().session.track.id).toBe(track.id);
  expect(vi.getTimerCount()).toBe(0);f.model.dispose();
});
test('failed tracks stay stopped until retry, timeout leaves loading state',async()=>{
  vi.useFakeTimers();const failed=fixture(()=>Promise.reject(Error('trial')));failed.model.receive(session);await settle();
  expect(failed.model.getSnapshot().error).toBe('trial');expect(failed.audio.src).toBe('');failed.model.dispose();
  const slow=fixture(()=>new Promise(()=>{}));slow.model.receive(session);await vi.advanceTimersByTimeAsync(30001);
  expect(slow.model.getSnapshot().status).toBe('idle');expect(slow.model.getSnapshot().error).toBe('unavailable');slow.model.dispose();
});
test('newer track survives stale responses and selection sends revision',async()=>{
  const f=fixture();f.model.receive(session);await settle();
  f.model.receive({...session,revision:5,track:{platform:'qq',id:'next'},startedAt:2000});await settle();
  f.model.receive(session);expect(f.model.getSnapshot().session.revision).toBe(5);
  f.model.choose(track);expect(f.sent.at(-1)).toMatchObject({action:'select',revision:5});f.model.dispose();
});
test('TuneWeave adapter rejects previews, headers and cross-track fallback',async()=>{
  let body={url:'http://cdn.example/a.mp3',backup_urls:['https://cdn.example/a.mp3'],duration_ms:120000,trial:null,resolved_track:'netease:123',headers:{}};
  const request=vi.fn(async()=>new Response(JSON.stringify({ok:true,data:body})));
  const provider=createTuneWeaveProvider('/music-api',request),signal=new AbortController().signal;
  expect(await provider.stream(track,signal)).toEqual({urls:['https://cdn.example/a.mp3'],duration:120});
  expect(request.mock.calls[0][0]).toContain('fallback=false');
  body={...body,trial:{start_ms:100,end_ms:500}};await expect(provider.stream(track,signal)).rejects.toThrow('trial');
  body={...body,trial:null,headers:{Cookie:'secret'}};await expect(provider.stream(track,signal)).rejects.toThrow('unavailable');
  body={...body,headers:{},resolved_track:'qq:other'};await expect(provider.stream(track,signal)).rejects.toThrow('unavailable');
});
test('playlist links accept public IDs from the selected service',()=>{
  expect(musicPlaylistId('netease','https://music.163.com/#/playlist?id=123')).toBe('123');
  expect(musicPlaylistId('qq','https://y.qq.com/n/ryqq/playlist/456')).toBe('456');
  expect(()=>musicPlaylistId('qq','https://example.com/?id=456')).toThrow('invalidPlaylist');
});

test('solo playback works without relay support and disposal prevents late work',async()=>{
  vi.useFakeTimers();const f=fixture();
  f.model.update({...room,listenTogetherSupported:false});f.model.choose(track);await settle();f.model.requestSync();
  expect(f.sent).toEqual([]);expect(f.model.getSnapshot().status).toBe('playing');
  f.model.dispose();f.factory.mockClear();f.model.choose(track);f.model.retry();
  expect(f.factory).not.toHaveBeenCalled();expect(vi.getTimerCount()).toBe(0);
});

test('sharing exports the personal queue and position only on explicit invitation',async()=>{
  vi.useFakeTimers();const f=fixture();
  const next={...track,id:'next'};
  f.model.choose(track,[track,next]);await settle();
  f.model.seek(32);f.model.togglePlayback();f.model.setRepeat('all');
  expect(f.sent).toEqual([]);expect(f.audio.currentTime).toBe(32);expect(f.audio.paused).toBe(true);
  f.model.requestSync();
  expect(f.sent).toEqual([expect.objectContaining({action:'invite',track,queue:[track,next],positionMs:32000,paused:true,repeat:'all'})]);
  f.model.receive({...session,startedAt:null,participants:['local'],queue:[track,next],queueIndex:0});
  expect(f.model.getSnapshot()).toMatchObject({syncState:'waiting',joined:false,position:32,status:'paused'});
  f.model.receive({...session,revision:3,startedAt:-27000,pausedAt:5000,queue:[track,next],queueIndex:0});
  await settle();expect(f.model.getSnapshot()).toMatchObject({syncState:'shared',joined:true,position:32,status:'paused'});
  f.model.dispose();
});

test('an offer leaves personal playback untouched until consent and leaving restores its queue',async()=>{
  vi.useFakeTimers();const f=fixture();
  const other={...track,id:'different'};
  f.model.choose(track);await settle();f.model.seek(17);
  f.model.receive({...session,track:other,startedAt:null,participants:['other']});
  expect(f.model.getSnapshot()).toMatchObject({syncState:'offered',current:track,position:17,status:'playing'});
  expect(f.sent).toEqual([]);
  f.model.requestSync();expect(f.sent.at(-1)).toMatchObject({action:'join',session:session.session});
  f.model.receive({...session,track:other,queue:[other],queueIndex:0,revision:3});await settle();
  expect(f.model.getSnapshot()).toMatchObject({syncState:'shared',position:4});
  f.model.leave();await settle();
  expect(f.model.getSnapshot()).toMatchObject({syncState:'solo',current:track,position:17,status:'paused'});
  f.model.dispose();
});

test('leave clears a resolver deadline immediately and media failure stops synchronization',async()=>{
  vi.useFakeTimers();const pending=fixture(()=>new Promise(()=>{}));pending.model.receive(session);await settle();
  pending.model.leave();expect(vi.getTimerCount()).toBe(0);pending.model.dispose();
  const f=fixture();f.model.receive(session);await settle();f.audio.error={code:2};
  await vi.advanceTimersByTimeAsync(1000);
  expect(f.model.getSnapshot().error).toBe('unavailable');expect(f.audio.paused).toBe(true);
  expect(f.audio.src).toBe('');expect(vi.getTimerCount()).toBe(0);f.model.dispose();
});

test('shared pause freezes time and resume reuses the loaded source',async()=>{
  vi.useFakeTimers();const stream=vi.fn(async()=>({urls:['https://music.example/a.mp3'],duration:120})),f=fixture(stream);
  f.model.receive(session);await settle();f.model.togglePlayback();
  expect(f.sent.at(-1)).toMatchObject({action:'pause',session:session.session,revision:2});
  f.model.receive({...session,revision:3,pausedAt:6500,serverTime:8000});await settle();
  expect(f.audio.paused).toBe(true);expect(f.audio.currentTime).toBe(5.5);expect(vi.getTimerCount()).toBe(0);
  await vi.advanceTimersByTimeAsync(10000);expect(f.audio.currentTime).toBe(5.5);
  f.model.togglePlayback();expect(f.sent.at(-1)).toMatchObject({action:'resume',revision:3});
  f.model.receive({...session,revision:4,startedAt:11000,pausedAt:null,serverTime:16500});await settle();
  expect(f.audio.paused).toBe(false);expect(f.audio.currentTime).toBe(5.5);expect(stream).toHaveBeenCalledTimes(1);f.model.dispose();
});

test('reselecting a finished track starts a fresh playback',async()=>{
  vi.useFakeTimers();const stream=vi.fn(async()=>({urls:['https://music.example/a.mp3'],duration:120})),f=fixture(stream);
  f.model.receive(session);await settle();f.audio.ended=true;await vi.advanceTimersByTimeAsync(1000);
  expect(f.model.getSnapshot().status).toBe('ended');f.audio.ended=false;
  f.model.receive({...session,revision:3,startedAt:5000,serverTime:5000});await settle();
  expect(f.model.getSnapshot().status).toBe('playing');expect(f.audio.currentTime).toBe(0);expect(stream).toHaveBeenCalledTimes(1);f.model.dispose();
});

test('pause received during resolution prevents late playback',async()=>{
  vi.useFakeTimers();let resolve;const f=fixture(()=>new Promise(r=>{resolve=r}));
  f.model.receive(session);f.model.receive({...session,revision:3,pausedAt:6000,serverTime:7000});
  resolve({urls:['https://music.example/a.mp3'],duration:120});await settle();
  expect(f.audio.paused).toBe(true);expect(f.model.getSnapshot().status).toBe('paused');expect(f.audio.currentTime).toBe(5);
  expect(vi.getTimerCount()).toBe(0);f.model.dispose();
});

test('end reports are deduplicated and single-track looping reuses the media',async()=>{
  vi.useFakeTimers();const stream=vi.fn(async()=>({urls:['https://music.example/a.mp3'],duration:120})),f=fixture(stream);
  f.model.receive({...session,playback:1,ended:false});await settle();f.audio.ended=true;
  await vi.advanceTimersByTimeAsync(1000);expect(f.sent.filter(m=>m.action==='ended')).toHaveLength(1);
  await vi.advanceTimersByTimeAsync(3000);expect(f.sent.filter(m=>m.action==='ended')).toHaveLength(1);
  f.audio.ended=false;f.model.receive({...session,revision:3,playback:2,ended:false,startedAt:5000});await settle();
  expect(f.audio.paused).toBe(false);expect(f.audio.currentTime).toBe(0);expect(stream).toHaveBeenCalledTimes(1);
  f.model.seek(40);expect(f.sent.at(-1)).toMatchObject({action:'seek',positionMs:40000,revision:3});
  f.model.receive({...session,revision:4,playback:3,pausedAt:5000,startedAt:-35000});await settle();
  expect(f.audio.paused).toBe(true);expect(f.audio.currentTime).toBe(40);expect(stream).toHaveBeenCalledTimes(1);
  expect(f.model.seek(2)).toBe(true);
  f.model.receive({...session,revision:5,playback:4,pausedAt:5000,startedAt:3000});
  expect(f.audio.currentTime).toBe(2);expect(f.model.getSnapshot().position).toBe(2);
  expect(stream).toHaveBeenCalledTimes(1);f.model.dispose();
});


test('rapid seeks send only the latest target after acknowledgement and release pending work on exit',async()=>{
  vi.useFakeTimers();const f=fixture();f.model.receive({...session,playback:1});await settle();
  expect(f.model.seek(90)).toBe(true);expect(f.model.seek(40)).toBe(true);expect(f.model.seek(2)).toBe(true);
  expect(f.sent.filter(m=>m.action==='seek').map(m=>m.positionMs)).toEqual([90000]);
  expect(f.model.getSnapshot().seekTarget).toBe(2);
  f.model.receive({...session,revision:3,playback:2,startedAt:-85000});await settle();
  expect(f.sent.filter(m=>m.action==='seek').map(m=>[m.revision,m.positionMs])).toEqual([[2,90000],[3,2000]]);
  expect(f.model.getSnapshot().seekTarget).toBe(2);
  f.model.receive({...session,revision:4,playback:3,startedAt:3000});await settle();
  expect(f.audio.currentTime).toBe(2);expect(f.model.getSnapshot().seekTarget).toBe(null);
  f.model.seek(30);f.model.seek(5);f.model.leave();
  expect(f.model.getSnapshot().seekTarget).toBe(null);expect(vi.getTimerCount()).toBe(0);f.model.dispose();
});

test('unacknowledged seeks expire and a changed track cannot inherit queued seeking',async()=>{
  vi.useFakeTimers();const f=fixture();f.model.receive({...session,playback:1});await settle();
  f.model.seek(30);f.model.seek(5);await vi.advanceTimersByTimeAsync(2501);
  expect(f.model.getSnapshot().seekTarget).toBe(null);expect(f.sent.at(-1).action).toBe('sync');
  f.model.seek(20);f.model.seek(1);
  f.model.receive({...session,revision:3,playback:2,track:{...track,id:'456'}});await settle();
  expect(f.model.getSnapshot().seekTarget).toBe(null);
  expect(f.sent.filter(m=>m.action==='seek').map(m=>m.positionMs)).toEqual([30000,20000]);f.model.dispose();
});
