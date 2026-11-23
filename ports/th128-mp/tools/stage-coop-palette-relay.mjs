import {readFile,writeFile,mkdir,lstat,realpath} from 'node:fs/promises';
import {basename,dirname,resolve,join,relative,isAbsolute,sep} from 'node:path';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';

// Extend the reviewed, out-of-tree author Relay. Existing deployments and
// unrelated room products retain their previous settings and behavior.
const owner='services/relay/src/netplay-relay.mjs',catalog='common/contracts/product-catalog.mjs',receipt='coop-receipt.json';
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
function localCatalog(source){
  const start=source.indexOf('const TH128_PREVIEW_ROOM ='),end=source.indexOf('export const PRODUCT_IDS',start);
  if(start<0||end<0)throw Error('The checkout does not have the TH128 local Launcher bridge');
  let room=source.slice(start,end);
  for(const [before,after] of [["gameplay: 'parallel'","gameplay: 'cooperative'"],
    ["playerCounts: Object.freeze([2]), difficulties: Object.freeze(['Normal'])","playerCounts: Object.freeze([2,3]), difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES"],
    ['challengeMode: false','challengeMode: true']]){
    if(room.split(before).length!==2)throw Error('TH128 local room catalog context changed: '+before);
    room=room.replace(before,after);
  }
  return Buffer.from(source.slice(0,start)+room+source.slice(end));
}
function addTh128RoomRules(source,checkout){
  const edits=[
    ['      challengeMode: false,',"      ...(id.startsWith('th128mp-')?{randomCirnoPalette:false,freezeMotivationMultiplier:1}:{}),\n      challengeMode: false,"],
    ['    challengeMode: room.lobby.challengeMode,', "    ...(typeof room.lobby.randomCirnoPalette==='boolean'?{randomCirnoPalette:room.lobby.randomCirnoPalette,freezeMotivationMultiplier:room.lobby.freezeMotivationMultiplier}:{}),\n    challengeMode: room.lobby.challengeMode,"],
    ["      const modesSupported = room.multiplayer?.gameplay === 'cooperative' && room.multiplayer.challengeMode !== false;",`      if(message.randomCirnoPalette!==undefined&&(typeof room.lobby.randomCirnoPalette!=='boolean'||typeof message.randomCirnoPalette!=='boolean')){
        sendLobby(socket,{type:'error',error:'invalid Cirno palette setting'});return;
      }
      if(message.freezeMotivationMultiplier!==undefined&&(room.lobby.freezeMotivationMultiplier===undefined||![2.5,2,1,0.75,0.5].includes(message.freezeMotivationMultiplier))){
        sendLobby(socket,{type:'error',error:'invalid TH128 freeze motivation multiplier'});return;
      }
      const randomCirnoPalette=message.randomCirnoPalette??room.lobby.randomCirnoPalette;
      const freezeMotivationMultiplier=message.freezeMotivationMultiplier??room.lobby.freezeMotivationMultiplier;
      const modesSupported = room.multiplayer?.gameplay === 'cooperative' && room.multiplayer.challengeMode !== false;`],
    ['        room.lobby.challengeMode !== challengeMode ||','        room.lobby.randomCirnoPalette !== randomCirnoPalette ||\n        room.lobby.freezeMotivationMultiplier !== freezeMotivationMultiplier ||\n        room.lobby.challengeMode !== challengeMode ||'],
    ['        room.lobby.challengeMode = challengeMode;',"        if(randomCirnoPalette!==undefined)room.lobby.randomCirnoPalette=randomCirnoPalette;\n        if(freezeMotivationMultiplier!==undefined)room.lobby.freezeMotivationMultiplier=freezeMotivationMultiplier;\n        room.lobby.challengeMode = challengeMode;"],
  ];
  let out=source.replaceAll('\r\n','\n');
  for(const [before,after] of edits){
    if(out.split(before).length!==2)throw Error('Author Relay lobby context changed: '+before);
    out=out.replace(before,after);
  }
  if(checkout){
    const start="const host = envValue('EAGLER_NETPLAY_RELAY_HOST', ['TH07_RELAY_HOST'], '0.0.0.0');";
    if(out.split(start).length!==2)throw Error('Author Relay host binding context changed');
    out=out.replace(start,start+"\nif (!['127.0.0.1','::1','localhost'].includes(host)) throw new Error('TH128 review Relay requires an explicit loopback host');");
  }
  return Buffer.from(out);
}
const args=process.argv.slice(2),checkout=args[0]==='--source';
if(checkout)args.shift();
const [sourceArg,destinationArg]=args;
if(args.length!==2||!sourceArg||!destinationArg)throw Error('Usage: stage-coop-palette-relay.mjs [--source] <checkout or pinned author Relay> <new destination>');
const root=await realpath(sourceArg),dest=resolve(destinationArg),target=join(await realpath(dirname(dest)),basename(dest));
const inside=(base,path)=>{const rel=relative(base,path);return !isAbsolute(rel)&&rel!=='..'&&!rel.startsWith('..'+sep);};
if(inside(root,target)||inside(target,root)||await lstat(target).catch(()=>null))throw Error('Palette Relay needs a fresh output outside its inputs');
const files=checkout?execFileSync('git',['-C',root,'ls-files','-z','--','common/contracts','services/relay/src',
  'services/relay/package.json','services/relay/package-lock.json','services/relay/LICENSE'],{encoding:'utf8',windowsHide:true})
  .split('\0').filter(Boolean).map(rel=>({rel})):JSON.parse(await readFile(join(root,receipt),'utf8'));
const contents=new Map();
for(const entry of files){
  const {rel,sha256}=entry;
  if(typeof rel!=='string'||!/^(?:[\w.-]+\/)*[\w.-]+$/.test(rel)||rel.split('/').some(part=>part==='..'||part==='.')||contents.has(rel))throw Error('Invalid Relay fixture filename');
  const path=join(root,rel);if(!inside(root,await realpath(path))||!(await lstat(path)).isFile())throw Error('Relay fixture escapes its source');
  const data=await readFile(path);if(!checkout&&sha(data)!==sha256)throw Error('Relay fixture byte mismatch: '+rel);
  contents.set(rel,rel===owner?addTh128RoomRules(data.toString('utf8'),checkout):checkout&&rel===catalog?localCatalog(data.toString('utf8')):data);
}
if(!contents.has(owner)||!contents.has(catalog))throw Error('Missing author Relay lobby or catalog');
await mkdir(target);
for(const [rel,data] of contents){const path=join(target,rel);await mkdir(dirname(path),{recursive:true});await writeFile(path,data,{flag:'wx'});}
await writeFile(join(target,receipt),JSON.stringify([...contents].map(([rel,data])=>({rel,sha256:sha(data)})),null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({relayRoot:target,files:contents.size}));
