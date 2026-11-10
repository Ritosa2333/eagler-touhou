// Static TH12 1.00b Result/NameRegist layout, read from original code/data.
// No game VM, original process, replay, browser or Oracle is run here.
const freeze=value=>{if(value&&typeof value==='object'){Object.values(value).forEach(freeze);Object.freeze(value);}return value;};

// 4321c9 /448937 reads exactly88 bytes at4a0e88. The following three
// native cells use ASCII129/127/128; font0 maps those to sprites97/95/96.
export const NATIVE_NAME_CHARACTERS='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*';
export const NATIVE_NAME_CELLS=freeze([
  ...Array.from(NATIVE_NAME_CHARACTERS,(text,index)=>({id:index,text,action:'character'})),
  {id:88,text:'\x81',action:'space'},
  {id:89,text:'\x7f',action:'delete'},
  {id:90,text:'\x80',action:'end'},
]);

export const NATIVE_DATA_LAYOUT=freeze({
  font:0,alignX:1,alignY:1,scaleX:1,scaleY:1,
  selected:0xffffff00,inactive:0xff808080,
  name:{columns:13,rows:7,count:91,maxLength:8,advance:9,
    //4488d8..4489a9 and data4a3f14/10/08,4a3d68.
    titleGrid:{x:212,y:360,stepX:18,stepY:16},
    //432163..43223a uses the same grid at a different origin.
    pauseGrid:{x:112,y:320,stepX:18,stepY:16},
    //448848..4488d3 overlays the new name in its ranking row.
    titleName:{x:84,y:160,rankStep:18},
    pauseName:{x:75,y:96,rankStep:18},
  },
  ranking:{x:48,y:160,rowStep:18,count:10,
    //447e73..447f94: R/G fade from255 to111; B stays255.
    tintStep:16,firstTint:255},
  totals:{x:328,playCountY:378,playTimeY:396,clearCountY:414},
  playerData:{loadoutCount:6,difficultyCount:5,defaultLoadout:0,defaultDifficulty:1,
    scorePage:0,spellRowsPerPage:10},
  //text.anm scripts25..34, @R sprites25..34; 4608f0 uses17px GDI text.
  spellRows:{bank:'text',firstScript:25,x:320,y:160,rowStep:18,width:512,height:17,
    fontHeight:17,fontFamily:'ＭＳ ゴシック',fontWeight:400,
    titleBytes:42,unknownTitle:'？'.repeat(21)},
  evidence:{nameDraw:'448690 /4320f0',dataController:'4470c0',dataDraw:'447e10',
    spellRows:'447b20',fontDefaults:'401000',nameTable:'4a0e88'},
});

const ascii=(text,x,y,colorARGB=0xffffffff)=>({text:String(text),x,y,font:0,
  colorARGB:colorARGB>>>0,scaleX:1,scaleY:1,alignX:1,alignY:1});
export function nativeNameDescriptors({name='',selection=90,rank=0,context='title',ranked=true}={}){
  //44883b: when the run does not enter the top ten, the ranking remains
  //visible with its ordinary gradient; no name/caret/grid is drawn.
  if(!ranked)return [];
  const grid=context==='pause'?NATIVE_DATA_LAYOUT.name.pauseGrid:NATIVE_DATA_LAYOUT.name.titleGrid;
  const entry=context==='pause'?NATIVE_DATA_LAYOUT.name.pauseName:NATIVE_DATA_LAYOUT.name.titleName;
  const text=String(name).slice(0,8).padEnd(8,' '),length=Math.min(String(name).length,8);
  const y=entry.y+entry.rankStep*rank;
  return [ascii(text,entry.x,y),ascii('_',entry.x+9*Math.min(length,7),y,0xffffff00),
    ...NATIVE_NAME_CELLS.map((c,i)=>ascii(c.text,grid.x+i%13*18,grid.y+Math.floor(i/13)*16,
      i===selection?0xffffff00:0xff808080))];
}

//4471ee..447274 selects one character, one shot and one difficulty header.
// Binding every member193..202 makes mutually exclusive headings overlap.
export function nativeDataHeaderScripts(character=0,shot=0,difficulty=1){
  return [193+character,196+shot,198+difficulty];
}
export function nativePlayerDataScripts(character=0,shot=0,difficulty=1){
  //4471c3..44739f, not a fabricated third summary screen.
  return [114,...nativeDataHeaderScripts(character,shot,difficulty),206,207,208,209,203,204,205,210];
}
export function nativeNameHeaderScripts(character=0,shot=0,difficulty=1){
  //44812a/448159/448188/4481b7.
  return [116,...nativeDataHeaderScripts(character,shot,difficulty)];
}

