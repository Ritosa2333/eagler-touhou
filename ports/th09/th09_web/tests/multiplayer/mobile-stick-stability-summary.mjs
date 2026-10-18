// One frozen controlled D0 stick tape. No prediction, correction, timing or FPS estimate.
import {readFileSync, writeFileSync, existsSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve, basename} from 'node:path';
import {fileURLToPath} from 'node:url';

const PIN = {
  name: 'mobile-rollback-stick-deep-c1-report.json', bytes: 2708848,
  sha256: '4d1bed8cbdd83f235ae9a7246356265c8e9c7c4f9e1f9dead6e990a3c87a178f',
  harness: '33db3f5578cc79373088f334d76b107abc0e47c0cab136fd8e4515d05c9c72ce',
  touchProbe: 'aa3bddbcd643ea7dcea9d36906ac4dddf17c1a735daaa679c1ff0fe419438eb0',
};
const PARAMETERS = {mode:0, delay:0, limit:900, touchProfile:'joystick', sourceHz:60,
  deliveryClock:'frontier', fixedDelay:8, cpuRate:4};
const sha = b => createHash('sha256').update(b).digest('hex');
const same = (a,b) => JSON.stringify(a) === JSON.stringify(b);
const need = (ok, why) => {if (!ok) throw Error(why);};
const finite = Number.isFinite, integer = Number.isInteger;
const axisWrite = e => ['stick','controls','cancel'].includes(e.kind);
const identity = path => {const bytes=readFileSync(path); return {path, bytes:bytes.length, sha256:sha(bytes)};};
const bits = n => {const b=Buffer.alloc(4); b.writeFloatLE(n); return b.readUInt32LE();};
const hex = n => n.toString(16).padStart(8,'0');
const normalizedAxis = n => {
  const argumentF32=Math.fround(n*32767), value=Math.fround(Math.max(-1,Math.min(1,argumentF32/32767)));
  return {sourceNumber:n, argumentF32, argumentBits:hex(bits(argumentF32)), value, valueBits:hex(bits(value))};
};
function decode(row,index) {
  need(row && row.frame===index && typeof row.rawHex==='string' && /^[a-f0-9]{24}$/.test(row.rawHex), 'Tape frame/12-byte format '+index);
  const b=Buffer.from(row.rawHex,'hex'), out={frame:index, buttons:b.readUInt16LE(), analogMode:b[2], flags:b[3],
    x:b.readFloatLE(4), y:b.readFloatLE(8), xBits:b.readUInt32LE(4), yBits:b.readUInt32LE(8), rawHex:row.rawHex};
  need(row.buttons===out.buttons && row.analogMode===out.analogMode && row.flags===out.flags &&
    row.x===out.x && row.y===out.y && row.xBits===out.xBits && row.yBits===out.yBits &&
    row.word0===b.readUInt32LE(), 'Tape decoded fields disagree '+index);
  need(finite(out.x)&&finite(out.y) && !(out.flags&~3) && (out.analogMode===0?
    out.x===0&&out.y===0&&out.flags===0 : out.analogMode===1?
    Math.abs(out.x)<=16&&Math.abs(out.y)<=16&&!(out.flags&1) :
    out.analogMode===2&&Math.abs(out.x)<=4096&&Math.abs(out.y)<=4096), 'TH09 invalid actual input '+index);
  return {...out, xBitsHex:hex(out.xBits), yBitsHex:hex(out.yBits), firstWireIteration:row.iteration};
}
function difference(a,b) {
  const fields={buttons:a.buttons!==b.buttons, analogMode:a.analogMode!==b.analogMode, flags:a.flags!==b.flags,
    xNumeric:a.x!==b.x, yNumeric:a.y!==b.y, xBits:a.xBits!==b.xBits, yBits:a.yBits!==b.yBits};
  const xy=fields.xNumeric||fields.yNumeric, other=fields.buttons||fields.analogMode||fields.flags;
  return {fromFrame:a.frame, toFrame:b.frame, fields, fullNumeric:xy||other, fullWire:a.rawHex!==b.rawHex,
    coordinatesNumeric:xy, coordinatesBits:fields.xBits||fields.yBits, nonXY:other,
    onlyXYNumeric:xy&&!other, onlyNonXY:other&&!xy, bothNumeric:xy&&other};
}
function counts(rows,changes) {
  const sum=key=>changes.filter(c=>c[key]).length;
  return {frames:rows.length, adjacentPairs:changes.length, joystickFrames:rows.filter(r=>r.analogMode===1).length,
    otherModeFrames:rows.filter(r=>r.analogMode!==1).length, fullNumeric:sum('fullNumeric'), fullWire:sum('fullWire'),
    coordinatesNumeric:sum('coordinatesNumeric'), coordinatesBits:sum('coordinatesBits'), nonXY:sum('nonXY'),
    buttons:changes.filter(c=>c.fields.buttons).length, analogMode:changes.filter(c=>c.fields.analogMode).length,
    flags:changes.filter(c=>c.fields.flags).length, onlyXYNumeric:sum('onlyXYNumeric'), onlyNonXY:sum('onlyNonXY'),
    bothNumeric:sum('bothNumeric')};
}
function inspectPeer(raw,side) {
  const peer={side, qualified:false, errors:[], coverage:null, intervals:[]}, limit=PARAMETERS.limit;
  try {
    const rows=raw.result.localTape[side], events=raw.result.events[side];
    need(Array.isArray(rows)&&rows.length===limit, 'Exactly 900 actual first captures required');
    need(Array.isArray(events)&&events.length>0&&events.length<20000, 'Missing/bounded source events');
    need(raw.result.summary?.[side]?.[4]===limit, 'Capture counter does not cover the tape');
    const decoded=rows.map(decode), tapeSha256=sha(JSON.stringify(rows.map(r=>[r.frame,r.rawHex])));
    need(tapeSha256===raw.localTapeSha256?.[side], 'Actual tape SHA mismatch');
    events.forEach((e,i)=>{
      need(e && e.sequence===i && integer(e.captureSlot)&&e.captureSlot>=0&&e.captureSlot<limit &&
        finite(e.atMs)&&e.atMs>=0&&['key','controls','stick','cancel','marker'].includes(e.kind), 'Source event schema '+i);
      need(!i || events[i-1].captureSlot<=e.captureSlot && events[i-1].atMs<=e.atMs, 'Source ordering '+i);
      need(e.atMs<=e.captureSlot*1000/60+1e-7 &&
        (e.captureSlot===0||e.atMs>(e.captureSlot-1)*1000/60+1e-7), 'Source first-due captureSlot '+i);
      if(e.kind==='stick') need(finite(e.x)&&finite(e.y)&&Math.abs(e.x)<=1&&Math.abs(e.y)<=1, 'Source stick axes '+i);
      if(e.kind==='controls') need([0,1].includes(e.fire)&&[0,1].includes(e.focus), 'Source controls '+i);
    });
    const markers=events.filter(e=>e.kind==='marker'&&e.label==='hold-stick-vector'), expected=[];
    for(let cycle=0;;cycle++){
      const atMs=2500+137*side+5200*cycle+2100;
      if(atMs>(limit-1)*1000/60+1e-7) break;
      expected.push({atMs,epoch:cycle*3+1});
    }
    need(markers.length===expected.length && markers.every((e,i)=>e.atMs===expected[i].atMs&&e.epoch===expected[i].epoch),
      'All source-declared hold intervals must be present');
    peer.coverage={tapeRows:rows.length, sourceEvents:events.length, tapeSha256,
      declaredHoldMarkers:markers.length, expectedHoldMarkers:expected, join:'D0 captureSlot == local frame; all first captures contiguous, counter=900'};
    for(const marker of markers) {
      const prior=events.slice(0,marker.sequence).reverse().find(axisWrite);
      const next=events.slice(marker.sequence+1).find(axisWrite);
      const controls=events.slice(0,marker.sequence).reverse().find(e=>e.kind==='controls');
      need(prior?.kind==='stick'&&prior.label==='reverse'&&prior.epoch===marker.epoch&&prior.atMs===marker.atMs,
        'Hold must follow its actual final reverse stick');
      need(next?.kind==='stick'&&next.label==='up'&&next.epoch===marker.epoch&&next.atMs===marker.atMs+500 &&
        next.x===0&&next.y===0, 'Hold must end at its next axis-writing UP; controls/cancel are also barriers');
      need(controls?.fire===1&&controls.focus===1, 'Declared held source controls unavailable');
      const from=marker.captureSlot,to=next.captureSlot;
      need(to>from&&to<=limit, 'Empty or censored hold frame range');
      // Every event sharing a capture slot is applied before that slot's first capture.
      need(!events.some(e=>axisWrite(e)&&e.sequence>marker.sequence&&e.captureSlot<to), 'Axis write inside hold');
      const actual=decoded.slice(from,to), changes=actual.slice(1).map((r,i)=>difference(actual[i],r));
      const coordinates=new Map();
      for(const r of actual){
        const key=r.xBitsHex+'/'+r.yBitsHex, priorValue=coordinates.get(key);
        if(priorValue){++priorValue.count;priorValue.lastFrame=r.frame;}
        else coordinates.set(key,{x:r.x,y:r.y,xBitsHex:r.xBitsHex,yBitsHex:r.yBitsHex,count:1,firstFrame:r.frame,lastFrame:r.frame});
      }
      peer.intervals.push({epoch:marker.epoch, sourceMs:[marker.atMs,next.atMs], frames:[from,to], endExclusive:true,
        sourceAxesQualified:true, source:{lastAxisWrite:prior, marker, controls, nextAxisWrite:next,
          axesF32DerivedFromFrozenABI:{x:normalizedAxis(prior.x),y:normalizedAxis(prior.y)},
          interveningEvents:events.slice(marker.sequence+1,next.sequence)},
        beforeBoundary:from?decoded[from-1]:null, afterBoundary:to<limit?decoded[to]:null,
        counts:counts(actual,changes), distinctCoordinates:[...coordinates.values()], actual, changes});
    }
    need(peer.intervals.length>0,'No qualified nonempty holds');
    peer.totals=Object.fromEntries(Object.keys(peer.intervals[0].counts).map(k=>[k,peer.intervals.reduce((n,h)=>n+h.counts[k],0)]));
    peer.qualified=true;
  } catch(error) {peer.errors.push(String(error?.stack||error));}
  return peer;
}

