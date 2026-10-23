import {stampRoomProtocolEvent} from './room-errors.mjs';

/** Read-only host observer shared by player and spectator pages. */
export function installRoomProtocolCapture({game,stamp}) {
  if(window!==window.top)return;
  window.acceptanceEvents=[];
  const pending=new Set(),errors=[];
  window.__flushRoomProtocolCaptures=async()=>{
    while(pending.size)await Promise.all([...pending]);
    if(errors.length)throw Error(`Room protocol capture failed: ${errors.join(', ')}`);
    return pending.size;
  };
  addEventListener('message',event=>{
    if(event.origin!==location.origin||event.source!==document.querySelector('#gameFrame')?.contentWindow||
      event.data?.protocol!=='eagler-touhou/1'||event.data.game!==game)return;
    const runtime=event.source,nativeAtReceipt={failed:runtime.__eaglerNetplayFailed===true,
      error:String(runtime.__eaglerNetplayError??''),
      transportState:{disconnected:runtime.__eaglerPeerTransport?.disconnected===true}};
    const value=stamp(event.data,Date.now(),nativeAtReceipt);
    window.acceptanceEvents.push(value);
    if(!['error','abort'].includes(value.event)&&value.ok!==false)return;
    try{
      const capture=Promise.resolve(window.__captureRoomProtocolFailure(value));
      pending.add(capture);
      void capture.then(()=>pending.delete(capture),error=>{errors.push(String(error));pending.delete(capture);});
    }catch(error){errors.push(String(error));}
  });
}

export async function attachRoomProtocolCapture(page,{game}) {
  const events=[];
  await page.exposeBinding('__captureRoomProtocolFailure',(_source,event)=>events.push(event));
  await page.addInitScript({content:`(${installRoomProtocolCapture.toString()})({game:${JSON.stringify(game)},stamp:(${stampRoomProtocolEvent.toString()})})`});
  return{events,async drain(){
    const pending=await page.evaluate(()=>window.__flushRoomProtocolCaptures());
    if(pending!==0)throw Error('Room protocol captures did not drain');
  }};
}