const width=(value,n)=>String(value).padStart(n,' ');
const two=value=>String(value).padStart(2,'0');
const stageLabels=['test   ','Stage 1','Stage 2','Stage 3','Stage 4','Stage 5','Stage 6','Extra  ','Clear  '];
export function nativeScoreRowText(record,index=0){
  const rank=width(index+1,2),name=String(record?.name??'        ').slice(0,8).padEnd(8,' ');
  const displayed=Math.max(0,Math.trunc(Number(record?.score??0)));
  const stored=Number.isInteger(record?.scoreStored)?record.scoreStored:Math.trunc(displayed/10);
  const digit=Number.isInteger(record?.continueCount)?record.continueCount:displayed%10;
  const score=width(stored,9)+digit;
  const date=record?.date?new Date(record.date):null;
  //4a204c is the original record with no timestamp.
  if(!date||!Number.isFinite(date.getTime()))return `${rank}  ${name}  ${score}  ----/--/-- --:--  Stage -  ---%`;
  const when=String(date.getFullYear()).padStart(4,'0')+'/'+two(date.getMonth()+1)+'/'+two(date.getDate())+' '+two(date.getHours())+':'+two(date.getMinutes());
  const stage=stageLabels[record.clear?8:record.stage]??stageLabels[0];
  //4a2014: rank/name/score-low-digit/date/stage/slow-rate. Keep zero instead
  // of inventing an unavailable slow-rate measurement for browser records.
  const slowdown=Number.isFinite(record.slowdown)?record.slowdown:0;
  return `${rank}  ${name}  ${score}  ${when}  ${stage}  ${slowdown.toFixed(1).padStart(3,' ')}%`;
}
export function nativeScoreDescriptors(records=[],{selectedRank=-1,nameEntry=false,ranked=true}={}){
  return Array.from({length:10},(_,i)=>{
    const tint=255-i*16;
    const color=nameEntry&&ranked?(i===selectedRank?0xffffffff:0xff404040):
      (0xff0000ff|(tint<<16)|(tint<<8))>>>0;
    return ascii(nativeScoreRowText(records[i],i),48,160+i*18,color);
  });
}
//43250c/432580: the in-game ranking uses a shorter date and a single space
//after its two-digit rank, so the name begins at75 rather than84.
export function nativePauseScoreDescriptors(records=[],{selectedRank=-1,ranked=true}={}){
  return [ascii('            Score Ranking!!',48,64),...Array.from({length:10},(_,i)=>{
    const r=records[i],name=String(r?.name??'        ').slice(0,8).padEnd(8,' '),stored=r?.scoreStored??Math.trunc((r?.score??0)/10),digit=r?.continueCount??(r?.score??0)%10;
    const prefix=width(i+1,2)+' '+name+' '+String(Math.trunc(stored)).padStart(9,'0')+Math.trunc(digit)+' ';
    const date=r?.date?new Date(r.date):null;
    const text=prefix+(date&&Number.isFinite(date.getTime())?[two(date.getFullYear()%100),two(date.getMonth()+1),two(date.getDate())].join('/')+' '+(stageLabels[r.clear?8:r.stage]??'Stage -'):'--/--/-- Stage -');
    return ascii(text,48,96+i*18,ranked&&i===selectedRank?0xffffff00:0xff808080);
  })];
}
export function nativeDataTotalDescriptors({playCount=0,playFrames=0,clearCount=0}={}){
  //447fd1 /44804d /448082; original time counter is in60Hz frames.
  const frames=Math.max(0,Math.trunc(playFrames)),seconds=Math.trunc(frames/60);
  const hours=Math.trunc(seconds/3600),minutes=Math.trunc(seconds/60)%60;
  return [ascii('    '+width(playCount,5),328,378),
    ascii(width(hours,3)+':'+two(minutes)+':'+two(seconds%60),328,396),
    ascii('    '+width(clearCount,5),328,414)];
}

// Read-only table4af208, consulted447b64/447ba0. There are113 actual cards,
// not128 menu slots. Result's filter uses native card difficulty ordering.
export const NATIVE_SPELL_DIFFICULTIES=freeze([2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,1,2,3,0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4]);
export function nativeSpellPageIds(difficulty,page=1){
  if(page<=0)return [];
  const ids=NATIVE_SPELL_DIFFICULTIES.flatMap((d,i)=>d===difficulty?[i]:[]);
  return ids.slice(Math.max(0,page-1)*10,Math.max(0,page-1)*10+10);
}
export function nativeSpellPageCount(difficulty){
  return 1+Math.ceil(NATIVE_SPELL_DIFFICULTIES.filter(d=>d===difficulty).length/10);
}

//447b20 copies the original Shift-JIS title and pads it to42 bytes before
// calling4608f0 with "No.%3d %s %4d/%4d" (4a1fc4). Captures are record+80,
// attempts record+84; 40e6d7 and40e13a respectively increment those fields.
// All-player attempts control whether the title is revealed. Selected-loadout
// captures control its yellow tint (447ce6..447cfc), not selected attempts.
export function nativeSpellRowText(card,{name='',nameByteLength,attempts=0,captures=0,anyAttempts=0}={}){
  const known=anyAttempts>0;
  //Original titles contain ASCII/halfwidth kana (one byte) and Japanese
  //characters (two bytes). An exact extracted byte length can be supplied.
  const bytes=Number.isInteger(nameByteLength)?nameByteLength:Array.from(String(name)).reduce((n,c)=>{
    const cp=c.codePointAt(0);return n+(cp<0x80||(cp>=0xff61&&cp<=0xff9f)?1:2);
  },0);
  const title=known?String(name)+' '.repeat(Math.max(0,42-bytes)):NATIVE_DATA_LAYOUT.spellRows.unknownTitle;
  return `No.${width(card+1,3)} ${title} ${width(captures,4)}/${width(attempts,4)}`;
}
export function nativeSpellRowDescriptor(card,record={},row=0){
  return {bank:'text',script:25+row,text:nativeSpellRowText(card,record),
    colorRGB:record.anyAttempts>0?(record.captures>0?0xffff80:0xefefef):0x808080,
    x:320,y:160+18*row,width:512,height:17,fontHeight:17,
    fontFamily:'ＭＳ ゴシック',fontWeight:400};
}
export function nativeSpellDescriptors(difficulty,page=1,records={}){
  return nativeSpellPageIds(difficulty,page).map((card,row)=>nativeSpellRowDescriptor(card,records[card]??{},row));
}
