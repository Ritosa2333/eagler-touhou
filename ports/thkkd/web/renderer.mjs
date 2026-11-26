// 绘制命令由逻辑步产生；额外呈现帧只读取命令快照。
const triangleVertices=[0,1,2,0,2,3];
const whiteTint=[1,1,1,1],flatDepth=[1,.5,1,.5,1,.5,1,.5];
export class Renderer {
  constructor(canvas){
    this.canvas=canvas;this.gl=canvas.getContext('webgl2',{alpha:false,antialias:false,preserveDrawingBuffer:true});
    if(!this.gl)throw Error('海惠堂需要 WebGL 2');
    const gl=this.gl;
    const shader=(kind,source)=>{const s=gl.createShader(kind);gl.shaderSource(s,source);gl.compileShader(s);if(!gl.getShaderParameter(s,gl.COMPILE_STATUS))throw Error(gl.getShaderInfoLog(s));return s;};
    const program=gl.createProgram();
    gl.attachShader(program,shader(gl.VERTEX_SHADER,`#version 300 es
      in vec2 position;in vec2 uv;in vec4 color;in vec2 depth;out vec2 texcoord;out vec4 tint;
      void main(){gl_Position=vec4((position.x/320.-1.)*depth.x,(1.-position.y/240.)*depth.x,(depth.y*2.-1.)*depth.x,depth.x);texcoord=uv;tint=color;}`));
    gl.attachShader(program,shader(gl.FRAGMENT_SHADER,`#version 300 es
      precision mediump float;uniform sampler2D image;uniform bool opaque;in vec2 texcoord;in vec4 tint;out vec4 outputColor;
      void main(){vec4 t=texture(image,texcoord);outputColor=vec4(t.rgb*tint.rgb,(opaque?1.:t.a)*tint.a);}`));
    gl.linkProgram(program);if(!gl.getProgramParameter(program,gl.LINK_STATUS))throw Error(gl.getProgramInfoLog(program));
    gl.useProgram(program);this.program=program;this.opaque=gl.getUniformLocation(program,'opaque');
    this.buffer=gl.createBuffer();gl.bindBuffer(gl.ARRAY_BUFFER,this.buffer);
    for(const [name,n,offset] of [['position',2,0],['uv',2,8],['color',4,16],['depth',2,32]]){const loc=gl.getAttribLocation(program,name);gl.enableVertexAttribArray(loc);gl.vertexAttribPointer(loc,n,gl.FLOAT,false,40,offset);}
    this.white=this.texture({width:1,height:1,data:new Uint8Array([255,255,255,255])});
    gl.enable(gl.BLEND);gl.enable(gl.SCISSOR_TEST);gl.disable(gl.DEPTH_TEST);
    this.commands=[];this.current=[];this.brightness=[1,1,1];this.mode=0;this.alpha=1;this.area=[0,0,640,480];this.textures=new Set([this.white]);this.identities=new Map();this.previous=new Map();this.identity=null;
    this.vertices=new Float32Array(60*256);
  }
  texture(image){
    const gl=this.gl,texture=gl.createTexture();gl.bindTexture(gl.TEXTURE_2D,texture);
    gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL,false);
    if(image.data)gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,image.width,image.height,0,gl.RGBA,gl.UNSIGNED_BYTE,image.data);
    else gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,image);
    gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.LINEAR);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_S,gl.CLAMP_TO_EDGE);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_T,gl.CLAMP_TO_EDGE);
    const graph={texture,width:image.videoWidth||image.width,height:image.videoHeight||image.height};this.textures?.add(graph);return graph;
  }
  releaseTexture(graph){const texture=graph?.texture;if(!texture)return;this.gl.deleteTexture(texture);for(const item of this.textures)if(item.texture===texture)this.textures.delete(item);}
  clear(){this.current.length=0;this.identities.clear();}
  add(texture,points,uv=null,transparent=true,color=whiteTint,depth=flatDepth,vertexColors=null){
    if(!uv){const u=(texture.x||0)/texture.width,v=(texture.y||0)/texture.height,w=(texture.w??texture.width)/texture.width,h=(texture.h??texture.height)/texture.height;uv=[u,v,u+w,v,u+w,v+h,u,v+h];}
    const tint=[this.brightness[0]*color[0],this.brightness[1]*color[1],this.brightness[2]*color[2],(this.mode===0?1:this.alpha)*color[3]];
    let identity=null;if(this.identity){const n=this.identities.get(this.identity)||0;this.identities.set(this.identity,n+1);identity=this.identity+':'+n;}
    this.current.push({texture,points,uv,tint,depth,vertexColors,identity,opaque:!transparent,mode:this.mode,area:[...this.area]});
  }
  sprite(graph,x,y,scale=1,angle=0,turn=false,center=true,transparent=true){
    if(!graph||!Number.isFinite(scale)||!Number.isFinite(angle)||scale===0)return;
    const w=graph.w??graph.width,h=graph.h??graph.height,c=Math.cos(angle)*scale,s=Math.sin(angle)*scale;
    const left=center?-w/2:0,right=center?w/2:w,top=center?-h/2:0,bottom=center?h/2:h;
    const points=[x+left*c-top*s,y+left*s+top*c,x+right*c-top*s,y+right*s+top*c,
      x+right*c-bottom*s,y+right*s+bottom*c,x+left*c-bottom*s,y+left*s+bottom*c];
    const tw=graph.width,th=graph.height,u=(graph.x??0)/tw,v=(graph.y??0)/th,du=w/tw,dv=h/th;
    const uv=turn?[u+du,v,u,v,u,v+dv,u+du,v+dv]:[u,v,u+du,v,u+du,v+dv,u,v+dv];
    this.add(graph,points,uv,transparent);
  }
  box(x,y,r,b,color,fill=true){
    if(!fill){this.line(x,y,r,y,color);this.line(r,y,r,b,color);this.line(r,b,x,b,color);this.line(x,b,x,y,color);return;}
    this.add(this.white,[x,y,r,y,r,b,x,b],undefined,true,color);
  }
  line(x,y,r,b,color,width=1){const a=Math.atan2(b-y,r-x),dx=Math.sin(a)*width/2,dy=Math.cos(a)*width/2;this.add(this.white,[x+dx,y-dy,r+dx,b-dy,r-dx,b+dy,x-dx,y+dy],undefined,true,color);}
  triangle(points,color){const [a,b,c,d,e,f]=points;this.add(this.white,[a,b,c,d,e,f,e,f],undefined,true,color);}
  circle(x,y,r,color,fill=true){if(r<0)return;const n=Math.max(12,Math.min(96,Math.ceil(r)));for(let i=0;i<n;i++){const a=i*Math.PI*2/n,b=(i+1)*Math.PI*2/n;if(fill)this.triangle([x,y,x+Math.cos(a)*r,y+Math.sin(a)*r,x+Math.cos(b)*r,y+Math.sin(b)*r],color);else this.line(x+Math.cos(a)*r,y+Math.sin(a)*r,x+Math.cos(b)*r,y+Math.sin(b)*r,color);}}
  commit(){
    const continuous=this.committedScene===this.scene&&this.tick===this.committedTick+1;
    this.previous??=new Map();this.previous.clear();
    if(continuous)for(const command of this.commands)if(command.identity)this.previous.set(command.identity,command);
    this.commands=this.current;this.current=[];this.committedTick=this.tick;this.committedScene=this.scene;
  }
  present(alpha=0,commands=this.commands){
    const gl=this.gl;gl.viewport(0,0,640,480);gl.scissor(0,0,640,480);gl.clearColor(0,0,0,1);gl.clear(gl.COLOR_BUFFER_BIT);
    let used=0,batchCount=0,last;
    const batches=this.batches??=[],required=commands.length*60;
    if(required>this.vertices.length)this.vertices=new Float32Array(Math.max(this.vertices.length*2,required));
    for(const command of commands){
      if(!last||command.texture.texture!==last.texture.texture||command.mode!==last.mode||command.opaque!==last.opaque||command.area[0]!==last.area[0]||command.area[1]!==last.area[1]||command.area[2]!==last.area[2]||command.area[3]!==last.area[3]){
        if(batchCount)batches[batchCount-1].count=used/10-batches[batchCount-1].first;
        const batch=batches[batchCount]??={};batch.command=command;batch.first=used/10;batchCount++;last=command;
      }
      const previous=alpha>0&&command.identity?this.previous.get(command.identity):null;
      const compatible=previous&&previous.texture.texture===command.texture.texture&&previous.mode===command.mode&&previous.uv.every((value,i)=>value===command.uv[i])&&command.points.every((value,i)=>Math.abs(value-previous.points[i])<64);
      for(const i of triangleVertices){
        const x=command.points[i*2],y=command.points[i*2+1],px=compatible?x+(x-previous.points[i*2])*Math.min(1,alpha):x,py=compatible?y+(y-previous.points[i*2+1])*Math.min(1,alpha):y;
        this.vertices[used++]=px;this.vertices[used++]=py;
        this.vertices[used++]=command.uv[i*2];this.vertices[used++]=command.uv[i*2+1];
        for(let n=0;n<4;n++)this.vertices[used++]=command.tint[n]*(command.vertexColors?.[i*4+n]??1);
        this.vertices[used++]=command.depth[i*2];this.vertices[used++]=command.depth[i*2+1];
      }
    }
    if(batchCount){
      batches[batchCount-1].count=used/10-batches[batchCount-1].first;
      gl.bufferData(gl.ARRAY_BUFFER,this.vertices.subarray(0,used),gl.STREAM_DRAW);
      let texture,opaque,equation,source,destination,area;
      for(let i=0;i<batchCount;i++){
        const batch=batches[i],command=batch.command,mode=command.mode;
        if(i===0||texture!==command.texture.texture){texture=command.texture.texture;gl.bindTexture(gl.TEXTURE_2D,texture);}
        if(i===0||opaque!==command.opaque){opaque=command.opaque;gl.uniform1i(this.opaque,+opaque);}
        const nextEquation=mode===3?gl.FUNC_REVERSE_SUBTRACT:gl.FUNC_ADD;
        if(equation!==nextEquation){equation=nextEquation;gl.blendEquation(equation);}
        const nextSource=mode===4?gl.ZERO:gl.SRC_ALPHA,nextDestination=mode===4?gl.SRC_COLOR:mode===2||mode===3?gl.ONE:gl.ONE_MINUS_SRC_ALPHA;
        if(source!==nextSource||destination!==nextDestination){source=nextSource;destination=nextDestination;gl.blendFunc(source,destination);}
        const nextArea=command.area;
        if(!area||area[0]!==nextArea[0]||area[1]!==nextArea[1]||area[2]!==nextArea[2]||area[3]!==nextArea[3]){
          area=nextArea;gl.scissor(Math.max(0,area[0]),Math.max(0,480-area[3]),Math.max(0,area[2]-area[0]),Math.max(0,area[3]-area[1]));
        }
        gl.drawArrays(gl.TRIANGLES,batch.first,batch.count);
      }
    }
    batches.length=batchCount;gl.flush();this.onPresent?.(performance.now());
  }
  destroy(){const gl=this.gl;for(const item of this.textures)gl.deleteTexture(item.texture);gl.deleteBuffer(this.buffer);gl.deleteProgram(this.program);gl.getExtension('WEBGL_lose_context')?.loseContext();}
}
export const rgb=value=>[(value>>>16&255)/255,(value>>>8&255)/255,(value&255)/255,1];
