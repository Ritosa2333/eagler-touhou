// front.anm root 0: 10 = motivation, 8 = Z ice power, 11 = Perfect Freeze,
// 12..14 = its three snowflakes. Child 9 is the lower, shared frozen area.
const frontSprite = new Map([[10,8], [8,7], [11,9], [12,10], [13,10], [14,10]]);
const center = 576;
const layouts = {
  // Row pairs are [caption top, value top] relative to each seat header.
  2: {tops:[42,188], title:1.25, label:1, value:1, perfectLabel:0.9375,
    normal:[[20,39],[59,78],[98,117]], challenge:[[29,49],[89,110]]},
  3: {tops:[44,142,240], title:1, label:0.875, value:0.875, perfectLabel:0.875,
    normal:[[20,20],[39,39],[56,74]], challenge:[[19,36],[56,74]]},
};
const seatTints = [0x92ddff,0xffb7d8,0xb6f5a2];
const slot = {title:0,taps:1,lifeLabel:2,life:3,iceLabel:4,ice:5,perfectLabel:6,perfect:7,stock:8};

// The renderer samples (bank,entry,UV), not sprite ID alone. These are the
// original cells in entry 0, including glyphs absent from this frame's P1 HUD.
function cell(sprite,x,y,width,height,size) {
  return {entry:0,sprite,width,height,u0:x/size,v0:y/size,u1:(x+width)/size,v1:(y+height)/size};
}
const snowflake = cell(10,416,0,16,16,512);
// OriginalMenuFont.hpp: sprite = ASCII - 32, 14px cells, 9px advance and a 2px shadow.
const letters = Array.from({length:96},(_,i)=>cell(i,(i%18)*14,Math.floor(i/18)*14,14,14,256));
const font0 = {width:14,advance:9,shadow:2,at:c=>letters[c.charCodeAt(0)-32]};
// PercentHud.hpp: style 4 digits 243..252 and percent 255, 14px advance.
const numbers = Array.from({length:13},(_,i)=>cell(243+i,16*i,216,16,16,256));
const font4 = {width:16,advance:14,shadow:0,at:c=>numbers[c==='%'?12:c.charCodeAt(0)-48]};
const percent = raw => `${Math.floor(raw/100)}%`;
const runWidth = (font,text) => font.width+font.advance*(text.length-1)+font.shadow;

function paintText(out,source,id,text,font,x,y,scale,{align=0.5,maxWidth=120,tint}={}) {
  const width=runWidth(font,text), size=Math.min(scale,maxWidth/width);
  const color=tint===undefined?source.color:((source.color&0xff000000)|tint)>>>0;
  const left=x-align*width*size;
  for(let i=0;i<text.length;i++) {
    const glyph={...source,...font.at(text[i]),id:id+2*i,x:left+i*font.advance*size,y,sx:size,sy:size,color};
    if(font.shadow) {
      const offset=font.shadow*size;
      out.push({...glyph,id:glyph.id+1,x:glyph.x+offset,y:y+offset,color:((color>>>25)<<24)>>>0});
    }
    out.push(glyph);
  }
}

