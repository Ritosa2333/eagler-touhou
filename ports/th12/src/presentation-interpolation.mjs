// Presentation endpoints contain JS-owned draw data only. No core, input,
// animation sampler, clock or random source is reachable from this module.
const PLAYER=0xffff0000,FOCUS=0xffff0020;
const family=owner=>(owner>>>28);
const itemId=owner=>(owner&0x0fffffff);
const finitePoint=p=>p&&Number.isFinite(p.x)&&Number.isFinite(p.y);
const near=(a,b)=>Math.abs(a-b)<.002;
const clampAlpha=alpha=>Number.isFinite(alpha)?Math.max(0,Math.min(1,alpha)):1;
const ownerKinds=['player','enemy','projectile','item','bomb'];
const meshKinds=['','stage-grid','ufo-trail','ufo-summon','laser','spell-ring','ufo-fill','leaf'];
const appearanceFields=['name','sprite','layer','mode','blend','anchorX','anchorY','point','visible','geometryCount'];

function readBinaryPresentationMotion(core){
  const ptr=core._th12_presentation_motion_binary(),heap=core.HEAPU8;
  const range=(p,n,stride,max=65536)=>{if(!Number.isSafeInteger(p)||p<0||p%4||!Number.isSafeInteger(n)||n<0||n>max||p+n*stride>heap.byteLength)throw Error('展示二进制记录范围无效');};
  range(ptr,17,4);const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),u=p=>view.getUint32(p,true),f=p=>view.getFloat32(p,true);
  if(u(ptr)!==2)throw Error('展示二进制记录版本无效');
  const state={version:1,epoch:u(ptr+4),frame:u(ptr+8),playerState:u(ptr+12),focusId:u(ptr+16),backgroundRevision:u(ptr+20),owners:[],nodes:[],lasers:[]};
  const owners=u(ptr+24),ownerCount=u(ptr+28),nodes=u(ptr+36),nodeCount=u(ptr+40),lasers=u(ptr+48),laserCount=u(ptr+52),lineages=u(ptr+60),lineageCount=u(ptr+64);
  if(u(ptr+32)!==24||u(ptr+44)!==64||u(ptr+56)!==40)throw Error('展示二进制记录步长无效');
  range(owners,ownerCount,24);range(nodes,nodeCount,64);range(lasers,laserCount,40);range(lineages,lineageCount,4,65536*34);
  for(let i=0;i<ownerCount;i++){
    const p=owners+i*24,kind=ownerKinds[u(p+8)];if(kind===undefined)throw Error('展示 owner 类型无效');
    state.owners.push({id:u(p),revision:u(p+4),kind,position:[f(p+12),f(p+16),f(p+20)]});
  }
  for(let i=0;i<nodeCount;i++){
    const p=nodes+i*64,meshKind=meshKinds[u(p+52)],offset=u(p+56),count=u(p+60);
    if(meshKind===undefined||count<2||count>34||count%2||offset+count>lineageCount)throw Error('展示节点链记录无效');
    const lineage=[];for(let j=0;j<count;j++)lineage.push(u(lineages+(offset+j)*4));
    state.nodes.push({id:u(p),owner:u(p+4),motionOwner:u(p+8),parent:u(p+12),local:[f(p+16),f(p+20),f(p+24)],engine:[f(p+28),f(p+32),f(p+36)],mask:u(p+40),flags:u(p+44),stageSlot:view.getInt32(p+48,true),meshKind,lineage});
  }
  for(let i=0;i<laserCount;i++){
    const p=lasers+i*40;
    state.lasers.push({owner:u(p),revision:u(p+4),topology:u(p+8),kind:u(p+12),phase:u(p+16),cursor:view.getInt32(p+20,true),nodes:u(p+24),followOwner:u(p+28),followRevision:u(p+32),age:f(p+36)});
  }
  return state;
}

