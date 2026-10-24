// One interleaved WebGL vertex: XY, UV, RGBA, clip W and fog.
// Capacity survives flushes and frames. Growth never splits an authored batch.
const quadIndices=[0,1,2,0,2,3];
export class VertexBatch {
  constructor(){
    this.data=new Float32Array(60*256);
    this.length=0;
  }
  push(x,y,u,v,r,g,b,a,depth,fog){
    const at=this.length;
    if(at+10>this.data.length){
      const next=new Float32Array(this.data.length*2);
      next.set(this.data);this.data=next;
    }
    const data=this.data;
    data[at]=x;data[at+1]=y;data[at+2]=u;data[at+3]=v;
    data[at+4]=r;data[at+5]=g;data[at+6]=b;data[at+7]=a;
    data[at+8]=depth;data[at+9]=fog;
    this.length=at+10;
  }
  quad(corners,u0,u1,v0,v1,flipY,r,g,b,a,ox,oy){
    let at=this.length;
    if(at+60>this.data.length){
      const next=new Float32Array(Math.max(this.data.length*2,at+60));
      next.set(this.data);this.data=next;
    }
    const data=this.data;
    for(const i of quadIndices){
      const u=i===1||i===2?u1:u0,v=i>=2?v1:v0;
      data[at]=corners[i][0]+ox;data[at+1]=corners[i][1]+oy;
      data[at+2]=u;data[at+3]=flipY?1-v:v;
      data[at+4]=r;data[at+5]=g;data[at+6]=b;data[at+7]=a;
      data[at+8]=1;data[at+9]=0;at+=10;
    }
    this.length=at;
  }
}
