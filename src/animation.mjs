// Presentation-only interpretation of TH12 ANM v7. Native evidence: 0x455630.
// Game-RNG VMs must be advanced by the logical core. This sampler never consumes core RNG.
import {easing as ease} from './easing.mjs';
const f32=Math.fround,PI=3.1415927410125732,TAU=6.2831854820251465;
const byte=n=>(Math.trunc(n)&255)/255;
function normalize(a){a=f32(a);while(a>PI)a=f32(a-TAU);while(a<-PI)a=f32(a+TAU);return a;}
function makeState(){return {sprite:null,x:0,y:0,z:0,offsetX:0,offsetY:0,offsetZ:0,rx:0,ry:0,rotation:0,sx:1,sy:1,r:1,g:1,b:1,alpha:1,r2:0,g2:0,b2:0,alpha2:0,useColor2:false,blend:0,layer:0,visible:true,mode:0,anchorX:0,anchorY:0,u:0,v:0,point:false,uvScaleX:1,uvScaleY:1,geometryCount:0,geometrySpan:1,geometryRadius:0,geometryWidth:0};}
function translateTree(frame,offset,parent=null){return {...frame,x:frame.x+offset.x,y:frame.y+offset.y,z:frame.z+offset.z,
  // TH12 quad/circle draw helpers multiply by the immediate parent's VM
  // scale (0x45a593, 0x45ccb5), and rotated modes add its Z rotation.
  sx:frame.sx*(parent?.sx??1),sy:frame.sy*(parent?.sy??1),rotation:frame.rotation+((frame.rotate||frame.mode>=14)?parent?.rotation??0:0),
  children:frame.children.map(child=>translateTree(child,offset))};}
