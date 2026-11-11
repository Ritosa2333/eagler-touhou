import {createReplica} from './replica.js';
import {digest,toBase64,MAX_POINT_BYTES,MAX_POINT_TOTAL} from './tape.js';
import {sceneKey} from './scenes.js';
// The player only appends small confirmed rows. Capture/compression belongs to this replica.
export function createBuilder(header,getRows,onPoint,onError){
  let replica,stopped=false,busy=false,ending=false,lastSaved=-1,total=0,points=0,lastScene='';
  const interval=Math.round(30000/header.tickMs);
  function stop(){stopped=true;replica?.stop();}
  async function save(){
    const point=await replica.save();if(stopped)return;
    if(point.frame===lastSaved)return;
    if(++points>128||point.bytes.length>MAX_POINT_BYTES||(total+=point.bytes.length)>MAX_POINT_TOTAL)throw Error('检查点达到容量上限，保留完整输入');
    const sha=await digest(point.bytes);if(stopped)return;
    onPoint({frame:point.frame,sha,data:toBase64(point.bytes),scene:replica.scene});lastSaved=point.frame;lastScene=sceneKey(replica.scene);
  }
  async function pump(){
    if(stopped||busy||!replica)return;busy=true;
    try{
      const rows=getRows();if(rows.length-replica.frame>3600)throw Error('检查点副本积压，保留完整输入');
      const started=performance.now();let count=0;
      while(!stopped&&replica.frame<rows.length&&count++<32&&performance.now()-started<16){
        await replica.advance(rows[replica.frame]);
        if(replica.frame%interval===0||replica.scene&&sceneKey(replica.scene)!==lastScene)await save();
      }
      if(!stopped&&ending&&replica.frame===getRows().length){await save();stop();}
    }catch(e){stop();onError(e);}finally{busy=false;if(!stopped&&replica.frame<getRows().length)setTimeout(pump,0);}
  }
  void(async()=>{try{const r=await createReplica(header);if(stopped){r.stop();return;}replica=r;await save();pump();}catch(e){if(!stopped){stop();onError(e);}}})();
  return {get done(){return stopped;},kick:pump,finish(){ending=true;pump();},stop};
}