export function readCorePresentationMotion(core){
  let state;
  if(core._th12_presentation_motion_binary)state=readBinaryPresentationMotion(core);
  else{
    if(!core._th12_presentation_motion||!core._th12_presentation_motion_size)return null;
    const ptr=core._th12_presentation_motion(),size=core._th12_presentation_motion_size(),heap=core.HEAPU8;
    if(!Number.isSafeInteger(ptr)||!Number.isSafeInteger(size)||ptr<0||size<2||size>32*1024*1024||ptr+size>heap.byteLength)throw Error('展示连续性记录范围无效');
    state=JSON.parse(new TextDecoder().decode(heap.subarray(ptr,ptr+size)));
  }
  if(state?.version!==1||!Number.isInteger(state.epoch)||!Number.isInteger(state.frame)||
    !Array.isArray(state.owners)||!Array.isArray(state.nodes)||!Array.isArray(state.lasers)||
    state.nodes.length>65536||state.owners.length>65536||state.lasers.length>65536)throw Error('展示连续性记录版本无效');
  const vec=v=>Array.isArray(v)&&v.length===3&&v.every(Number.isFinite);
  for(const owner of state.owners)if(!Number.isInteger(owner.id)||!Number.isInteger(owner.revision)||!vec(owner.position)||typeof owner.kind!=='string')throw Error('展示 owner 记录无效');
  for(const node of state.nodes)if(!Number.isInteger(node.id)||!Number.isInteger(node.owner)||!Number.isInteger(node.motionOwner)||!Number.isInteger(node.parent)||
    !vec(node.local)||!vec(node.engine)||!Array.isArray(node.lineage)||!node.lineage.length||!node.lineage.every(Number.isInteger)||!Number.isInteger(node.mask)||typeof node.meshKind!=='string')throw Error('展示节点记录无效');
  for(const laser of state.lasers)if(!['owner','revision','topology','kind','phase','cursor','nodes','followOwner','followRevision'].every(k=>Number.isInteger(laser[k])))throw Error('展示激光记录无效');
  state.ownerMap=new Map(state.owners.map(o=>[o.id,o]));state.nodeMap=new Map(state.nodes.map(n=>[n.id,n]));state.laserMap=new Map(state.lasers.map(l=>[l.owner,l]));
  if(state.ownerMap.size!==state.owners.length||state.nodeMap.size!==state.nodes.length||state.laserMap.size!==state.lasers.length)throw Error('展示身份记录重复');
  return state;
}

function poseGroup(p){
  const owner=p.owner>>>0;
  if(owner===FOCUS)return 'focus';
  if(owner>=PLAYER&&owner<PLAYER+0x20)return 'player';
  if(family(owner)===1)return 'bullet:'+owner;
  if([2,3,4].includes(family(owner)))return 'item:'+itemId(owner);
  // Enemy IDs, detached effects, UFO fffd*, lasers b*, stage surfaces,
  // ASCII queues and HUD VMs have different position/lifecycle owners.
  return null;
}

function poseIdentity(p){
  if(p.logical)return 'anm:'+p.id;
  if(p.mesh)return 'mesh:'+p.id+':'+p.renderPass;
  return p.presentationKey??null;
}

function sameAppearance(a,b){
  if(!a||!b||a.owner!==b.owner||a.logical!==b.logical||a.mesh!==b.mesh||
    a.drawPriority!==b.drawPriority||a.clip!==b.clip)return false;
  for(const key of appearanceFields)if(a.a?.[key]!==b.a?.[key])return false;
  if(a.mesh&&(a.primitive!==b.primitive||a.renderPass!==b.renderPass||a.untextured!==b.untextured||
    a.filter!==b.filter||a.addressFlags!==b.addressFlags||a.vertices.length!==b.vertices.length||a.a.entry!==b.a.entry))return false;
  return true;
}

