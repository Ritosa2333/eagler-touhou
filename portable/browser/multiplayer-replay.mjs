// TH11 display metadata only. The pinned native InputReplay decoder remains
// the authority for checksums, every seat's inputs and the gameplay ABI.
export const MULTIPLAYER_REPLAY_MAX_FRAMES=250000;
export function multiplayerReplayPath(value) {
 const name=String(value).replaceAll('\\','/').toLowerCase()
  .replace(/^\/savesth11(?:mp)?\//,'').replace(/^\//,'');
 if(!/^replay\/th11_(?:\d{2}|ud[a-z0-9]{4})\.rpy$/.test(name))
  throw Error('多人录像路径无效');
 return name;
}
export function inspectMultiplayerReplay(value,{validate,buildWords}={}) {
 const bytes=value instanceof Uint8Array?value:new Uint8Array(value);
 if(bytes.length<176||bytes.length>16*1024*1024||
    new TextDecoder().decode(bytes.subarray(0,8))!=='EAGLRPY1')
  throw Error('这不是 TH11 多人录像');
 const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),u32=offset=>view.getUint32(offset,true);
 if(u32(8)!==1||u32(12)!==11||u32(24)!==128||u32(40)!==0x4d313154||u32(44)!==1||u32(48)!==1)
  throw Error('不支持的 TH11 多人录像版本');
 const playerCount=bytes[20],recordedPlayer=bytes[21],chapterCount=u32(28),frameCount=u32(32);
 if(![2,3].includes(playerCount)||recordedPlayer>=playerCount||chapterCount<1||chapterCount>7||!frameCount||frameCount>MULTIPLAYER_REPLAY_MAX_FRAMES||
    bytes.length!==40+128+chapterCount*8+frameCount*playerCount*12)
  throw Error('多人录像内容不完整');
 const setup=Array.from({length:22},(_,i)=>u32(80+i*4));
 if(setup[0]!==5||setup[1]!==playerCount||setup[2]!==recordedPlayer||setup[14]!==1||setup[21]!==0)
  throw Error('多人录像配置不匹配');
 if(buildWords&&(!Array.isArray(buildWords)||buildWords.length!==4||buildWords.some((n,i)=>(n>>>0)!==setup[17+i])))
  throw Error('该多人录像使用了不同的 Runtime 版本，请用录制时的版本播放');
 if(typeof validate!=='function'||validate(bytes)!==true)throw Error('多人录像校验失败');
 const chapters=Array.from({length:chapterCount},(_,i)=>({stage:u32(168+i*8),frame:u32(172+i*8)}));
 const timestamp=u32(56)+u32(60)*4294967296;
 return Object.freeze({
  playerCount,recordedPlayer,frameCount,chapters:Object.freeze(chapters),
  difficulty:setup[3],seed:setup[4],inputDelay:u32(52),prediction:0,
  loadouts:Object.freeze(Array.from({length:playerCount},(_,i)=>({character:setup[8+i*2],shot:setup[9+i*2]}))),
  name:new TextDecoder().decode(bytes.subarray(72,80)).replace(/\0.*$/s,'').trim(),
  timestamp:Number.isSafeInteger(timestamp)?timestamp:0,score:u32(64)*10,
  completed:!!(u32(68)&1),cheat:!!(u32(68)&2),buildWords:Object.freeze(setup.slice(17,21)),
 });
}
