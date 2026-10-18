// Test-only admission through the actual room/seat/member protocol.
// Keep the lobby sockets alive until their owning browser is closed.
export async function admitPerformanceRoom(relay,room,count,difficulty,timing) {
  const clients=[];
  const open=seat=>new Promise((resolve,reject)=>{
    const id=`cadence_client_${seat}`,member=`cadence_member_${seat}_${room}`;
    const url=new URL(relay);url.searchParams.set('room',room);url.searchParams.set('lobby',id);
    url.searchParams.set('member',member);url.searchParams.set('intent',seat?'join':'create');
    const ws=new WebSocket(url),queue=[],waiters=[];
    ws.onerror=()=>reject(Error('Performance lobby connection failed'));
    ws.onmessage=event=>{const value=JSON.parse(event.data),waiter=waiters.shift();waiter?waiter(value):queue.push(value);};
    ws.onopen=()=>resolve({ws,member,next:()=>queue.length?Promise.resolve(queue.shift()):new Promise((done,fail)=>{
      const timer=setTimeout(()=>fail(Error('Performance lobby timeout')),15000);
      waiters.push(value=>{clearTimeout(timer);done(value);});
    })});
  });
  try {
    for(let seat=0;seat<count;seat++)clients.push(await open(seat));
    globalThis.__th08PerformanceLobbies=clients;
    for(const client of clients)await client.next();
    const wait=async predicate=>{for(;;){const value=await clients[0].next();if(value.type==='error')throw Error(JSON.stringify(value));if(predicate(value))return value;}};
    clients[0].ws.send(JSON.stringify({type:'take-seat',seat:0,loadout:0}));
    await wait(value=>value.type==='state'&&value.room?.seats?.[0]);
    clients[0].ws.send(JSON.stringify({type:'settings',playerCount:count,difficulty}));
    await wait(value=>value.type==='state'&&value.room?.playerCount===count&&value.room?.difficulty===difficulty);
    clients.forEach((client,seat)=>{if(seat)client.ws.send(JSON.stringify({type:'take-seat',seat,loadout:seat}));});
    const state=await wait(value=>value.type==='state'&&value.room?.seats?.slice(0,count).every(Boolean));
    for(const client of clients)client.ws.send(JSON.stringify({type:'set-ready',ready:true,settingsVersion:state.room.settingsVersion}));
    await wait(value=>value.type==='state'&&value.room.seats.slice(0,count).every(seat=>seat?.ready));
    clients[0].ws.send(JSON.stringify({type:'start',...timing}));
    const admission=await wait(value=>value.type==='start');
    const urls=clients.map(client=>{const url=new URL(relay);url.searchParams.set('room',room);url.searchParams.set('run',admission.serial);url.searchParams.set('member',client.member);return url.href;});
    return {admission,urls};
  } catch(error) {for(const client of clients)client.ws.close();throw error;}
}