if(process.argv.length!==4) throw Error('Usage: node mobile-stick-stability-summary.mjs NEW_OUTPUT.json FROZEN_RAW.json');
const output=resolve(process.argv[2]), input=resolve(process.argv[3]), self=fileURLToPath(import.meta.url);
need(output!==input&&output!==self&&!existsSync(output),'Output must be a new distinct file');
const evidence={schema:'th09-mobile-stick-stability-v1', generatedAt:new Date().toISOString(), pin:PIN, errors:[], peers:[],
  scope:'One frozen controlled native-ABI tape. Counts describe actual input changes while submitted source axes are fixed, not prediction misses, corrections, resimulation, CPU or FPS.',
  selection:'Every captured hold-stick-vector marker in both peers; interval starts at marker captureSlot and ends before the next axis-writing event. No selection by actual mode, coordinate change or apparent benefit.',
  joinContract:'Frozen producer applies all due source events before the next Captures slot; each first capture schedules frame+delay. D0 plus contiguous 900-frame tape and capture counter makes captureSlot==frame. firstWireIteration is retained only as wire observation, not capture timing.',
  sourceSemantics:{controls:'Native th09_touch_controls calls gestures.controls(...,0,0), clearing axes.',
    cancel:'This frozen joystick producer calls th09_touch_stick(0,0) before th09_touch_cancel; cancel alone would retain axes.',
    f32:'Source x*32767 is a JS number; the native float argument and division by 32767.f are rounded to f32. Derived axis fields are not extra recorded native samples.',
    automaticFire:'3-on/3-off buttons remain real input; full changes and xy changes are reported separately.'},
  references:{producer:'D:/workspace/eagler/worktrees/th09-mobile-rollback-4x/th09_web/tests/multiplayer/touch-peer-browser.mjs:64-82,107-133',
    generator:'D:/workspace/eagler/worktrees/th09-mobile-rollback-4x/th09_web/tests/multiplayer/touch-input-probe.mjs:5-40',
    capture:'th09_web/cpp/sdl/Application.cpp:437-440,492-500; multiplayer/RollbackSession.cpp:193-211',
    axes:'th09_web/cpp/sdl/Application.cpp:653-655; portable/input/TouchController.hpp:23,58; Application.cpp:344-346'},
  limits:['No exact capture-time player position, speed, motion scale or readiness is retained by this join. Coordinate changes cannot be attributed to add/subtract cancellation alone.',
    'Boundary rows are kept separately and excluded from within-hold change counts. Unchanged intervals and non-Joystick actual frames remain included.',
    'No real input is modified, quantized or predicted. There is no counterfactual gameplay or phone/Launcher/wall-clock conclusion.']};
