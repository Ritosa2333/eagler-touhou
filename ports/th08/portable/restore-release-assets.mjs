// Recover build inputs from a verified release without redistributing a second
// copy of its game data inside the source directory.
import {readFileSync,writeFileSync,mkdirSync,copyFileSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const workspace=fileURLToPath(new URL('../',import.meta.url)),site=resolve(process.argv[2]||resolve(workspace,'../site')),inventory=JSON.parse(readFileSync(resolve(site,'files.json')));
const read=name=>{const b=readFileSync(resolve(site,name));if(createHash('sha256').update(b).digest('hex')!==inventory[name])throw Error('Release hash mismatch: '+name);return b;};
const game=JSON.parse(read('manifest.json')).game;if(!['th08','th10'].includes(game))throw Error('Unknown game');
const write=(p,b)=>{mkdirSync(dirname(p),{recursive:true});writeFileSync(p,b);},data=read(`packages/${game}/${game}.data`),index=JSON.parse(read(`runtime/${game}/${game}.data.json`)),native=resolve(workspace,game+'_web/assets/sdl-native');
for(const f of index.files){if(!/^\/(?:game|fonts|midi)\/[a-z0-9_.-]+$/.test(f.filename)||!Number.isSafeInteger(f.start)||!Number.isSafeInteger(f.end)||f.start<0||f.end<=f.start||f.end>data.length)throw Error('Invalid DATA entry');
 let target;if(f.filename.startsWith('/fonts/'))target=resolve(native,f.filename.slice(7));
 else if(f.filename==='/midi/gm.dls')target=resolve(workspace,'th08_web/assets/native/gm.dls');
 else if(game==='th08'&&f.filename==='/game/thbgm.dat')target=resolve(native,'thbgm-header.dat');
 else if(f.filename.startsWith('/game/'))target=resolve(workspace,game==='th10'?'[th10] 东方风神录 (汉化版+日文版)':'[th08] 东方永夜抄 (日文版)',f.filename.slice(6));
 else throw Error('Unsupported DATA entry');write(target,data.subarray(f.start,f.end));
}
const music=game==='th08'?index.music:Array.from({length:18},(_,i)=>String(i).padStart(2,'0')+'.flac');
for(const name of music){if(!/^[a-z0-9_]+\.(?:flac|ogg)$/.test(name))throw Error('Invalid music file');write(resolve(native,'music',name),read(`packages/${game}/music/${name}`));}
write(resolve(workspace,game+'_web/artifacts/sdl3/browser/title.png'),read(`assets/${game}-card.png`));
const verification=resolve(workspace,game==='th08'?'../validation/music-ogg.json':'../validation/music-lossless.json');copyFileSync(verification,resolve(native,'music-verification.json'));
console.log('Restored game archives, fonts, music and card artwork from the verified release.');
