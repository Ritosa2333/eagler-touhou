import {takeOwnedDrawValidation} from './world-presentation.mjs';

export class FrameStepper {
  constructor() {
    this.previous = null;
    this.accumulator = 0;
    this.step = 1000 / 60;
  }

  reset() {
    this.previous = null;
    this.accumulator = 0;
  }

  advance(now, active) {
    if (!Number.isFinite(now)) throw new Error('Invalid frame timestamp');
    if (this.previous === null || !active) {
      this.previous = now;
      this.accumulator = 0;
      return { steps: 0, stalled: false };
    }
    const elapsed = now - this.previous;
    this.previous = now;
    if (elapsed < 0 || elapsed > 250) {
      this.accumulator = 0;
      return { steps: 0, stalled: true };
    }
    this.accumulator += elapsed;
    const steps = Math.min(4,Math.floor((this.accumulator + 0.0000001) / this.step));
    this.accumulator -= steps * this.step;
    return { steps, stalled: false };
  }
}

// Display-only history. Native game/input clocks never enter this data helper.
const presentationScalars=['x','y','z','sx','sy'];
const presentationAngles=['rotation','rx','ry'];
const motionFields=[...presentationScalars,...presentationAngles];
const presentationIdentityFields=['id','parent','drawPass','bank','sprite','entry','script','layer','mode','anchorX','anchorY','blend','flags','flags2','effectSystem','stageSurface'];
const presentationNumberFields=[...presentationScalars,...presentationAngles,'width','height','u','v','u0','v0','u1','v1','uvScaleX','uvScaleY','clock','visualTicks'];
const continuousWorldPasses=new Set(['hostile-bullets','hostile-lasers','player','playerShotGroup','items','frozen-bullets']);
const continuousGeometryKinds=new Set(['triangle-strip','circle-fan','circle-line','anm-ring','laser-selector3-quad']);

export function renderInterpolationPhase(clock){
  if(!(clock instanceof FrameStepper)||!Number.isFinite(clock.accumulator)||!Number.isFinite(clock.step)||clock.step<=0)
    throw new Error('Invalid display interpolation clock');
  return Math.min(1,Math.max(0,clock.accumulator/clock.step));
}

