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
function uniquePresentationRecords(records){
  const index=new Map();
  for(const record of records){const key=presentationKey(record);if(key!==null)index.set(key,index.has(key)?null:record);}
  return index;
}
function detachedPresentation(snapshot){
  if(!snapshot||!Number.isSafeInteger(snapshot.generation)||snapshot.generation<1||!Number.isInteger(snapshot.stage)||snapshot.stage<1||snapshot.stage>16
    ||!Number.isSafeInteger(snapshot.frame)||snapshot.frame<0||!Array.isArray(snapshot.records)||snapshot.records.length>65536)
    throw new Error('Invalid display presentation snapshot');
  const records=snapshot.records.map(cloneDisplayRecord);let vertices=0;
  for(const record of records){
    if(!record||typeof record!=='object'||Array.isArray(record)||presentationNumberFields.some(key=>Object.hasOwn(record,key)&&!Number.isFinite(record[key])))
      throw new Error('Invalid display presentation record');
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
  if(Object.hasOwn(snapshot,'background'))detached.background=detachedBackground(snapshot.background);
  return detached;
}
function jumpedPosition(first,last){
  let squared=0;
  for(const key of ['x','y','z'])if(Number.isFinite(first[key])&&Number.isFinite(last[key]))squared+=(last[key]-first[key])**2;
  return squared>128**2;
}
function sameVertexMetadata(first,last){
  const a=Object.keys(first).filter(key=>!['x','y','z'].includes(key)),b=Object.keys(last).filter(key=>!['x','y','z'].includes(key));
  return a.length===b.length&&a.every(key=>Object.hasOwn(last,key)&&first[key]===last[key])
    &&Object.hasOwn(first,'z')===Object.hasOwn(last,'z');
}
function blendDisplayRecord(first,last,alpha){
  if(!continuousWorldRecord(last)||jumpedPosition(first,last)
    ||['clock','visualTicks'].some(key=>Number.isFinite(first[key])&&Number.isFinite(last[key])&&last[key]<first[key]))return;
  last.presentationInterpolated=true;
  for(const key of presentationScalars)if(Number.isFinite(first[key])&&Number.isFinite(last[key]))last[key]=first[key]+(last[key]-first[key])*alpha;
  for(const key of presentationAngles)if(Number.isFinite(first[key])&&Number.isFinite(last[key])){
    const distance=last[key]-first[key];last[key]=first[key]+(distance===0?distance:Math.atan2(Math.sin(distance),Math.cos(distance)))*alpha;
  }
  const a=first.geometry,b=last.geometry;
  if(!a||!b||!continuousGeometryKinds.has(b.kind)||a.kind!==b.kind||a.owner!==b.owner||a.column!==b.column||a.vertices.length!==b.vertices.length
    ||a.vertices.some((vertex,index)=>!sameVertexMetadata(vertex,b.vertices[index])||jumpedPosition(vertex,b.vertices[index])))return;
  for(let index=0;index<b.vertices.length;++index)for(const key of ['x','y','z']){
    if(Number.isFinite(a.vertices[index][key])&&Number.isFinite(b.vertices[index][key]))
      b.vertices[index][key]=a.vertices[index][key]+(b.vertices[index][key]-a.vertices[index][key])*alpha;
  }
}

export class WorldPresentationInterpolator {
  #previous=null;
  #current=null;
  #index=null;
  #pairs=null;
  #deep=null;
  reset(){this.#previous=null;this.#current=null;this.#index=null;this.#pairs=null;this.#deep=null;}
  capture(snapshot){
    const next=detachedPresentation(snapshot),current=this.#current;
    if(current&&current.generation===next.generation&&current.stage===next.stage&&current.frame===next.frame)return false;
    const consecutive=current&&current.generation===next.generation&&current.stage===next.stage&&next.frame===current.frame+1;
    const keys=next.records.map(presentationKey),index=new Map();
    for(let i=0;i<keys.length;++i){const key=keys[i];if(key!==null)index.set(key,index.has(key)?null:next.records[i]);}
    const pairs=consecutive?keys.map(key=>key!==null&&index.get(key)?this.#index.get(key)??null:null):null;
    const deep=next.records.map(record=>Object.values(record).some(value=>value!==null&&typeof value==='object'));
    this.#previous=consecutive?current:null;this.#pairs=pairs;this.#deep=deep;this.#index=index;
    this.#current=next;return true;
  }
  sample(alpha){
    if(!Number.isFinite(alpha)||alpha<0||alpha>1)throw new Error('Invalid display interpolation phase');
    if(!this.#current)return null;
    const output={...this.#current,records:this.#current.records.map((record,index)=>this.#deep[index]?structuredClone(record):{...record})};
    if(this.#current.background)output.background=structuredClone(this.#current.background);
    if(!this.#previous||alpha===1)return output;
    if(this.#previous.background&&output.background)blendDisplayBackground(this.#previous.background,output.background,alpha);
    for(let index=0;index<output.records.length;++index){const before=this.#pairs[index];if(before)blendDisplayRecord(before,output.records[index],alpha);}
    return output;
  }
}

function cloneDisplayRecord(record){
  if(!record||typeof record!=='object'||Array.isArray(record))return record;
  const copy={...record};
  for(const key of Object.keys(copy))if(copy[key]!==null&&typeof copy[key]==='object')copy[key]=structuredClone(copy[key]);
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
  for(const mesh of current){
    const key=backgroundKey(mesh),old=before.get(key);if(!old||!now.get(key))continue;
    if(old.fog.some((value,index)=>index<4&&value!==mesh.fog[index]))continue;
    let matches=true;
    for(let base=0;base<66&&matches;base+=11){
      if(old.vertices[base+3]<30||mesh.vertices[base+3]<30){matches=false;break;}
      // UV/tint animation must not split the camera phase of coplanar meshes.
    }
    if(!matches)continue;
    for(let base=0;base<66;base+=11)for(let component=0;component<4;++component){
      const index=base+component;mesh.vertices[index]=old.vertices[index]+(mesh.vertices[index]-old.vertices[index])*alpha;
    }
  }
}