function sameRecord(a,b){
  return a&&b&&a.owner===b.owner&&a.bank===b.bank&&a.animation===b.animation&&
    a.kind===b.kind&&a.color===b.color&&a.interrupt===b.interrupt&&
    (b.age===a.age||b.age===a.age+1);
}

// Called while publishing a fixed-tick endpoint. Records are copied before
// another core export can grow WASM memory or rebuild a scratch draw queue.
export function capturePresentationOwners(hud,records,poses){
  const anchors=new Map(),groups=new Map();
  anchors.set('player',{x:hud[1]+224,y:hud[2]+16,limit:16,record:null});
  anchors.set('focus',anchors.get('player'));
  for(const record of records){
    const owner=record.owner>>>0,kind=family(owner);
    if(kind===1)anchors.set('bullet:'+owner,{...record,record,limit:64});
    // A body is the world anchor. Offscreen arrows only expose horizontal
    // motion; hints use the same item offset, never an independent lerp.
    if(kind===2)anchors.set('item:'+itemId(owner),{...record,record,limit:32});
    else if(kind===3&&!anchors.has('item:'+itemId(owner)))
      anchors.set('item:'+itemId(owner),{...record,record,limit:32});
  }
  for(const p of poses){
    const group=poseGroup(p),identity=poseIdentity(p);
    if(!group||!identity)continue;
    if(!groups.has(group))groups.set(group,new Map());
    const entries=groups.get(group);
    // Duplicate identities are ambiguous; poison the group rather than
    // matching a recycled fallback child or a mesh by array index.
    if(entries.has(identity))entries.set(identity,null);else entries.set(identity,p);
  }
  return {anchors,groups};
}

function groupOffset(previous,current,group,alpha){
  const before=previous.motion.anchors.get(group),after=current.motion.anchors.get(group),
    oldPoses=previous.motion.groups.get(group),newPoses=current.motion.groups.get(group);
  if(!finitePoint(before)||!finitePoint(after)||!oldPoses||!newPoses||oldPoses.size!==newPoses.size)return null;
  if(after.record&&!sameRecord(before.record,after.record))return null;
  if(group==='player'||group==='focus'){
    // Body animation resets, visibility/respawn boundaries and option changes
    // must snap the entire player-owned set together.
    const oldBody=previous.records.find(r=>r.owner===PLAYER),newBody=current.records.find(r=>r.owner===PLAYER);
    if(!sameRecord(oldBody,newBody))return null;
  }
  const dx=after.x-before.x,dy=after.y-before.y;
  if(Math.hypot(dx,dy)>after.limit)return null;
  for(const [identity,p]of newPoses){
    const old=oldPoses.get(identity);
    if(!sameAppearance(old,p))return null;
    // Focus77/child76 retain their old coordinates after the live focus
    // handle is released. Only a still-attached set follows the player.
    if(group==='focus'&&(!finitePoint(old)||!finitePoint(p)||!near(p.x-old.x,dx)||!near(p.y-old.y,dy)))return null;
  }
  return {x:-dx*(1-alpha),y:-dy*(1-alpha)};
}

function translatePose(p,offset){
  const x=offset.x,y=family(p.owner>>>0)===3?0:offset.y;
  if(!x&&!y)return p;
  // Geometry is translated rigidly with its explicit owner. Laser curve
  // vertices are never paired by index; their owners are ineligible above.
  if(p.mesh)return {...p,vertices:p.vertices.map(v=>({...v,x:v.x+x,y:v.y+y}))};
  if(p.nativeText)return {...p,nativeText:{...p.nativeText,x:p.nativeText.x+x,y:p.nativeText.y+y}};
  if(finitePoint(p))return {...p,presentationOffset:[x,y]};
  return p;
}

