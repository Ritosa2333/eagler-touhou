import {test} from 'node:test';
import assert from 'node:assert/strict';
import {cooperativeArt} from '../runtime/coop-hud.mjs';
import {orderDrawLayers} from '../../th128/site/runtime/draw.mjs';
import {bounds,runs,seatGroups,style0,style4} from './coop-hud-readback.mjs';

// Small native-frame fixture. In particular it contains neither font-0 letters nor
// a full alphabet of style-4 digits. P1 normally has no visible stock template.
const pose = {parent:0,entry:0,mode:0,blend:0,z:0,rotation:0,rx:0,ry:0,sx:1,sy:1,u:0,v:0,uvScaleX:1,uvScaleY:1,
  clock:60,visualTicks:60,flags:2151677959,flags2:6,color:0xffffffff,secondary:0,effectSystem:'other'};
function front(script, sprite, x, y, tx, ty, w, h, extra) {
  return Object.freeze({...pose,id:7000000+({8:9,9:8}[script]??script),script,bank:5,sprite,x,y,width:w,height:h,
    u0:tx/512,v0:ty/512,u1:(tx+w)/512,v1:(ty+h)/512,layer:22,anchorX:0,anchorY:1,...extra});
}
function pct(id, sprite, x, y, extra) {
  return Object.freeze({...pose,id,script:-1,bank:11,sprite,x,y,width:16,height:16,u0:(sprite-243)/16,u1:(sprite-242)/16,
    v0:216/256,v1:232/256,layer:0,anchorX:1,anchorY:1,drawPass:'original-percent-hud',flags:6,flags2:0,...extra});
}
const life=front(10,8,576,48,320,36,96,18), ice=front(8,7,576,96,320,18,96,18), skill=front(11,9,576,144,256,54,128,18);
const oldStock=front(12,10,128,464,416,0,16,16,{layer:17,anchorX:1,anchorY:2});
const back=front(3,1,512,0,128,0,128,480,{layer:21,anchorX:1});
const area=front(9,6,576,336,320,0,96,18), areaValue=pct(41000010,255,576,358);
const score=pct(41200000,244,128,16), difficulty=front(67,53,576,24,268,320,60,16,{layer:26,anchorY:0});
const oldValues=[pct(41000000,245,548,70),pct(41000004,244,548,118),pct(41000008,243,562,166)];
const frame=Object.freeze([back,score,ice,area,...oldValues,life,areaValue,skill,difficulty]);
const seat=(seat,motivation,icePower,charge,deaths=0) => Object.freeze({seat,motivation,icePower,charge,deaths,transferTaps:0});
const two=[seat(0,20099,7999,0),seat(1,0,12,29999)];
const three=[seat(0,100000,10000,9999,9),seat(1,-1,3456,10000,0),seat(2,17999,7,30000,21)];
const state=(seats,challengeMode=false) => Object.freeze({seats:Object.freeze(seats),challengeMode,firepower:2765.5,rank:1});
const labels=g=>g.filter(r=>r.bank===5 && [8,10,11].includes(r.script)).map(r=>r.sprite).sort((a,b)=>a-b);
const stocks=g=>g.filter(r=>r.bank===5 && r.sprite===10);

test('two native seat bands: original percent truncation, live zero motivation, and P2 stocks with no P1 stock record',()=>{
  const groups=seatGroups(frame,cooperativeArt(frame,state(two)),2);
  assert.deepEqual(groups.map(g=>runs(g,style0)),[['P1'],['P2']]);
  assert.deepEqual(groups.map(g=>runs(g,style4)),[['200%','79%','0%'],['0%','0%','299%']]);
  assert.deepEqual(groups.map(labels),[[7,8,9],[7,8,9]]);
  assert.deepEqual(groups.map(g=>stocks(g).map(r=>r.script)),[[],[12,13]]);
});

test('three seats: negative legacy motivation clamps to 0%; every Z value and X threshold belongs to its seat',()=>{
  const groups=seatGroups(frame,cooperativeArt(frame,state(three)),3);
  assert.deepEqual(groups.map(g=>runs(g,style0)),[['P1'],['P2'],['P3']]);
  assert.deepEqual(groups.map(g=>runs(g,style4)),[['1000%','100%','99%'],['0%','34%','100%'],['179%','0%','300%']]);
  assert.deepEqual(groups.map(g=>stocks(g).map(r=>r.script)),[[],[12],[12,13,14]]);
  assert.deepEqual(groups.map(labels),Array(3).fill([7,8,9]));
});

