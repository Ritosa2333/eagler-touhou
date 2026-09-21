import {defaultSeverity,groupFindings as groupFindingsCore,compactReport,mergeIssueGroups as mergeIssueGroupsCore} from '../../third_party/eagler-common/testkit/presentation-lab/report-core.mjs';
import {compareStateEvidence} from '../../third_party/eagler-common/testkit/presentation-lab/analyzer.mjs';
import {OWNERS,STATE_GROUPS,WORLD_OWNER_IDS,MOTION_OWNER_IDS} from './owners.mjs';
export {OWNERS,STATE_GROUPS} from './owners.mjs';

// Pure classifier used by both the browser inspector and tests. TH08 policy
// stays here; title-neutral report aggregation lives in eagler-common.
// It reports measured evidence; it does not decide that every visual must lerp.
export const SCHEMA = 'th08/presentation-audit/1';
export const STATUS = {
  'interpolated': '端点与中间值吻合', 'responsive': '有中间变化，曲线待核对',
  'missing-interpolation': '连续属性没有中间值', 'held-review': '保持当前值，需核对离散语义',
  'endpoint-mismatch': '显示端点不等于原始绘制', 'non-idempotent': '相同 α 重画结果不同',
  'downstream-held': '属性已变化，但对应提交数据不变',
  'static': '本窗口无可测变化', 'lifecycle': '生命周期边界，不作插值判定',
  'unobserved': '样本不足', 'unscoped': '对象身份未接入', 'ambiguous': '对象键冲突',
  'frozen': '世界被原作暂停', 'sprite-change': '贴图/几何换帧，需单独核对',
  'nonfinite': '出现非有限数值', 'offscreen': '未进入屏幕区域',
  'depth-change': '仅绘制深度层变化，保持离散',
  'projection-clip': 'CPU 投影穿过相机裁剪平面，屏幕几何不可观测',
  'upstream-parameter': '原始 ANM 参数；由专用几何所有者解释',
  'inactive-channel': '本次绘制未使用此颜色通道',
};
const TAU = Math.PI * 2;
const worldOwners = WORLD_OWNER_IDS;
const motionOwners = MOTION_OWNER_IDS;
export function keyOf(r) { return [r.meta[0],r.meta[1],r.meta[2],r.meta[3],r.meta[4]].join(':'); }
export function decodeRecords(buffer, pointer, count, stride = 752) {
  if (stride !== 752 || !Number.isInteger(count) || count < 0 || count > 8192 || !Number.isInteger(pointer) || pointer%4 || pointer < 0 || pointer + count*stride > buffer.byteLength) throw Error('Invalid presentation audit buffer');
  const result=[];
  for(let i=0;i<count;i++) {
    const at=pointer+i*stride;
    const meta=Array.from(new Uint32Array(buffer,at,12));
    result.push({meta,values:Array.from(new Float32Array(buffer,at+48,32)),vertices:Array.from(new Float32Array(buffer,at+176,Math.min(meta[10],16)*9)),
      spriteIdentity:meta[11]&(1<<20)?new Uint32Array(buffer,at+48+29*4,1)[0]:null});
  }
  return result;
}
function indexed(records) {
  const map=new Map(),duplicates=new Set();
  for(const r of records){const key=keyOf(r);if(map.has(key))duplicates.add(key);else map.set(key,r);}
  return {map,duplicates};
}
function wrap(x,period) { return period ? ((x+period/2)%period+period)%period-period/2 : x; }
function delta(a,b,period=0) { return a.map((v,i)=>wrap(b[i]-v,period)); }
function norm(v){return Math.max(0,...v.map(Math.abs));}
function distance(a,b,period=0){return a.length!==b.length?Infinity:norm(delta(a,b,period));}
function shape(r){const out=[];for(let i=0;i<r.vertices.length;i+=9)out.push(r.vertices[i]-r.values[25],r.vertices[i+1]-r.values[26]);return out;}
function uv(r){const out=[];for(let i=0;i<r.vertices.length;i+=9)out.push(r.vertices[i+3],r.vertices[i+4]);return out;}
function screenPosition(r){const shake=r.meta[11]&(1<<16);return [r.values[25]-(shake?r.values[27]:0),r.values[26]-(shake?r.values[28]:0)];}
const field = (id,label,start,end,epsilon,bit=0,period=0) => ({id,label,read:r=>r.values.slice(start,end),epsilon,bit,period});
const fields = [
  field('position','位置',0,3,.01,8),field('offset','附加位置',3,6,.01,8),
  field('scale','缩放',6,8,.0001,9),field('rotation','旋转',8,11,.0001,10,TAU),
  field('uv','UV滚动',11,13,.00001,11,1),field('rgb','颜色',13,16,1.01/255,12),
  field('opacity','透明度',16,17,1.01/255,13),field('rgb2','第二颜色',17,20,1.01/255,14),
  field('opacity2','第二透明度',20,21,1.01/255,15),
  {id:'screen-position',label:'提交几何中心（排除已记录的画面震动）',read:screenPosition,epsilon:.025},
  {id:'screen-shape',label:'提交几何形状',read:shape,epsilon:.025},
  {id:'submitted-uv',label:'提交UV',read:uv,epsilon:.00001,period:1},
  field('submitted-opacity','提交透明度',30,31,1.01/255),
];
function continuity(f,r) {
  return !!(f.bit && (r.meta[11] & (1<<f.bit))) || (['position','screen-position'].includes(f.id)&&motionOwners.has(r.meta[0]));
}
function classifyField(f,previous,current,samples) {
  const p=f.read(previous),c=f.read(current),observed=samples.map(s=>({alpha:s.alpha,value:f.read(s.record)}));
  const tolerance=f.epsilon,period=f.period||0,motion=distance(p,c,period);
  const evidence={previous:p,current:c,samples:observed,motion,tolerance,authoredContinuous:continuity(f,previous)||continuity(f,current)};
  if([p,c,...observed.map(s=>s.value)].some(a=>a.some(n=>!Number.isFinite(n))))return {...evidence,status:'nonfinite'};
  for(let i=0;i<observed.length;i++)for(let j=i+1;j<observed.length;j++){
    if(observed[i].alpha===observed[j].alpha&&distance(observed[i].value,observed[j].value,period)>tolerance)return {...evidence,status:'non-idempotent'};
  }
  const endpoint=observed.find(s=>s.alpha===1);
  if(endpoint&&distance(endpoint.value,c,period)>tolerance*2)return {...evidence,status:'endpoint-mismatch'};
  const variation=Math.max(0,...observed.map(s=>distance(s.value,observed[0].value,period)));
  if(motion<=tolerance*2)return {...evidence,status:variation>tolerance*2?'responsive':'static',variation};
  // epsilon sets meaningful endpoint motion, not proof of zero response.
  // A small but real subpixel change or one 8-bit alpha step is still a
  // response; do not confuse quantization with a missing interpolation path.
  const noise=Math.max(1e-6,tolerance*.02);
  if(variation<=noise){return {...evidence,variation,status:evidence.authoredContinuous?'missing-interpolation':'held-review'};}
  const d=delta(p,c,period);
  const residual=Math.max(...observed.map(s=>distance(s.value,p.map((x,i)=>x+d[i]*s.alpha),period)));
  return {...evidence,variation,residual,status:residual<=Math.max(tolerance*2,motion*.02)?'interpolated':'responsive'};
}
export const severity = defaultSeverity;
export function analyzeWindow({previous,current,samples,stateBefore=null,statesAfter=[],worldFrozen=false,gate=true,build={},label='',negativeControl=false}) {
  if(!previous||!current||!Array.isArray(samples)||samples.length<3)throw Error('Need two authoritative draws and at least three alpha samples');
  if(samples.some(s=>!Number.isFinite(s.alpha)||s.alpha<0||s.alpha>1))throw Error('Invalid alpha');
  const before=indexed(previous.records),now=indexed(current.records),sampleMaps=samples.map(s=>({...s,...indexed(s.records)}));
  const consecutive=previous.tick+1===current.tick;
  const state=compareStateEvidence(stateBefore,statesAfter);
  const report={schema:SCHEMA,build,label,negativeControl,tick:current.tick,previousTick:previous.tick,consecutive,
    gate:!!gate,worldFrozen,alphas:samples.map(s=>s.alpha),purityStatus:state.status,stateChanges:state.changes,objects:[],counts:{},coverage:{},
    sampledGeometry:true,dropped:(previous.dropped||0)+(current.dropped||0)+samples.reduce((n,s)=>n+(s.dropped||0),0),
    limitations:['screen geometry is not an occlusion/pixel visibility proof','unmarked owners and unvisited scenes are not counted as passed','state hashes cover named fields, not the complete simulation','geometry over 16 vertices is sampled; no interpolated GPU command replay','CPU-projected 3D UV/fog/color shader output is unobserved']};
  for(const [key,c] of now.map){
    const p=before.map.get(key),selected=sampleMaps.map(s=>({alpha:s.alpha,record:s.map.get(key)}));
    const bomb=c.meta[0]===16,rawPart=c.meta[2];
    const row={key,owner:OWNERS[c.meta[0]]||'未标记',ownerId:c.meta[0],object:c.meta[1],part:bomb?(rawPart&255):rawPart,lifecycleState:bomb?(rawPart>>>8):null,draw:c.meta[3],kind:c.meta[4],anm:c.meta[5]|0,script:c.meta[6]|0,sprite:c.meta[7]|0,age:c.meta[8]|0,
      bounds:c.values.slice(21,25),sampled:!!(c.meta[11]&2),projected:!!(c.meta[11]&8),fields:[],status:'static'};
    let skip='';
    if(!c.meta[0])skip='unscoped';
    else if(now.duplicates.has(key)||before.duplicates.has(key)||sampleMaps.some(s=>s.duplicates.has(key)))skip='ambiguous';
    else if(!consecutive||!p||selected.some(s=>!s.record))skip='unobserved';
    else if(p.meta[5]!==c.meta[5]||p.meta[6]!==c.meta[6]||(c.meta[8]|0)<(p.meta[8]|0))skip='lifecycle';
    else if(worldFrozen&&worldOwners.has(c.meta[0]))skip='frozen';
    else if(c.values[23]<0||c.values[21]>640||c.values[24]<0||c.values[22]>480)skip='offscreen';
    if(skip)row.status=skip;
    else {
      const spriteChanged=p.meta[7]!==c.meta[7]||p.meta[9]!==c.meta[9]||p.meta[10]!==c.meta[10]||
        (p.spriteIdentity!=null&&c.spriteIdentity!=null&&p.spriteIdentity!==c.spriteIdentity);
      const jump=distance(p.values.slice(0,2),c.values.slice(0,2))>=128;
      for(const f of fields){
        let value;
        if(jump&&['position','offset','screen-position'].includes(f.id))value={status:'lifecycle'};
        else if(spriteChanged&&(f.id.startsWith('screen-')||f.id.startsWith('submitted-')))value={status:'sprite-change'};
        else if(row.projected&&f.id.startsWith('submitted-'))value={status:'unobserved'};
        else value=classifyField(c.meta[4]===5&&f.id==='offset'?{...f,read:r=>r.values.slice(3,4)}:f,p,c,selected);
        // Some gameplay owners intentionally snap their spatial transform at
        // a lifecycle boundary while retaining the same render object.  The
        // owner marks that interval explicitly; do not infer it from output.
        if((c.meta[11]&(1<<22))&&['screen-position','screen-shape'].includes(f.id))
          value={...value,status:'projection-clip'};
        else if((c.meta[11]&(1<<21))&&['position','screen-position'].includes(f.id)&&
            !['nonfinite','non-idempotent','endpoint-mismatch'].includes(value.status))
          value={...value,status:'lifecycle'};
        // TH08's screen-space gameplay owners use VM.pos.z as an authored
        // draw-order layer (for example Enemy main=.25, trail/satellite=.3),
        // not as a continuously visible spatial coordinate. Do not promote a
        // Z-only layer transition to a high-refresh movement failure when XY
        // and the submitted screen geometry are unchanged. Background/world
        // owners are deliberately excluded so their real XYZ motion remains
        // observable.
        if(f.id==='position'&&motionOwners.has(c.meta[0])&&
            distance(p.values.slice(0,2),c.values.slice(0,2))<=f.epsilon*2&&
            Math.abs(p.values[2]-c.values[2])>f.epsilon*2&&
            !['nonfinite','non-idempotent','endpoint-mismatch'].includes(value.status))
          value={...value,status:'depth-change'};
        // EffectGeometry consumes center/radius/etc., not a sprite transform.
        // Kind 1 is the raw strip submission; kind 5 independently records the
        // actual geometry parameters. Keep the original values as evidence,
        // but do not demand interpolation of an unused raw ANM transform.
        if(c.meta[0]===7&&c.meta[4]===1&&['position','offset','scale','rotation'].includes(f.id)&&
            !['nonfinite','non-idempotent'].includes(value.status))value={...value,status:'upstream-parameter'};
        const channelKnown=!!(c.meta[11]&(1<<19)),secondary=!!(c.meta[11]&(1<<18));
        const unused=channelKnown&&(c.meta[4]===0||c.meta[4]===4)&&
          (secondary?['rgb','opacity'].includes(f.id):['rgb2','opacity2'].includes(f.id));
        if(unused&&!['nonfinite','non-idempotent'].includes(value.status))value={...value,status:'inactive-channel'};
        const geometryLabels={position:'特效几何中心',offset:'特效几何高度',scale:'特效半径 / 环宽',rotation:'特效几何角度'};
        row.fields.push({id:f.id,label:c.meta[4]===5?(geometryLabels[f.id]||f.label):f.label,...value});
      }
      // A property changing in the sampler cannot hide a stuck final submission.
      const at=id=>row.fields.find(f=>f.id===id);
      for(const [property,submission] of [['position','screen-position'],['scale','screen-shape'],['rotation','screen-shape'],['uv','submitted-uv'],['opacity','submitted-opacity']]){
        const a=at(property),b=at(submission);
        if(a?.variation>Math.max(a.tolerance*2,.0001)&&['held-review','missing-interpolation'].includes(b?.status))b.status='downstream-held';
      }
      row.status=row.fields.reduce((status,f)=>severity(f.status)>severity(status)?f.status:status,'static');
      if(row.status==='static'&&row.fields.some(f=>f.status==='interpolated'))row.status='interpolated';
    }
    row.severity=severity(row.status);report.objects.push(row);
    report.counts[row.status]=(report.counts[row.status]||0)+1;
    const group=report.coverage[row.owner]??={observed:0,changing:0,issues:0,unresolved:0};group.observed++;
    if(row.fields.some(f=>f.motion>f.tolerance*2))group.changing++;
    if(row.severity>=3)group.issues++;
    if(['unobserved','unscoped','ambiguous','lifecycle','offscreen'].includes(row.status))group.unresolved++;
  }
  report.disappeared=[...before.map].filter(([key])=>!now.map.has(key)).map(([key,r])=>({key,ownerId:r.meta[0],script:r.meta[6]|0,status:'lifecycle'}));
  report.objects.sort((a,b)=>b.severity-a.severity||a.ownerId-b.ownerId||a.key.localeCompare(b.key));
  report.purity=report.purityStatus==='pass';
  report.valid=consecutive&&report.dropped===0&&samples.every(s=>s.tick===undefined||s.tick===current.tick)&&samples.some(s=>s.alpha===0)&&samples.some(s=>s.alpha===1)&&samples.filter(s=>s.alpha===.5).length>=2;
  report.issueGroups=groupFindings([report]);
  return report;
}

// Merge particles and repeated windows without calling 50 instances 50 bugs.
// Identity keys remain in each original window for source/geometry inspection.
export const groupFindings=reports=>groupFindingsCore(reports,{severityOf:severity});
export {compactReport};
export const mergeIssueGroups=reports=>mergeIssueGroupsCore(reports,{group:groupFindings});
