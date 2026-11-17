// Read-only browser HUD. Keep DOS memory, the disk and replay inputs untouched.
import {powerBarGlyphs} from './solo-boss-glyphs.js';
const bosses=[['RIKA',2261],['MEIRA',2400],['STONES',1450],['MARISA',900],['MIMA',4700],['SIGMA',8500]];
const phases={4:[600,700,800,1500,1100],5:[1800,1800,1800,1800,1300]};
const stones=[140,140,120,120,930];
// DOSBox-X jfont.h pc98_freecg_sbcs, matching the multiplayer TRAM font.
// See vendor/dosbox-x/LICENSE (GPL-2.0). Draw integer pixels without filtering.
const captionGlyphs={
  A:[0,0,0,0,24,36,36,66,66,126,66,66,66,66,0,0],
  E:[0,0,0,0,126,64,64,64,124,64,64,64,64,126,0,0],
  G:[0,0,0,0,60,66,66,64,64,78,66,66,70,58,0,0],
  I:[0,0,0,0,62,8,8,8,8,8,8,8,8,62,0,0],
  K:[0,0,0,0,66,68,72,80,96,96,80,72,68,66,0,0],
  M:[0,0,0,0,66,66,102,102,90,90,66,66,66,66,0,0],
  N:[0,0,0,0,66,98,98,82,82,74,74,70,70,66,0,0],
  O:[0,0,0,0,60,66,66,66,66,66,66,66,66,60,0,0],
  R:[0,0,0,0,124,66,66,66,124,72,68,68,66,66,0,0],
  S:[0,0,0,0,60,66,66,64,48,12,2,66,66,60,0,0],
  T:[0,0,0,0,127,8,8,8,8,8,8,8,8,8,0,0],
  Y:[0,0,0,0,65,65,34,34,20,8,8,8,8,8,0,0],
};

export function drawBossCaption(context,text,x,y,color='#fff'){
  context.fillStyle=color;
  for(const letter of text){
    const glyph=captionGlyphs[letter];
    if(glyph)for(let row=0;row<16;row++)for(let col=0;col<8;col++){
      if(glyph[row]&(0x80>>col))context.fillRect(x+col,y+row,1,1);
    }
    x+=8;
  }
}

export function drawBossBar(context,bar,x,y){
  // Match patches/boss-hud.asm: eight native gaiji and TH04 health colors.
  // Empty cells stay transparent, as in the multiplayer TRAM gauge.
  context.fillStyle=['#f00','#f0f','#0ff','#ff0','#fff'][bar>>5];
  for(let cell=0;cell<8;cell++){
    const width=Math.min(16,bar-cell*16);if(width<=0)break;
    const glyph=powerBarGlyphs[width-1];
    for(let row=0;row<16;row++)for(let col=0;col<16;col++){
      if(glyph[row]&(0x8000>>col))context.fillRect(x+cell*16+col,y+row,1,1);
    }
  }
}

export function readSoloBoss(heap,data,mode){
  if(!heap||!Number.isInteger(data)||data<0||data+0x91fa>heap.length||![1,2].includes(mode))return null;
  const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),stage=heap[data+0x8f5];
  const update=view.getUint32(data+0x1a0c,true);
  if(stage>=bosses.length||heap[data+0x19f6]!==1||!update||update!==view.getUint32(data+0x1a10,true)||
    stage!==2&&heap[data+0x2bfa]!==0)return null;
  const [name,max]=bosses[stage],damage=Math.max(0,view.getInt16(data+0x2bea,true));
  let current;
  if(stage===2){
    current=stones.reduce((total,hp,i)=>total+(heap[data+0x2bf0+i]>=2?0:Math.max(0,hp-view.getInt16(data+0x2bd4+i*2,true))),0);
  }else if(phases[stage]){
    const phase=Math.min(10,stage===4?view.getUint16(data+0x91f8,true):heap[data+0x7b42]);
    const index=Math.floor(phase/2),limits=phases[stage];
    current=limits.slice(index).reduce((sum,hp)=>sum+hp,0)-(phase%2?Math.min(damage,limits[index]):0);
  }else current=Math.max(0,max-damage);
  current=Math.max(0,Math.min(max,current));
  return {stage,name,current,max,bar:Math.ceil(current*128/max)};
}

export function mountSoloBossHud(screen){
  const canvas=document.createElement('canvas');canvas.width=640;canvas.height=400;
  canvas.className='solo-boss-hud';canvas.hidden=true;canvas.setAttribute('aria-hidden','true');screen.append(canvas);
  const context=canvas.getContext('2d');
  let previous='',key='',bar=0,tick=-1;
  return {
    update(boss,generation,ticks){
      if(!boss){canvas.hidden=true;previous=key='';bar=0;tick=-1;return;}
      const nextKey=`${generation}:${boss.stage}`;
      if(key!==nextKey||ticks<tick){key=nextKey;bar=0;tick=ticks;}
      // Fill on entry using native game ticks, so pause and fast-forward agree.
      bar=boss.bar<=bar?boss.bar:Math.min(boss.bar,bar+Math.max(0,ticks-tick));tick=ticks;
      const next=`${key}:${bar}`;
      canvas.hidden=false;if(next===previous)return;previous=next;
      context.clearRect(0,0,640,400);
      // The solo Power display occupies the multiplayer gauge's row 20.
      // Use the free space below Score while keeping its left edge/layout.
      drawBossCaption(context,`ENEMY ${boss.name}`,448,144,'#ff0');
      drawBossBar(context,bar,448,160);
    },
    hide(){this.update(null);},
  };
}

// Permit only the reviewed before/after fingerprints for this visual update.
// Any later simulation/runtime change produces a different current identity.
export async function compatibleSoloReplay(recorded,current){
  if(recorded===current)return true;
  try{
    const response=await fetch(new URL('./solo-boss-hud-replay.json',import.meta.url),{cache:'no-cache'});
    if(!response.ok)return false;
    const pair=await response.json();return pair.previous===recorded&&pair.current===current;
  }catch{return false;}
}
