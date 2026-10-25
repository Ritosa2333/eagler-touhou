/** Copy a declared little-endian native trace without conflating its integer and float slots. */
export function readNativeTrace(buffer,pointer,{wordCount,uint32={},float32={},bytes={}}={}){
  if(!Number.isSafeInteger(wordCount)||wordCount<=0||!Number.isSafeInteger(pointer)||pointer<0||pointer%4)
    throw new TypeError('Native trace requires a positive word count and aligned byte pointer');
  const size=wordCount*4,view=new DataView(buffer,pointer,size),row={},names=new Set();
  const field=(name,at,length,bound)=>{
    if(names.has(name)||!Number.isSafeInteger(at)||at<0||!Number.isSafeInteger(length)||length<=0||at+length>bound)
      throw new TypeError(`Invalid native trace field ${name}`);
    names.add(name);
  };
  for(const [name,at]of Object.entries(uint32)){field(name,at,1,wordCount);row[name]=view.getUint32(at*4,true);}
  for(const [name,{at,length}]of Object.entries(float32)){
    field(name,at,length,wordCount);row[name]=Array.from({length},(_,index)=>view.getFloat32((at+index)*4,true));
  }
  for(const [name,{at,length}]of Object.entries(bytes)){
    field(name,at,length,size);row[name]=Array.from(new Uint8Array(buffer,pointer+at,length));
  }
  return row;
}
