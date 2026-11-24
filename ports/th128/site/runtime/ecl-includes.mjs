// Decode only the authored SCPT/1 resource include directory needed by stage
// startup. Native code still executes the unchanged original ECL bytes.
export function parseEclIncludes(data){
 const need=(ok,message)=>{if(!ok)throw new Error('Invalid ECL include metadata: '+message);};
 need(data instanceof Uint8Array&&data.length>=36,'truncated header');
 const v=new DataView(data.buffer,data.byteOffset,data.byteLength);
 const text=(a,b)=>String.fromCharCode(...data.subarray(a,b));
 need(text(0,4)==='SCPT'&&v.getUint16(4,true)===1,'unsupported SCPT version');
 need(v.getUint32(8,true)===36,'unsupported include offset');
 const table=36+v.getUint16(6,true),subCount=v.getUint32(16,true);
 need(table<=data.length&&subCount<=4096&&subCount*4<=data.length-table,'include/table bounds');
 const includes={ANIM:[],ECLI:[]},sections=new Set();
 let p=36;
 while(p<table){
  need(p+8<=table,'truncated include section');
  const kind=text(p,p+4),count=v.getUint32(p+4,true);
  need(Object.hasOwn(includes,kind)&&!sections.has(kind),'unknown or duplicate section');
  need(count<=4096,'excessive include count');sections.add(kind);p+=8;
  for(let i=0;i<count;i++){
   const start=p;while(p<table&&data[p]!==0)p++;
   need(p<table&&p>start&&p-start<=255,'unterminated include name');
   const name=text(start,p++);
   need(/^[A-Za-z0-9][A-Za-z0-9_.-]*$/.test(name),'unsafe include name');
   need(name.endsWith(kind==='ANIM'?'.anm':'.ecl'),'unexpected include type');
   need(!includes[kind].includes(name),'duplicate include name');includes[kind].push(name);
  }
  p=(p+3)&~3;need(p<=table,'include alignment exceeds section');
 }
 need(p===table,'include extent differs from header');
 return{format:'SCPT/1',includes};
}
