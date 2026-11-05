// Isolated candidate only: retain native SDL RGBA bytes through the GPU upload.
const fs=require('node:fs'),path=require('node:path');
const root=path.resolve(__dirname,'..');
let source=fs.readFileSync(path.join(root,'web/frame-presenter.js'),'utf8');
function replace(before,after){if(source.split(before).length!==2)throw Error('Native framebuffer anchor changed: '+before);source=source.replace(before,after);}
replace('let width=0,height=0;', 'let width=0,height=0,textureBytes=0,nativeBufferPtr=0,nativeCapacity=0;');
replace('uniform sampler2D image;', 'uniform sampler2D image; uniform bool swapRB;');
replace('gl_FragColor=vec4(texture2D(image,texcoord).rgb,1.0);', 'vec3 color=texture2D(image,texcoord).rgb; gl_FragColor=vec4(swapRB?color.bgr:color,1.0);');
replace("const sampler=gl.getUniformLocation(program,'image'),buffer=", "const swap=gl.getUniformLocation(program,'swapRB'),sampler=gl.getUniformLocation(program,'image'),buffer=");
replace('const {frame,frameWidth,frameHeight}=latest,size=frameWidth*frameHeight*3;', 'const {frame,frameWidth,frameHeight}=latest,bytesPerPixel=latest.bytesPerPixel||3,size=frameWidth*frameHeight*bytesPerPixel,format=bytesPerPixel===4?gl.RGBA:gl.RGB;');
replace('gl.uniform1i(sampler,0);', 'gl.uniform1i(sampler,0);gl.uniform1i(swap,latest.swapRB?1:0);');
replace('if(width!==frameWidth||height!==frameHeight){', 'if(width!==frameWidth||height!==frameHeight||textureBytes!==bytesPerPixel){');
replace('width=frameWidth;height=frameHeight;', 'width=frameWidth;height=frameHeight;textureBytes=bytesPerPixel;');
replace('gl.texImage2D(gl.TEXTURE_2D,0,gl.RGB,width,height,0,gl.RGB,gl.UNSIGNED_BYTE,pixels);', 'gl.texImage2D(gl.TEXTURE_2D,0,format,width,height,0,format,gl.UNSIGNED_BYTE,pixels);');
replace('gl.texSubImage2D(gl.TEXTURE_2D,0,0,0,width,height,gl.RGB,gl.UNSIGNED_BYTE,pixels);', 'gl.texSubImage2D(gl.TEXTURE_2D,0,0,0,width,height,format,gl.UNSIGNED_BYTE,pixels);');
replace('mod.updateTexture=(frame,frameWidth,frameHeight)=>{', 'const submit=(frame,frameWidth,frameHeight,bytesPerPixel=3,swapRB=false)=>{');
replace('if(disposed)return;\n    metrics.submitted++;', 'if(disposed)return false;\n    metrics.submitted++;');
replace('latest={frame,frameWidth,frameHeight};dirty=true;', 'latest={frame,frameWidth,frameHeight,bytesPerPixel,swapRB};dirty=true;');
replace('if(!deferred)flush();\n  };', 'if(!deferred)flush();return true;\n  };\n  mod.updateTexture=(frame,width,height)=>submit(frame,width,height);');
replace('return {\n    flush,', `return {
    submitNative:(frame,width,height,bgra)=>{
      if(disposed)return false;
      const size=width*height*4;
      if(size<=0||frame<0||frame+size>mod.HEAPU8.length)throw Error('Native frame range is invalid');
      if(nativeCapacity<size){
        const next=mod._malloc(size);if(!next)throw Error('Native frame allocation failed');
        if(nativeBufferPtr)mod._free(nativeBufferPtr);
        nativeBufferPtr=next;nativeCapacity=size;
      }
      // The SDL surface can receive the next raster before the host presents.
      // Seal the completed frame with one bulk copy, without RGB conversion.
      mod.thDirtyMark?.(nativeBufferPtr,size);
      mod.HEAPU8.copyWithin(nativeBufferPtr,frame,frame+size);
      const accepted=submit(nativeBufferPtr,width,height,4,bgra);
      if(accepted)++mod.messageFrame;return accepted;
    },
    flush,`);
replace('capture:()=>latest?{...latest}:null,', 'capture:()=>latest?{...latest,nativeBufferPtr,nativeCapacity}:null,');
replace('restore(saved){latest=saved?{...saved}:null;', 'restore(saved){nativeBufferPtr=saved?.nativeBufferPtr||0;nativeCapacity=saved?.nativeCapacity||0;latest=saved?{...saved}:null;');
const out=path.join(root,'build/native-scheduler/runtime/frame-presenter-native.js');
fs.writeFileSync(out,source);console.log('Prepared native RGBA framebuffer presenter');
