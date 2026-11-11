import {hudDigits} from './hud-digits.js';
import {drawCollisionPoints,collisionPoints} from './collision-points.js';

// Only final pixels differ by local seat. Never write a local seat selector
// into NP21 RAM, VRAM, devices, snapshots or the state checksum.
export function createPersonalHud(canvas,nativeCanvas,{overlay=false}={}){
  const makeCanvas=()=>typeof document==='undefined'||typeof document.createElement!=='function'?new OffscreenCanvas(1,1):document.createElement('canvas');
  const layer=overlay?makeCanvas():canvas;
  if(overlay){layer.width=256;layer.height=400;}
  const context=layer.getContext('2d',{alpha:overlay});
  let rows=[],points=[],signature='',revision=0;
  let tramAddress=-1,tramBuffer=null,lastTramSearch=-Infinity;
  const atlas=makeCanvas();atlas.width=160;atlas.height=16;
  const ink=atlas.getContext('2d'),pixels=ink.createImageData(160,16);
  for(let digit=0;digit<10;digit++)for(let y=0;y<16;y++)for(let x=0;x<16;x++){
    if(!(hudDigits[digit][y*2+(x>>>3)]&(0x80>>>(x&7))))continue;
    const at=(y*160+digit*16+x)*4;pixels.data.fill(255,at,at+4);
  }
  ink.putImageData(pixels,0,0);
  const numbers=new Map();
  function number(value,x,y,count,zeros=false){
    const text=String(value).padStart(count,zeros?'0':' ').slice(-count);
    if(overlay){rows.push([text,x>=448?x-448:x-272+128,y]);return;}
    drawNumber(text,x,y);
  }
  function drawNumber(text,x,y){
    const count=text.length;
    const key=x+','+y+','+count;
    let row=numbers.get(key);
    if(!row){
      const image=makeCanvas();image.width=count*16;image.height=16;
      row={image,ink:image.getContext('2d',{alpha:false}),text:null};numbers.set(key,row);
    }
    if(row.text!==text){
      row.ink.fillStyle='#000';row.ink.fillRect(0,0,count*16,16);
      for(let i=0;i<count;i++)if(text[i]!==' ')
        row.ink.drawImage(atlas,Number(text[i])*16,0,16,16,i*16,0,16,16);
      row.text=text;
    }
    context.drawImage(row.image,x,y);
  }
  function paint({heap,mailbox,patch,slot,guestBase,raw=false,points:preferences={}}){
    if(!overlay&&(canvas.width!==640||canvas.height!==400)){
      canvas.width=640;canvas.height=400;
    }
    context.imageSmoothingEnabled=false;
    if(!overlay)context.drawImage(nativeCanvas,0,0,640,400);
    if(raw||mailbox<0||!patch?.personal_stats||slot==null)return;
    const view=new DataView(heap.buffer);
    if(tramBuffer!==heap.buffer){tramBuffer=heap.buffer;tramAddress=-1;lastTramSearch=-Infinity;}
    // DOSBox-X backs PC-98 text VRAM separately from ordinary guest RAM.
    // Locate its two eight-digit HUD rows once, then use the same native guard
    // for menus/endings. Presentation address discovery never enters the state.
    if(tramAddress<0&&performance.now()-lastTramSearch>1000){lastTramSearch=performance.now();tramAddress=findDosboxTram(heap);}
    const tram=tramAddress>=0?tramAddress:guestBase+0xa0000;
    // Native score digits are gaiji 0xA0..0xA9 -> TRAM 0x2057..0x2957.
    // Don't paint over the boot screen, cleared menus or ending artwork.
    const cell=view.getUint16(tram+6*160+56*2,true);
    if((cell&255)!==0x57||(cell>>>8)<0x20||(cell>>>8)>0x29)return;
    if(overlay)points=collisionPoints({heap,mailbox,patch,...preferences});
    else drawCollisionPoints(context,{heap,mailbox,patch,...preferences});
    const schema=patch.personal_stats;
    const at=mailbox-patch.mailbox_cs_offset+schema.cs_offset;
    number(view.getUint32(at+schema.high_score,true),448,64,8,true);
    number(view.getUint32(at+schema.score,true),448,96,8,true);
    number(heap[at+schema.point],496,240,5);
    number(view.getUint16(at+schema.dream,true)*10,496,272,5);
    number(view.getUint16(at+schema.graze,true),496,304,5);
    const bonus=mailbox-patch.mailbox_cs_offset+schema.bonus_cs_offset;
    const mode=heap[bonus];
    const stageTitle=view.getUint16(tram+4*160+20*2,true)===0x4d56;
    const clearTitle=view.getUint16(tram+4*160+19*2,true)===0x5c56;
    if(mode===1&&stageTitle||mode===2&&clearTitle){
      const data=mailbox-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16;
      const resource=mailbox+patch.resource_schema.offset;
      const all=mode===2;
      number(all?10000:(heap[data+0x5394]+1)*1000,272,all?96:112,8);
      number(heap[resource]*50,272,all?128:144,8);
      number(view.getUint16(at+schema.dream,true)*10,272,all?160:176,8);
      number(view.getUint16(at+schema.graze,true)*50,272,all?192:208,8);
      if(all)number(Math.max(0,heap[resource+4]-1)*(heap[data+0x4348]===4?30000:10000),272,224,8);
      number(heap[at+schema.point],320,all?272:256,5);
      number(view.getUint32(bonus+1,true),272,336,8);
    }
  }
  return state=>{
    if(!overlay)return paint(state);
    rows=[];points=[];paint(state);
    const next=JSON.stringify(rows);
    if(next!==signature){
      context.clearRect(0,0,256,400);
      for(const [text,x,y]of rows)drawNumber(text,x,y);
      signature=next;revision++;
    }
    return {canvas:layer,revision,points};
  };
}

export function findDosboxTram(heap){
  const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),score=6*160+56*2,high=4*160+56*2;
  function row(at){if(at<2||at+34>heap.length||view.getUint16(at-2,true)!==0x20||view.getUint16(at+32,true)!==0x20)return false;
    for(let i=0;i<8;i++){const word=view.getUint16(at+i*4,true);if((word&255)!==0x57||(word>>>8)<0x20||(word>>>8)>0x29||view.getUint16(at+i*4+2,true)!==(word|0x8000))return false;}return true;}
  for(let at=heap.indexOf(0x57,score);at>=0;at=heap.indexOf(0x57,at+1))if(!(at&1)&&row(at)&&row(at-score+high))return at-score;
  return -1;
}
