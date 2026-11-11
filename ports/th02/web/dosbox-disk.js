import {Fat12} from './solo-fat12.js';

export function extractGame(disk){
  const fat=new Fat12(disk),files=[],entries=new Map(),decode=new TextDecoder('shift_jis');
  function walk(ranges,parent){
    for(const [start,size] of ranges)for(let at=start;at<start+size;at+=32){
      const first=disk[at],attr=disk[at+11];
      if(!first)return;
      if(first===0xe5||(attr&8)||attr===15)continue;
      const stem=decode.decode(disk.subarray(at,at+8)).trim(),ext=decode.decode(disk.subarray(at+8,at+11)).trim();
      if(stem==='.'||stem==='..')continue;
      const name=parent+stem+(ext?'.'+ext:'');
      if(attr&16){
        if(name==='HUMAROKU')walk(fat.chain(fat.view.getUint16(at+26,true)).map(c=>[fat.offset(c),fat.clusterSize]),name+'/');
      }else if(name.startsWith('HUMAROKU/')||name==='NBHUD.BIN'){
        files.push({path:'game/'+name,contents:fat.read(at)});entries.set(name,at);
      }
    }
  }
  walk([[fat.root,fat.rootSize]],'');
  if(!entries.has('HUMAROKU/MAIN.EXE')||!entries.has('HUMAROKU/GAME.BAT'))throw Error('TH02 游戏文件缺失');
  return {files,export(fs){
    const copy=new Uint8Array(disk),saved=new Fat12(copy);
    for(const [name,at]of entries){
      const bytes=fs.readFile('/home/web_user/game/'+name),old=saved.read(at);
      if(bytes.length!==old.length||bytes.some((b,i)=>b!==old[i]))saved.replace(at,bytes);
    }
    return copy;
  }};
}

export function prepareFastBoot(extracted,{network=false,directLaunch=false}={}){
  const bat=extracted.files.find(f=>f.path==='game/HUMAROKU/GAME.BAT');
  if(!bat)throw Error('TH02 启动脚本缺失');
  let text=Array.from(bat.contents,c=>String.fromCharCode(c)).join('');
  if(directLaunch){
    // WEBSTART replaces OP's introduction. Keep ZUN's resident setup and PMD,
    // but omit the separate, timed ZUNSOFT logo before entering MAIN.
    let logos=0;
    text=text.replace(/^zun zunsoft\.com[ \t]*\r*$/gm,()=>{logos++;return '';});
    if(logos!==6)throw Error('TH02 标志动画入口与已校验版本不同');
  }
  let entries=0;
  const replacement=text.replace(/^(op|main)[ \t]*\r*$/gm,(_,command)=>{
    entries++;return `z:\\system\\config.com -set "cpu cycles=${network?'30000':'max 100%'}"\r\n${directLaunch?'WEBSTART\r\nif errorlevel 1 goto fin\r\nmain':command}\r`;
  });
  if(entries!==6)throw Error('TH02 启动入口与已校验版本不同');
  return {path:'game/HUMAROKU/FASTBOOT.BAT',contents:Uint8Array.from(replacement,c=>c.charCodeAt(0))};
}

export function dosboxConfig({network=false,fastBoot=false}={}){
  return `[dosbox]\nmachine=pc98\nfastbioslogo=${fastBoot}\nmemsize=16\n[cpu]\ncore=simple\ncputype=386\ncycles=${fastBoot?1000:network?30000:'max 100%'}\n[render]\nframeskip=0\nscaler=none\naspect=false\n[sblaster]\nsbtype=none\n[pc98]\npc-98 fm board=board86\n[mixer]\nrate=44100\nblocksize=512\nprebuffer=20\n[autoexec]\n@echo off\nmount a /home/web_user/game\na:\npath a:\\HUMAROKU;a:\\\ncd HUMAROKU\n${fastBoot?'FASTBOOT.BAT':'GAME.BAT'}\n`;
}
