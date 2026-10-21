import assert from 'node:assert/strict';

export const TOUCH_TRACE_SCHEMA='eagler-touhou/touch-motion-trace/1';
export const TOUCH_TRACE_POSITION_EPSILON=1/4096;
const finite=(value,label)=>assert.ok(Number.isFinite(value),`${label} must be finite`);
const close=(actual,expected,label,epsilon)=>assert.ok(Math.abs(actual-expected)<=epsilon,
  `${label}: expected ${expected}, received ${actual} (tolerance ${epsilon})`);

export function isTouchMotionTickEligible(sample){
  return sample?.abiVersion===2&&Number.isSafeInteger(sample.sequence)&&sample.sequence>0&&
    Number.isSafeInteger(sample.frame)&&sample.frame>=0&&sample.frame<=0xffffffff&&
    Number.isInteger(sample.localSeat)&&sample.localSeat>=0&&sample.localSeat<4&&
    sample.executionKind===0&&Number.isInteger(sample.flags)&&sample.flags>=0&&
    (sample.flags&~4)===1;
}

function strictForwardStream(samples,{localSeat=0,executionKind=0,label='Native touch trace'}={}){
  assert.ok(Array.isArray(samples)&&samples.length>0,`${label} samples are missing`);
  assert.ok(Number.isInteger(localSeat)&&localSeat>=0&&localSeat<4,'Expected local seat must be between 0 and 3');
  assert.equal(executionKind,0,'Strict touch evidence requires the forward execution stream');
  for(const [i,sample] of samples.entries()){
    assert.equal(sample?.abiVersion,2,`${label} requires ABI v2 rows`);
    assert.equal(sample?.localSeat,localSeat,`${label} changed local-seat stream`);
    assert.equal(sample?.executionKind,executionKind,`${label} changed execution stream`);
    assert.ok(isTouchMotionTickEligible(sample),`${label} row ${i} is not an eligible completed gameplay tick`);
    if(i){
      const previous=samples[i-1];
      assert.equal(sample.sequence,previous.sequence+1,`Missing native trace sequence in ${label}`);
      assert.equal(sample.frame,(previous.frame+1)>>>0,`Missing native movement tick in ${label}`);
    }
  }
}

/** Map a normalized pointer drag through th10's float32 TouchController input path. */
export function mapFloat32TouchDragTarget(before,requestedDelta,{inputSpace,sensitivity=1,focusFactor=1,
  pointerOrigin={x:.5,y:.5},pixelsPerUnit}={}){
  finite(before?.x,'touch start X');finite(before?.y,'touch start Y');
  finite(requestedDelta?.x,'requested pointer delta X');finite(requestedDelta?.y,'requested pointer delta Y');
  finite(sensitivity,'native touch sensitivity');finite(focusFactor,'native focus ratio');
  assert.ok(sensitivity>=1&&sensitivity<=3,'Native touch sensitivity must be between 100 and 300 percent');
  assert.ok(focusFactor>0,'Native focus ratio must be positive');
  assert.ok(inputSpace&&Number.isFinite(inputSpace.width)&&inputSpace.width>0&&
    Number.isFinite(inputSpace.height)&&inputSpace.height>0,'Touch input space must have positive dimensions');
  const extent={x:pixelsPerUnit?.x??inputSpace.width,y:pixelsPerUnit?.y??inputSpace.height};
  for(const axis of ['x','y']){
    finite(pointerOrigin?.[axis],`pointer origin ${axis.toUpperCase()}`);
    finite(extent[axis],`native ${axis.toUpperCase()} pixel extent`);
    assert.ok(extent[axis]>0,'Native pointer pixel extent must be positive');
  }
  const f32=Math.fround;
  const mapAxis=(axis)=>{
    const normalizedStart=f32(pointerOrigin[axis]);
    const normalizedEnd=f32(pointerOrigin[axis]+requestedDelta[axis]/inputSpace[axis==='x'?'width':'height']);
    const normalizedDelta=f32(normalizedEnd-normalizedStart);
    const pixelDelta=f32(normalizedDelta*f32(extent[axis]));
    const boundedDelta=f32(Math.max(-extent[axis],Math.min(extent[axis],pixelDelta)));
    const sensitivityDelta=f32(boundedDelta*f32(sensitivity));
    const focusedDelta=f32(sensitivityDelta*f32(focusFactor));
    return f32(f32(before[axis])+focusedDelta);
  };
  return{x:mapAxis('x'),y:mapAxis('y')};
}

