// Static TH12 1.00b presentation reconstruction. No native execution or visual
// comparison was performed for this module. Evidence is the reviewed assembly
// and local title.anm/ascii.anm instruction dumps, with PE strings read as data.
const freeze=value=>{if(value&&typeof value==='object'){Object.values(value).forEach(freeze);Object.freeze(value);}return value;};
const labels=freeze({
  characters:['ReimuA ','ReimuB ','MarisaA','MarisaB','SanaeA ','SanaeB '],
  difficulties:['Easy   ','Normal ','Hard   ','Lunatic','Extra  '],
  stages:['test   ','Stage 1','Stage 2','Stage 3','Stage 4','Stage 5','Stage 6','Extra  '],
  finalStages:['tst','St1','St2','St3','St4','St5','St6','Ex ','All'],
});

export const NATIVE_REPLAY_LAYOUT=freeze({
  bank:'title',font:0,fontAdvance:9,fontHeight:14,scaleX:1,scaleY:1,alignX:1,alignY:1,drawFlag:1,
  colors:{selected:0xffffff00,inactive:0xff808080,lockedSelected:0xffdfdfdf,confirmBlink:0xff000000,white:0xffffffff},
  labels,
  formats:{
    slot:'No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%',
    emptySlot:'No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%',
    user:'%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%',
    emptyUser:'User  -------- --/--/-- --:-- ------- ------- --- ---%%',
    unavailableStage:'%s  ---------',practiceScore:'%s  %.8d0',replayStageScore:'%s  %.8d%d',
  },
  replay:{titleState:11,tick:'4466a0',draw:'446c20',heading:113,
    rows:25,pages:3,fixedSlots:25,importedSlots:50,x:58,y:80,rowStep:15,
    //446560: directory enumeration order, not a new sort by filename/date.
    fixedPattern:'th12_%.2d.rpy',importedPattern:'th12_ud????.rpy',
    columns:{identifier:58,name:112,date:193,time:274,character:328,difficulty:400,stage:472,slowdown:508},
    states:{initialize:0,enter:1,list:2,start:3,stageSelect:4,exit:5},
    readyAfter:6,stageInputReadyAt:15,fadeAt:2,startAt:32,
    pageIndicator:false,columnHeadings:false,
    selection:{rowWrap:true,pageWrap:true,preserveRowOnPageChange:true,allowEmptyHighlight:true,confirmEmpty:false},
  },
  replayStage:{rows:7,x:220,y:128,rowStep:18,heading:113,summary:{x:58,y:80},
    summaryTransitionFrames:10,readyAt:15,missingRecordsDisabled:true,selectedYellow:true,
    //446930 tests the stage header pointer at manager+stage*36+b8, and stores
    //missing stages in SelectionItem's skip array. All seven rows still draw.
    scoreForStage1To5:'next recorded stage initial.score and initial.nativeGlobals[0], otherwise final scoreStored/continues',
    scoreForStage6AndExtra:'final scoreStored/continues',
  },
  replaySave:{titleState:15,tick:'4489e0',draw:'448fb0',heading:117,
    rows:25,pages:1,x:58,y:80,rowStep:15,readyAfter:6,
    states:{initialize:0,enter:1,slotSelect:2,nameEntry:3,exit:4},
    extraCancelRow:false,overwriteConfirmation:false,nameEntryAfterSlot:true,returnToSlotsAfterSave:true,
    //449099 submits the current record with eight spaces for the name and
    //the literal All stage label. Naming is part of ReplaySave, not a separate
    //generic heading or a "Do not save" slot.
    summary:{x:58,y:240,transitionFrames:10,name:'        ',finalStageLabel:'All'},
    name:{x:112,y:240,length:8,cursorAdvance:9,cursorAtFullLength:7,
      gridX:212,gridY:360,columns:13,rows:7,columnStep:18,rowStep:16,
      characters:'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*',
      special:[{index:88,action:'space',glyphCode:0x81,sprite:97},{index:89,action:'delete',glyphCode:0x7f,sprite:95},{index:90,action:'end',glyphCode:0x80,sprite:96}],
      fullNameNextSelection:90,horizontalWrapWithinRow:true,verticalStep:13,
      cancelDeletesThenReturnsToSlots:true,replaceLastCharacterWhenFull:true},
  },
  practice:{titleState:8,tick:'445fc0',draw:'4463c0',heading:118,highlight:120,
    rows:6,x:296,y:216,rowStep:18,readyAfter:10,textVisibleAt:10,fadeAt:10,startAt:40,
    highlightLockedRows:true,confirmLocked:false,rememberSelection:true,
    unlock:'Score + (character*2+shot)*0x45f4 + (difficulty*6+stageIndex)*8 +0x5b1',
  },
  controls:{pressed:'4d48c4',repeat:'4d48c0',up:0x10,down:0x20,left:0x40,right:0x80,
    confirm:0x80001,cancel:0x102,selectionSound:10,confirmSound:7,cancelSound:9,lockedSound:17},
  animation:{
    //title/select01.png entry6. These are actual textured headings; an extra
    //ASCII English title, arbitrary arrow character or CSS button has no source.
    heading:{scripts:[113,117,118],sprites:[75,79,80],texture:'title/select01.png',
      spriteRects:[[0,144,256,48],[0,336,256,48],[0,384,256,48]],
      x:32,y:8,fromX:-256,enterFrames:20,interpolation:4,retireInterrupt:1,retireFrames:20,layer:25},
    practiceHighlight:{script:120,sprite:81,spriteRect:[1,497,14,14],x:382,y:270,
      alpha:128,targetAlpha:208,scale:[14,10],enterFrames:20,retireInterrupt:1,layer:24},
    //ASCII script20 spawns twenty script19 VMs. It is the async scan's loading
    //animation (446725), not a page number. Interrupt1 retires it.
    replayLoading:{bank:'ascii',script:20,children:19,childCount:20,retireInterrupt:1},
  },
  evidence:{
    rowsAndPages:['4466dc','4466f0','446ee3','447090','449319'],
    coordinates:['446ec3','446edd','447076','446dc3','446dda','446e84','44640f','44641b','446524'],
    strings:{formats:[0x4a1e78,0x4a1e3c,0x4a1e08,0x4a1dd0,0x4a1f58,0x4a1f68,0x4a1f88],
      characters:0x4aee20,difficulties:0x4aee38,stages:0x4aee60,finalStages:0x4aee88,nameGrid:0x4a0e88},
    font:['401019 zero-initializes18f8c','401026 unit scales','401569 reads18f8c into text submission','401bxx font0 ASCII'],
    replayControl:['4467df..4468dd','446983..446a43','446a5b..446b59'],
    saveControl:['448b18..448c8c','448c96..448ed2'],
    practiceControl:['446004..44606d','446108..446152','4462d3..44638d'],
  },
});

