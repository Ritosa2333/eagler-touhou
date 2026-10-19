import {oracle,sha} from './helpers.mjs';

const STATE_BYTES=0x33b4c;

// Build candidate inputs in an isolated emulator from the selected original
// executable. No host file is read or written by the emulated save boundary.
export async function originalRecordFixture(){
 const m=await oracle();
 try{
  const manager=m.allocate(STATE_BYTES+8),writes=[];
  m.view(0x503d86,1)[0]=0;
  m.u32(0x4e9a48,0xabcd1234);
  m.u32(0x4e9a4c,5432);
  m.replace(0x490f2b,'working directory boundary',()=>0);
  m.replace(0x402db0,'no previous score file boundary',()=>0,1);
  m.call(0x45dbd0,{ecx:manager});
  m.u32(0x4e9bc8,manager);

  m.replace(0x403130,'score file creation boundary',()=>{m.u32(0x4e0ef0,777);return 0;});
  const writeAddress=m.u32(0x4be09c),writeEntry=m.importMap.get(writeAddress);
  if(!writeEntry)throw Error('Original score WriteFile import is missing');
  m.importMap.set(writeAddress,{...writeEntry,argc:5,handler:()=>{
   const sp=m.reg('ESP'),handle=m.u32(sp+4),buffer=m.u32(sp+8),size=m.u32(sp+12),written=m.u32(sp+16);
   if(handle!==777)throw Error('Original score file used an unexpected handle');
   writes.push(Buffer.from(m.bytes(buffer,size)));
   m.u32(written,size);
   return 1;
  }});
  const closeAddress=m.u32(0x4be088),closeEntry=m.importMap.get(closeAddress);
  if(!closeEntry)throw Error('Original score CloseHandle import is missing');
  m.importMap.set(closeAddress,{...closeEntry,argc:1,handler:()=>1});

  if(m.call(0x45df50,{limit:1000000000})!==0||writes.length!==2)throw Error('Original score write failed');
  const defaultRecords=Buffer.concat(writes);
  const savedState=Buffer.from(m.bytes(manager+8,STATE_BYTES));
  const reopened=m.allocate(STATE_BYTES+8),source=m.allocate(defaultRecords.length);
  // Poison the destination so a decoder that returns success without writing
  // the decoded state cannot inherit the expected bytes and pass.
  m.view(reopened,STATE_BYTES+8).fill(0xa5);
  m.write(source,defaultRecords);
  m.u32(reopened,source);
  m.u32(reopened+4,0);
  const assertSavedState=state=>{
   if(!state.equals(savedState)||sha(state)!==sha(savedState))throw Error('Original saved-file round trip differs');
  };
  const poison=Buffer.from(m.bytes(reopened+8,STATE_BYTES));
  if(poison.equals(savedState))throw Error('Original saved-state poison control unexpectedly matches');
  let noDecodeRejected=false;
  try{assertSavedState(poison);}catch{noDecodeRejected=true;}
  if(!noDecodeRejected)throw Error('Original saved-state poison control was accepted without decoding');
  if(m.call(0x45ddd0,{ecx:reopened,limit:1000000000})!==0)throw Error('Original score reopen failed');
  const reopenedState=Buffer.from(m.bytes(reopened+8,STATE_BYTES));
  assertSavedState(reopenedState);
  return {savedState:Buffer.from(savedState),defaultRecords:Buffer.from(defaultRecords),savedStateSha256:sha(savedState),defaultRecordsSha256:sha(defaultRecords)};
 }finally{
  m.close();
 }
}
