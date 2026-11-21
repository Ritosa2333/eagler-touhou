const width=640,height=480;
const finite=value=>Number.isFinite(value)&&Math.abs(value)<=65536;
function bytes(input,name){
    if(!(input instanceof Uint8Array))throw new Error('Invalid original '+name+' bytes');
    return new DataView(input.buffer,input.byteOffset,input.byteLength);
}
function originalTextures(input){
    const view=bytes(input,'ANM'),output=[];let base=0;
    for(let count=0;count<64;count++){
        if(base+64>input.length||view.getUint32(base,true)!==7)throw new Error('Invalid original ending ANM entry');
        const next=view.getUint32(base+36,true),offset=base+view.getUint32(base+28,true);
        if(offset+16>input.length||view.getUint32(offset,true)!==0x58544854||view.getUint16(offset+6,true)!==3)
            throw new Error('Unsupported original ending texture format');
        const textureWidth=view.getUint16(offset+8,true),textureHeight=view.getUint16(offset+10,true);
        if(!textureWidth||!textureHeight||textureWidth>4096||textureHeight>4096
            ||offset+16+textureWidth*textureHeight*2>input.length)throw new Error('Original ending texture extent rejected');
        const rgba=new Uint8Array(textureWidth*textureHeight*4);
        for(let pixel=0;pixel<textureWidth*textureHeight;pixel++){
            const value=view.getUint16(offset+16+pixel*2,true),red=value>>>11&31,green=value>>>5&63,blue=value&31;
            rgba[pixel*4]=(red<<3)|(red>>>2);rgba[pixel*4+1]=(green<<2)|(green>>>4);
            rgba[pixel*4+2]=(blue<<3)|(blue>>>2);rgba[pixel*4+3]=255;
        }
        output.push({width:textureWidth,height:textureHeight,declaredWidth:view.getUint16(base+10,true),
            declaredHeight:view.getUint16(base+12,true),rgba});
        if(!next)return output;
        if(next<64||base+next<=base||base+next>=input.length)throw new Error('Original ending ANM chain rejected');
        base+=next;
    }
    throw new Error('Original ending ANM entry budget exhausted');
}
export function decodeEndingGlyph(input){
    const view=bytes(input,'glyph');
    if(input.length!==512*512*2)throw new Error('Invalid original ending glyph texture size');
    const rgba=new Uint8Array(512*512*4);
    for(let pixel=0;pixel<512*512;pixel++){
        const value=view.getUint16(pixel*2,true);
        rgba[pixel*4]=(value>>>8&15)*17;rgba[pixel*4+1]=(value>>>4&15)*17;
        rgba[pixel*4+2]=(value&15)*17;rgba[pixel*4+3]=(value>>>12)*17;
    }
    return{width:512,height:512,rgba};
}
function blend(frame,texture,rectangle,pose){
    const [left,top,right,bottom]=rectangle;
    if(!rectangle.every(Number.isInteger)||left<0||top<0||right>texture.width||bottom>texture.height
        ||right<left||bottom<top||![pose.sx,pose.sy,pose.x,pose.y,pose.alpha].every(finite)
        ||pose.sx<0||pose.sy<0||pose.sx>4||pose.sy>4||pose.alpha<0||pose.alpha>1)
        throw new Error('Original ending quad domain rejected');
    const sourceWidth=right-left,sourceHeight=bottom-top;
    const scaledWidth=Math.round(Math.fround(sourceWidth*pose.sx)),scaledHeight=Math.round(Math.fround(sourceHeight*pose.sy));
    if(!scaledWidth||!scaledHeight)return;
    const positionX=pose.anchorX===1?Math.round(pose.x):pose.anchorX===2?Math.round(pose.x-scaledWidth):Math.floor(pose.x-scaledWidth/2);
    const positionY=pose.anchorY===1?Math.round(pose.y):pose.anchorY===2?Math.round(pose.y-scaledHeight):Math.floor(pose.y-scaledHeight/2);
    for(let row=0;row<scaledHeight;row++)for(let column=0;column<scaledWidth;column++){
        const targetX=positionX+column,targetY=positionY+row;
        if(targetX<0||targetX>=width||targetY<0||targetY>=height)continue;
        const sourceX=left+Math.min(sourceWidth-1,Math.floor((column+.5)*sourceWidth/scaledWidth));
        const sourceY=top+Math.min(sourceHeight-1,Math.floor((row+.5)*sourceHeight/scaledHeight));
        const source=(sourceY*texture.width+sourceX)*4,destination=(targetY*width+targetX)*4;
        const alpha=texture.rgba[source+3]/255*pose.alpha;
        for(let channel=0;channel<3;channel++)frame[destination+channel]=Math.round(
            texture.rgba[source+channel]*alpha+frame[destination+channel]*(1-alpha));
    }
}
export class OriginalEndingPixels{
    constructor(base,art){
        this.base=originalTextures(base);this.art=originalTextures(art);
    }
    frame(state,glyph){
        if(!state||!Number.isInteger(state.index)||state.index<0||state.index>5
            ||!Array.isArray(state.draws)||!Array.isArray(state.texts)||state.draws.length>1024||state.texts.length>5)
            throw new Error('Original ending draw snapshot rejected');
        const frame=new Uint8Array(width*height*4);
        for(let pixel=0;pixel<width*height;pixel++)frame[pixel*4+3]=255;
        const ordered=state.draws.filter(pose=>pose.visible&&pose.sprite>=0&&pose.alpha>0).sort((first,second)=>first.layer-second.layer);
        for(const pose of ordered){
            if(pose.bank!==32&&pose.bank!==33)throw new Error('Original ending draw bank rejected');
            const texture=(pose.bank===32?this.base:this.art)[pose.textureEntry];
            if(!texture)throw new Error('Original ending texture owner is unavailable');
            blend(frame,texture,[pose.u0*texture.declaredWidth,pose.v0*texture.declaredHeight,
                pose.u1*texture.declaredWidth,pose.v1*texture.declaredHeight],pose);
        }
        for(const text of state.texts)if(text.visible&&text.alpha>0){
            if(!glyph||glyph.width!==512||glyph.height!==512||text.script!==68+text.slot||text.sprite!==4+text.slot)
                throw new Error('Original ending glyph owner or slot is unavailable');
            blend(frame,glyph,[0,1+23*text.slot,512,21+23*text.slot],{...text,anchorX:1,anchorY:1});
        }
        if(state.fade?.active&&state.fade.alpha){
            if(!Number.isInteger(state.fade.alpha)||state.fade.alpha<0||state.fade.alpha>255)
                throw new Error('Original ending fade alpha rejected');
            for(let pixel=0;pixel<width*height;pixel++)for(let channel=0;channel<3;channel++)
                frame[pixel*4+channel]=Math.round(frame[pixel*4+channel]*(255-state.fade.alpha)/255);
        }
        return frame;
    }
}