test('challenge displays actual death counts without a percent sign; keeps Z but no motivation, ghost, X label, percent or stock',()=>{
  for(const n of [2,3]) {
    const native=[...frame,oldStock];
    const groups=seatGroups(native,cooperativeArt(native,state(three.slice(0,n),true)),n);
    assert.deepEqual(groups.map(g=>runs(g,style0)),Array.from({length:n},(_,i)=>[`P${i+1}`,'DEATHS']));
    assert.deepEqual(groups.map(g=>runs(g,style4)),[['9','100%'],['0','34%'],['21','0%']].slice(0,n));
    assert.ok(groups.every(g=>stocks(g).length===0));
    assert.deepEqual(groups.map(labels),Array(n).fill([7]));
  }
});

test('preserve the native score, rank, single lower area, background, notices and lookalike gameplay records by identity and in order',()=>{
  const other=[back,score,difficulty,area,areaValue,{...life,bank:7},{...ice,id:4000000},
    {...life,script:49},{...skill,sprite:10},{...oldValues[0],drawPass:'items'},{...score,y:70},{...oldValues[0],x:320}];
  const native=Object.freeze([...other.slice(0,3),life,ice,...other.slice(3,8),oldStock,skill,...oldValues,...other.slice(8)]);
  const art=cooperativeArt(native,state(three)),kept=art.filter(r=>native.includes(r));
  assert.equal(kept.length,other.length); other.forEach((r,i)=>assert.strictEqual(kept[i],r));
  assert.equal(new Set(art.map(r=>r.id).filter(id=>!native.some(r=>r.id===id))).size,art.length-kept.length,'each emitted sprite has its own non-native ID');
  assert.deepEqual(cooperativeArt(native,state(three)),art,'a fresh replay projection must be identical');
});

test('native UV rectangles, paint/fade and callback priority survive projection, even when a digit or stock is absent from P1',()=>{
  const faded={...life,color:0x4fffffff,clock:26,visualTicks:26};
  const native=frame.map(r=>r===life?faded:r), art=cooperativeArt(native,state(three));
  const ordered=orderDrawLayers(art,{world:true});
  for(const g of seatGroups(native,art,3)) for(const r of g) {
    assert.ok(ordered.indexOf(r)>ordered.indexOf(back),'not hidden behind front.anm sidebar');
    let tex;
    if(r.bank===5 && r.sprite!==10) {
      const source=[faded,ice,skill].find(s=>s.sprite===r.sprite);
      for(const key of ['entry','u0','v0','u1','v1','width','height','script','clock','color','flags','flags2','blend','layer']) assert.equal(r[key],source[key],key);
      continue;
    } else if(r.bank===5) tex=[416,0,16,16,512];
    else {
      assert.equal(r.bank,11); assert.equal(r.entry,0); assert.equal(r.drawPass,'original-percent-hud');
      tex=r.sprite<98?[(r.sprite%18)*14,Math.floor(r.sprite/18)*14,14,14,256]:[(r.sprite-243)*16,216,16,16,256];
    }
    const [x,y,w,h,size]=tex;assert.deepEqual([r.u0,r.v0,r.u1,r.v1,r.width,r.height],[x/size,y/size,(x+w)/size,(y+h)/size,w,h]);
  }
});

function overlaps(a,b) {return a.left<b.right && b.left<a.right && a.top<b.bottom && b.top<a.bottom;}
test('rendered native quad bounds protect the playfield and area, separate entire bands, and fit even five stocks or long death counts',()=>{
  for(const n of [2,3]) for(const challenge of [false,true]) {
    const seats=Array.from({length:n},(_,i)=>seat(i,100000,10000,50000,4294967295));
    const groups=seatGroups(frame,cooperativeArt(frame,state(seats,challenge)),n);
    let previous=32;
    for(const g of groups) {
      const boxes=g.map(bounds);assert.ok(boxes.every(b=>b.left>=512 && b.right<=640 && b.bottom<=332 && b.bottom>b.top));
      assert.ok(Math.min(...boxes.map(b=>b.top))-previous>=8,'visible breathing room above each group');
      previous=Math.max(...boxes.map(b=>b.bottom));
      g.forEach((a,i)=>g.slice(i+1).forEach((b,j)=>{
        // Within one line the original ASCII cells intentionally kern/overprint their shadows.
        if(a.bank!==11 || b.bank!==11 || (a.color&0xffffff) && (b.color&0xffffff) && a.y!==b.y)
          assert.ok(!overlaps(boxes[i],boxes[i+j+1]),`overlap: ${a.bank}:${a.sprite} / ${b.bank}:${b.sprite}`);
      }));
    }
    assert.ok(previous>=300,'use the vertical space instead of bunching seats at the top');
  }
  const scales=[two,three].map(s=>seatGroups(frame,cooperativeArt(frame,state(s)),s.length)[0].find(r=>r.script===10).sy);
  assert.ok(scales[0]>scales[1],'2P labels should be larger than 3P labels');
});

