import {easing as ease} from './easing.mjs';
import {AnimationSampler} from './animation.mjs';
const dot=(a,b)=>a.reduce((s,v,i)=>s+v*b[i],0),sub=(a,b)=>a.map((v,i)=>v-b[i]);
const cross=(a,b)=>[a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]];
const unit=a=>{const n=Math.hypot(...a)||1;return a.map(v=>v/n);};
const f32=Math.fround;
// TH12's STD draw routine overwrites the ANM scale for every nonzero
// primitive dimension (0x4047f8..0x404839), including cloud script 4.
export function primitiveSize(q,a,sprite){return [q.size[0]||sprite.w*a.sx,q.size[1]||sprite.h*a.sy];}
export class StageBackground {
  constructor(data,atlases){this.data=data;this.bank=data.bank||'stage01';this.atlases=atlases;this.animations=new AnimationSampler(atlases);this.frames=[];this.pc=0;this.clock=0;this.tracks=new Map();this.offsets=new Map(data.commands.map((c,i)=>[c.offset,i]));this.camera={position:[0,0,-400],direction:[0,0,1],up:[0,1,0],fov:Math.PI/5,fog:[1,1,1,200,1100]};}
  tick(){
    const s=this.camera,vector=(c,i)=>c.floats.slice(i,i+3),fog=(c,i)=>{const n=c.ints[i]>>>0;return [((n>>>16)&255)/255,((n>>>8)&255)/255,(n&255)/255,c.floats[i+1],c.floats[i+2]];};
    for(let budget=0;budget<200&&this.data.commands[this.pc]?.time<=this.clock;budget++){
      const c=this.data.commands[this.pc++],I=c.ints,F=c.floats;
      if(c.op===1){this.pc=this.offsets.get(I[0])??this.pc;this.clock=I[1];continue;}
      if(c.op===2||c.op===4||c.op===6){const key=c.op===2?'position':c.op===4?'direction':'up';s[key]=vector(c,0);this.tracks.delete(key);}
      if(c.op===7)s.fov=F[0];if(c.op===8){s.fog=fog(c,0);this.tracks.delete('fog');}
      if([3,5,9,10,11].includes(c.op)){
        const key=c.op===3||c.op===10?'position':c.op===5||c.op===11?'direction':'fog',hermite=c.op===10||c.op===11;
        this.tracks.set(key,{from:[...s[key]],to:key==='fog'?fog(c,2):vector(c,hermite?5:2),duration:I[0],mode:I[1],age:0,hermite,a:vector(c,2),b:vector(c,8)});
      }
    }
    // The original STD interpreter advances each interpolation Timer before
    // evaluating it, as sampled directly through 0x4049a0/0x405900.
    for(const [key,t]of this.tracks){const n=Math.min(1,++t.age/(t.duration||1));s[key]=t.from.map((v,i)=>{const value=t.hermite?(2*n**3-3*n**2+1)*v+(-2*n**3+3*n**2)*t.to[i]+(n**3-2*n**2+n)*t.a[i]+(n**3-n**2)*t.b[i]:v+(t.to[i]-v)*ease(n,t.mode);return key==='fog'&&i<3?Math.trunc(value*255)/255:f32(value);});if(t.age>=t.duration)this.tracks.delete(key);}
    this.frames.push({position:[...s.position],direction:[...s.direction],up:[...s.up],fov:s.fov,fog:[...s.fog]});this.clock++;
  }
  sample(frame){frame=Math.max(0,Math.min(100000,frame|0));while(this.frames.length<=frame)this.tick();return this.frames[frame];}
  geometry(frame){
    const camera=this.sample(frame),z=unit(camera.direction),x=unit(cross(camera.up,z)),y=cross(z,x),viewport=this.data.viewport??[13,0,422,480],viewportCenter=[viewport[0]+viewport[2]/2,viewport[1]+viewport[3]/2],focal=viewport[3]/2/Math.tan(camera.fov/2),triangles=[];
    for(const instance of this.data.instances){const object=this.data.objects[instance.object];
      for(const q of object.quads){if(q.kind&&q.kind!==0)continue;const animation=this.animations.sample(this.bank,q.script,frame),sprite=this.atlases[this.bank].sprites[animation?.sprite];
        if(!sprite||!animation.visible||animation.alpha<=0)continue;const rotation=[animation.rx,animation.ry,animation.rotation],center=q.position.map((v,i)=>v+instance.position[i]+[animation.x,animation.y,animation.z][i]),size=primitiveSize(q,animation,sprite);
        let polygon=[[-.5,-.5,0,0],[.5,-.5,1,0],[.5,.5,1,1],[-.5,.5,0,1]].map(([a,b,u,v])=>{
          let px=a*size[0],py=b*size[1],pz=0;
          // ANM mode 8: the STD primitive determines dimensions, and the
          // script supplies the rotation, including the mirrored snow edges.
          const cx=Math.cos(rotation[0]),sx=Math.sin(rotation[0]),cy=Math.cos(rotation[1]),sy=Math.sin(rotation[1]),cz=Math.cos(rotation[2]),sz=Math.sin(rotation[2]);
          [py,pz]=[py*cx-pz*sx,py*sx+pz*cx];[px,pz]=[px*cy+pz*sy,-px*sy+pz*cy];[px,py]=[px*cz-py*sz,px*sz+py*cz];
          const d=sub([center[0]+px,center[1]+py,center[2]+pz],camera.position);return {cx:dot(d,x),cy:dot(d,y),depth:dot(d,z),distance:Math.hypot(...d),u:sprite.x+u*sprite.w,v:sprite.y+v*sprite.h};
        });
        // Clip the near plane before projection; never discard a whole tile
        // just because one corner is behind the camera.
        for(const [plane,keep]of [[this.data.near??30,d=>d>=0],[this.data.far??1800,d=>d<=0]]){const clipped=[];for(let i=0;i<polygon.length;i++){const a=polygon[i],b=polygon[(i+1)%polygon.length],inside=keep(a.depth-plane);if(inside)clipped.push(a);if(inside!==keep(b.depth-plane)){const t=(plane-a.depth)/(b.depth-a.depth);clipped.push(Object.fromEntries(Object.keys(a).map(k=>[k,a[k]+(b[k]-a[k])*t])));}}polygon=clipped;}
        const texture=this.atlases[this.bank].entries[sprite.entry];
        // Original device setup disables table fog and selects linear vertex
        // fog (0x451326..0x45134f); range fog is not enabled. Use view Z.
        polygon=polygon.map(p=>({...p,x:viewportCenter[0]+p.cx/p.depth*focal,y:viewportCenter[1]-p.cy/p.depth*focal,u:p.u+animation.u*texture.width,v:p.v+animation.v*texture.height,alpha:animation.alpha,r:animation.r,g:animation.g,b:animation.b,fog:Math.max(0,Math.min(1,(p.depth-camera.fog[3])/(camera.fog[4]-camera.fog[3])))}));
        if(polygon.length<3||polygon.every(p=>p.x<32)||polygon.every(p=>p.x>416)||polygon.every(p=>p.y<16)||polygon.every(p=>p.y>464))continue;
        for(let i=1;i<polygon.length-1;i++)triangles.push({bank:this.bank,layer:object.layer,entry:sprite.entry,blend:animation.blend,point:animation.point,vertices:[polygon[0],polygon[i],polygon[i+1]]});
      }
    }
    // STD tiles share a plane; sorting also makes their few z offsets stable.
    triangles.sort((a,b)=>a.layer-b.layer||b.vertices.reduce((s,v)=>s+v.depth,0)-a.vertices.reduce((s,v)=>s+v.depth,0));return {camera,triangles};
  }
}