// Previous/current are explicit caller-owned endpoints, not implicit render
// history. Repeating this function at one logical tick cannot advance them.
export function interpolatePresentation(previous,current,alpha=1){
  if(!current)throw Error('缺少当前展示端点');
  alpha=clampAlpha(alpha);
  if(alpha===1||!previous||previous===current||
    previous.epoch!==current.epoch||previous.stage!==current.stage||
    current.frame!==previous.frame+1)return current;
  if(previous.nativeMotion&&current.nativeMotion)return interpolateNative(previous,current,alpha);
  if(!previous.motion||!current.motion)return current;
  const offsets=new Map();
  for(const group of current.motion.groups.keys()){
    const offset=groupOffset(previous,current,group,alpha);
    if(offset)offsets.set(group,offset);
  }
  return {...current,poses:current.poses.map(p=>{
    const offset=offsets.get(poseGroup(p));return offset?translatePose(p,offset):p;
  })};
}

const lerp=(a,b,t)=>a+(b-a)*t;
const wrappedLerp=(a,b,t,period)=>a+(((b-a+period/2)%period+period)%period-period/2)*t;
const sameArray=(a,b)=>a?.length===b?.length&&a?.every((v,i)=>v===b[i]);
const sameNode=(a,b)=>a&&b&&a.owner===b.owner&&a.motionOwner===b.motionOwner&&a.parent===b.parent&&sameArray(a.lineage,b.lineage)&&a.meshKind===b.meshKind&&(a.flags&~12)===(b.flags&~12)&&a.stageSlot===b.stageSlot;
function nativeOwner(p,state){
  const node=p.logical||p.mesh?state.nodeMap.get(p.id):null;
  let owner=node?.motionOwner??(p.owner>>>0);
  if(owner>=PLAYER&&owner<PLAYER+0x20)owner=PLAYER;
  if([2,3,4].includes(family(owner)))owner=0x20000000+itemId(owner);
  return {node,owner,source:state.ownerMap.get(owner)};
}
function nativeKey(p){return poseIdentity(p)??(p.nativeText&&p.presentationKey?String(p.presentationKey):null);}
const nativeIndices=new WeakMap();
function nativeIndex(endpoint){
  let index=nativeIndices.get(endpoint);if(index)return index;
  const poses=new Map(),laserGroups=new Map();
  for(const p of endpoint.poses){
    const key=nativeKey(p);if(key!==null)poses.set(key,p);
    if(endpoint.nativeMotion.laserMap.has(p.owner)){
      if(!laserGroups.has(p.owner))laserGroups.set(p.owner,[]);
      laserGroups.get(p.owner).push(p);
    }
  }
  // Published endpoints are immutable; the WeakMap does not retain old frames.
  index={poses,groups:nativeGroups(endpoint),laserGroups};nativeIndices.set(endpoint,index);return index;
}
function nativeGroups(endpoint){
  const result=new Map();
  for(const p of endpoint.poses){
    const {owner,source}=nativeOwner(p,endpoint.nativeMotion),key=nativeKey(p);
    if(!source||!key)continue;
    if(!result.has(owner))result.set(owner,new Map());
    const group=result.get(owner);if(group.has(key))group.set(key,null);else group.set(key,p);
  }
  return result;
}
function resolvedEngine(node,state){
  const output=[...node.engine];let parent=node.parent;
  for(let depth=0;parent&&depth<16;depth++){
    const next=state.nodeMap.get(parent);if(!next)return null;
    for(let i=0;i<3;i++)output[i]+=next.engine[i];parent=next.parent;
  }
  return output;
}
function nativeScalars(p,old,node,oldNode,alpha){
  if(!p.a||!old.a||!sameNode(oldNode,node))return p;
  const mask=node.mask|oldNode.mask;if(!(mask&62))return p;
  let a=null;
  const fields=(keys,wrap=0)=>{for(const key of keys){const x=old.a[key],y=p.a[key];if(!Number.isFinite(x)||!Number.isFinite(y))continue;let delta=y-x;if(wrap)delta=((delta+wrap/2)%wrap+wrap)%wrap-wrap/2;const value=x+delta*alpha;if(value===y)continue;a??={...p.a};a[key]=value;}};
  if(mask&2)fields(['rx','ry','rotation'],Math.PI*2);
  if(mask&4)fields(['sx','sy']);
  if(mask&8)fields(['u','v'],1);
  if(mask&32)fields(['uvScaleX','uvScaleY']);
  if(mask&16){fields(['r','g','b','alpha','r2','g2','b2','alpha2']);
    if(Array.isArray(old.a.primaryColor)&&Array.isArray(p.a.primaryColor)&&old.a.primaryColor.some((v,i)=>v!==p.a.primaryColor[i])){
      a??={...p.a};a.primaryColor=p.a.primaryColor.map((v,i)=>lerp(old.a.primaryColor[i],v,alpha));}}
  return a?{...p,a}:p;
}
function interpolateNative(previous,current,alpha){
  const before=previous.nativeMotion,after=current.nativeMotion;
  if(before.epoch!==after.epoch||before.frame!==previous.frame||after.frame!==current.frame)return current;
  const oldIndex=nativeIndex(previous),newIndex=nativeIndex(current),previousPoses=oldIndex.poses,
    oldGroups=oldIndex.groups,newGroups=newIndex.groups,continuousOwners=new Set(),continuousLasers=new Set(),
    oldLaserGroups=oldIndex.laserGroups,newLaserGroups=newIndex.laserGroups;
  for(const [owner,laser]of after.laserMap){
    const oldLaser=before.laserMap.get(owner);
    if(!oldLaser||!['revision','topology','kind','phase','cursor','nodes','followOwner','followRevision'].every(k=>oldLaser[k]===laser[k]))continue;
    const oldSet=oldLaserGroups.get(owner)??[],newSet=newLaserGroups.get(owner)??[];
    if(oldSet.length!==newSet.length)continue;
    if(newSet.every(p=>{const old=previousPoses.get(nativeKey(p));return old&&sameAppearance(old,p)&&sameNode(before.nodeMap.get(p.id),after.nodeMap.get(p.id));}))continuousLasers.add(owner);
  }
  for(const [owner,group]of newGroups){
    const oldGroup=oldGroups.get(owner),a=before.ownerMap.get(owner),b=after.ownerMap.get(owner);
    if(!oldGroup||oldGroup.size!==group.size||!a||a.revision!==b.revision||a.kind!==b.kind)continue;
    let continuous=true;
    for(const [key,p]of group){const old=oldGroup.get(key);
      if(!sameAppearance(old,p)){continuous=false;break;}
      const n=after.nodeMap.get(p.id),m=before.nodeMap.get(old.id);
      if((n||m)&&!sameNode(m,n)){continuous=false;break;}
    }
    if(continuous)continuousOwners.add(owner);
  }
  const poses=current.poses.map(p=>{
    const key=nativeKey(p),old=key===null?null:previousPoses.get(key);
    if(!old||!sameAppearance(old,p))return p;
    const {node,owner,source}=nativeOwner(p,after),oldNode=node?before.nodeMap.get(node.id):null;
    if(node&&!sameNode(oldNode,node))return p;
    // Laser mesh corners/cross-sections have explicit source-owned roles.
    // A history cut, external set, extension transition or phase change
    // invalidates the complete role correspondence, even at equal lengths.
    if(p.mesh&&node?.meshKind==='laser'){
      if(!continuousLasers.has(p.owner))return p;
      return {...p,vertices:p.vertices.map((v,i)=>({...v,x:lerp(old.vertices[i].x,v.x,alpha),y:lerp(old.vertices[i].y,v.y,alpha)}))};
    }
    const owned=source&&continuousOwners.has(owner);
    const a=before.ownerMap.get(owner),b=source;
    if(source&&!owned)return p;
    let result=nativeScalars(p,old,node,oldNode,alpha);
    if(owned){
      const dx=(a.position[0]-b.position[0])*(1-alpha),dy=(a.position[1]-b.position[1])*(1-alpha);
      if(p.mesh){
        if(node&&['ufo-fill','spell-ring'].includes(node.meshKind))return {...p,vertices:p.vertices.map((v,i)=>({...v,x:lerp(old.vertices[i].x,v.x,alpha),y:lerp(old.vertices[i].y,v.y,alpha)}))};
        return translatePose(result,{x:dx,y:dy});
      }
      // Options, focus and the optional hit marker share player displacement;
      // offscreen arrows and the Boss indicator retain their projected Y.
      if(owner===PLAYER||p.nativeText||family(p.owner>>>0)===3||p.owner===0xfffe0200)
        return translatePose(result,{x:dx,y:p.owner===0xfffe0200?0:dy});
      // World VM positions include authored local tracks and resolved parents.
      // Direct ECL/VM writes are gated by the owner revision and lineage.
      if(finitePoint(p)&&finitePoint(old))return {...result,presentationOffset:[(old.x-p.x)*(1-alpha),(old.y-p.y)*(1-alpha)]};
      return result;
    }
    if(p.mesh&&node&&['stage-grid','ufo-summon','ufo-trail','leaf','spell-ring','ufo-fill'].includes(node.meshKind)){
      if(node.meshKind==='stage-grid'&&before.backgroundRevision!==after.backgroundRevision)return p;
      return {...p,vertices:p.vertices.map((v,i)=>({...v,x:lerp(old.vertices[i].x,v.x,alpha),y:lerp(old.vertices[i].y,v.y,alpha)}))};
    }
    // Explicit authored ANM local position tracks are eligible when their
    // resolved engine origin did not move. Detached/global positions do not
    // acquire an inferred world-motion rule from being in the ANM list.
    if(node&&((node.mask|oldNode.mask)&1)&&sameArray(resolvedEngine(node,after),resolvedEngine(oldNode,before))&&finitePoint(p)&&finitePoint(old))
      result={...result,presentationOffset:[(old.x-p.x)*(1-alpha),(old.y-p.y)*(1-alpha)]};
    return result;
  });
  const logicalBackground=interpolateBackground(previous,current,alpha);
  return {...current,poses,logicalBackground,screenShake:logicalBackground?.camera?.screenShake??current.screenShake};
}

