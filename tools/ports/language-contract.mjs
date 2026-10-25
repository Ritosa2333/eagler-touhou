import assert from 'node:assert/strict';

export function languageBytes(hex){
  assert.ok(typeof hex==='string'&&/^(?:[a-f0-9]{2})*$/i.test(hex),'Language observation must preserve complete byte pairs');
  return Buffer.from(hex,'hex');
}

function successfulPresent(event){
  return event.phase==='runtime'&&Number.isSafeInteger(event.first)&&Number.isSafeInteger(event.last)&&event.last>=event.first&&
    Number.isSafeInteger(event.count)&&event.count>0&&Number.isSafeInteger(event.a)&&event.a>0&&
    Number.isSafeInteger(event.b)&&event.b>=event.a;
}

function hasSubsequentSuccessfulPresent(raster,presents){
  return presents.some(event=>successfulPresent(event)&&event.last>raster.last&&
    (event.scene===raster.scene||event.first===raster.last+1));
}

/** Keep selected checks and their independent Japanese baseline in one preparation. */
export function planLanguageScenes({checks,languages}){
  const known=['LANG-01','LANG-02','LANG-03','LANG-04','LANG-05','LANG-06'],selected=new Set(checks??known);
  assert.ok(selected.size>0&&[...selected].every(id=>known.includes(id)),'Select declared language checks');
  assert.ok(Array.isArray(languages)&&languages.length>0&&new Set(languages).size===languages.length);
  const scenes=[{id:'japanese-offline',language:'ja',offline:true}];
  if(['LANG-02','LANG-03','LANG-05'].some(id=>selected.has(id)))scenes.push(...languages.map(language=>({id:`translated-${language}`,language})));
  if(selected.has('LANG-06'))scenes.push({id:'partial-key',language:'lang_acceptance-partial'});
  if(selected.has('LANG-04'))scenes.push(...['network-failure','corrupt-archive'].map(failure=>({id:failure,language:languages[0],failure})));
  return{checks:[...selected],consumers:selected.has('LANG-01')||selected.has('LANG-05'),scenes};
}

function rasterGlyphProof({raster,presents,glyphs,after=0,label}){
  if(raster.textTruncated)return{status:'incomplete',reason:`Target text raster was truncated: ${label}`};
  if(!hasSubsequentSuccessfulPresent(raster,presents))return{status:'incomplete',reason:`Rasterized text has no subsequent successful native present in its draw boundary: ${label}`};
  assert.ok(['utf8','raw','cp932'].includes(raster.encoding),'Unknown native text encoding');
  const text=new TextDecoder(raster.encoding==='utf8'?'utf-8':raster.encoding==='raw'?'windows-1252':'shift_jis',{fatal:true}).decode(languageBytes(raster.textHex));
  const codepoints=[...new Set([...text].map(character=>character.codePointAt(0)).filter(code=>code>=32))],coverage=[];
  for(const code of codepoints){
    const used=glyphs.filter(event=>event.scene===raster.scene&&event.a===raster.a&&event.b===raster.b&&
      event.c===code&&event.first<=raster.last&&event.last>=after);
    if(!used.length)return{status:'incomplete',reason:`No actual glyph consumption for U+${code.toString(16)} in ${label}`};
    for(const glyph of used)assert.ok((glyph.flags&33)===33&&!(glyph.flags&2)&&glyph.d===code&&
      ['bitmap','msgothic','pack','unifont','ascii-atlas'].includes(glyph.source),`Missing or substituted glyph U+${code.toString(16)} in ${label}`);
    coverage.push({code,sources:[...new Set(used.map(event=>event.source))]});
  }
  return{status:'observed',text,coverage};
}

