export const ROOM_DEFAULTS={language:'jp',difficulty:3,clock:32,rollback:false,focusEnabled:true,touchUnlimitedAllowed:true,hostSeat:0};
export const ROOM_COOKIE='th03_fast_room_preferences_v1';
const valid={language:v=>['jp','cn'].includes(v),difficulty:v=>Number.isInteger(v)&&v>=0&&v<=3,
  clock:v=>v===32,hostSeat:v=>v===0||v===1,
  rollback:v=>typeof v==='boolean',focusEnabled:v=>typeof v==='boolean',touchUnlimitedAllowed:v=>typeof v==='boolean'};

export function readRoomPreferences(cookie){
  let saved;
  try{cookie??=document.cookie;const value=cookie.split(';').map(part=>part.trim()).find(part=>part.startsWith(ROOM_COOKIE+'='));
    saved=value?JSON.parse(decodeURIComponent(value.slice(ROOM_COOKIE.length+1))):null;}catch{}
  const settings={...ROOM_DEFAULTS},connections={};
  for(const [key,check]of Object.entries(valid))if(check(saved?.settings?.[key]))settings[key]=saved.settings[key];
  for(const network of ['lan','public']){
    const c=saved?.connections?.[network];
    if(c&&['rtc','relay'].includes(c.mode)&&['all','relay'].includes(c.ice))connections[network]={mode:c.mode,ice:c.ice};
  }
  return {settings,connections};
}

export function saveRoomPreferences({settings,connection},doc=document){
  try{
    const preferences=readRoomPreferences(doc.cookie);
    if(settings)for(const [key,check]of Object.entries(valid))if(check(settings[key]))preferences.settings[key]=settings[key];
    if(connection&&['lan','public'].includes(connection.network)&&['rtc','relay'].includes(connection.mode)&&['all','relay'].includes(connection.ice))
      preferences.connections[connection.network]={mode:connection.mode,ice:connection.ice};
    const value=encodeURIComponent(JSON.stringify(preferences));
    if(doc.cookie.split(';').some(part=>part.trim()===ROOM_COOKIE+'='+value))return;
    doc.cookie=ROOM_COOKIE+'='+value+'; Path=/; Max-Age=31536000; SameSite=Lax'+(location.protocol==='https:'?'; Secure':'');
  }catch{} // Cookie restrictions must not stop creating or configuring a room.
}