function movementPrecision(value={}){
  const precision={unitsPerPixel:1,rounding:'float',scalePlacement:'before-limit',...value};
  assert.ok(Number.isInteger(precision.unitsPerPixel)&&precision.unitsPerPixel>0,'Position units must be a positive integer');
  assert.ok(['float','nearest','toward-zero'].includes(precision.rounding),'Unknown native movement rounding rule');
  assert.ok(precision.limitedQuantization===undefined||precision.limitedQuantization==='nearest-within-speed',
    'Unknown limited touch quantization rule');
  assert.ok(['before-limit','after-limit'].includes(precision.scalePlacement),'Unknown movement scale placement');
  return precision;
}

function nativeInteger(units,rounding){
  return rounding==='nearest'?Math.sign(units)*Math.floor(Math.abs(units)+.5):Math.trunc(units);
}

function nativeDelta(value,precision){
  return precision.rounding==='float'?value:nativeInteger(value*precision.unitsPerPixel,precision.rounding)/precision.unitsPerPixel;
}

// Independent specification: minimize distance to the requested native vector
// over integer points inside the speed disc. A feasible point obtained by
// truncating both coordinates is within sqrt(2), which bounds the search.
function nearestFeasiblePoint(x,y,radius){
  let best;
  const rank=(a,b)=>{
    for(let i=0;i<a.length;i++){if(a[i]<b[i])return -1;if(a[i]>b[i])return 1;}
    return 0;
  };
  for(let a=Math.ceil(x-Math.SQRT2);a<=Math.floor(x+Math.SQRT2);a++){
    for(let b=Math.ceil(y-Math.SQRT2);b<=Math.floor(y+Math.SQRT2);b++){
      const square=a*a+b*b;
      if(square>radius*radius)continue;
      const cross=a*y-b*x;
      const score=[(a-x)**2+(b-y)**2,square?cross*cross/square:x*x+y*y,-square,a,b];
      if(!best||rank(score,best.score)<0)best={x:a,y:b,score};
    }
  }
  assert.ok(best,'No integer touch vector satisfies the native speed budget');
  return best;
}

/** Model native units from the accepted title contract, without reading candidate movement code. */
export function predictTouchMotionStep(before,target,{mode,speed,scale={x:1,y:1},precision}={}){
  for(const axis of ['x','y']){finite(before?.[axis],`before ${axis.toUpperCase()}`);finite(target?.[axis],`target ${axis.toUpperCase()}`);}
  assert.ok(mode===1||mode===2,'Touch movement mode must be limited (1) or unlimited (2)');
  finite(speed,'expected original speed');assert.ok(speed>0,'Original speed must be positive');
  for(const axis of ['x','y']){finite(scale?.[axis],`expected ${axis.toUpperCase()} scale`);assert.ok(scale[axis]>0,'Movement scale must be positive');}
  const rule=movementPrecision(precision);
  const dx=(target.x-before.x)/(rule.scalePlacement==='before-limit'?scale.x:1);
  const dy=(target.y-before.y)/(rule.scalePlacement==='before-limit'?scale.y:1);
  const distance=Math.hypot(dx,dy),fraction=mode===1&&distance>speed?speed/distance:1;
  const integer=mode===1&&rule.limitedQuantization==='nearest-within-speed'?
    nearestFeasiblePoint(dx*fraction*rule.unitsPerPixel,dy*fraction*rule.unitsPerPixel,speed*rule.unitsPerPixel):{
      x:nativeInteger(dx*fraction*rule.unitsPerPixel,rule.rounding),
      y:nativeInteger(dy*fraction*rule.unitsPerPixel,rule.rounding),
    };
  const step=rule.rounding==='float'?{x:dx*fraction*scale.x,y:dy*fraction*scale.y}:{
    x:Math.trunc(integer.x*scale.x)/rule.unitsPerPixel,
    y:Math.trunc(integer.y*scale.y)/rule.unitsPerPixel,
  };
  return{x:before.x+step.x,y:before.y+step.y};
}

export function isTouchMotionTargetReached(position,target,{precision,epsilon=TOUCH_TRACE_POSITION_EPSILON}={}){
  for(const axis of ['x','y']){finite(position?.[axis],`position ${axis.toUpperCase()}`);finite(target?.[axis],`target ${axis.toUpperCase()}`);}
  finite(epsilon,'target epsilon');assert.ok(epsilon>=0,'Target epsilon must be non-negative');
  const rule=movementPrecision(precision);
  return Math.abs(nativeDelta(target.x-position.x,rule))<=epsilon&&
    Math.abs(nativeDelta(target.y-position.y,rule))<=epsilon;
}

