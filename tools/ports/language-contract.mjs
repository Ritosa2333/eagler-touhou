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
    if(raster.textTruncated)return{status:'incomplete',reason:`Target text raster was truncated: ${sample.key}`,rows,trace};
    if(!hasSubsequentSuccessfulPresent(raster,presents))
      return{status:'incomplete',reason:`Rasterized text has no subsequent successful native present in its draw boundary: ${sample.key}`,rows,trace};
    const text=new TextDecoder(raster.encoding==='utf8'?'utf-8':raster.encoding==='raw'?'windows-1252':'shift_jis')
      .decode(languageBytes(actual.valueHex));
    const codepoints=[...new Set([...text].map(character=>character.codePointAt(0)).filter(code=>code>=32))];
    const coverage=[];
    for(const code of codepoints){
      const used=glyphs.filter(event=>event.scene===raster.scene&&event.a===raster.a&&event.b===raster.b&&
        event.c===code&&event.first<=raster.last&&event.last>=actual.first);
      if(!used.length)return{status:'incomplete',reason:`No actual glyph consumption for U+${code.toString(16)} in ${sample.key}`,rows,trace};
      for(const glyph of used)assert.ok((glyph.flags&33)===33&&!(glyph.flags&2)&&glyph.d===code&&
        ['bitmap','msgothic','pack','unifont','ascii-atlas'].includes(glyph.source),
      `Missing or substituted glyph U+${code.toString(16)} in ${sample.key}`);
      coverage.push({code,sources:[...new Set(used.map(event=>event.source))]});
    }
    rows.push({key:sample.key,operation:sample.operation,valueHex:actual.valueHex,
      expectedSource:translation?'selected-pack':'japanese-original',lookupFirst:actual.first,rasterFirst:raster.first,
      scene:raster.scene,text,coverage});
  }
  return{status:'observed',complete:false,rows,glyphs,counts:{lookups:lookups.length,rasters:rasters.length,presents:presents.length,glyphs:glyphs.length},
    baseline:Object.fromEntries(rows.map(row=>[row.key,row.valueHex]))};
}