test('donor alone keeps the native L tap-chain readout; it vanishes on reset and in challenge',()=>{
  const s=two.map((p,i)=>({...p,transferTaps:i?3:0}));
  const read=(s,c=false)=>seatGroups(frame,cooperativeArt(frame,state(s,c)),2).map(g=>runs(g,style0).join(''));
  assert.deepEqual(read(s),['P1','P2L3/5']); assert.deepEqual(read(two),['P1','P2']);
  assert.deepEqual(read(s,true),['P1DEATHS','P2DEATHS']);
});

test('no matched native HUD or no 2/3-seat world: pass through rather than stripping or synthesizing a HUD',()=>{
  for(const s of [undefined,null,state([two[0]])]) assert.strictEqual(cooperativeArt(frame,s),frame);
  const nonHud=[back,area,areaValue];assert.strictEqual(cooperativeArt(nonHud,state(two)),nonHud);
});

test('fatal miss, ghost and revived at zero show the same percentage',()=>{
  const read=(motivation,ghost)=>seatGroups(frame,cooperativeArt(frame,state([two[0],{...two[1],motivation,ghost}])),2)[1];
  assert.deepEqual(runs(read(-10000,false),style4),['0%','0%','299%']);
  assert.deepEqual(runs(read(-10000,true),style0),['P2']);
  assert.deepEqual(runs(read(0,false),style4),['0%','0%','299%']);
});

test('project by native seat number, not viewer or array order; an incomplete frame is not a source of invented values',()=>{
  const groups=seatGroups(frame,cooperativeArt(frame,state([two[1],two[0]])),2);
  assert.deepEqual(groups.map(g=>runs(g,style4)),[['200%','79%','0%'],['0%','0%','299%']]);
  assert.strictEqual(cooperativeArt(frame,state([two[0],{...two[1],charge:undefined}])),frame);
  const missingLife=frame.filter(r=>r!==life);assert.strictEqual(cooperativeArt(missingLife,state(two)),missingLife);
});

const head=(seat,x,y)=>Array.from({length:4},(_,i)=>({...pose,id:12000000+seat*10+i,bank:11,sprite:i===3?269:i===0?259:258-seat,x:x+13.5*i,y,anchorX:1,anchorY:1,sx:1.5,sy:1.5,width:12,height:12}));
const onlyHeads=rows=>rows.filter(r=>r.id>=12000000&&r.id<12000030);
const focus=(seat,x,y)=>[80,79].map((script,i)=>({...pose,id:0x3c000000+seat*0x01000000+1+i,bank:0,script,sprite:444,layer:16,x,y}));
const onlyFocus=rows=>rows.filter(r=>r.bank===0&&r.id>=0x3c000000&&r.id<0x3f000000);

test('each seat sees only its own unshifted native head and focus cues even at identical coordinates',()=>{
  for(const seats of [two,three])for(const x of [125,274,475]) {
    const heads=seats.map(p=>head(p.seat,x,394)), markers=seats.map(p=>focus(p.seat,x,416));
    const bullet={...pose,id:0x80000001,bank:0,script:79,sprite:444,layer:16,x,y:416};
    const native=Object.freeze([...frame,bullet,...heads.flat(),...markers.flat()]), s=state(seats);
    for(const {seat} of seats) {
      const art=cooperativeArt(native,s,seat);
      assert.deepEqual(onlyHeads(art),heads[seat],`P${seat+1} original head records`);
      assert.deepEqual(onlyFocus(art),markers[seat],`P${seat+1} original focus records`);
      assert.ok([...heads[seat],...markers[seat],bullet].every(r=>art.includes(r)));
      assert.deepEqual(seatGroups(native,art,seats.length),seatGroups(native,cooperativeArt(frame,s,0),seats.length));
    }
  }
});

test('private cues are filtered before sidebar templates are available; a missing own marker stays missing',()=>{
  const own=head(1,265,394), other=head(0,265,394), f=focus(0,290,416);
  const native=[back,...other,...own,...f,areaValue];
  assert.deepEqual(cooperativeArt(native,state(two),1),[back,...own,areaValue]);
  assert.deepEqual(cooperativeArt(native,null,1),[back,...own,areaValue]);
  assert.deepEqual(cooperativeArt(native,state(two),null),[back,areaValue]);
});

test('retired players use the same original percentage glyphs at zero and after shared rewards',()=>{
 for(const amount of [0,12537]){
  const rows=[{...two[0],retired:true,ghost:true,motivation:amount,charge:26300},two[1]];
  const group=seatGroups(frame,cooperativeArt(frame,state(rows)),2)[0];
  assert.deepEqual(runs(group,style0),['P1']);
  assert.deepEqual(runs(group,style4),[`${Math.floor(amount/100)}%`,'79%','263%']);
 }
});