function paintSeat(out,front,glyph,seat,layout,challenge) {
  const top=layout.tops[seat.seat], [life,ice,perfect]=challenge?layout.challenge:layout.normal;
  // Stable IDs per seat, field and character; no dependence on draw order, other
  // seats' digit counts, a previous frame, or which seat is watching.
  const id=field=>0x3f000000+seat.seat*0x1000+field*32;
  function label(field,script,x,dy,scale) {
    const r=front[script];
    out.push({...r,id:id(field),x,y:top+dy,sx:r.sx*scale,sy:r.sy*scale});
  }
  function text(field,string,font,x,dy,scale,options) {
    paintText(out,glyph,id(field),string,font,x,top+dy,scale,options);
  }
  text(slot.title,`P${seat.seat+1}`,font0,center,0,layout.title,{tint:seatTints[seat.seat]});
  if(!challenge && seat.transferTaps>=3 && seat.transferTaps<5)
    text(slot.taps,`L${seat.transferTaps}/5`,font0,636,0,0.875,{align:1,tint:0xaeffbb});

  // In compact normal play, put the first two values beside their captions so
  // the long Perfect Freeze caption and its stock can still use the whole panel.
  const inline=life[0]===life[1], lx=inline?551:center, vx=inline?636:center;
  const labelSize=inline?0.71875:layout.label, valueOptions=inline?{align:1,maxWidth:50}:{};
  if(challenge) {
    text(slot.lifeLabel,'DEATHS',font0,center,life[0],layout.label,{tint:0xffe2a0});
    text(slot.life,String(seat.deaths),font4,center,life[1],layout.value);
  } else {
    label(slot.lifeLabel,10,lx,life[0],labelSize);
    text(slot.life,percent(Math.max(0,seat.motivation)),font4,vx,life[1],layout.value,valueOptions);
  }
  label(slot.iceLabel,8,lx,ice[0],labelSize);
  text(slot.ice,percent(seat.icePower),font4,vx,ice[1],layout.value,valueOptions);
  if(challenge)return;

  label(slot.perfectLabel,11,center,perfect[0],layout.perfectLabel);
  const amount=percent(seat.charge), stocks=Math.floor(seat.charge/10000);
  const width=runWidth(font4,amount), gap=stocks?4:0, stockAdvance=snowflake.width;
  const rowWidth=width+gap+stocks*stockAdvance, scale=Math.min(layout.value,122/rowWidth);
  const left=center-rowWidth*scale/2;
  text(slot.perfect,amount,font4,left,perfect[1],scale,{align:0});
  for(let i=0;i<stocks;i++) {
    const script=12+(i%3), source=front[script]??front[11];
    // The solo snowflakes are bottom-of-playfield layer 17. On the sidebar
    // they belong on the caption layer, above the opaque layer-21 background.
    out.push({...source,...snowflake,id:id(slot.stock)+i,script,layer:front[11].layer,anchorX:1,anchorY:1,
      x:left+(width+gap+i*stockAdvance)*scale,y:top+perfect[1],sx:scale,sy:scale});
  }
}

// Leave the authored Y, size, spacing and glyphs alone. Where nearby labels
// collide, fit their original X positions into a horizontally separated row.
// Only this render pass shifts; world poses, ghost state and Z charge are unchanged.
// Native head glyphs and focus animations are private to the viewed seat.
// CoopSeats.inc emits original focus records in contiguous ranges per seat.
function cueSeat(r) {
  if(r.bank===11 && r.id>=12000000 && r.id<12000030)return Math.floor((r.id-12000000)/10);
  if(r.bank===0 && r.id>=0x3c000000 && r.id<0x3f000000)return Math.floor((r.id-0x3c000000)/0x01000000);
  return null;
}

function isSoloPercent(r) {
  return r.bank===11 && r.entry===0 && r.drawPass==='original-percent-hud' && r.id>=41000000 && r.id<41200000 &&
    r.x>=512 && r.x<640 && (r.y===70 || r.y===118 || r.y===166) && ((r.sprite>=243&&r.sprite<=252) || r.sprite===255);
}
function validSeats(seats,challenge) {
  if(!Array.isArray(seats) || !layouts[seats.length])return false;
  let seen=0;
  for(const s of seats) {
    if(!s || !Number.isInteger(s.seat) || s.seat<0 || s.seat>=seats.length || (seen & (1<<s.seat)) ||
      !Number.isInteger(s.icePower) || s.icePower<0 || s.icePower>10000)return false;
    if(challenge ? !Number.isSafeInteger(s.deaths) || s.deaths<0 :
      !Number.isSafeInteger(s.motivation) || !Number.isInteger(s.charge) || s.charge<0 || s.charge>50000)return false;
    seen|=1<<s.seat;
  }
  return true;
}

/**
 * Project pair.draws(seat) with pair.state(seat) from the SAME native frame,
 * before orderDrawLayers(...,{world:true}). Live and replay supply their
 * viewed seat; a spectator has none. Private cues keep their original poses; the team sidebar
 * does not depend on perspective. Filter cues even without sidebar templates.
 */
export function cooperativeArt(records,state,seat) {
  const visible=records.filter(r=>{const owner=cueSeat(r);return owner===null||owner===seat;});
  const native=visible.length===records.length?records:visible;
  if(typeof state?.challengeMode!=='boolean' || !validSeats(state.seats,state.challengeMode))return native;
  const front=[], out=[];let glyph;
  for(const r of native) {
    if(r.bank===5 && r.entry===0 && r.id>=7000000 && r.id<8000000 && frontSprite.get(r.script)===r.sprite && frontSprite.has(r.script))
      front[r.script]=r;
    else if(isSoloPercent(r)) glyph??=r;
    else out.push(r);
  }
  if(!front[8] || !front[10] || !front[11] || !glyph)return native;
  for(const seat of state.seats)paintSeat(out,front,glyph,seat,layouts[state.seats.length],state.challengeMode);
  return out;
}