/** Read-only, self-contained browser boundary; no sampling timer can discard native ticks. */
export function readTouchMotionTrace({game,adapter,localSeat=0,executionKind=0}){
  if(!Number.isInteger(localSeat)||localSeat<0||localSeat>=4||!Number.isInteger(executionKind)||executionKind<0||executionKind>=4)
    throw Error('Invalid touch trace stream selection');
  const root=globalThis,mapping=adapter?.runtime,runtime=root[mapping?.global??`__${game}Runtime`];
  if(mapping&&!runtime)return{available:false,reason:'The declared touch trace Runtime mount is unavailable'};
  const module=mapping?runtime[mapping.module]:runtime?.Module??root.Module;
  const core=mapping?runtime[mapping.core]??module:runtime?.core??root.core??module;
  let read,version;
  for(const candidate of [2,1]){
    for(const owner of [core,module]){
      for(const name of [`${game}_touch_motion_trace_v${candidate}`,`sdl_touch_trace_v${candidate}`]){
        const fn=owner?.[name]??owner?.[`_${name}`];
        if(typeof fn==='function'){read=fn.bind(owner);version=candidate;break;}
      }
      if(read)break;
    }
    if(read)break;
  }
  if(!read)return{available:false,reason:'The package has no completed-native-tick touch trace export'};
  if(version===1&&(localSeat!==0||executionKind!==0))
    return{available:false,reason:'The legacy touch trace does not identify the selected seat and execution stream'};
  const pointer=version===2?read(localSeat,executionKind):read(),buffer=core?.memory?.buffer??module?.HEAPU8?.buffer??(mapping?null:root.HEAPU8?.buffer);
  if(!buffer||!Number.isInteger(pointer)||pointer<=0||pointer%4||pointer+32>buffer.byteLength)
    throw Error('Invalid touch trace pointer');
  const prefix=Array.from(new Uint32Array(buffer,pointer,8));
  if(prefix[0]!==version||prefix[1]!==8||prefix[2]!==16||prefix[3]!==512||prefix[5]!==Math.min(prefix[4],512)||
    prefix[6]!==localSeat||prefix[7]!==executionKind)
    throw Error('Invalid touch trace ABI header');
  const words=8+prefix[5]*16;
  if(pointer+words*4>buffer.byteLength)throw Error('Touch trace exceeds native memory');
  return{available:true,raw:Array.from(new Uint32Array(buffer,pointer,words))};
}

export function decodeTouchMotionTrace(raw){
  assert.ok(Array.isArray(raw)&&raw.length>=8,'Touch trace header is missing');
  const version=raw[0];
  assert.ok(version===1||version===2,'Touch trace ABI version differs');
  assert.deepEqual(raw.slice(1,4),[8,16,512],'Touch trace ABI layout differs');
  const localSeat=raw[6],executionKind=raw[7];
  if(version===1)assert.deepEqual(raw.slice(6,8),[0,0],'Touch trace reserved header fields differ');
  else assert.ok(localSeat<4&&executionKind<4,'Invalid touch trace stream identity');
  assert.ok(raw.every(value=>Number.isInteger(value)&&value>=0&&value<=0xffffffff),'Touch trace words must be u32');
  const [total,count]=raw.slice(4,6);
  assert.ok(Number.isSafeInteger(total)&&total>=0&&count===Math.min(total,512),'Touch trace count differs');
  assert.equal(raw.length,8+count*16,'Touch trace record inventory differs');
  const samples=[];
  for(let i=0;i<count;i++){
    const row=raw.slice(8+i*16,8+(i+1)*16),bytes=new Uint32Array(row),floats=new Float32Array(bytes.buffer);
    assert.equal(row[0],total-count+i+1,'Touch trace sequence is missing, duplicated or reordered');
    assert.equal(row[14],localSeat,'Touch trace record belongs to another seat');
    assert.equal(row[15],executionKind,'Touch trace record belongs to another execution stream');
    const names=['beforeX','beforeY','targetX','targetY','afterX','afterY','speed','scaleX','scaleY','sensitivity'];
    const sample={sequence:row[0],frame:row[1],mode:row[2],flags:row[3],abiVersion:version,localSeat,executionKind};
    for(const [j,name] of names.entries()){sample[name]=floats[j+4];finite(sample[name],name);}
    samples.push(sample);
  }
  return{schema:TOUCH_TRACE_SCHEMA,abiVersion:version,localSeat,executionKind,total,samples};
}

