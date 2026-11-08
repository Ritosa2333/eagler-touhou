const LIMIT=16383; // The relay's 16 KiB limit also includes its one-byte envelope.
const encoder=new TextEncoder();
const checkpointBytes=Symbol.for('pc98.spectator.checkpoint.bytes');
async function compress(bytes){return new Uint8Array(await new Response(new Blob([bytes]).stream().pipeThrough(new CompressionStream('gzip'))).arrayBuffer());}
function packet(kind,players,first,count,body){
  const bytes=new Uint8Array(17+body.length),view=new DataView(bytes.buffer);bytes.set([0xe8,0x50,0x39,0x38,0x53,1,kind,players,0]);
  view.setUint32(9,first,true);view.setUint32(13,count,true);bytes.set(body,17);return bytes;
}
export function createPc98SpectatorPublisher(header,tape,socket,onError=()=>{},getPoints=()=>[]){
  const pointDelay=['02','03','04'].includes(header.game)?35:50;
  let cursor=0,started=false,ended=false,closed=false,busy=false,timer,pointIndex=0,pointOffset=0,pointBytes,nextPointAt=0,lastStage=null,lastSegment=0,matchArmed=true,matchSeen=false;
  function stop(){closed=true;clearTimeout(timer);pointBytes=null;for(const point of getPoints())delete point[checkpointBytes];}
  async function pump(){
    timer=null;if(closed||ended||busy)return;busy=true;
    try{
      const ws=socket();if(!ws||ws.readyState!==1||ws.bufferedAmount>262144)return;
      if(!started){const body=encoder.encode(JSON.stringify(header));if(body.length+16>LIMIT)throw Error('观战开局数据过大');ws.send(packet(1,header.players,0,0,body));started=true;}
      let batches=0;
      while(cursor<tape.length&&batches++<4&&ws.readyState===1&&ws.bufferedAmount<=262144){
        let count=Math.min(60,tape.length-cursor),body;
        do{body=await compress(encoder.encode(JSON.stringify(tape.slice(cursor,cursor+count))));if(body.length+16<=LIMIT)break;count=Math.floor(count/2);}while(count);
        if(!count)throw Error('观战单帧数据过大');
        if(closed)return;ws.send(packet(2,header.players,cursor,count,body));
        if(header.game==='03'&&!header.spectatorInputOnly)for(let i=0;i<count;i++){
          const scene=tape.row(cursor+i).scene;
          if(scene===2)matchArmed=true;
          else if(scene===1&&matchArmed){matchArmed=false;if(matchSeen)ws.send(packet(7,header.players,cursor+count,cursor+i+1,new Uint8Array()));matchSeen=true;}
        }
        cursor+=count;
      }
      const points=getPoints(),point=points[pointIndex];
      if(!closed&&cursor===tape.length&&point&&point.frame<=cursor&&ws.bufferedAmount<=262144&&performance.now()>=nextPointAt){
        pointBytes??=point[checkpointBytes]??(typeof Uint8Array.fromBase64==='function'?Uint8Array.fromBase64(point.data):Uint8Array.from(atob(point.data),c=>c.charCodeAt(0)));
        const chunk=pointBytes.subarray(pointOffset,pointOffset+12000),body=new Uint8Array(72+chunk.length),view=new DataView(body.buffer);
        view.setUint32(0,pointBytes.length,true);view.setUint32(4,pointOffset,true);body.set(encoder.encode(point.sha),8);body.set(chunk,72);
        ws.send(packet(4,header.players,point.frame,0,body));pointOffset+=chunk.length;nextPointAt=performance.now()+pointDelay;
        if(pointOffset===pointBytes.length){
          if(point.frame>0&&(header.game==='03'||(['02','04'].includes(header.game)&&point.scene?.kind==='stage'&&point.scene.stage!==lastStage))){ws.send(packet(6,header.players,cursor,point.frame,new Uint8Array()));lastSegment=point.frame;if(point.scene?.kind==='stage')lastStage=point.scene.stage;}
          delete point[checkpointBytes];pointIndex++;pointOffset=0;pointBytes=null;
        }
      }
      if(!closed&&tape.end&&cursor===tape.length&&!pointBytes){ws.send(packet(3,header.players,cursor,0,new Uint8Array()));ended=true;}
    }catch(error){stop();onError(error);}
    finally{busy=false;if(!closed&&!ended)timer=setTimeout(pump,pointBytes?pointDelay:250);}
  }
  void pump();return {kick(){if(tape.end){clearTimeout(timer);timer=null;void pump();}else if(!timer&&!busy)timer=setTimeout(()=>{timer=null;void pump();},250);},stop,get ended(){return ended;},get segment(){return lastSegment;},
    async seal(timeout=2000){const deadline=performance.now()+timeout;this.kick();while(!ended&&!closed&&performance.now()<deadline)await new Promise(resolve=>setTimeout(resolve,25));return ended;}};
}