class Track {
  constructor(owner,name,id,variant,options={},depth=0){
    const script=owner.atlases[name]?.scripts[id];this.owner=owner;this.name=name;this.id=id;this.depth=depth;
    this.commands=script?.commands||[];this.offsets=new Map(this.commands.map((c,i)=>[c.offset,i]));this.pc=0;this.clock=0;this.locals=new Map();this.frames=[];this.tickCount=0;this.variant=variant;this.options=options;
    this.state=makeState();this.velocity={rx:0,ry:0,rotation:0,sx:0,sy:0,u:0,v:0};this.interpolations=new Map();this.children=[];this.saved=null;this.stopped=false;this.ended=false;this.seed=(options.seed??0x1234)&65535;this.displayRng=false;
    // Native binding runs initial commands before accepting a launch interrupt.
    if(options.interrupts){this.commandsAtTime();for(const code of options.interrupts){this.interrupt(code);this.commandsAtTime();}}
    else if(options.interrupt||variant){this.commandsAtTime();this.interrupt(options.interrupt??2);}
  }
  random32(forceGame=false){if(forceGame||!this.displayRng)this.owner.unresolvedGameRandom.add(this.name+':'+this.id);const a=((this.seed^0x9630)-0x6553)&65535;this.seed=((a<<2)|(a>>>14))&65535;const b=((this.seed^0x9630)-0x6553)&65535;this.seed=((b<<2)|(b>>>14))&65535;return ((a<<16)|b)>>>0;}
  random(){
    if(!this.displayRng)this.owner.unresolvedGameRandom.add(this.name+':'+this.id);
    // A deterministic local display fallback. Shared retail display-RNG scheduling is not verified.
    return this.random32()/4294967296;
  }
  value(c,i,float=false){
    const v=(float?c.floats:c.ints)[i]??0;if(!(c.mask&(1<<i)))return v;
    if(this.locals.has(v))return this.locals.get(v);
    if(v>=10000&&v<=10009)return 0;
    // Native integer and float readers have different builtin tables.
    // Integer 10010..10021 are literal IDs, not random or position variables.
    if(!float)return v===10022?this.random32()|0:v;
    if(v===10010)return f32(f32(this.random()*2-1)*PI);if(v===10011)return f32(this.random());if(v===10012)return f32(this.random()*2-1);
    if(v>=10013&&v<=10015)return this.state[['x','y','z'][v-10013]];
    if(v>=10016&&v<=10018)return this.options.cameraPosition?.[v-10016]??0;
    if(v>=10019&&v<=10021)return this.options.cameraEyeOffset?.[v-10019]??0;
    if(v===10022)return f32(this.random32());
    if(v>=10023&&v<=10025)return this.state[['rx','ry','rotation'][v-10023]];
    return v;
  }
  interrupt(code){
    let at=this.commands.findIndex(c=>c.op===64&&c.ints[0]===code);if(at<0)at=this.commands.findIndex(c=>c.op===64&&c.ints[0]===-1);if(at<0)return false;
    this.saved={pc:this.pc,clock:this.clock,stopped:this.stopped};this.pc=at+1;this.clock=this.commands[at].time;this.stopped=false;this.ended=false;this.state.visible=true;return true;
  }
  interpolate(keys,to,duration,mode,hermite=null){
    const from=keys.map(k=>this.state[k]);if(duration<=0){keys.forEach((k,i)=>this.state[k]=f32(to[i]));this.interpolations.delete(keys.join());}
    else this.interpolations.set(keys.join(),{keys,from,to:to.map(f32),duration,mode,time:0,hermite});
  }
  child(op,id,offset=null){
    if(this.depth>=12||this.children.length>=256){this.owner.unimplemented.add('child-budget');return;}
    if(!this.owner.atlases[this.name]?.scripts[id])return;
    const child=new Track(this.owner,this.name,id,null,{seed:this.seed},this.depth+1),detached=op===95||op===97;
    child.state.layer=this.state.layer;child.born=this.tickCount;child.detached=detached;
    child.base=detached?{x:this.state.x,y:this.state.y,z:this.state.z}:{x:0,y:0,z:0};
    if(detached){child.state.rx=this.state.rx;child.state.ry=this.state.ry;child.state.rotation=this.state.rotation;}
    // Native child constructor binds and updates the child immediately.
    child.tick();if(offset){const [x,y]=offset();child.state.offsetX=f32(x);child.state.offsetY=f32(y);child.frames[0].x=f32(child.frames[0].x+x);child.frames[0].y=f32(child.frames[0].y+y);}this.children.unshift(child);
  }
  command(c){
    const I=i=>this.value(c,i),F=i=>this.value(c,i,true),s=this.state;
    if(c.op>=6&&c.op<=27){
      const float=c.op%2,three=c.op>=18,op=three?Math.floor((c.op-18)/2)+1:Math.floor((c.op-6)/2),key=(float?c.floats:c.ints)[0],a=three?this.value(c,1,float):this.locals.get(key)||0,b=this.value(c,three?2:1,float);
      const v=op===0?b:op===1?a+b:op===2?a-b:op===3?a*b:op===4?(b?a/b:0):(b?a%b:0);this.locals.set(key,float?f32(v):v|0);return false;
    }
    if(c.op>=28&&c.op<=39){
      const float=c.op%2,a=this.value(c,0,float),b=this.value(c,1,float),kind=Math.floor((c.op-28)/2),pass=[a===b,a!==b,a<b,a<=b,a>b,a>=b][kind];
      if(pass){this.pc=this.offsets.get(I(2))??Infinity;this.clock=I(3);return true;}return false;
    }
    switch(c.op){
      case -1:case 65535:this.ended=true;this.pc=Infinity;break;
      case 0:break;
      case 1:s.visible=false;this.ended=true;this.pc=Infinity;break;
      case 2:this.ended=true;this.pc=Infinity;break;
      case 3:{const n=I(0);s.sprite=this.variant&&n>=0&&n<3?this.variant[['sprite','spawnSprite','cancelSprite'][n]]:n;s.visible=true;break;}
      case 4:this.pc=this.offsets.get(I(0))??Infinity;this.clock=I(1);return true;
      case 5:{const key=c.ints[0],n=(this.locals.get(key)||0)-1;this.locals.set(key,n);if(n>0){this.pc=this.offsets.get(I(1))??Infinity;this.clock=I(2);return true;}break;}
      case 40:this.locals.set(c.ints[0],I(1)?this.random32()%(I(1)>>>0):0);break;
      case 41:this.locals.set(c.floats[0],f32(this.random()*F(1)));break;
      case 42:case 43:this.locals.set(c.floats[0],f32((c.op===42?Math.sin:Math.cos)(F(1))));break;
      case 48:{const z=f32(F(2)),y=f32(F(1)),x=f32(F(0)),prefix=this.options.positionOffset?'offset':'';s[prefix?prefix+'X':'x']=x;s[prefix?prefix+'Y':'y']=y;s[prefix?prefix+'Z':'z']=z;break;}
      case 49:s.rx=f32(F(0));s.ry=f32(F(1));s.rotation=f32(F(2));break;
      case 50:s.sx=f32(F(0));s.sy=f32(F(1));break;
      case 51:s.alpha=byte(I(0));break;
      case 52:case 76:{const suffix=c.op===76?'2':'';s['r'+suffix]=byte(I(0));s['g'+suffix]=byte(I(1));s['b'+suffix]=byte(I(2));break;}
      case 53:this.velocity.rx=f32(F(0));this.velocity.ry=f32(F(1));this.velocity.rotation=f32(F(2));break;
      case 54:this.velocity.sx=f32(F(0));this.velocity.sy=f32(F(1));break;
      case 55:this.interpolate(['alpha'],[byte(I(0))],I(1),0);break;
      case 56:this.interpolate(this.options.positionOffset?['offsetX','offsetY','offsetZ']:['x','y','z'],[F(2),F(3),F(4)],I(0),I(1));break;
      case 57:case 78:{const suffix=c.op===78?'2':'';this.interpolate(['r'+suffix,'g'+suffix,'b'+suffix],[byte(I(2)),byte(I(3)),byte(I(4))],I(0),I(1));break;}
      case 58:case 79:this.interpolate([c.op===79?'alpha2':'alpha'],[byte(I(2))],I(0),I(1));break;
      case 59:this.interpolate(['rx','ry','rotation'],[F(2),F(3),F(4)],I(0),I(1));break;
      case 60:this.interpolate(['sx','sy'],[F(2),F(3)],I(0),I(1));break;
      case 61:s.sx=-s.sx;break;case 62:s.sy=-s.sy;break;
      case 63:this.stopped=true;return true;
      case 64:break;
      case 65:s.anchorX=I(0)&3;s.anchorY=(I(0)>>>16)&3;break;
      case 66:s.blend=I(0)&7;break;case 67:s.mode=I(0)&31;break;case 68:s.layer=I(0)&255;break;
      case 69:s.visible=false;this.stopped=true;return true;
      case 70:this.velocity.u=f32(F(0));break;case 71:this.velocity.v=f32(F(0));break;
      case 72:s.visible=!!(I(0)&1);break;
      case 73:s.filterFlag=!!I(0);break;
      case 75:this.clock-=I(0);break;
      case 77:s.alpha2=byte(I(0));break;case 80:s.useColor2=!!(I(0)&1);break;
      case 81:if(this.saved){this.pc=this.saved.pc;this.clock=this.saved.clock;this.stopped=this.saved.stopped;this.saved=null;return true;}break;
      case 82:s.depthWrite=!!I(0);break;
      case 83:this.owner.unimplemented.add('83-origin-transfer');break;
      case 84:case 101:s.mode=c.op===84?9:13;s.geometryCount=I(0);break;
      case 85:s.screenSpace=!!I(0);break;case 86:s.reservedFlag=!!I(0);break;
      case 87:this.displayRng=!!(I(0)&1);break;
      case 88:case 90:case 91:case 92:case 95:this.child(c.op,I(0));break;
      case 89:s.point=!!(I(0)&1);break;
      case 93:case 94:this.interpolate([c.op===93?'scrollU':'scrollV'],[F(2)],I(0),I(1));break;
      case 96:case 97:this.child(c.op,I(0),()=>[F(1),F(2)]);break;
      case 100:this.interpolate(this.options.positionOffset?['offsetX','offsetY','offsetZ']:['x','y','z'],[F(4),F(5),F(6)],I(0),8,{a:[F(1),F(2),F(3)],b:[F(7),F(8),F(9)]});break;
      case 102:{const n=I(0),range=I(1),random=this.random32(true);if(!range){this.owner.unimplemented.add('102-native-division-by-zero');this.ended=true;break;}s.sprite=n+random%(range>>>0);s.visible=true;break;}
      case 103:s.mode=14;s.geometryRadius=F(0);s.geometryWidth=F(1);break;
      case 104:case 105:s.mode=c.op===104?15:16;s.geometryRadius=F(0);s.geometryCount=I(1);break;
      case 110:s.mode=20;s.geometryRadius=F(0);s.geometryWidth=F(1);this.owner.unimplemented.add('geometry-20');break;
      default:this.owner.unimplemented.add('opcode-'+c.op);break;
    }
    return false;
  }
  commandsAtTime(){
    if(this.stopped||this.ended)return;
    for(let budget=0;budget<1000&&this.commands[this.pc]?.time<=this.clock;budget++){
      const pc=this.pc,c=this.commands[pc],jump=this.command(c);if(!jump&&this.pc===pc)this.pc++;if(this.stopped||this.ended)break;
      if(budget===999)this.owner.unimplemented.add('instruction-budget');
    }
  }
  tick(){
    const born=this.tickCount;this.commandsAtTime();const s=this.state;
    for(const key of ['rx','ry','rotation'])s[key]=normalize(s[key]+this.velocity[key]);
    for(const key of ['sx','sy'])s[key]=f32(s[key]+this.velocity[key]);
    s.u=f32(s.u+this.velocity.u);s.v=f32(s.v+this.velocity.v);if(s.u>=1)s.u=f32(s.u-1);else if(s.u<0)s.u=f32(s.u+1);if(s.v>=1)s.v=f32(s.v-1);else if(s.v<0)s.v=f32(s.v+1);
    for(const [key,t]of this.interpolations){
      // TH12 interpolation helpers advance their own Timer before sampling (0x458cff etc).
      const n=Math.min(1,++t.time/t.duration),e=ease(n,t.mode);
      t.keys.forEach((k,i)=>{const value=t.hermite?(2*n**3-3*n**2+1)*t.from[i]+(-2*n**3+3*n**2)*t.to[i]+(n**3-2*n**2+n)*t.hermite.a[i]+(n**3-n**2)*t.hermite.b[i]:t.from[i]+(t.to[i]-t.from[i])*e;
        if(/^(?:r|g|b)2?$/.test(k)){const a=Math.round(t.from[i]*255),b=Math.round(t.to[i]*255);s[k]=byte(a-Math.trunc((a-b)*ease(f32(n),t.mode)));}
        else if(/^alpha2?$/.test(k)){const a=Math.round(t.from[i]*255),b=Math.round(t.to[i]*255);s[k]=byte(Math.trunc(a+(b-a)*ease(f32(n),t.mode)));}
        else if(k==='scrollU'||k==='scrollV')this.velocity[k==='scrollU'?'u':'v']=f32(value);else s[k]=f32(value);});
      if(t.time>=t.duration)this.interpolations.delete(key);
    }
    const color=s.useColor2?[s.r2,s.g2,s.b2,s.alpha2]:[s.r,s.g,s.b,s.alpha];
    const snapshot={...s,x:f32(s.x+s.offsetX),y:f32(s.y+s.offsetY),z:f32(s.z+s.offsetZ),r:color[0],g:color[1],b:color[2],alpha:color[3],rotate:s.mode===1||s.mode===3,name:this.name,script:this.id,children:[]};
    for(const child of this.children){if(child.born!==born)child.tick();const frame=child.frames.at(-1);if(!frame)continue;const offset=child.detached?child.base:{x:snapshot.x+child.base.x,y:snapshot.y+child.base.y,z:snapshot.z+child.base.z};snapshot.children.unshift(translateTree(frame,offset,child.detached?null:s));}
    this.children=this.children.filter(child=>!child.ended||child.state.visible);
    // Retain a current pose, rather than one complete child tree per elapsed
    // frame. Seeking backwards reconstructs the VM from its binding inputs.
    this.frames[0]=snapshot;this.tickCount++;if(!this.stopped&&!this.ended)this.clock++;
  }
  sample(age){age=Math.max(0,Math.min(100000,age|0));while(this.tickCount<=age)this.tick();return this.frames[0];}
}
export class AnimationSampler {
  constructor(atlases){this.atlases=atlases;this.tracks=new Map();this.unimplemented=new Set();this.unresolvedGameRandom=new Set();}
  beginFrame(){this.usedTracks=new Set();}
  endFrame(){if(this.usedTracks){for(const key of this.tracks.keys())if(!this.usedTracks.has(key))this.tracks.delete(key);this.usedTracks=null;}}
  sample(name,id,age=0,variant=null,options={}){
    if(!this.atlases[name]?.scripts[id])return null;
    const key=name+':'+id+':'+(variant?JSON.stringify(variant):'')+':'+JSON.stringify(options);
    age=Math.max(0,Math.min(100000,age|0));this.usedTracks?.add(key);
    if(!this.tracks.has(key)||age<this.tracks.get(key).tickCount-1)this.tracks.set(key,new Track(this,name,id,variant,options));return this.tracks.get(key).sample(age);
  }
  sampleAll(name,id,age=0,variant=null,options={}){
    const root=this.sample(name,id,age,variant,options);if(!root)return [];const result=[];
    const walk=(s,offset={x:0,y:0,z:0})=>{result.push({...s,x:s.x+offset.x,y:s.y+offset.y,z:s.z+offset.z,children:undefined});for(const child of s.children)walk(child,offset);};walk(root);return result;
  }
  diagnostics(){return {unimplemented:[...this.unimplemented],unresolvedGameRandom:[...this.unresolvedGameRandom],wholePresentationOracle:false};}
}