/** Decode one reader result while preserving explicit capability state. Malformed ABI remains an error. */
export function decodeTouchMotionTraceResult(result){
  if(result?.available===false){
    return{available:false,reason:typeof result.reason==='string'&&result.reason.trim()?result.reason:'Native touch trace is unavailable'};
  }
  assert.equal(result?.available,true,'Touch trace reader must explicitly declare availability');
  return{available:true,...decodeTouchMotionTrace(Array.from(result.raw))};
}

/** Assemble a stable trace/input boundary from the same reader and adapter used by the Runtime probe. */
export async function readTouchMotionBoundary({readTrace,readObservation,policy=null}={}){
  assert.equal(typeof readTrace,'function','Touch boundary requires a native trace reader');
  const first=decodeTouchMotionTraceResult(await readTrace());
  if(!first.available)return{trace:first,stableTraceBoundary:false,nextMovementInputBoundary:null};
  if(!policy)return{trace:first,stableTraceBoundary:true,nextMovementInputBoundary:null};

  const observation=typeof readObservation==='function'?await readObservation():null;
  const final=decodeTouchMotionTraceResult(await readTrace());
  if(!final.available)return{trace:final,stableTraceBoundary:false,nextMovementInputBoundary:null};
  const stableTraceBoundary=first.total===final.total&&first.abiVersion===final.abiVersion&&
    first.localSeat===final.localSeat&&first.executionKind===final.executionKind;
  assert.equal(policy.semantics,'queued-for-next-movement','Touch boundary policy must declare queued movement semantics');
  assert.ok(typeof policy.observationSource==='string'&&policy.observationSource.trim(),
    'Touch boundary policy must declare its native input source');
  assert.ok(typeof policy.observationPhase==='string'&&policy.observationPhase.trim(),
    'Touch boundary policy must declare its native sample boundary');
  const input=observation?.input;
  const inputSemantics=input?.inputSemantics,source=input?.source,sampleBoundary=input?.sampleBoundary;
  const inputMatches=input?.kind==='native-queued-for-next-movement-buttons'&&
    inputSemantics===policy.semantics&&source===policy.observationSource&&
    sampleBoundary===policy.observationPhase&&Number.isInteger(input.value);
  const observedInput={kind:input?.kind??null,inputSemantics:inputSemantics??null,
    source:source??null,sampleBoundary:sampleBoundary??null};
  const nextMovementInputBoundary=inputMatches?{
      available:true,
      semantics:policy.semantics,
      observationSource:source,
      observationPhase:sampleBoundary,
      inputSemantics,source,sampleBoundary,
      value:input.value,
      traceTotal:final.total,
    }:{available:false,reason:typeof readObservation==='function'?
      'Native queued-movement observation does not match its declared kind, semantics, source, or sample boundary':
      'Native queued-movement observation reader is unavailable',observedInput,traceTotal:final.total};
  return{trace:final,stableTraceBoundary,nextMovementInputBoundary};
}

