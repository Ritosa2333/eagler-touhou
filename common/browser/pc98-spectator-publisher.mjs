const LIMIT=16383; // The relay's 16 KiB limit also includes its one-byte envelope.
const encoder=new TextEncoder();
async function compress(bytes){return new Uint8Array(await new Response(new Blob([bytes]).stream().pipeThrough(new CompressionStream('gzip'))).arrayBuffer());}
function packet(kind,players,first,count,body){
  const bytes=new Uint8Array(17+body.length),view=new DataView(bytes.buffer);bytes.set([0xe8,0x50,0x39,0x38,0x53,1,kind,players,0]);
  view.setUint32(9,first,true);view.setUint32(13,count,true);bytes.set(body,17);return bytes;
}
export function createPc98SpectatorPublisher(header,tape,socket,onError=()=>{}){
  let cursor=0,started=false,ended=false,closed=false,busy=false,timer;
  function stop(){closed=true;clearTimeout(timer);}
  async function pump(){
    if(closed||ended||busy)return;busy=true;
    try{
      const ws=socket();if(!ws||ws.readyState!==1||ws.bufferedAmount>262144)return;
      if(!started){const body=encoder.encode(JSON.stringify(header));if(body.length+16>LIMIT)throw Error('观战开局数据过大');ws.send(packet(1,header.players,0,0,body));started=true;}
      let batches=0;
      while(cursor<tape.length&&batches++<4&&ws.readyState===1&&ws.bufferedAmount<=262144){
        let count=Math.min(60,tape.length-cursor),body;
        do{body=await compress(encoder.encode(JSON.stringify(tape.slice(cursor,cursor+count))));if(body.length+16<=LIMIT)break;count=Math.floor(count/2);}while(count);
        if(!count)throw Error('观战单帧数据过大');
        if(closed)return;ws.send(packet(2,header.players,cursor,count,body));cursor+=count;
      }
      if(!closed&&tape.end&&cursor===tape.length){ws.send(packet(3,header.players,cursor,0,new Uint8Array()));ended=true;}
    }catch(error){stop();onError(error);}
    finally{busy=false;if(!closed&&!ended)timer=setTimeout(pump,cursor<tape.length?20:250);}
  }
  void pump();return {kick(){clearTimeout(timer);void pump();},stop,get ended(){return ended;}};
}
