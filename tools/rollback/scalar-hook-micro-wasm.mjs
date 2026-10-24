// Binary identity audit for the test-only scalar hook microbenchmark.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
export const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const equal=(a,b)=>Buffer.from(a).equals(Buffer.from(b));
const uleb=(data,state)=>{
  let value=0,shift=0;
  for(;;){assert(state.p<data.length,'truncated LEB');const v=data[state.p++];
    value+=(v&127)*2**shift;if(!(v&128))return value;shift+=7;assert(shift<=49,'oversized LEB');}
};
const encode=value=>{
  const out=[];do{const b=value%128;value=Math.floor(value/128);out.push(b|(value?128:0));}while(value);return Buffer.from(out);
};
const name=(data,state)=>{const size=uleb(data,state),start=state.p;state.p+=size;assert(state.p<=data.length);return Buffer.from(data.subarray(start,state.p)).toString('utf8');};
const limits=(data,state)=>{const flags=uleb(data,state);assert.equal(flags&~7,0);uleb(data,state);if(flags&1)uleb(data,state);};
const valueType=(data,state)=>{const value=data[state.p++];assert([0x7f,0x7e,0x7d,0x7c,0x7b,0x70,0x6f].includes(value),'unsupported reference/value type');return value;};

export function parseModule(input){
  const data=Buffer.from(input);
  assert(equal(data.subarray(0,8),Buffer.from([0,97,115,109,1,0,0,0])),'Wasm v1 required');
  const sections=[],types=[],imports=[],functionTypes=[],bodies=[],exports=[];
  const cursor={p:8};
  while(cursor.p<data.length){
    const sectionStart=cursor.p,id=data[cursor.p++],size=uleb(data,cursor),start=cursor.p,end=start+size;
    assert(end<=data.length,'truncated section');const s={p:start};
    sections.push({id,sectionStart,start,end,bytes:data.subarray(sectionStart,end)});
    if(id===1){
      const count=uleb(data,s);for(let i=0;i<count;i++){
        assert.equal(data[s.p++],0x60,'function type required');
        const params=Array.from({length:uleb(data,s)},()=>valueType(data,s));
        const results=Array.from({length:uleb(data,s)},()=>valueType(data,s));
        types.push({params,results});
      }assert.equal(s.p,end);
    }else if(id===2){
      const count=uleb(data,s);for(let i=0;i<count;i++){
        const module=name(data,s),field=name(data,s),kind=data[s.p++];
        const item={module,field,kind};
        if(kind===0){item.type=uleb(data,s);functionTypes.push(item.type);}
        else if(kind===1){valueType(data,s);limits(data,s);}
        else if(kind===2)limits(data,s);
        else if(kind===3){valueType(data,s);s.p++;}
        else if(kind===4){uleb(data,s);uleb(data,s);}
        else assert.fail('unsupported import kind');
        imports.push(item);
      }assert.equal(s.p,end);
    }else if(id===3){
      const count=uleb(data,s);for(let i=0;i<count;i++)functionTypes.push(uleb(data,s));
      assert.equal(s.p,end);
    }else if(id===7){
      const count=uleb(data,s);for(let i=0;i<count;i++)exports.push({name:name(data,s),kind:data[s.p++],index:uleb(data,s)});
      assert.equal(s.p,end);
    }else if(id===10){
      const count=uleb(data,s);for(let i=0;i<count;i++){
        const length=uleb(data,s),offset=s.p;s.p+=length;assert(s.p<=end);
        bodies.push({offset,end:s.p,bytes:data.subarray(offset,s.p)});
      }assert.equal(s.p,end);
    }
    cursor.p=end;
  }
  const importedFunctions=imports.filter(x=>x.kind===0).length;
  assert.equal(bodies.length,functionTypes.length-importedFunctions);
  bodies.forEach((body,i)=>{body.index=i+importedFunctions;body.sha256=digest(body.bytes);});
  const signatures=functionTypes.map(i=>{assert(types[i]);return types[i];});
  const byExport=new Map(exports.filter(x=>x.kind===0).map(x=>[x.name,x.index]));
  const byIndex=new Map(bodies.map(x=>[x.index,x]));
  return {data,sections,types,imports,signatures,importedFunctions,bodies,exports,byExport,byIndex};
}

