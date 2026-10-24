import assert from 'node:assert/strict';

/** Independent DATA tables, without using the Runtime's selected track as an expectation. */
export function parseTh08MusicCatalog(commentBytes,formatBytes){
  const text=new TextDecoder('shift_jis',{fatal:true}).decode(commentBytes);
  const lines=text.split(/[\r\n]+/),entries=[];
  for(let index=0;index<lines.length;index++)if(lines[index].startsWith('@')){
    const path=lines[index].slice(1),title=lines[index+1];
    assert.match(path,/^bgm\/th08_[0-9]+b?\.mid$/);assert.ok(title,'Music Room title is missing');
    entries.push({index:entries.length,path,expectedTitle:title,file:path.slice(4).replace(/\.mid$/,'.ogg')});
  }
  const bytes=Buffer.from(formatBytes),formats=new Map();
  for(let offset=0;offset+52<=bytes.length&&bytes[offset]!==0;offset+=52){
    const name=bytes.subarray(offset,offset+16).toString('ascii').split('\0')[0];
    assert.match(name,/^th08_[0-9]+b?\.wav$/);assert.ok(!formats.has(name),'Duplicate BGM format');
    const introBytes=bytes.readUInt32LE(offset+24),totalBytes=bytes.readUInt32LE(offset+28);
    const sampleRate=bytes.readUInt32LE(offset+36),align=bytes.readUInt16LE(offset+44);
    assert.equal(bytes.readUInt16LE(offset+32),1);assert.equal(bytes.readUInt16LE(offset+34),2);
    assert.equal(sampleRate,44100);assert.equal(align,4);assert.equal(bytes.readUInt16LE(offset+46),16);
    assert.ok(introBytes<totalBytes&&introBytes%align===0&&totalBytes%align===0,'Invalid BGM loop bounds');
    formats.set(name,{introFrames:introBytes/align,totalFrames:totalBytes/align,sampleRate});
  }
  assert.equal(entries.length,21,'Expected the complete th08 Music Room catalog');assert.equal(formats.size,entries.length);
  assert.equal(new Set(entries.map(entry=>entry.file)).size,entries.length,'Duplicate Music Room track');
  return entries.map(entry=>{const format=formats.get(entry.file.replace(/\.ogg$/,'.wav'));assert.ok(format,'Music Room track lacks BGM format');return{...entry,...format};});
}


export function parseTh08MessageMusicCues(messageBytes,stageBytes,entry){
  const message=Buffer.from(messageBytes),stage=Buffer.from(stageBytes);
  assert.ok(message.length>=8&&message.length<=16*1024*1024,'Invalid message byte range');
  const count=message.readUInt32LE(0),tableEnd=4+count*4;
  assert.ok(count>0&&count<=4096&&tableEnd<=message.length,'Invalid message entry table');
  assert.ok(Number.isInteger(entry)&&entry>=0&&entry<count,'Message entry is out of range');
  const offsets=Array.from({length:count},(_,index)=>message.readUInt32LE(4+index*4));
  assert.ok(offsets.every(offset=>offset>=tableEnd&&offset<=message.length-4),'Invalid message entry offset');
  const start=offsets[entry],end=Math.min(message.length,...offsets.filter(offset=>offset>start));
  assert.ok(stage.length>=0x490,'Truncated StageHeader');
  const paths=Array.from({length:4},(_,index)=>{
    const bytes=stage.subarray(0x290+index*128,0x290+(index+1)*128),nul=bytes.indexOf(0);
    assert.ok(nul>=0,'Stage music path is not terminated');return bytes.subarray(0,nul).toString('ascii');
  });
  const cues=[];
  for(let at=start;at<end;){
    assert.ok(at+4<=end,'Truncated message instruction');
    const time=message.readUInt16LE(at),opcode=message[at+2],size=message[at+3],next=at+4+size;
    assert.ok(next<=end,'Message instruction crosses its entry boundary');
    if(opcode===7){
      assert.ok(size>=4,'Truncated message Music argument');
      const slot=message.readInt32LE(at+4);
      assert.ok(slot>=-1&&slot<3,'Music slot is outside the title contract');
      const path=slot<0?null:paths[slot];
      if(path!==null)assert.match(path,/^bgm\/th08_[0-9]+b?\.mid$/);
      cues.push({entry,offset:at,time,slot,path,file:path?.slice(4).replace(/\.mid$/,'.ogg')??null});
    }
    at=next;
  }
  const fnv=bytes=>{let value=14695981039346656037n;for(const byte of bytes)value=BigInt.asUintN(64,(value^BigInt(byte))*1099511628211n);return value.toString(16).padStart(16,'0');};
  return{entry,message:{bytes:message.length,fnv64:fnv(message),entryCount:count},stage:{bytes:stage.length,fnv64:fnv(stage),paths},cues};
}