function presentationKey(record){
  if(!Number.isSafeInteger(record.id)||record.id<0||record.id>0xffffffff)return null;
  return JSON.stringify([...presentationIdentityFields.map(key=>record[key]),record.geometry?.kind,record.geometry?.owner,record.geometry?.column]);
}
function continuousWorldRecord(record){
  const id=record.id,pass=record.drawPass;
  if(record.stageSurface||record.bank===11||record.bank===5||typeof pass==='string'&&pass.startsWith('original-')
    ||id>=15000000&&id<30000000||id>=40000000&&id<70000000)return false;
  return continuousWorldPasses.has(pass)||record.effectSystem==='unified-primary'
    ||pass===undefined&&(id>=2000000&&id<7000000||id>=8000000&&id<9000000||id>=13000000&&id<15000000||id>=30000000&&id<31000000);
}
function displayRecordIndex(records){
  const ids=new Map();let duplicates=null;
  for(const record of records){
    const id=record.id;
    if(!Number.isSafeInteger(id)||id<0||id>0xffffffff)continue;
    const first=ids.get(id);
    if(!first){ids.set(id,record);continue;}
    duplicates??=new Map();
    let keys=duplicates.get(id);
    if(!keys){keys=new Map([[presentationKey(first),first]]);duplicates.set(id,keys);}
    const key=presentationKey(record);keys.set(key,keys.has(key)?null:record);
  }
  return {ids,duplicates};
}
function samePresentationIdentity(first,last,compareGeometryKey){
  // Keep the hot comparison on concrete fields; the rarely needed key path
  // still handles absent/null metadata exactly like the generic index.
  if(first.parent!==last.parent||first.drawPass!==last.drawPass||first.bank!==last.bank||first.sprite!==last.sprite
    ||first.entry!==last.entry||first.script!==last.script||first.layer!==last.layer||first.mode!==last.mode
    ||first.anchorX!==last.anchorX||first.anchorY!==last.anchorY||first.blend!==last.blend||first.flags!==last.flags
    ||first.flags2!==last.flags2||first.effectSystem!==last.effectSystem||first.stageSurface!==last.stageSurface)
      return presentationKey(first)===presentationKey(last);
  return first.geometry?.kind===last.geometry?.kind&&first.geometry?.owner===last.geometry?.owner&&first.geometry?.column===last.geometry?.column
    ||compareGeometryKey&&presentationKey(first)===presentationKey(last);
}
function previousDisplayRecord(before,now,record){
  const first=before.ids.get(record.id);if(!first)return null;
  const earlier=before.duplicates?.get(record.id),later=now.duplicates?.get(record.id);
  if(!earlier&&!later)return samePresentationIdentity(first,record,before.duplicates||now.duplicates)?first:null;
  const key=presentationKey(record);
  if(later&&!later.get(key))return null;
  return earlier?(earlier.get(key)??null):presentationKey(first)===key?first:null;
}
function detachedPresentation(snapshot,owned=false){
  if(!snapshot||!Number.isSafeInteger(snapshot.generation)||snapshot.generation<1||!Number.isInteger(snapshot.stage)||snapshot.stage<1||snapshot.stage>16
    ||!Number.isSafeInteger(snapshot.frame)||snapshot.frame<0||!Array.isArray(snapshot.records)||snapshot.records.length>65536)
    throw new Error('Invalid display presentation snapshot');
  const records=owned?snapshot.records:snapshot.records.map(cloneDisplayRecord),validated=owned?takeOwnedDrawValidation(records):null;
  let vertices=validated?.vertices??0;
  for(const record of validated?.unchecked??records){
    if(!record||typeof record!=='object'||Array.isArray(record))
      throw new Error('Invalid display presentation record');
    for(const key of presentationNumberFields)if(!Number.isFinite(record[key])&&Object.hasOwn(record,key))throw new Error('Invalid display presentation record');
    if(record.geometry!==undefined){
      const geometry=record.geometry;
      if(!geometry||!Array.isArray(geometry.vertices)||geometry.vertices.length>65536||(vertices+=geometry.vertices.length)>65536)
        throw new Error('Invalid display geometry extent');
      for(const vertex of geometry.vertices){
        if(!vertex||typeof vertex!=='object'||Array.isArray(vertex)||!Number.isFinite(vertex.x)||!Number.isFinite(vertex.y)
          ||['z','rhw','u','v'].some(key=>Object.hasOwn(vertex,key)&&!Number.isFinite(vertex[key])))throw new Error('Invalid display geometry coordinate');
      }
    }
  }
  const detached={generation:snapshot.generation,stage:snapshot.stage,frame:snapshot.frame,records};
  if(Object.hasOwn(snapshot,'background'))detached.background=owned?snapshot.background:detachedBackground(snapshot.background);
  return detached;
}
function jumpedPosition(first,last){
  const x=Number.isFinite(first.x)&&Number.isFinite(last.x)?last.x-first.x:0,
    y=Number.isFinite(first.y)&&Number.isFinite(last.y)?last.y-first.y:0,
    z=Number.isFinite(first.z)&&Number.isFinite(last.z)?last.z-first.z:0;
  return x*x+y*y+z*z>128**2;
}
function sameVertexMetadata(first,last){
  const a=Object.keys(first).filter(key=>!['x','y','z'].includes(key)),b=Object.keys(last).filter(key=>!['x','y','z'].includes(key));
  return a.length===b.length&&a.every(key=>Object.hasOwn(last,key)&&first[key]===last[key])
    &&Object.hasOwn(first,'z')===Object.hasOwn(last,'z');
}
function canBlendGeometry(a,b){
  return a&&b&&continuousGeometryKinds.has(b.kind)&&a.kind===b.kind&&a.owner===b.owner&&a.column===b.column&&a.vertices.length===b.vertices.length
    &&!a.vertices.some((vertex,index)=>!sameVertexMetadata(vertex,b.vertices[index])||jumpedPosition(vertex,b.vertices[index]));
}
function displayMotionMask(first,last){
  let mask=0;
  for(let i=0;i<motionFields.length;++i){
    const key=motionFields[i],a=first[key],b=last[key];
    if((a!==b||Object.is(b,-0))&&Number.isFinite(a)&&Number.isFinite(b))mask|=1<<i;
  }
  return mask;
}
function blendDisplayRecord({first,last,mask,geometry},output,alpha){
  for(let bits=mask;bits;bits&=bits-1){
    const i=31-Math.clz32(bits&-bits),key=motionFields[i],a=first[key],delta=last[key]-a;
    const angle=i>=presentationScalars.length&&delta!==0?Math.atan2(Math.sin(delta),Math.cos(delta)):delta;
    output[key]=a+angle*alpha;
  }
  if(!geometry)return;
  const a=first.geometry,b=last.geometry;
  for(let index=0;index<b.vertices.length;++index)for(const key of ['x','y','z']){
    if(Number.isFinite(a.vertices[index][key])&&Number.isFinite(b.vertices[index][key]))
      output.geometry.vertices[index][key]=a.vertices[index][key]+(b.vertices[index][key]-a.vertices[index][key])*alpha;
  }
}
function checkDisplayPhase(alpha){
  if(!Number.isFinite(alpha)||alpha<0||alpha>1)throw new Error('Invalid display interpolation phase');
}
function copyDisplaySample(record){
  for(const key in record)if(record[key]!==null&&typeof record[key]==='object')return structuredClone(record);
  return {...record};
}