function watFunctions(text){
  const functions=new Map(),exports=new Map();
  for(const m of text.matchAll(/\(export "([^"]+)" \(func (\$[^\s()]+)\)\)/g))exports.set(m[1],m[2]);
  for(const m of text.matchAll(/^[ \t]*\(func (\$[^\s()]+)/gm)){
    const start=text.indexOf('(',m.index);let depth=0,quoted=false,escape=false,end=start;
    for(;end<text.length;end++){
      const c=text[end];
      if(quoted){if(escape)escape=false;else if(c==='\\')escape=true;else if(c==='"')quoted=false;continue;}
      if(c==='"'){quoted=true;continue;}
      if(c==='(')depth++;else if(c===')'&&!--depth){end++;break;}
    }
    assert(depth===0&&!quoted,'unclosed function text');functions.set(m[1],text.slice(start,end));
  }
  return {functions,exports};
}
const directCalls=body=>[...body.matchAll(/\((?:call|return_call)\s+(\$[^\s()]+)/g)].map(x=>x[1]);
const stores=body=>{
  const result={};for(const m of body.matchAll(/\((i32|i64|f32|f64|v128)\.(store(?:8|16|32)?)\b/g)){
    const key=m[1]+'.'+m[2];result[key]=(result[key]||0)+1;
  }return result;
};
const rejectIndirect=body=>assert(!/\b(?:call_indirect|return_call_indirect|call_ref|return_call_ref|ref\.func)\b/.test(body),'indirect/reference call in micro hot body');
function transparentForwardTarget(body){
  const state={p:0};
  try{
    // No locals, side effects or argument transformation: get 0; get 1;
    // call target; end. wasm-opt may redirect callers past this exact thunk.
    if(uleb(body,state)!==0||body[state.p++]!==0x20||uleb(body,state)!==0||
       body[state.p++]!==0x20||uleb(body,state)!==1||body[state.p++]!==0x10)return null;
    const target=uleb(body,state);
    return body[state.p++]===0x0b&&state.p===body.length?target:null;
  }catch{return null;}
}

export function patchAndAudit({raw,instrumented,symbolText,binaryen,configAddress}){
  const a=parseModule(raw),b=parseModule(instrumented);
  assert.deepEqual(a.imports,b.imports,'instrumentation changed imports');
  assert.deepEqual(a.exports,b.exports,'instrumentation changed export indices');
  assert.deepEqual(a.signatures,b.signatures,'instrumentation changed function-index signatures');
  assert.equal(a.bodies.length,b.bodies.length);
  const widths=[1,2,4,8,16,'mixed'],variants=['plain','generic','scalar'];
  const names=variants.flatMap(v=>widths.map(w=>'micro_loop_'+v+'_'+w));
  const indices=new Set(names.map(n=>{assert(a.byExport.has(n),'missing '+n);return a.byExport.get(n);}));
  assert.equal(indices.size,names.length,'timing-loop aliases are not accepted');
  for(const index of indices)assert.deepEqual(a.signatures[index],{params:[127,127,127,127],results:[127]});
  const replacementBodies=b.bodies.map(body=>indices.has(body.index)?a.byIndex.get(body.index).bytes:body.bytes);
  const payload=Buffer.concat([encode(replacementBodies.length),...replacementBodies.flatMap(body=>[encode(body.length),body])]);
  const code=Buffer.concat([Buffer.from([10]),encode(payload.length),payload]);
  assert.equal(b.sections.filter(x=>x.id===10).length,1);
  const final=Buffer.concat([b.data.subarray(0,8),...b.sections.map(s=>s.id===10?code:s.bytes)]);
  assert(WebAssembly.validate(final),'final test-only module is not valid Wasm');
  const c=parseModule(final);
  assert.deepEqual(b.exports,c.exports);assert.deepEqual(b.signatures,c.signatures);
  assert.deepEqual(b.sections.filter(x=>x.id!==10).map(x=>digest(x.bytes)),
                   c.sections.filter(x=>x.id!==10).map(x=>digest(x.bytes)),'non-code section changed');
  const bodyAudit=b.bodies.map(body=>{
    const before=a.byIndex.get(body.index),after=c.byIndex.get(body.index),selected=indices.has(body.index);
    assert(equal(after.bytes,selected?before.bytes:body.bytes),'body whitelist violation');
    if(selected)assert(!equal(before.bytes,body.bytes),'timing loop had no removed automatic instrumentation');
    return {index:body.index,rawSha256:before.sha256,instrumentedSha256:body.sha256,finalSha256:after.sha256,
      restoredRawTimingLoop:selected,rawBytes:before.bytes.length,instrumentedBytes:body.bytes.length,finalBytes:after.bytes.length};
  });
  const symbols=new Map(symbolText.trim().split('\n').map(line=>{
    const separator=line.indexOf(':');assert(separator>0);return [Number(line.slice(0,separator)),line.slice(separator+1)];
  }));
  assert.equal(symbols.size,a.signatures.length,'symbol map does not cover every function index');
  assert(![...symbols.values()].some(n=>n.includes('TouchStore')),'out-of-line TouchStore survived compilation');
  const module=binaryen.readBinary(final);
  let text;try{text=module.emitText();}finally{module.dispose();}
  const view=watFunctions(text),generic=view.exports.get('eagler_rb_touch'),capture=view.exports.get('eagler_rb_capture');
  assert(generic&&capture,'missing journal hooks');
  assert(Number.isInteger(configAddress)&&configAddress>0&&configAddress%4===0);
  const captureIndex=c.byExport.get('eagler_rb_capture'),captureBody=c.byIndex.get(captureIndex);
  const forwarded=transparentForwardTarget(captureBody.bytes),coldTargets=new Set([capture]);
  let captureForwardingThunk=null;
  if(forwarded!==null){
    assert(c.byIndex.has(forwarded),'capture must forward to a defined function');
    assert.deepEqual(c.signatures[forwarded],{params:[127,127],results:[]});
    assert.equal(symbols.get(forwarded),'Netplay::WriteJournal::Touch(void*, unsigned long)');
    assert(equal(a.byIndex.get(captureIndex).bytes,captureBody.bytes),'capture thunk changed during instrumentation');
    const calls=directCalls(view.functions.get(capture));assert.equal(calls.length,1);
    assert(view.functions.has(calls[0]),'missing capture forwarding target');coldTargets.add(calls[0]);
    captureForwardingThunk={exportIndex:captureIndex,targetIndex:forwarded,targetSymbol:symbols.get(forwarded),
      targetLabel:calls[0],exactForwardingBodySha256:captureBody.sha256,bodyBytes:captureBody.bytes.length,
      targetInstrumentedBodySha256:c.byIndex.get(forwarded).sha256,wat:view.functions.get(capture)};
  }
  const scalarTargets=new Set([1,2,4,8,16].map(w=>view.exports.get('eagler_rb_touch'+w)));
  assert.equal(scalarTargets.size,5);assert(!scalarTargets.has(undefined));
  const hookAudit=[];
  for(const name of ['eagler_rb_touch',...[1,2,4,8,16].map(w=>'eagler_rb_touch'+w)]){
    const body=view.functions.get(view.exports.get(name));assert(body,'missing hook body '+name);
    rejectIndirect(body);const calls=directCalls(body);
    assert(calls.length>0&&calls.every(x=>coldTargets.has(x)),'hook contains a helper/generic/non-capture call: '+name);
    const normalized=body.replace(/\s+/g,' ').replace(/\s+\)/g,')');
    for(const callee of new Set(calls)){
      const prefix='(i32.store (i32.const '+configAddress+') (i32.const 0)) (call '+callee+' ';
      assert.equal(normalized.split(prefix).length-1,calls.filter(x=>x===callee).length,
        'every cold call must immediately follow tracking disable: '+name);
    }
    const index=c.byExport.get(name);
    hookAudit.push({export:name,index,symbol:symbols.get(index),calls,onlyColdCaptureCallee:true,
      bodySha256:c.byIndex.get(index).sha256,bodyBytes:c.byIndex.get(index).bytes.length,wat:body});
  }
  const loopAudit=[];
  for(const variant of variants)for(const width of widths){
    const name='micro_loop_'+variant+'_'+width,body=view.functions.get(view.exports.get(name));
    assert(body&&/\(loop\b/.test(body),'timing loop missing '+name);
    rejectIndirect(body);
    assert(!/\bglobal\.(?:get|set)\b/.test(body),'raw timing loop references mutable globals');
    assert(!/\bmemory\.(?:copy|fill|init|grow)\b/.test(body),'timing loop uses bulk/growth helper');
    const calls=directCalls(body),allowed=variant==='plain'?new Set():variant==='generic'?new Set([generic]):
      width==='mixed'?scalarTargets:new Set([view.exports.get('eagler_rb_touch'+width)]);
    assert(calls.every(x=>allowed.has(x)),'unexpected timed-loop callee in '+name);
    if(variant==='plain')assert.equal(calls.length,0);
    else if(width!=='mixed')assert.equal(calls.length,1,'fixed-width loop must contain one direct hook call site');
    else assert(calls.length>0);
    if(variant==='scalar'&&width==='mixed')assert.deepEqual(new Set(calls),scalarTargets);
    const actualStores=stores(body);
    if(width!=='mixed'){
      const expected=width===1?{'i32.store8':1}:width===2?{'i32.store16':1}:width===4?{'i32.store':1}:
                     width===8?{'i64.store':1}:{'i64.store':2};
      assert.deepEqual(actualStores,expected,'unexpected volatile store shape in '+name);
    }else assert(Object.keys(actualStores).length>0);
    const index=c.byExport.get(name);
    loopAudit.push({export:name,index,variant,width,calls,stores:actualStores,bodySha256:c.byIndex.get(index).sha256,
      bodyBytes:c.byIndex.get(index).bytes.length,wat:body});
  }
  return {bytes:final,report:{schema:'eagler-hook-micro-body-audit/1',status:'PASS',
    testOnly:true,productionModule:false,backendProfile:'native generic hook',
    rawSha256:digest(raw),instrumentedSha256:digest(instrumented),finalSha256:digest(final),
    symbolMapSha256:digest(symbolText),functionCount:a.signatures.length,importedFunctions:a.importedFunctions,
    rawTimingLoopExports:names,exactReplacementCount:indices.size,bodyAudit,captureForwardingThunk,hookAudit,loopAudit,
    guarantees:['All function-index signatures and exports match raw/instrumented/final.',
      'Only the named timing-loop code bodies differ from the production-instrumented module.',
      'Every replaced timing-loop body is byte-identical to its linked raw body.',
      'Every scalar hook only calls capture or its byte-proven transparent forwarding target, immediately after disabling tracking; no TouchStore helper survives.',
      'Fixed-width loops contain one direct target-hook call and the expected volatile stores.',
      'No indirect calls, mutable-global references or bulk/growth operations occur in timing loops.']}};
}