/** Independent mathematical oracle; expected targets and speed come from the scenario, not the limiter. */
export function assertTouchMotionSamples(samples,{mode,target,targets,speed,sensitivity=1,scale={x:1,y:1},precision,focused,
  localSeat=0,executionKind=0,requireArrival=true,epsilon=TOUCH_TRACE_POSITION_EPSILON}={}){
  assert.ok(Array.isArray(samples)&&samples.length>=3,'Touch movement needs at least three consecutive native ticks');
  strictForwardStream(samples,{localSeat,executionKind,label:'Touch movement trace'});
  assert.ok(mode===1||mode===2,'Touch movement mode must be explicit');
  assert.ok(!(target!==undefined&&targets!==undefined),'Provide one independent target source, not both target and targets');
  const expectedTargets=targets??samples.map(()=>target);
  assert.ok(Array.isArray(expectedTargets)&&expectedTargets.length===samples.length,'Each native tick needs an independent target');
  for(const value of expectedTargets){finite(value?.x,'expected target X');finite(value?.y,'expected target Y');}
  finite(speed,'expected original speed');assert.ok(speed>0,'Original speed must be positive');
  finite(sensitivity,'expected sensitivity');assert.ok(sensitivity>=1&&sensitivity<=3,'Sensitivity must be between 100 and 300 percent');
  for(const axis of ['x','y']){finite(scale[axis],`expected ${axis} scale`);assert.ok(scale[axis]>0,'Movement scale must be positive');}
  let saturated=0,subLimit=0,stationary=0;
  for(const [i,s] of samples.entries()){
    const target=expectedTargets[i];
    for(const field of ['beforeX','beforeY','targetX','targetY','afterX','afterY','speed','scaleX','scaleY','sensitivity'])finite(s[field],field);
    const resting=s.mode===0&&isTouchMotionTargetReached({x:s.beforeX,y:s.beforeY},target,{precision,epsilon});
    assert.ok(s.mode===mode||resting,'Configured limited/unlimited mode was not consumed');
    assert.ok(isTouchMotionTickEligible(s),'Native movement eligibility changed during the scenario');
    if(s.abiVersion===2&&focused!==undefined)assert.equal(Boolean(s.flags&4),focused,'Native focus state differs');
    close(s.speed,speed,'Original player speed',epsilon);close(s.sensitivity,sensitivity,'Consumed sensitivity',epsilon);
    close(s.scaleX,scale.x,'Native X movement scale',epsilon);close(s.scaleY,scale.y,'Native Y movement scale',epsilon);
    close(s.targetX,resting?s.beforeX:target.x,'Mapped touch target X',epsilon);
    close(s.targetY,resting?s.beforeY:target.y,'Mapped touch target Y',epsilon);
    if(i){const p=samples[i-1];assert.equal(s.sequence,p.sequence+1,'Missing native trace sequence');
      assert.equal(s.frame,p.frame+1,'Missing native movement tick');
      close(s.beforeX,p.afterX,'Native position continuity X',epsilon);close(s.beforeY,p.afterY,'Native position continuity Y',epsilon);}
    const rule=movementPrecision(precision);
    const dx=(target.x-s.beforeX)/(rule.scalePlacement==='before-limit'?scale.x:1);
    const dy=(target.y-s.beforeY)/(rule.scalePlacement==='before-limit'?scale.y:1),distance=Math.hypot(dx,dy);
    const expected=resting?{x:s.beforeX,y:s.beforeY}:
      predictTouchMotionStep({x:s.beforeX,y:s.beforeY},target,{mode,speed,scale,precision:rule});
    close(s.afterX,expected.x,'Per-tick touch position X',epsilon);close(s.afterY,expected.y,'Per-tick touch position Y',epsilon);
    const length=Math.hypot((s.afterX-s.beforeX)/scale.x,(s.afterY-s.beforeY)/scale.y);
    if(mode===1)assert.ok(length<=speed+epsilon,'Touch movement exceeded original Euclidean speed cap');
    if(distance>speed+epsilon)saturated++;else if(distance>epsilon)subLimit++;else stationary++;
  }
  const last=samples.at(-1),finalTarget=expectedTargets.at(-1);
  const arrived=isTouchMotionTargetReached({x:last.afterX,y:last.afterY},finalTarget,{precision,epsilon});
  if(requireArrival)assert.ok(arrived,'Final target differs at native movement precision');
  return{ticks:samples.length,saturated,subLimit,stationary,arrived};
}

export function assertTouchReleaseSamples(samples,{position,afterSequence,minimumTicks=3,localSeat=0,executionKind=0,
  epsilon=TOUCH_TRACE_POSITION_EPSILON}={}){
  strictForwardStream(samples,{localSeat,executionKind,label:'Touch release trace'});
  finite(position?.x,'release position X');finite(position?.y,'release position Y');
  assert.ok(Number.isSafeInteger(afterSequence)&&afterSequence>=0,'Release boundary must be an explicit native sequence');
  assert.ok(Number.isSafeInteger(minimumTicks)&&minimumTicks>=3,'Release proof needs at least three native ticks');
  finite(epsilon,'release epsilon');assert.ok(epsilon>=0,'Release epsilon must be non-negative');
  const released=samples.filter(s=>s.sequence>afterSequence);
  assert.ok(released.length>=minimumTicks,'Release needs consecutive native ticks after the command');
  assert.equal(released[0].sequence,afterSequence+1,'Release trace omitted the first native tick after the command');
  for(const [i,s] of released.entries()){
    assert.equal(s.mode,0,'Touch movement remains active after release');
    for(const [field,value] of [['targetX',position.x],['beforeX',position.x],['afterX',position.x],
      ['targetY',position.y],['beforeY',position.y],['afterY',position.y]])
      close(s[field],value,`Position drift after release (${field})`,epsilon);
  }
  return{ticks:released.length,stationary:true};
}
