import assert from 'node:assert/strict';

const utf8=new TextDecoder('utf-8',{fatal:true});
/** Independent byte oracle for compiled language tables, without calling the Runtime lookup. */
export function decodeLanguageTable(input){
  assert.ok(input instanceof Uint8Array,'Language table must be bytes');
  const bytes=Buffer.from(input),kind=bytes.toString('ascii',0,4);
  assert.ok(bytes.length>=8&&['EST1','ETL1'].includes(kind),'Unsupported or truncated language table');
  const count=bytes.readUInt32LE(4);assert.ok(count<=65536,'Language table record count is too large');
  let offset=8;const entries=[],keys=new Set();
  const text=length=>{
    assert.ok(offset+length<=bytes.length,'Truncated language table text');
    const value=bytes.subarray(offset,offset+length);offset+=length;
    assert.ok(!value.includes(0),'Language table text contains NUL');
    return utf8.decode(value);
  };
  for(let index=0;index<count;index++){
    const start=offset;assert.ok(offset+8<=bytes.length,'Truncated language table record');
    let key,line=0,hasTranslation=true,value;
    if(kind==='EST1'){
      const idLength=bytes.readUInt16LE(offset),valueLength=bytes.readUInt16LE(offset+2);
      const flags=bytes.readUInt16LE(offset+4),reserved=bytes.readUInt16LE(offset+6);offset+=8;
      assert.ok(idLength>0&&(flags&~1)===0&&reserved===0,'Invalid string table record');
      key=text(idLength);value=text(valueLength);hasTranslation=(flags&1)!==0;
      assert.ok(hasTranslation||value.length===0,'Untranslated string contains translated bytes');
    }else{
      key=bytes.readUInt32LE(offset);line=bytes.readUInt16LE(offset+4);
      const length=bytes.readUInt16LE(offset+6);offset+=8;value=text(length);
    }
    const identity=JSON.stringify([key,line]);assert.ok(!keys.has(identity),'Duplicate language table key');keys.add(identity);
    entries.push({key,line,value,hasTranslation,range:{start,end:offset}});
  }
  assert.equal(offset,bytes.length,'Language table has undeclared trailing bytes');
  return{kind,entries};
}

/** Preserve original encoded records except the selected missing keys for a valid partial-pack control. */
export function omitLanguageTableKeys(input,missingKeys){
  const table=decodeLanguageTable(input),missing=new Set(missingKeys);
  assert.ok(missing.size>0,'Select missing language keys explicitly');
  for(const key of missing)assert.ok(table.entries.some(entry=>entry.key===key),`Missing-key control key is absent: ${key}`);
  const kept=table.entries.filter(entry=>!missing.has(entry.key)),header=Buffer.alloc(8);
  header.write(table.kind);header.writeUInt32LE(kept.length,4);
  return Buffer.concat([header,...kept.map(entry=>Buffer.from(input.subarray(entry.range.start,entry.range.end)))]);
}
