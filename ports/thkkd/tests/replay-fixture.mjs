// 合成原版格式输入，按生产保存器的七个元数据字段排列。
export function replayFixture({character=0,shot=0,difficulty=1,stage=0,practice=false,count=360,bombFrame=80}={}){
  const result=new Uint8Array(count+62),footer=new DataView(result.buffer,count,62);
  result.fill(32,0,count);if(bombFrame>=0&&bombFrame<count)result[bombFrame]|=16;
  [difficulty,practice?2:1,shot,stage,0,count,character].forEach((value,i)=>footer.setUint32(i*4,value,true));
  const text=(offset,length,value)=>result.set(new TextEncoder().encode(value).subarray(0,length),count+offset);
  text(28,8,'WEBTEST');text(36,8,['Reimu','Marisa','Sanae'][character]+String.fromCharCode(65+shot));text(44,10,['Easy','Normal','Hard','Lunatic','Extra'][difficulty]||'Omniverse');
  footer.setUint32(58,((footer.getUint32(28,true)+footer.getUint32(32,true))>>>0)%119,true);return result;
}
