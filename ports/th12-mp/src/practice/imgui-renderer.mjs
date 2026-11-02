// Render cached native ImDrawData. Widget state and input stay in C++ ImGui.
export class ImGuiRenderer {
  constructor(core,canvas){
    this.core=core;this.canvas=canvas;
    const gl=this.gl=canvas.getContext('webgl2',{alpha:true,premultipliedAlpha:true,antialias:false});
    if(!gl)throw Error('THPrac needs WebGL2');
    const shader=(kind,text)=>{const value=gl.createShader(kind);gl.shaderSource(value,text);gl.compileShader(value);if(!gl.getShaderParameter(value,gl.COMPILE_STATUS))throw Error(gl.getShaderInfoLog(value));return value;};
    const vertex=shader(gl.VERTEX_SHADER,`#version 300 es
      in vec2 position;in vec2 uv;in vec4 color;out vec2 texcoord;out vec4 tint;
      void main(){texcoord=uv;tint=color.bgra;gl_Position=vec4(position.x/320.-1.,1.-position.y/240.,0,1);}`);
    const fragment=shader(gl.FRAGMENT_SHADER,`#version 300 es
      precision mediump float;uniform sampler2D atlas;in vec2 texcoord;in vec4 tint;out vec4 outputColor;
      void main(){outputColor=tint*texture(atlas,texcoord);}`);
    this.program=gl.createProgram();gl.attachShader(this.program,vertex);gl.attachShader(this.program,fragment);gl.linkProgram(this.program);
    if(!gl.getProgramParameter(this.program,gl.LINK_STATUS))throw Error(gl.getProgramInfoLog(this.program));
    gl.deleteShader(vertex);gl.deleteShader(fragment);
    this.attributes=['position','uv','color'].map(name=>gl.getAttribLocation(this.program,name));
    this.vao=gl.createVertexArray();this.vertices=gl.createBuffer();this.indices=gl.createBuffer();this.texture=gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D,this.texture);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.LINEAR);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_S,gl.CLAMP_TO_EDGE);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_T,gl.CLAMP_TO_EDGE);
    const info=new Uint32Array(core.HEAPU8.buffer,core._th12_practice_ui_font(),3),[pointer,width,height]=info;
    if(width>gl.getParameter(gl.MAX_TEXTURE_SIZE)||height>gl.getParameter(gl.MAX_TEXTURE_SIZE))throw Error('THPrac font atlas exceeds this device texture limit');
    gl.pixelStorei(gl.UNPACK_ALIGNMENT,1);gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,width,height,0,gl.RGBA,gl.UNSIGNED_BYTE,core.HEAPU8.subarray(pointer,pointer+width*height*4));
  }
  render(){
    const gl=this.gl,heap=this.core.HEAPU8;
    const [vertices,indices,commands,vertexCount,indexCount,commandCount,stride,indexSize]=new Uint32Array(heap.buffer,this.core._th12_practice_ui_draw(),8);
    gl.viewport(0,0,640,480);gl.disable(gl.SCISSOR_TEST);gl.clearColor(0,0,0,0);gl.clear(gl.COLOR_BUFFER_BIT);
    if(!commandCount)return;
    if(stride!==20||![2,4].includes(indexSize)||vertices+vertexCount*stride>heap.length||indices+indexCount*indexSize>heap.length||commands+commandCount*28>heap.length)throw Error('THPrac draw data is invalid');
    gl.useProgram(this.program);gl.bindVertexArray(this.vao);gl.bindBuffer(gl.ARRAY_BUFFER,this.vertices);gl.bufferData(gl.ARRAY_BUFFER,heap.subarray(vertices,vertices+vertexCount*stride),gl.STREAM_DRAW);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,this.indices);gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,heap.subarray(indices,indices+indexCount*indexSize),gl.STREAM_DRAW);
    gl.activeTexture(gl.TEXTURE0);gl.bindTexture(gl.TEXTURE_2D,this.texture);gl.uniform1i(gl.getUniformLocation(this.program,'atlas'),0);
    gl.enable(gl.BLEND);gl.blendEquation(gl.FUNC_ADD);gl.blendFuncSeparate(gl.SRC_ALPHA,gl.ONE_MINUS_SRC_ALPHA,gl.ONE,gl.ONE_MINUS_SRC_ALPHA);
    gl.disable(gl.CULL_FACE);gl.disable(gl.DEPTH_TEST);gl.enable(gl.SCISSOR_TEST);
    const view=new DataView(heap.buffer,commands,commandCount*28);
    for(let index=0;index<commandCount;index++){
      const at=index*28,count=view.getUint32(at,true),start=view.getUint32(at+4,true),base=view.getUint32(at+8,true);
      const x=Math.max(0,view.getFloat32(at+12,true)),y=Math.max(0,view.getFloat32(at+16,true));
      const right=Math.min(640,view.getFloat32(at+20,true)),bottom=Math.min(480,view.getFloat32(at+24,true));
      if(right<=x||bottom<=y)continue;
      gl.scissor(Math.floor(x),Math.floor(480-bottom),Math.ceil(right-x),Math.ceil(bottom-y));
      this.attributes.forEach(location=>gl.enableVertexAttribArray(location));
      gl.vertexAttribPointer(this.attributes[0],2,gl.FLOAT,false,stride,base*stride);
      gl.vertexAttribPointer(this.attributes[1],2,gl.FLOAT,false,stride,base*stride+8);
      gl.vertexAttribPointer(this.attributes[2],4,gl.UNSIGNED_BYTE,true,stride,base*stride+16);
      gl.drawElements(gl.TRIANGLES,count,indexSize===2?gl.UNSIGNED_SHORT:gl.UNSIGNED_INT,start*indexSize);
    }
    gl.disable(gl.SCISSOR_TEST);
  }
  destroy(){const gl=this.gl;gl.deleteTexture(this.texture);gl.deleteBuffer(this.vertices);gl.deleteBuffer(this.indices);gl.deleteVertexArray(this.vao);gl.deleteProgram(this.program);}
}