/** Loaded-script text reaches the same glyph and presentation checks without a synthetic localization lookup. */
export function assessMessageTextRasters({trace,expected,scene,after=0}){
  assert.equal(trace.schema,'eagler-touhou/language-trace/1');assert.equal(trace.enabled,true);assert.equal(trace.droppedEvents,0);
  const presents=trace.events.filter(event=>event.kind==='present'&&event.phase==='runtime'),glyphs=trace.events.filter(event=>event.kind==='glyph'&&event.phase==='runtime');
  const rows=[];
  for(const source of expected.rows){
    const raster=trace.events.find(event=>event.kind==='raster'&&event.phase==='runtime'&&event.scene===scene&&event.last>after&&event.textHex===source.textHex);
    if(!raster)continue;
    const proof=rasterGlyphProof({raster,presents,glyphs,after,label:`MSG ${expected.entry}:${source.offset}`});
    if(proof.status==='observed')rows.push({source,...proof,scene,rasterFirst:raster.first});
  }
  return rows.length?{status:'observed',rows}:{status:'incomplete',reason:'No source-bound original MessageProgram text reached glyphs and presentation',rows};
}

/** Compare naturally consumed native values with an independently parsed pack or cold Japanese baseline. */
export function assessLanguageScene({trace,profile,expected,baseline,original=false}){
  assert.equal(trace?.schema,'eagler-touhou/language-trace/1','Language trace is unavailable');
  assert.equal(trace.enabled,true,'Language observation was not enabled before launch');
  assert.ok(Array.isArray(trace.events),'Language trace events are absent');
  for(const field of ['eventCapacity','uniqueEvents','totalObservations','droppedEvents'])
    assert.ok(Number.isSafeInteger(trace[field])&&trace[field]>=0,`Invalid language trace ${field}`);
  if(trace.droppedEvents)return{status:'incomplete',reason:'Native language observation overflowed',trace};
  const lookups=trace.events.filter(event=>event.kind==='lookup'&&event.phase==='runtime');
  const rasters=trace.events.filter(event=>event.kind==='raster'&&event.phase==='runtime');
  const presents=trace.events.filter(event=>event.kind==='present'&&event.phase==='runtime');
  const glyphs=trace.events.filter(event=>event.kind==='glyph'&&event.phase==='runtime');
  const rows=[];
  for(const sample of profile.samples){
    const matching=lookups.filter(event=>event.operation===sample.operation&&languageBytes(event.textHex).toString('utf8')===sample.key);
    if(!matching.length)return{status:'incomplete',reason:`The game did not consume the declared text ${sample.key}`,rows,trace};
    const translation=!original&&expected?.entries.find(entry=>entry.key===sample.key&&entry.hasTranslation);
    const expectedHex=translation?Buffer.from(translation.value,'utf8').toString('hex'):baseline?.[sample.key];
    const actual=matching[0];
    if(matching.some(event=>event.textTruncated||event.valueTruncated))
      return{status:'incomplete',reason:`Target language sample was truncated: ${sample.key}`,rows,trace};
    if(!original&&!translation&&expectedHex===undefined)
      return{status:'incomplete',reason:`Missing-key fallback has no independent Japanese baseline: ${sample.key}`,rows,trace};
    if(expectedHex!==undefined)assert.equal(actual.valueHex,expectedHex,`Native text differs for ${sample.key}`);
    assert.ok(matching.every(event=>event.valueHex===actual.valueHex),`Native text changed source within the scene: ${sample.key}`);
    assert.ok(matching.every(event=>Boolean(event.flags&1)===Boolean(translation)),`Native translation source differs for ${sample.key}`);
    const raster=rasters.find(event=>event.textHex===actual.valueHex&&event.scene===actual.scene);
    if(!raster)return{status:'incomplete',reason:`Consumed lookup did not reach the real text rasterizer: ${sample.key}`,rows,trace};
    const proof=rasterGlyphProof({raster,presents,glyphs,after:actual.first,label:sample.key});
    if(proof.status!=='observed')return{...proof,rows,trace};
    rows.push({key:sample.key,operation:sample.operation,valueHex:actual.valueHex,
      expectedSource:translation?'selected-pack':'japanese-original',lookupFirst:actual.first,rasterFirst:raster.first,
      scene:raster.scene,text:proof.text,coverage:proof.coverage});
  }
  return{status:'observed',complete:false,rows,glyphs,counts:{lookups:lookups.length,rasters:rasters.length,presents:presents.length,glyphs:glyphs.length},
    baseline:Object.fromEntries(rows.map(row=>[row.key,row.valueHex]))};
}