const pad=(value,digits=2)=>String(Math.trunc(value)).padStart(digits,'0');
const precisionInteger=(value,digits=8)=>{value=Math.trunc(Number(value)||0);return (value<0?'-':'')+String(Math.abs(value)).padStart(digits,'0');};
const ascii=(text,x,y,colorARGB=0xffffffff)=>({text:String(text),x,y,font:0,colorARGB:colorARGB>>>0,scaleX:1,scaleY:1,alignX:1,alignY:1,drawFlag:1});
const moveY=(from,to,age,frames=10)=>from+(to-from)*Math.min(1,Math.max(0,Number(age)||0)/frames);
function fields(record){
  if(record?.data instanceof Uint8Array&&record.data.length>=112){
    const data=record.data,v=new DataView(data.buffer,data.byteOffset,data.byteLength);
    return {...record,name:new TextDecoder('shift-jis').decode(data.subarray(0,8)),timestamp:Number(v.getBigInt64(12,true)),
      scoreStored:v.getInt32(20,true),slowdown:v.getFloat32(84,true),character:v.getUint32(92,true),shot:v.getUint32(96,true),
      difficulty:v.getUint32(100,true),finalStage:v.getUint32(104,true),continues:v.getInt32(108,true)};
  }
  return record??{};
}

//25 fixed slots are always drawn, including absent files. Pages1/2 preserve
//the file enumeration's user suffix; displaying the host filename is separate.
export function nativeReplayRow(record,{slot=1,user=false,userCode='',name,finalStageLabel}={}){
  const f=NATIVE_REPLAY_LAYOUT.formats;
  if(!record)return user?f.emptyUser.replace('%%','%'):f.emptySlot.replace('%.2d',pad(slot)).replace('%%','%');
  record=fields(record);
  const date=record.timestamp instanceof Date?record.timestamp:new Date(Number(record.timestamp??0)*1000);
  const dateText=[date.getFullYear()%100,date.getMonth()+1,date.getDate()].map(n=>pad(n)).join('/');
  const timeText=[date.getHours(),date.getMinutes()].map(n=>pad(n)).join(':');
  const prefix=user?String(userCode||record.userCode||String(record.id??'').match(/th12_ud(.{4})\.rpy$/i)?.[1]||'User')+'  ':'No.'+pad(slot)+' ';
  return prefix+String(name??record.name??'').slice(0,8).padEnd(8,' ')+' '+dateText+' '+timeText+' '+
    (labels.characters[(record.character??0)*2+(record.shot??0)]??'-------')+' '+(labels.difficulties[record.difficulty??0]??'-------')+' '+
    (finalStageLabel??labels.finalStages[record.finalStage??0]??'---')+' '+Number(record.slowdown??0).toFixed(1)+'%';
}