try {
  evidence.selfBefore=identity(self);
  const bytes=readFileSync(input); evidence.inputBefore={path:input,bytes:bytes.length,sha256:sha(bytes)};
  need(basename(input)===PIN.name&&bytes.length===PIN.bytes&&sha(bytes)===PIN.sha256,'Frozen raw identity mismatch');
  const raw=JSON.parse(bytes);
  evidence.recorded={passed:raw.passed,scope:raw.scope,parameters:raw.parameters,identity:raw.identity,identityAfter:raw.identityAfter,
    errors:raw.errors,localTapeSha256:raw.localTapeSha256,scheduleSha256:raw.scheduleSha256};
  need(raw.passed===true&&Array.isArray(raw.errors)&&raw.errors.length===0,'Source raw not clean PASS');
  need(raw.identity&&same(raw.identity,raw.identityAfter)&&raw.identity.harness===PIN.harness&&raw.identity.touchProbe===PIN.touchProbe,
    'Frozen producer identity absent/changed');
  need(Object.entries(PARAMETERS).every(([k,v])=>raw.parameters?.[k]===v),'Frozen controlled parameters differ');
  need(Array.isArray(raw.result?.localTape)&&raw.result.localTape.length===2&&Array.isArray(raw.result.events)&&raw.result.events.length===2,
    'Expected both controlled peers');
  evidence.peers=[0,1].map(side=>inspectPeer(raw,side));
  for(const p of evidence.peers) if(!p.qualified) evidence.errors.push('Peer '+p.side+': '+p.errors.join('; '));
} catch(error) {evidence.errors.push(String(error?.stack||error));}
for(const [key,path,before] of [['selfAfter',self,evidence.selfBefore],['inputAfter',input,evidence.inputBefore]]) {
  try {evidence[key]=identity(path);need(before&&same(before,evidence[key]),key+' identity changed');}
  catch(error){evidence.errors.push(String(error?.stack||error));}
}
evidence.analysisPassed=evidence.errors.length===0&&evidence.peers.length===2&&evidence.peers.every(p=>p.qualified);
const encoded=JSON.stringify(evidence)+'\n';
writeFileSync(output,encoded,{flag:'wx'});
console.log(JSON.stringify({analysisPassed:evidence.analysisPassed,output,bytes:Buffer.byteLength(encoded),
  sha256:sha(encoded),errors:evidence.errors,holds:evidence.peers.flatMap(p=>p.intervals.map(h=>({side:p.side,frames:h.frames,counts:h.counts,distinctXY:h.distinctCoordinates.length})))}));
if(!evidence.analysisPassed) process.exitCode=1;
