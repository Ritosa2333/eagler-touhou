import {Fat12} from './solo-fat12.js';
// Keep a private FAT12 image for the existing import/export save interface.
export function extractGame(disk){
  const fat=new Fat12(disk),files=[],entries=new Map(),decode=new TextDecoder('shift_jis');
  function walk(ranges,parent){for(const [start,size]of ranges)for(let at=start;at<start+size;at+=32){
    const b=disk[at],attr=disk[at+11];if(!b)return;if(b===0xe5||(attr&8)||attr===15)continue;
    const stem=decode.decode(disk.subarray(at,at+8)).trim(),ext=decode.decode(disk.subarray(at+8,at+11)).trim();if(stem==='.'||stem==='..')continue;
    const name=parent+stem+(ext?'.'+ext:'');
    if(attr&16){if(name==='GENSO')walk(fat.chain(fat.view.getUint16(at+26,true)).map(c=>[fat.offset(c),fat.clusterSize]),name+'/');}
    else if(name.startsWith('GENSO/')||name==='COOP.COM'||name==='NBHUD.BIN'){files.push({path:'game/'+name,contents:fat.view.getUint32(at+28,true)?fat.read(at):new Uint8Array()});entries.set(name,at);}
  }}
  walk([[fat.root,fat.rootSize]],'');
  if(!entries.has('GENSO/MAIN.EXE')||!entries.has('GENSO/GAME.BAT'))throw Error('TH04 游戏文件缺失');
  return {files,export(fs){const copy=new Uint8Array(disk),saved=new Fat12(copy);for(const [name,at]of entries){const bytes=fs.readFile('/home/web_user/game/'+name);const old=saved.view.getUint32(at+28,true)?saved.read(at):new Uint8Array();if(bytes.length!==old.length||bytes.some((b,i)=>b!==old[i]))saved.replace(at,bytes);}return copy;}};
}
// Only the private launcher uses a smaller CPU budget. Every native game entry
// restores its normal budget before executing MAIN / OP, including on replay.
export function prepareFastBoot(extracted,{network=false,practice=false}={}){
  const bat=extracted.files.find(f=>f.path==='game/GENSO/GAME.BAT');
  if(!bat)throw Error('TH04 启动脚本缺失');
  const text=Array.from(bat.contents,c=>String.fromCharCode(c)).join('');
  let entries=0;
  const replacement=text.replace(/^(main|op)\r*$/gm,(_,command)=>{
    entries++;
    return `z:\\system\\config.com -set "cpu cycles=${network?30000:practice?16000:'max 100%'}"\r\n${command}\r`;
  });
  if(entries!==5)throw Error('TH04 启动入口与当前版本不匹配');
  return {path:'game/GENSO/FASTBOOT.BAT',contents:Uint8Array.from(replacement,c=>c.charCodeAt(0))};
}
export function dosboxConfig({network=false,fastBoot=false}={}){
  return `[dosbox]\nmachine=pc98\nmemsize=16\n[cpu]\ncore=simple\ncputype=386\ncycles=${fastBoot?1000:network?30000:'max 100%'}\n[render]\nframeskip=0\nscaler=none\naspect=false\n[sblaster]\nsbtype=none\n[pc98]\npc-98 fm board=board86\n[mixer]\nrate=44100\nblocksize=512\nprebuffer=20\n[autoexec]\n@echo off\nmount a /home/web_user/game\na:\npath a:\\GENSO;a:\\\ncd GENSO\n${fastBoot?'FASTBOOT.BAT':'GAME.BAT'}\n`;
}
