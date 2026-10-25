// Snapshot-owned native XYZRHW/ARGB/UV storage. Keep double precision for
// display interpolation until VertexBatch performs the final float32 write.
const decoded=new WeakMap();
const verticesDescriptor={configurable:true,get(){
  let vertices=decoded.get(this);if(vertices)return vertices;
  const {floats,colors,offset,count}=this.vertexData;vertices=[];
  for(let i=0;i<count;i++){
    const at=(offset+i)*7;
    vertices.push({x:meshCoordinate(this,i,0),y:meshCoordinate(this,i,1),
      z:floats[at+2],rhw:floats[at+3],color:colors[at+4],u:floats[at+5],v:floats[at+6]});
  }
  decoded.set(this,vertices);return vertices;
}};

// The object-array view remains available to inspectors. It is deliberately
// non-enumerable so copying pose metadata never materializes every vertex.
export function copyMeshPose(p,changes){
  const result={...p,...changes};
  if(result.vertexData)Object.defineProperty(result,'vertices',verticesDescriptor);
  return result;
}
export const meshVertexCount=p=>p.vertexData?.count??p.vertices.length;
export function meshCoordinate(p,index,axis){
  if(p.vertexXY)return p.vertexXY[index*2+axis];
  if(p.vertexData)return p.vertexData.floats[(p.vertexData.offset+index)*7+axis];
  return axis===0?p.vertices[index].x:p.vertices[index].y;
}
export function interpolateMesh(p,previous,alpha){
  if(!p.vertexData)return {...p,vertices:p.vertices.map((v,i)=>({...v,
    x:meshCoordinate(previous,i,0)+(v.x-meshCoordinate(previous,i,0))*alpha,
    y:meshCoordinate(previous,i,1)+(v.y-meshCoordinate(previous,i,1))*alpha}))};
  const vertexXY=new Float64Array(p.vertexData.count*2);
  for(let i=0;i<p.vertexData.count;i++)for(let axis=0;axis<2;axis++){
    const a=meshCoordinate(previous,i,axis),b=meshCoordinate(p,i,axis);
    vertexXY[i*2+axis]=a+(b-a)*alpha;
  }
  return copyMeshPose(p,{vertexXY});
}
export function translateMesh(p,x,y,round=false){
  if(!p.vertexData)return {...p,vertices:p.vertices.map(v=>({...v,
    x:round?Math.fround(v.x+x):v.x+x,y:round?Math.fround(v.y+y):v.y+y}))};
  const vertexXY=new Float64Array(p.vertexData.count*2);
  for(let i=0;i<p.vertexData.count;i++){
    const px=meshCoordinate(p,i,0)+x,py=meshCoordinate(p,i,1)+y;
    vertexXY[i*2]=round?Math.fround(px):px;vertexXY[i*2+1]=round?Math.fround(py):py;
  }
  return copyMeshPose(p,{vertexXY});
}
