// Retail title.anm scripts and Ending entry selection (410d76..410d95).
// Original images/comments/replays are prepared locally and are not source.
const freeze=value=>{if(value&&typeof value==='object'){for(const child of Object.values(value))freeze(child);Object.freeze(value);}return value;};
export const MENU_SCRIPTS=freeze({
  bank:'title',
  title:{background:99,backgroundLeaves:[100,101,102,103],logo:[91,92],version:{bank:'title_v',script:0},
    menuRoot:0,origin:[520,148],inactive:[11,12,13,14,15,16,17,18],active:[3,4,5,6,7,8,9,10],
    entries:['start','extra','practice','replay','result','music','options','quit'],selectionInterrupts:[7,8,9,10,11,12,13,14]},
  options:{root:1,origin:[500,180],inactive:[24,25,26,27,28],active:[19,20,21,22,23],
    entries:['bgmVolume','seVolume','keyConfig','defaults','quit'],selectionInterrupts:[7,8,9,10,11]},
  keyConfig:{root:2,origin:[480,180],inactive:[52,53,54,55,56,57,58],active:[45,46,47,48,49,50,51],
    entries:['shot','bomb','slow','pause','skip','defaults','quit'],selectionInterrupts:[7,8,9,10,11,12,13]},
  select:{background:107,firstBackground:106,highlights:[119,120],difficultyRoot:131,extraDifficultyRoot:132,
    difficultyOrigin:[192,178],difficultyDescriptions:[121,122,123,124,125],difficultyLabels:[126,127,128,129,130],
    characterRoot:143,characterNames:[133,134,135],characterPortraits:[140,141,142],
    shotRoots:[175,176,177,178,179,180],shotDescriptions:[[156,157],[158,159],[160,161],[162,163],[164,165],[166,167]],
    shotLabels:[[144,145],[146,147],[148,149],[150,151],[152,153],[154,155]],
    shotWeaponNames:[[181,182],[183,184],[185,186],[187,188],[189,190],[191,192]],
    headings:{difficulty:110,character:111,shot:112,replay:113,result:114,music:115,options:116,replaySave:117,practiceStage:118}},
  result:{headers:[193,194,195,196,197,198,199,200,201,202],totals:[203,204,205]},
  music:{headers:[206,207,208,209,210],firstLine:211,lineCount:22,selectedInterrupt:2,inactiveInterrupt:3},
  interrupts:{retire:1,selected:2,unselected:3,hide:6,enable:28,disable:27},
  evidence:{titleConstructor:'0x43f483',musicRoom:'0x449360',endingSelection:'0x410d76',endingInterpreter:'0x411420'}
});

export function selectEnding(character,shot,continued=false){
  if(!Number.isInteger(character)||character<0||character>2||!Number.isInteger(shot)||shot<0||shot>1)throw Error('结局机体无效');
  return character*2+shot+(continued?0:6);
}

export function musicComment(resources,id){
  if(typeof id==='number')return resources?.musicComments?.[id]??null;
  const key=String(id??'').replace(/^bgm\//,'').replace(/\.(?:wav|ogg)$/,'');
  return resources?.musicComments?.find(item=>item.id===key)??null;
}

// Native420e40: Shift-JIS bytes XOR rolling key77, step7 increasing by10.
export function decodeEndingText(source){
  const data=source instanceof Uint8Array?source:new Uint8Array(source),decoded=[];
  let key=0x77,step=7;
  for(const value of data){const byte=value^key;key=(key+step)&255;step=(step+16)&255;
    if(!byte)return new TextDecoder('shift-jis',{fatal:true}).decode(Uint8Array.from(decoded));decoded.push(byte);}
  throw Error('结局文字缺少结束标记');
}

export function decodeEndingMessage(source){
  const data=source instanceof Uint8Array?source:new Uint8Array(source),view=new DataView(data.buffer,data.byteOffset,data.byteLength);
  if(data.length<12||view.getUint32(0,true)!==1||view.getUint32(4,true)!==12)throw Error('结局对话头无效');
  let cursor=12,terminated=false;const commands=[];
  while(cursor+4<=data.length){
    const time=view.getUint16(cursor,true),op=data[cursor+2],length=data[cursor+3],end=cursor+4+length;
    if(end>data.length)throw Error('结局对话命令越界');
    const payload=data.slice(cursor+4,end),ints=length%4===0?Array.from({length:length/4},(_,i)=>view.getInt32(cursor+4+i*4,true)):null;
    const command={time,op,offset:cursor,payloadHex:Array.from(payload,b=>b.toString(16).padStart(2,'0')).join('')};
    if(ints)command.ints=ints;
    if(op===3){command.kind='text';command.text=decodeEndingText(payload);}
    else if(op===7){if(length<5)throw Error('结局动画资源命令无效');command.kind='load-bank';command.bankSlot=ints?.[0]??new DataView(payload.buffer).getInt32(0,true);command.file=new TextDecoder().decode(payload.subarray(4)).split('\0')[0];}
    else if([8,15,16,17].includes(op)){if(length!==12)throw Error('结局动画命令无效');Object.assign(command,{kind:'animation',slot:ints[0],bankSlot:ints[1],script:ints[2]});if(op>=15)command.difficulty=op-14;}
    else if(op===5||op===6){if(length!==4)throw Error('结局等待命令无效');Object.assign(command,{kind:'wait',frames:ints[0],clearText:op===6});}
    else if(op===9){if(length!==4)throw Error('结局文字颜色命令无效');command.kind='text-color';command.colorRGB=ints[0]>>>0;}
    else if(op===10){command.kind='music';command.file=new TextDecoder().decode(payload).split('\0')[0];command.track=command.file.split('/').at(-1).replace(/\.wav$/,'');}
    else if(op===12){command.kind='next-message';command.file=new TextDecoder().decode(payload).split('\0')[0];}
    else if(op===13||op===14){if(length!==4)throw Error('结局淡出命令无效');Object.assign(command,{kind:'fade',frames:ints[0],fadeOut:op===14});}
    else command.kind=({0:'end',4:'hide-text',11:'fade-music'})[op]??'native-command';
    commands.push(command);cursor=end;if(op===0){terminated=true;break;}
  }
  if(!terminated||data.subarray(cursor).some(b=>b!==0))throw Error('结局对话结束标记无效');
  return commands;
}

export function validateMenuResources(resources){
  if(resources?.schema!=='th12-menu-resources/1'||!resources.atlases||!Array.isArray(resources.musicComments)||!Array.isArray(resources.endings)||resources.endings.length!==12||!Array.isArray(resources.demos)||resources.demos.length!==4||!Array.isArray(resources.credits?.commands))throw Error('菜单原版资源不完整');
  for(const bank of ['sig','staff',...Array.from({length:12},(_,i)=>'e'+String(i).padStart(2,'0'))]){
    const atlas=resources.atlases[bank];if(!atlas?.entries?.length||!atlas.sprites||!atlas.scripts)throw Error('菜单动画资源不完整');
  }
  for(const item of [...resources.endings,resources.credits])if(!item.commands.length||item.commands.at(-1).op!==0)throw Error('菜单结局流程不完整');
  return resources;
}