export class WorldPresentationInterpolator {
  #previous=null;
  #current=null;
  #index=null;
  #pairs=null;
  #draw=null;
  reset(){this.#previous=null;this.#current=null;this.#index=null;this.#pairs=null;this.#draw=null;}
  capture(snapshot){return this.#capture(snapshot,false);}
  captureOwned(snapshot){return this.#capture(snapshot,true);}
  #capture(snapshot,owned){
    const next=detachedPresentation(snapshot,owned),current=this.#current;
    if(current&&current.generation===next.generation&&current.stage===next.stage&&current.frame===next.frame)return false;
    const consecutive=current&&current.generation===next.generation&&current.stage===next.stage&&next.frame===current.frame+1;
    const index=displayRecordIndex(next.records);
    const pairs=[];
    if(consecutive){
      for(let i=0;i<next.records.length;++i){
        const last=next.records[i];if(!continuousWorldRecord(last))continue;
        const first=previousDisplayRecord(this.#index,index,last);
        if(!first||jumpedPosition(first,last)
          ||(Number.isFinite(first.clock)&&Number.isFinite(last.clock)&&last.clock<first.clock)
          ||(Number.isFinite(first.visualTicks)&&Number.isFinite(last.visualTicks)&&last.visualTicks<first.visualTicks))continue;
        pairs.push({index:i,first,last,mask:displayMotionMask(first,last),geometry:canBlendGeometry(first.geometry,last.geometry)});
      }
    }
    this.#previous=consecutive?current:null;this.#pairs=pairs;this.#index=index;this.#draw=null;
    this.#current=next;return true;
  }
  // The renderer borrows this view synchronously. Only records that move need
  // writable copies, and their storage is reused until the next World frame.
  sampleForDraw(alpha){
    checkDisplayPhase(alpha);
    if(alpha===1||!this.#previous)return this.#current;
    if(!this.#draw){
      const records=this.#current.records.slice();
      for(const {index,last,geometry} of this.#pairs){
        const copy={...last,presentationInterpolated:true};
        if(geometry)copy.geometry={...last.geometry,vertices:last.geometry.vertices.map(vertex=>({...vertex}))};
        records[index]=copy;
      }
      this.#draw={...this.#current,records};
    }
    return this.#blend(this.#draw,alpha);
  }
  sample(alpha){
    checkDisplayPhase(alpha);
    if(!this.#current)return null;
    const output={...this.#current,records:this.#current.records.map(copyDisplaySample)};
    if(alpha!==1)for(const {index} of this.#pairs)output.records[index].presentationInterpolated=true;
    return this.#blend(output,alpha);
  }
  #blend(output,alpha){
    if(this.#current.background){
      output.background=structuredClone(this.#current.background);
      if(alpha!==1&&this.#previous?.background)blendDisplayBackground(this.#previous.background,output.background,alpha);
    }
    if(alpha!==1)for(const pair of this.#pairs)if(pair.mask||pair.geometry)blendDisplayRecord(pair,output.records[pair.index],alpha);
    return output;
  }
}

function cloneDisplayRecord(record){
  if(!record||typeof record!=='object'||Array.isArray(record))return record;
  const copy={...record};
  for(const key in copy)if(copy[key]!==null&&typeof copy[key]==='object')copy[key]=structuredClone(copy[key]);
  return copy;
}

function detachedBackground(meshes){
  if(!Array.isArray(meshes)||meshes.length>4096)throw new Error('Invalid display background budget');
  const copy=structuredClone(meshes);
  for(const mesh of copy)if(!mesh||typeof mesh.presentationId!=='string'||mesh.presentationId.length>128
    ||!Array.isArray(mesh.vertices)||mesh.vertices.length!==66||!mesh.vertices.every(Number.isFinite)
    ||!Array.isArray(mesh.fog)||mesh.fog.length!==6||!mesh.fog.every(Number.isFinite))throw new Error('Invalid display background mesh');
  return copy;
}
function backgroundKey(mesh){return JSON.stringify([mesh.presentationId,mesh.bank,mesh.entry,mesh.blend,mesh.layer]);}
function uniqueBackground(meshes){
  const index=new Map();for(const mesh of meshes){const key=backgroundKey(mesh);index.set(key,index.has(key)?null:mesh);}return index;
}
function blendDisplayBackground(previous,current,alpha){
  const before=uniqueBackground(previous),now=uniqueBackground(current);
  // A background is one camera view. Never mix a snapped tile with interpolated
  // neighbours. Clip coordinates can cross the near plane; WebGL clips them
  // after interpolation, keeping shared edges at exactly the same phase.
  if(before.size!==previous.length||now.size!==current.length||before.size!==now.size)return;
  for(const mesh of current){
    const old=before.get(backgroundKey(mesh));
    if(!old||old.fog.some((value,index)=>index<4&&value!==mesh.fog[index]))return;
  }
  for(const mesh of current){
    const old=before.get(backgroundKey(mesh));
    for(let base=0;base<66;base+=11)for(let component=0;component<4;++component){
      const index=base+component;mesh.vertices[index]=old.vertices[index]+(mesh.vertices[index]-old.vertices[index])*alpha;
    }
  }
}
