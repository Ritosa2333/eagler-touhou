import {createReplica} from './replica.js';
import {digest,toBase64,MAX_POINT_BYTES,MAX_POINT_TOTAL} from './tape.js';
// The player only appends small confirmed rows. Capture/compression belongs to this replica.
export function createBuilder(header,getRows,onPoint,onError,{openReplica=createReplica}={}){
  let replica,stopped=false,busy=false,ending=false,lastSaved=-1,total=0,points=0,armed=true;
  function stop(){stopped=true;replica?.stop();}
  async function save(){
    const point=await replica.save();if(stopped)return;
    if(point.frame===lastSaved)return;
    if(++points>128||point.bytes.length>MAX_POINT_BYTES||(total+=point.bytes.length)>MAX_POINT_TOTAL)throw Error('检查点达到容量上限，保留完整输入');
    const sha=await digest(point.bytes);if(stopped)return;
    onPoint({frame:point.frame,sha,data:toBase64(point.bytes),scene:replica.scene});lastSaved=point.frame;
  }
  async function pump(){
    if(stopped||busy||!replica)return;busy=true;
    try{
      const rows=getRows();
      const started=performance.now();let count=0;
      while(!stopped&&replica.frame<rows.length&&count<32&&performance.now()-started<16){
        const batch=[];let saveAfter=false;
        while(replica.frame+batch.length<rows.length&&count+batch.length<32){
          const row=rows.row(replica.frame+batch.length);batch.push(row);
          if(row.scene===2)armed=true;
          else if(row.scene===1&&armed){armed=false;saveAfter=true;break;}
        }
        await replica.advanceBatch(batch);count+=batch.length;
        if(saveAfter)await save();
      }
      if(!stopped&&ending&&replica.frame===getRows().length){await save();stop();}
    }catch(e){stop();onError(e);}finally{busy=false;if(!stopped&&replica.frame<getRows().length)setTimeout(pump,0);}
  }
  void(async()=>{try{const r=await openReplica(header);if(stopped){r.stop();return;}replica=r;await save();pump();}catch(e){if(!stopped){stop();onError(e);}}})();
  return {get done(){return stopped;},kick:pump,finish(){ending=true;pump();},stop};
}
