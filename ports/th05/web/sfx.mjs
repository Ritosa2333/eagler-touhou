// uth05win CSoundEffect: restart only this effect; other voices keep playing.
export function decodeWave(context,bytes){
 const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),tag=at=>String.fromCharCode(...bytes.subarray(at,at+4));
 if(bytes.length<44||tag(0)!=='RIFF'||tag(8)!=='WAVE')throw Error('Invalid uth05win SE WAV');
 let channels=0,rate=0,bits=0,data;
 for(let at=12;at+8<=bytes.length;){const size=view.getUint32(at+4,true),begin=at+8;if(begin+size>bytes.length)throw Error('Truncated SE WAV');
  if(tag(at)==='fmt '){if(size<16||view.getUint16(begin,true)!==1)throw Error('Unsupported SE WAV');channels=view.getUint16(begin+2,true);rate=view.getUint32(begin+4,true);bits=view.getUint16(begin+14,true);}
  if(tag(at)==='data')data={begin,size};at=begin+size+(size&1);
 }
 if(!data||![1,2].includes(channels)||bits!==16||rate<8000||rate>96000||data.size%(2*channels))throw Error('Unsupported SE PCM format');
 const frames=data.size/(2*channels),buffer=context.createBuffer(channels,frames,rate);
 for(let channel=0;channel<channels;channel++){const output=buffer.getChannelData(channel);for(let i=0;i<frames;i++)output[i]=view.getInt16(data.begin+(i*channels+channel)*2,true)/32768;}
 return buffer;
}
export function createSfx(context,files){
 const buffers=new Map(),active=new Map();let closed=false,played=0,restarted=0;
 for(let id=1;id<16;id++){const name=`s${String(id).padStart(2,'0')}.wav`,bytes=files.get(name);if(!bytes)throw Error('Missing original SE/'+name);buffers.set(id,decodeWave(context,bytes));}
 function stop(id){const source=active.get(id);if(source){active.delete(id);try{source.stop();}catch{}}}
 return {
  trigger(id){if(closed||!buffers.has(id)||context.state!=='running')return;if(active.has(id)){restarted++;stop(id);}const source=context.createBufferSource();source.buffer=buffers.get(id);source.connect(context.destination);active.set(id,source);source.onended=()=>{source.disconnect();if(active.get(id)===source)active.delete(id);};source.start();played++;},
  pump(){},
  clear(){for(const id of [...active.keys()])stop(id);},
  info:()=>({backend:'uth05win-original-se',played,restarted,active:[...active.keys()]}),
  close(){closed=true;for(const id of [...active.keys()])stop(id);buffers.clear();},
 };
}