function interpolateBackground(previous,current,alpha){
  const a=previous.logicalBackground,b=current.logicalBackground,x=previous.nativeMotion,y=current.nativeMotion;
  if(!a?.loaded||!b?.loaded||x.backgroundRevision!==y.backgroundRevision||a.flags!==b.flags||b.frame!==a.frame+1)return b;
  const camera={...b.camera};
  for(const key of ['position','direction','up','offset','eyeOffset','fog','screenShake']){
    if(Array.isArray(a.camera[key])&&a.camera[key].length===b.camera[key]?.length)
      camera[key]=b.camera[key].map((v,i)=>lerp(a.camera[key][i],v,alpha));
  }
  camera.fov=lerp(a.camera.fov,b.camera.fov,alpha);
  const oldRows=new Map(a.poses.map(row=>[row[0],row])),oldSlots=new Map(x.nodes.filter(n=>n.stageSlot>=0).map(n=>[n.stageSlot,n])),newSlots=new Map(y.nodes.filter(n=>n.stageSlot>=0).map(n=>[n.stageSlot,n]));
  const poses=b.poses.map(row=>{
    const old=oldRows.get(row[0]);if(!old||!sameNode(oldSlots.get(row[0]),newSlots.get(row[0]))||![1,16,17,18,19,20].every(i=>old[i]===row[i]))return row;
    const output=[...row];for(let i=2;i<=13;i++)output[i]=i>=5&&i<=7?wrappedLerp(old[i],row[i],alpha,Math.PI*2):i===10||i===11?wrappedLerp(old[i],row[i],alpha,1):lerp(old[i],row[i],alpha);return output;
  });
  return {...b,camera,poses};
}
