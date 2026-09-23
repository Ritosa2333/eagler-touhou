import {exportReplayName,importReplayName} from './motion-replay.mjs';

const magic=[69,65,71,76,82,80,89,49]; // EAGLRPY1; schema is validated by C++.
export function hasMultiplayerReplaySignature(bytes){
 return bytes instanceof Uint8Array&&bytes.length>=magic.length&&magic.every((value,i)=>bytes[i]===value);
}

export function createReplayFilePolicy({game,multiplayer=false,validateMultiplayer}){
 if(!Number.isInteger(game)||game<1)throw new TypeError('Replay game ID required');
 if(multiplayer&&typeof validateMultiplayer!=='function')throw new TypeError('Native multiplayer validator required');
 const isReplay=path=>/\.rpyx?$/.test(path);
 return Object.freeze({
  physical(path){return path.endsWith('.rpyx')?path.slice(0,-1):path;},
  exported(path,bytes){
   if(!isReplay(path))return path;
   // Even a corrupt MP file is never mislabeled as a retail-compatible .rpy.
   if(multiplayer||hasMultiplayerReplaySignature(bytes))return path.endsWith('.rpy')?path+'x':path;
   return exportReplayName(path,bytes,game);
  },
  imported(path,bytes){
   if(!isReplay(path))return path;
   if(!(bytes instanceof Uint8Array)||!bytes.length||bytes.length>16*1024*1024)throw Error('录像数据无效');
   if(multiplayer){
    if(!hasMultiplayerReplaySignature(bytes)||!validateMultiplayer(bytes))
     throw Error('多人录像损坏、版本不兼容或不属于这款游戏');
    return path.endsWith('.rpyx')?path.slice(0,-1):path;
   }
   if(hasMultiplayerReplaySignature(bytes))throw Error('多人录像只能在多人录像播放器中打开');
   return importReplayName(path,bytes,game);
  },
 });
}