//`records` follows native file slots: 0..24 fixed,25..74 imported. `selection`
//is the row within the page, rather than a flattened list of existing files.
export function nativeReplayListDescriptors(records=[],{page=0,selection=0,save=false}={}){
  const layout=save?NATIVE_REPLAY_LAYOUT.replaySave:NATIVE_REPLAY_LAYOUT.replay;
  page=Math.max(0,Math.min(save?0:2,Math.trunc(page)||0));
  return Array.from({length:25},(_,row)=>ascii(nativeReplayRow(records[page*25+row],{slot:row+1,user:page>0}),
    layout.x,layout.y+row*layout.rowStep,row===selection?0xffffff00:0xff808080));
}

export function nativeReplayStageDescriptors(record,{selection=0,fileIndex=0,page=0,age=15}={}){
  record=fields(record);const layout=NATIVE_REPLAY_LAYOUT.replayStage;
  const out=[ascii(nativeReplayRow(record,{slot:fileIndex+1,user:page>0}),58,moveY(80+(fileIndex%25)*15,80,age))];
  if(age<10)return out;
  for(let number=1;number<=7;number++){
    const stage=record.stages?.find(stage=>stage.number===number),next=number<6?record.stages?.find(stage=>stage.number===number+1):null;
    const score=next?.initial?.score??record.scoreStored??0,digit=next?.initial?.nativeGlobals?.[0]??record.continues??0;
    out.push(ascii(labels.stages[number]+'  '+(stage?precisionInteger(score)+String(Math.trunc(digit)):'---------'),
      layout.x,layout.y+(number-1)*layout.rowStep,selection===number-1?0xffffff00:0xff808080));
  }
  return out;
}

export function nativePracticeDescriptors(rows=[],{selection=0,age=10,confirming=false}={}){
  if(age<10&&!confirming)return [];
  return Array.from({length:6},(_,index)=>{
    const row=rows[index]??{},unlocked=row.unlocked??(row.disabled!==undefined?!row.disabled:false);
    const color=index!==selection?0xff808080:!unlocked?0xffdfdfdf:confirming&&(age%4)>=2?0xff000000:0xffffff00;
    return ascii(labels.stages[index+1]+'  '+(unlocked?precisionInteger(row.scoreStored??0)+'0':'---------'),296,216+index*18,color);
  });
}

export function nativeReplayNameDescriptors(record,{slot=1,name='        ',cursor=0,selection=0,age=10}={}){
  const out=[ascii(nativeReplayRow(record,{slot,name:'        ',finalStageLabel:'All'}),58,moveY(80+(slot-1)*15,240,age))];
  if(age<10)return out;
  out.push(ascii(String(name).slice(0,8).padEnd(8,' '),112,240));
  out.push(ascii('_',112+9*Math.min(7,Math.max(0,Math.trunc(cursor))),240,0xffffff00));
  const chars=NATIVE_REPLAY_LAYOUT.replaySave.name.characters;
  for(let index=0;index<91;index++)out.push(ascii(index<88?chars[index]:String.fromCharCode([0x81,0x7f,0x80][index-88]),
    212+(index%13)*18,360+Math.floor(index/13)*16,index===selection?0xffffff00:0xff808080));
  return out;
}

//43230f..4324f5 draws the common in-game save page within the playfield;
//its shorter labels and date differ from TitleInf's full-width replay page.
function pauseReplayRow(record,slot=1,{naming=false}={}){
  if(!record)return 'No.'+pad(slot)+' -------- --/--/-- ------- - St-';
  record=fields(record);const date=new Date(Number(record.timestamp??0)*1000);
  const when=[date.getFullYear()%100,date.getMonth()+1,date.getDate()].map(n=>pad(n)).join('/');
  return 'No.'+pad(slot)+' '+(naming?'        ':String(record.name??'').slice(0,8).padEnd(8,' '))+' '+when+' '+
    (labels.characters[(record.character??0)*2+(record.shot??0)]??'-------')+' '+(['E','N','H','L','X'][record.difficulty]??'-')+' '+
    (labels.finalStages[record.finalStage??0]??'St-');
}
export function nativePauseReplayListDescriptors(records=[],{selection=0}={}){
  return Array.from({length:25},(_,i)=>ascii(pauseReplayRow(records[i],i+1),48,64+i*15,i===selection?0xffffff00:0xff808080));
}
export function nativePauseReplayNameDescriptors(record,{slot=1,name='',cursor=0,selection=90,age=10}={}){
  const y=moveY(64+(slot-1)*15,224,age),out=[ascii(pauseReplayRow(record,slot,{naming:true}),48,y,0xffffff00)];
  if(age<10)return out;
  out.push(ascii(String(name).slice(0,8).padEnd(8,' '),102,224),ascii('_',102+9*Math.min(7,cursor),224,0xffffff00));
  const chars=NATIVE_REPLAY_LAYOUT.replaySave.name.characters;
  for(let i=0;i<91;i++)out.push(ascii(i<88?chars[i]:String.fromCharCode([0x81,0x7f,0x80][i-88]),112+i%13*18,320+Math.floor(i/13)*16,i===selection?0xffffff00:0xff808080));
  return out;
}
