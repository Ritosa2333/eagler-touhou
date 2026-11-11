function transform(wat){
  wat=wat.replace('(module','(module\n  (import "a" "__present565" (func $present565 (param i32 i32) (result i32)))');
  let count=0;
  wat=wat.replace(/        i32.const 187916\n        i32.load\n        i32.const 0\n        local.get (\$\w+)\n        i32.load offset=20\n        local.get \1\n        i32.load offset=16\n        call \$f560\n        drop\n        i32.const 187912\n        i32.load\n        call \$f1107\n        i32.const 187912\n        i32.load\n        i32.const 187916\n        i32.load\n        call \$f788\n        drop\n        i32.const 187912\n        i32.load\n        call \$f787/g,(original,surface)=>{count++;return `        local.get ${surface}\n        i32.load offset=20\n        local.get ${surface}\n        i32.load offset=16\n        call $present565\n        i32.eqz\n        if\n${original}\n        end`;});
  if(count!==2)throw Error('Presentation anchors changed: '+count);return wat;
}
function injection(){return `
let mobilePresenter;
function mobilePresent565(pixels,pitch){
  const gl=GLctx;if(!gl||pitch!==1280)return 0;
  if(gl.isContextLost()){mobilePresenter=null;return 1;}
  if(!mobilePresenter){
    const compile=(type,source)=>{const s=gl.createShader(type);gl.shaderSource(s,source);gl.compileShader(s);if(!gl.getShaderParameter(s,gl.COMPILE_STATUS))throw Error(gl.getShaderInfoLog(s));return s;};
    const p=gl.createProgram();
    const vs=compile(gl.VERTEX_SHADER,'attribute vec2 a;varying vec2 uv;void main(){gl_Position=vec4(a,0.,1.);uv=vec2((a.x+1.)*.5,(1.-a.y)*.5);}');
    const fs=compile(gl.FRAGMENT_SHADER,'precision mediump float;varying vec2 uv;uniform sampler2D tex;void main(){gl_FragColor=vec4(texture2D(tex,uv).rgb,1.);}');
    gl.attachShader(p,vs);gl.attachShader(p,fs);gl.linkProgram(p);if(!gl.getProgramParameter(p,gl.LINK_STATUS))throw Error(gl.getProgramInfoLog(p));gl.deleteShader(vs);gl.deleteShader(fs);
    const b=gl.createBuffer();gl.bindBuffer(gl.ARRAY_BUFFER,b);gl.bufferData(gl.ARRAY_BUFFER,new Float32Array([-1,-1,1,-1,-1,1,1,1]),gl.STATIC_DRAW);
    const t=gl.createTexture();gl.activeTexture(gl.TEXTURE0);gl.bindTexture(gl.TEXTURE_2D,t);
    gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.NEAREST);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.NEAREST);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_S,gl.CLAMP_TO_EDGE);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_T,gl.CLAMP_TO_EDGE);
    gl.pixelStorei(gl.UNPACK_ALIGNMENT,2);gl.texImage2D(gl.TEXTURE_2D,0,gl.RGB,640,400,0,gl.RGB,gl.UNSIGNED_SHORT_5_6_5,null);
    const a=gl.getAttribLocation(p,'a');for(let i=0;i<gl.getParameter(gl.MAX_VERTEX_ATTRIBS);i++)if(i!==a)gl.disableVertexAttribArray(i);
    mobilePresenter={p,b,t,a,sampler:gl.getUniformLocation(p,'tex')};
  }
  const r=mobilePresenter;
  gl.bindFramebuffer(gl.FRAMEBUFFER,null);gl.viewport(0,0,gl.drawingBufferWidth,gl.drawingBufferHeight);
  gl.disable(gl.SCISSOR_TEST);gl.disable(gl.BLEND);gl.disable(gl.DEPTH_TEST);gl.disable(gl.CULL_FACE);gl.colorMask(true,true,true,true);
  gl.useProgram(r.p);gl.bindBuffer(gl.ARRAY_BUFFER,r.b);gl.enableVertexAttribArray(r.a);gl.vertexAttribPointer(r.a,2,gl.FLOAT,false,0,0);
  gl.activeTexture(gl.TEXTURE0);gl.bindTexture(gl.TEXTURE_2D,r.t);gl.pixelStorei(gl.UNPACK_ALIGNMENT,2);gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL,false);
  gl.texSubImage2D(gl.TEXTURE_2D,0,0,0,640,400,gl.RGB,gl.UNSIGNED_SHORT_5_6_5,HEAPU16.subarray(pixels>>>1,(pixels>>>1)+256000));
  gl.uniform1i(r.sampler,0);gl.drawArrays(gl.TRIANGLE_STRIP,0,4);Module.frameUploads=(Module.frameUploads||0)+1;return 1;
}
`;}
module.exports={transform,injection};
