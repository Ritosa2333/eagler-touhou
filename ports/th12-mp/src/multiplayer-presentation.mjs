// TH15/TH11 proximity policy, applied only to captured presentation data.
export function multiplayerOpacity(player,local){
 if(player.ghost)return (player===local?112:88)/255;
 if(player===local)return 1;
 return (local&&(player.x-local.x)**2+(player.y-local.y)**2<48**2?72:160)/255;
}
export function readMultiplayerVisualOwners(core){
 if(!core._th12_mp_visual_owners_count)return new Map();
 const count=core._th12_mp_visual_owners_count(),ptr=core._th12_mp_visual_owners();
 if(!Number.isInteger(count)||count<0||count>100000||ptr<0||ptr+count*8>core.HEAPU8.byteLength)throw Error('多人画面归属记录无效');
 const data=new Uint32Array(core.HEAPU8.buffer,ptr,count*2),owners=new Map();
 for(let i=0;i<count;i++)owners.set(data[i*2],data[i*2+1]);
 return owners;
}
export function applyMultiplayerOpacity(poses,world,localSeat,owners=new Map()){
 if(!world?.players)return;
 const local=world.players.find(p=>p.seat===localSeat),players=new Map(world.players.map(p=>[p.seat,p]));
 const fadeColor=(color,alpha)=>((color&0xffffff)|Math.round((color>>>24)*alpha)<<24)>>>0;
 for(const pose of poses){
  const owner=pose.owner>>>0,seat=(owner>>>16)===0xffff?((owner>>>8)&255):owners.get(owner),player=players.get(seat);
  if(!player)continue;
  const opacity=multiplayerOpacity(player,local);
  if(pose.mesh&&pose.vertexData)pose.meshOpacity=(pose.meshOpacity??1)*opacity;
  else if(pose.mesh)pose.vertices=pose.vertices.map(v=>({...v,color:fadeColor(v.color,opacity)}));
  else if(pose.beamTrail){pose.start=fadeColor(pose.start,opacity);pose.end=fadeColor(pose.end,opacity);}
  else pose.alpha=(pose.alpha??1)*opacity;
 }
}
