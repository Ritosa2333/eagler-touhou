import {OriginalEndingBridge,originalEndingRequest} from './native-ending-bridge-v6.mjs';
export {originalEndingRequest};
async function sha256(bytes){
    const value=new Uint8Array(await globalThis.crypto.subtle.digest('SHA-256',bytes));
    return [...value].map(byte=>byte.toString(16).padStart(2,'0')).join('');
}
function encoded(value){return new TextEncoder().encode(JSON.stringify(value));}
function transactionDone(transaction){
    return new Promise((resolve,reject)=>{
        transaction.oncomplete=()=>resolve();
        transaction.onabort=()=>reject(transaction.error??new Error('Owned ending receipt transaction aborted'));
        transaction.onerror=()=>reject(transaction.error??new Error('Owned ending receipt transaction failed'));
    });
}
export class OwnedEndingReceipts{
    constructor(database){this.database=database;this.closed=false;this.count=0;}
    static async open(){
        if(typeof globalThis.indexedDB?.open!=='function')throw new Error('Strict owned ending receipt storage is unavailable');
        const name='th128-owned-ending-'+globalThis.crypto.randomUUID();
        const database=await new Promise((resolve,reject)=>{
            const opening=globalThis.indexedDB.open(name,1);
            opening.onupgradeneeded=()=>opening.result.createObjectStore('receipts',{keyPath:'key'});
            opening.onsuccess=()=>resolve(opening.result);opening.onerror=()=>reject(opening.error);
            opening.onblocked=()=>reject(new Error('Owned ending receipt storage is blocked'));
        });
        return new OwnedEndingReceipts(database);
    }
    async commit(key,metadata,payload=encoded(metadata)){return this.#commitBounded(key,metadata,payload,4*1024*1024);}
    async commitOrdinaryReplay(key,metadata,payload){
        if(!['ordinary-full-variable-replay-save','extra-full-variable-replay-save'].includes(metadata?.domain)||!Number.isSafeInteger(metadata.generation)||metadata.generation<1
            ||!Number.isSafeInteger(metadata.serial)||metadata.serial<1||!Number.isSafeInteger(metadata.logicalTick)||metadata.logicalTick<1
            ||!Number.isSafeInteger(metadata.index)||metadata.index<0||metadata.index>=25||!Number.isSafeInteger(metadata.rawBytes)||metadata.rawBytes<(metadata.domain==='extra-full-variable-replay-save'?299:580)||metadata.rawBytes>32*1024*1024
            ||!(payload instanceof Uint8Array)||payload.length!==metadata.rawBytes+128+32+3412)
            throw new Error('Ordinary complete replay receipt bounded domain rejected');
        return this.#commitBounded(key,metadata,payload,32*1024*1024+128+32+3412);
    }
    async #commitBounded(key,metadata,payload,maxBytes){
        if(this.closed||typeof key!=='string'||!key.length||!(payload instanceof Uint8Array)||payload.length>maxBytes)
            throw new Error('Owned ending durable receipt domain rejected');
        const body=Uint8Array.from(payload),digest=await sha256(body),transaction=this.database.transaction('receipts','readwrite',{durability:'strict'});
        if(transaction.durability!=='strict'){transaction.abort();throw new Error('Strict ending receipt durability was not granted');}
        const completed=transactionDone(transaction);
        transaction.objectStore('receipts').add({key,metadata,payload:body,bytes:body.length,sha256:digest});
        await completed;
        const readTransaction=this.database.transaction('receipts','readonly'),readCompleted=transactionDone(readTransaction);
        const value=await new Promise((resolve,reject)=>{
            const request=readTransaction.objectStore('receipts').get(key);
            request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);
        });
        await readCompleted;
        if(!value||value.bytes!==body.length||value.sha256!==digest||!(value.payload instanceof Uint8Array)
            ||value.payload.length!==body.length||!value.payload.every((byte,index)=>byte===body[index])
            ||JSON.stringify(value.metadata)!==JSON.stringify(metadata)||await sha256(value.payload)!==digest)
            throw new Error('Owned ending complete durable readback rejected');
        this.count++;return{committed:true,fullReadback:true,identitySHA256:metadata.identitySHA256??null,
            database:this.database.name,key,bytes:body.length,sha256:digest,durability:'strict-indexeddb-transaction',
            domain:metadata.domain,generation:metadata.generation,index:metadata.index,logicalTick:metadata.logicalTick,stopped:metadata.stopped};
    }
    close(){if(!this.closed){this.database.close();this.closed=true;}}
}
export function ordinaryWorldAudioIdentity(world){
    const terminal=world?.normalTerminal;
    if(!terminal?.configured||!terminal.begun)return null;
    const record=world.normalRecord,flow=world.flow,control=world.stageControl;
    if(terminal.ownerBound!==true||terminal.ownerClaimed!==true||terminal.ownerRecorded!==true||terminal.ownerMatches!==true
        ||terminal.provenance!=='ordinary-native-terminal-owner'||terminal.normalClear!==false||terminal.debugTainted!==false
        ||terminal.replayPlayback!==false||terminal.sessionRetired!==false||terminal.endingStarted!==false
        ||world.debug?.runTainted!==false||world.debug?.infiniteRespawn===true
        ||!Number.isSafeInteger(flow?.worldGeneration)||flow.worldGeneration<1||terminal.generation!==flow.worldGeneration
        ||!Number.isInteger(terminal.index)||terminal.index<0||terminal.index>=6
        ||[3,5,8,10,13,15][terminal.index]!==terminal.stage||flow.currentId!==terminal.stage
        ||!Number.isSafeInteger(world.frame)||world.frame<1||control?.current!==7||![7,15].includes(control.pending)||control.gate!==false
        ||record?.bound!==true||record.started!==true||record.tainted!==false||record.normalClear!==false
        ||record.provenance!=='ordinary-native-postrun'||record.generation!==flow.worldGeneration
        ||record.route!==terminal.index||record.rank!==flow.difficulty||!Number.isInteger(record.rank)||record.rank<0||record.rank>=4
        ||!Number.isSafeInteger(record.frames)||record.frames<1||!Number.isInteger(terminal.continues)||terminal.continues<0||terminal.continues>9
        ||!Number.isSafeInteger(terminal.score)||terminal.score<0||terminal.score>999999999)
        throw new Error('Ordinary post-MSG21 audio lacks its bound native record owner');
    return{generation:flow.worldGeneration,index:terminal.index,stage:terminal.stage,rank:record.rank,frame:world.frame,
        recordFrames:record.frames,score:terminal.score,continues:terminal.continues};
}
export class OrdinaryWorldAudioConsumer{
    constructor({module,audio,soundQueue,legacyMusic,receipts,isCurrent,presentAudio=true}){
        if(!module||!audio||audio.state==='closed'||presentAudio!==true||typeof isCurrent!=='function'
            ||typeof receipts?.commit!=='function'||typeof receipts?.close!=='function'
            ||typeof soundQueue?.assertIdentityHost!=='function'||typeof soundQueue?.consumeNativeRequest!=='function'
            ||typeof soundQueue?.flush!=='function'||typeof legacyMusic?.snapshot!=='function'
            ||typeof legacyMusic?.fadeOutFrames!=='function'||typeof legacyMusic?.fadeOut!=='function')
            throw new Error('Ordinary World audio requires actual sound/music and strict receipt consumers');
        for(const name of ['_th128_world_state','_th128_world_sound_epoch','_th128_world_sound_begin_window',
            '_th128_world_sound_window_state','_th128_world_sound_id','_th128_world_sound_x','_th128_world_sound_operation',
            '_th128_world_sound_ack_window','_th128_world_music_control_state','_th128_world_acknowledge_music_control'])
            if(typeof module[name]!=='function')throw new Error('Ordinary World audio native API unavailable: '+name);
        Object.assign(this,{module,audio,soundQueue,legacyMusic,receipts,isCurrent});
        this.closed=false;this.busy=false;this.failure=null;
        soundQueue.assertIdentityHost();this.boundOwner=this.current();
    }
    current(expected=null){
        if(this.closed||!this.isCurrent())throw new DOMException('Ordinary World audio owner was replaced','AbortError');
        if(this.failure)throw this.failure;
        const actual=ordinaryWorldAudioIdentity(JSON.parse(this.module.UTF8ToString(this.module._th128_world_state())));
        if(!actual||this.boundOwner&&['generation','index','stage','rank'].some(key=>actual[key]!==this.boundOwner[key])
            ||expected&&JSON.stringify(actual)!==JSON.stringify(expected))
            throw new DOMException('Ordinary World audio native frame or generation changed','AbortError');
        return actual;
    }
    track(){
        const value=this.legacyMusic.snapshot();
        if(!value||typeof value.track!=='string'||!/^th128_\d\d\.wav$/.test(value.track)
            ||!Number.isFinite(value.loopStart)||!Number.isFinite(value.loopEnd)||value.loopEnd<=value.loopStart)
            throw new Error('Ordinary World audio track identity unavailable');
        return{track:value.track,loopStart:value.loopStart,loopEnd:value.loopEnd};
    }
    sameTrack(before){if(JSON.stringify(this.track())!==JSON.stringify(before))throw new Error('Ordinary World audio track owner changed');}
    async commit(key,metadata){
        metadata.identitySHA256=await sha256(encoded(metadata));
        const payload=encoded(metadata),digest=await sha256(payload),receipt=await this.receipts.commit(key,metadata,payload);
        if(receipt?.committed!==true||receipt.fullReadback!==true||receipt.durability!=='strict-indexeddb-transaction'
            ||receipt.identitySHA256!==metadata.identitySHA256||receipt.bytes!==payload.length||receipt.sha256!==digest)
            throw new Error('Ordinary World audio strict complete payload receipt rejected');
        return receipt;
    }
    async sound(){
        if(this.busy)throw new Error('Ordinary World audio consumer is not serial');
        this.busy=true;
        try{
            const owner=this.current(),track=this.track(),module=this.module,queue=this.soundQueue;
            queue.assertIdentityHost();
            if(queue.context?.state!=='running')throw new Error('Actual ordinary sound context is not running');
            const prior=JSON.parse(module.UTF8ToString(module._th128_world_sound_window_state()));
            if(prior.serial!==0)throw new Error('Ordinary World audio cannot adopt an unowned sound window');
            const epoch=module._th128_world_sound_epoch(),serial=module._th128_world_sound_begin_window();
            if(serial===0)return null;
            const opened=JSON.parse(module.UTF8ToString(module._th128_world_sound_window_state()));
            if(!Number.isSafeInteger(epoch)||epoch<1||serial<1||opened.serial!==serial||opened.epoch!==epoch
                ||!Number.isSafeInteger(opened.count)||opened.count<1||opened.count>65536
                ||![opened.baseLow,opened.baseHigh].every(value=>Number.isInteger(value)&&value>=0&&value<=0xffffffff))
                throw new Error('Ordinary World sound native window identity rejected');
            const requests=[];
            for(let index=0;index<opened.count;++index){
                const identifier=module._th128_world_sound_id(index),position=module._th128_world_sound_x(index),operation=module._th128_world_sound_operation(index);
                if(!Number.isInteger(identifier)||identifier< -1||identifier>=76||!Number.isFinite(position)||![0,1,2].includes(operation)
                    ||identifier===-1&&operation!==0)throw new Error('Ordinary World sound request rejected');
                const accepted=queue.consumeNativeRequest(identifier,position,operation,{present:true});
                if(typeof accepted!=='boolean')throw new Error('Original sound consumer result is unavailable');
                requests.push({identifier,position,operation,accepted});
            }
            const flushed=queue.flush({present:true});
            const metadata={domain:'ordinary-world-sound',...owner,epoch,serial,count:opened.count,baseLow:opened.baseLow,baseHigh:opened.baseHigh,
                track,requests,flushed,actualQueueConsumedAndFlushed:true,actualAudibleDeviceOutputProved:false};
            const receipt=await this.commit('ordinary/sound/'+epoch+'/'+serial,metadata);
            this.current(owner);this.sameTrack(track);queue.assertIdentityHost();
            if(queue.context?.state!=='running'||module._th128_world_sound_epoch()!==epoch
                ||JSON.stringify(JSON.parse(module.UTF8ToString(module._th128_world_sound_window_state())))!==JSON.stringify(opened))
                throw new Error('Ordinary World sound window changed during full durable readback');
            const remaining=module._th128_world_sound_ack_window(serial,opened.count,opened.baseLow,opened.baseHigh,epoch);
            if(!Number.isInteger(remaining)||remaining<0)throw new Error('Ordinary World sound exact ACK rejected');
            return{...receipt,remaining};
        }catch(error){
            if(this.closed||!this.isCurrent())throw new DOMException('Ordinary World audio receipt owner was cancelled','AbortError');
            if(error.name!=='AbortError')this.failure=error;throw error;
        }
        finally{this.busy=false;}
    }
    async music(){
        if(this.busy)throw new Error('Ordinary World audio consumer is not serial');
        this.busy=true;
        try{
            const owner=this.current(),module=this.module,control=JSON.parse(module.UTF8ToString(module._th128_world_music_control_state()));
            if(!control||control.serial===0||control.acknowledged===true)return null;
            const before=JSON.parse(module.UTF8ToString(module._th128_world_state()));
            if(!Number.isSafeInteger(control.serial)||control.serial<1||control.serial!==before.normalTerminal.musicSerial
                ||control.operation!==5||typeof control.immediate!=='boolean'||control.acknowledged!==false
                ||!Number.isInteger(control.durationFrames)||control.durationFrames<0||control.durationFrames>36000
                ||!Number.isFinite(control.durationSeconds)||Math.abs(control.durationSeconds-control.durationFrames/60)>1e-5
                ||this.audio.state!=='running')throw new Error('Ordinary World music request/audio ownership rejected');
            const track=this.track(),effect=control.immediate?this.legacyMusic.fadeOut(0):this.legacyMusic.fadeOutFrames(control.durationFrames);
            if(!effect||typeof effect!=='object')throw new Error('Original music fade result is unavailable');
            const metadata={domain:'ordinary-world-music',...owner,...control,track,effect,actualMusicFadeIssued:true,actualAudibleDeviceOutputProved:false};
            const receipt=await this.commit('ordinary/music/'+owner.generation+'/'+control.serial,metadata);
            this.current(owner);this.sameTrack(track);
            if(this.audio.state!=='running'||JSON.stringify(JSON.parse(module.UTF8ToString(module._th128_world_music_control_state())))!==JSON.stringify(control))
                throw new Error('Ordinary World music request changed during full durable readback');
            if(module._th128_world_acknowledge_music_control(control.serial)!==1)throw new Error('Ordinary World music exact ACK rejected');
            return{...receipt,serial:control.serial};
        }catch(error){
            if(this.closed||!this.isCurrent())throw new DOMException('Ordinary World audio receipt owner was cancelled','AbortError');
            if(error.name!=='AbortError')this.failure=error;throw error;
        }
        finally{this.busy=false;}
    }
    close(){if(!this.closed){this.closed=true;this.receipts.close();}}
}
export async function createOrdinaryWorldAudioConsumer(options){
    const receipts=await OwnedEndingReceipts.open();
    try{return new OrdinaryWorldAudioConsumer({...options,receipts});}
    catch(error){receipts.close();throw error;}
}

function endingWavPCM(input,metadata){
    const view=new DataView(input.buffer,input.byteOffset,input.byteLength);
    const text=offset=>String.fromCharCode(...input.subarray(offset,offset+4));
    if(input.length<44||text(0)!=='RIFF'||text(8)!=='WAVE')throw new Error('Original ending WAV identity rejected');
    let format=null,offset=-1,count=0;
    for(let position=12;position+8<=input.length;){
        const size=view.getUint32(position+4,true);if(position+8+size>input.length)throw new Error('Truncated original ending WAV');
        if(text(position)==='fmt ')format={encoding:view.getUint16(position+8,true),channels:view.getUint16(position+10,true),
            sampleRate:view.getUint32(position+12,true),align:view.getUint16(position+20,true),bits:view.getUint16(position+22,true)};
        if(text(position)==='data'){offset=position+8;count=size;}
        position+=8+size+(size&1);
    }
    if(!format||format.encoding!==1||format.channels!==2||format.sampleRate!==44100||format.align!==4||format.bits!==16
        ||offset<0||count!==metadata.pcmBytes||count%4||metadata.loopStartBytes%4||metadata.loopStartBytes>=count)
        throw new Error('Original ending PCM or loop metadata rejected');
    const bytes=input.subarray(offset,offset+count);
    return{view:new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),offset:0,frames:count/4,loopStart:metadata.loopStartBytes/4,bytes,audioFormat:'pcm-s16le'};
}
export class OrdinaryEndingStopConsumer{
    constructor({module,receipts,readPCM,isCurrent}){
        if(!module||typeof receipts?.commit!=='function'||typeof readPCM!=='function'||typeof isCurrent!=='function')
            throw new Error('Ordinary ending exact stop consumers are unavailable');
        Object.assign(this,{module,receipts,readPCM,isCurrent});this.busy=false;this.committed=false;
    }
    nativePacket(){
        const module=this.module,size=module._th128_world_ending_return_packet_size(),pointer=module._th128_world_ending_return_packet_data();
        if(size!==128||!Number.isSafeInteger(pointer)||pointer<1||pointer+size>module.HEAPU8.length)
            throw new Error('Ordinary native return descriptor unavailable');
        return module.HEAPU8.slice(pointer,pointer+size);
    }
    async consume(packet){
        if(this.busy||this.committed||!this.isCurrent()||!(packet.descriptor instanceof Uint8Array)||packet.descriptor.length!==128)
            throw new Error('Ordinary stopped PCM receipt lifecycle rejected');
        this.busy=true;
        try{
            const current=this.nativePacket(),view=new DataView(current.buffer,current.byteOffset,current.byteLength),word=offset=>view.getUint32(offset,true);
            if(!current.every((v,i)=>v===packet.descriptor[i])||word(0)!==0x3152544f||word(4)!==1||word(8)!==128||word(12)!==1
                ||word(16)!==packet.generation||word(20)!==0||word(24)!==packet.logicalTick||word(28)!==0||word(36)!==packet.index
                ||word(64)!==2||word(68)!==2||word(72)!==5||word(76)!==180||word(112)!==16||word(116)!==1||word(120)!==0x394d4350)
                throw new Error('Ordinary stopped PCM native descriptor fields rejected');
            const observedPCM=this.readPCM();
            const pcm={...observedPCM,bytes:observedPCM.bytes instanceof Uint8Array?observedPCM.bytes.slice():observedPCM.bytes},samples=pcm.bytes;
            const legacy=pcm.audioFormat===undefined,format=legacy?'pcm-s16le':pcm.audioFormat;
            const assetSHA256=legacy?pcm.sourceWAVSHA256:pcm.sourceAssetSHA256;
            if(pcm.presentAudio!==true||pcm.stopped!==true||pcm.deviceStopIssued!==true||pcm.closed||pcm.ticks!==packet.logicalTick
                ||pcm.fadeRemaining!==0||pcm.fadeDuration!==180||!(samples instanceof Uint8Array)||samples.length!==735*4||samples.some(value=>value!==0)
                ||!Number.isSafeInteger(pcm.cursor)||pcm.cursor<0||(format!=='pcm-s16le'&&format!=='vorbis-ogg')
                ||!/^[a-f0-9]{64}$/.test(assetSHA256)
                ||(!legacy&&!/^[a-f0-9]{64}$/.test(pcm.sourcePCMInt16SHA256))
                ||(legacy&&pcm.sourcePCMInt16SHA256!==undefined)
                ||(legacy&&pcm.sourceAssetSHA256!==undefined&&pcm.sourceAssetSHA256!==assetSHA256)
                ||(format==='pcm-s16le'&&!legacy&&pcm.sourceWAVSHA256!==assetSHA256)
                ||(format==='vorbis-ogg'&&pcm.sourceWAVSHA256!==undefined))
                throw new Error('Ordinary return requires actual source.stop and exact complete final silent PCM');
            const metadata={domain:'ordinary-ending-stop',generation:packet.generation,index:packet.index,logicalTick:packet.logicalTick,
                descriptor:[...current],deviceStopIssued:pcm.deviceStopIssued,stopped:pcm.stopped,presentAudio:pcm.presentAudio,
                actualPCMFrameCursor:pcm.cursor,fadeRemaining:pcm.fadeRemaining,fadeDuration:pcm.fadeDuration,
                audioFormat:format,sourceAssetSHA256:assetSHA256,
                ...(legacy?{}:{sourcePCMInt16SHA256:pcm.sourcePCMInt16SHA256}),
                sourceWAVSHA256:format==='pcm-s16le'?assetSHA256:undefined,
                encodedSourceSHA256:format==='vorbis-ogg'?assetSHA256:undefined,
                pcmFrames:735,channels:2,sampleRate:44100};
            const payload=new Uint8Array(current.length+samples.length);payload.set(current);payload.set(samples,current.length);
            metadata.identitySHA256=await sha256(encoded(metadata));const payloadSHA256=await sha256(payload);
            const receipt=await this.receipts.commit('ordinary/return-stop/'+packet.generation+'/'+packet.logicalTick,metadata,payload);
            if(!this.isCurrent())throw new DOMException('Ordinary ending stop owner was cancelled','AbortError');
            const after=this.readPCM(),afterFormat=after.audioFormat===undefined?'pcm-s16le':after.audioFormat;
            if(!this.nativePacket().every((v,i)=>v===current[i])||after.closed||!after.stopped||!after.deviceStopIssued||after.presentAudio!==true
                ||after.ticks!==pcm.ticks||after.cursor!==pcm.cursor||after.fadeRemaining!==0||after.fadeDuration!==180
                ||(after.audioFormat===undefined)!==legacy||afterFormat!==format||after.sourceAssetSHA256!==(legacy?undefined:pcm.sourceAssetSHA256)
                ||(legacy?after.sourcePCMInt16SHA256!==undefined:after.sourcePCMInt16SHA256!==pcm.sourcePCMInt16SHA256)
                ||after.sourceWAVSHA256!==pcm.sourceWAVSHA256||!(after.bytes instanceof Uint8Array)||after.bytes.length!==samples.length
                ||!after.bytes.every((v,i)=>v===samples[i])||receipt?.committed!==true||receipt.fullReadback!==true
                ||receipt.durability!=='strict-indexeddb-transaction'||receipt.domain!==metadata.domain||receipt.identitySHA256!==metadata.identitySHA256
                ||receipt.bytes!==payload.length||receipt.sha256!==payloadSHA256)
                throw new Error('Ordinary ending stopped PCM complete durable receipt rejected');
            this.committed=true;return{...receipt,descriptor:current,logicalTick:pcm.ticks,generation:packet.generation,index:packet.index,
                deviceStopIssued:true,audioFormat:format,sourceAssetSHA256:assetSHA256,
                ...(legacy?{}:{sourcePCMInt16SHA256:pcm.sourcePCMInt16SHA256})};
        }catch(error){if(!this.isCurrent())throw new DOMException('Ordinary ending stop owner was cancelled','AbortError');throw error;}
        finally{this.busy=false;}
    }
}
async function endingOggPCM(input,metadata,decode){
    if(metadata?.audioFormat!=='vorbis-ogg'&&metadata?.codec!=='vorbis')throw new Error('Original ending OGG source format rejected');
    if(typeof decode!=='function')throw new Error('Original ending OGG decode seam is unavailable');
    if(!(input instanceof Uint8Array)||input.length<12||String.fromCharCode(input[0],input[1],input[2],input[3])!=='OggS')
        throw new Error('Original ending OGG encoded source bytes rejected');
    const decoded=await decode(input);
    if(!decoded||decoded.sampleRate!==44100||decoded.numberOfChannels!==2||!Number.isSafeInteger(decoded.length)||decoded.length<1
        ||typeof decoded.getChannelData!=='function')throw new Error('Original ending OGG decoded audio metadata rejected');
    const frames=decoded.length;
    if(frames>44100*600)throw new Error('Original ending OGG decoded frame count exceeds the owned bound');
    const expected=Number.isSafeInteger(metadata.decodedFrames)?metadata.decodedFrames
        :(Number.isSafeInteger(metadata.pcmBytes)&&metadata.pcmBytes%4===0?metadata.pcmBytes/4:NaN);
    if(!Number.isSafeInteger(expected)||expected<1)throw new Error('Original ending OGG decoded frame count is unverifiable');
    if(frames!==expected)throw new Error('Original ending OGG decoded frame count mismatch');
    const loopStart=Number.isSafeInteger(metadata.loopStartSamples)?metadata.loopStartSamples
        :(Number.isSafeInteger(metadata.loopStartBytes)&&metadata.loopStartBytes%4===0?metadata.loopStartBytes/4:NaN);
    if(!Number.isSafeInteger(loopStart)||loopStart<0||loopStart>=frames)throw new Error('Original ending OGG loop metadata rejected');
    const channels=[decoded.getChannelData(0),decoded.getChannelData(1)];
    for(const channel of channels){
        if(!channel||channel.length!==frames||typeof channel.every!=='function'||!channel.every(value=>Number.isFinite(value)))
            throw new Error('Original ending OGG decoded PCM samples rejected');
    }
    const bytes=new Uint8Array(frames*4),view=new DataView(bytes.buffer);
    for(let frame=0;frame<frames;frame++){
        view.setInt16(frame*4,Math.max(-32768,Math.min(32767,Math.round(channels[0][frame]*32768))),true);
        view.setInt16(frame*4+2,Math.max(-32768,Math.min(32767,Math.round(channels[1][frame]*32768))),true);
    }
    return{view,offset:0,frames,loopStart,bytes,audioFormat:'vorbis-ogg'};
}
async function endingMusicPCM(input,metadata,decode){
    if(input.length>=12&&String.fromCharCode(...input.subarray(0,4))==='RIFF')return endingWavPCM(input,metadata);
    return endingOggPCM(input,metadata,decode);
}
export async function createEndingBrowserHost({module,renderer,resources,readAsset,audio,gain,soundQueue,legacyMusic,isCurrent,presentAudio=true,prepareOrdinary=null,decodeMusic=null}){
    if(typeof renderer?.renderEndingPixels!=='function'||typeof renderer?.releaseEndingPixels!=='function'||typeof renderer?.renderOriginalTerminalFade!=='function'||typeof readAsset!=='function'
        ||!audio||!gain||typeof soundQueue?.consumeNativeRequest!=='function'||typeof soundQueue?.flush!=='function'
        ||typeof soundQueue?.assertIdentityHost!=='function'||typeof legacyMusic?.stopSource!=='function')
        throw new Error('Original ending concrete browser services are unavailable');
    soundQueue.assertIdentityHost();
    const body=await readAsset(resources.music.asset);
    if(!(body instanceof Uint8Array)||body.length!==resources.music.asset.bytes||await sha256(body)!==resources.music.asset.sha256)
        throw new Error('Original ending music asset bytes rejected');
    if(!isCurrent())throw new Error('Original ending browser generation changed during music preload');
    const assetSHA256=resources.music.asset.sha256;
    const decode=decodeMusic??(async bytes=>{
        const decoder=typeof globalThis.OfflineAudioContext==='function'?new OfflineAudioContext(2,1,44100):audio;
        if(!decoder||typeof decoder.decodeAudioData!=='function')throw new Error('Original ending OGG decode seam is unavailable');
        return decoder.decodeAudioData(bytes.buffer.slice(bytes.byteOffset,bytes.byteOffset+bytes.byteLength));
    });
    const pcm=await endingMusicPCM(body,resources.music.metadata,decode);
    if(!isCurrent())throw new Error('Original ending browser generation changed during music decode');
    const pcmSHA256=await sha256(pcm.bytes);
    if(!isCurrent())throw new Error('Original ending browser generation changed during decoded PCM identity');
    let receipts=null,output=null;
    try{
        receipts=await OwnedEndingReceipts.open();
        if(!isCurrent())throw new Error('Original ending browser generation changed while opening the strict ending receipt store');
        output=audio.createGain();
        if(!isCurrent())throw new Error('Original ending browser generation changed while creating the ending output');
        output.gain.value=1;output.connect(gain);
    }catch(error){
        if(output&&typeof output.disconnect==='function'){try{output.disconnect();}catch{}}
        if(receipts&&typeof receipts.close==='function'){try{receipts.close();}catch{}}
        throw error;
    }
    const sourceIdentity={audioFormat:pcm.audioFormat,sourceAssetSHA256:assetSHA256,sourcePCMInt16SHA256:pcmSHA256,
        ...(pcm.audioFormat==='pcm-s16le'?{sourceWAVSHA256:assetSHA256}:{})};
    let source=null,cursor=0,fadeRemaining=0,fadeDuration=0,stopped=false,ticks=0,closed=false,lastPixels=null,deviceStopIssued=false,lastPCM=null;
    const stoppedPCM=new OrdinaryEndingStopConsumer({module,receipts,isCurrent,readPCM:()=>({bytes:lastPCM,cursor,fadeRemaining,fadeDuration,stopped,ticks,closed,deviceStopIssued,presentAudio,audioFormat:pcm.audioFormat,sourceAssetSHA256:assetSHA256,sourcePCMInt16SHA256:pcmSHA256,sourceWAVSHA256:pcm.audioFormat==='pcm-s16le'?assetSHA256:undefined})});
    const media={
        async stopAndCommit(packet){return stoppedPCM.consume(packet);},
        async consumeAndCommit(packet){
            if(closed||!isCurrent()||presentAudio&&audio.state!=='running')throw new Error('Original ending actual audio owner is not running');
            if(packet.operation===8){
                if(source||ticks)throw new Error('Original ending BGM was already started');
                const previous=typeof legacyMusic.snapshot==='function'?legacyMusic.snapshot():null;
                legacyMusic.stopSource();
                const buffer=audio.createBuffer(2,pcm.frames,44100);
                for(let channel=0;channel<2;channel++){
                    const samples=buffer.getChannelData(channel);
                    for(let frame=0;frame<pcm.frames;frame++)samples[frame]=pcm.view.getInt16(pcm.offset+frame*4+channel*2,true)/32768;
                }
                source=audio.createBufferSource();source.buffer=buffer;source.loop=true;
                source.loopStart=pcm.loopStart/44100;source.loopEnd=pcm.frames/44100;source.connect(output);
                if(presentAudio)source.start();
                return receipts.commit('media/'+packet.serial,{...packet,actualLegacyOwner:previous,
                    ...sourceIdentity,actualBufferFrames:buffer.length,sampleRate:buffer.sampleRate,
                    loopStartFrame:pcm.loopStart,loopEndFrame:pcm.frames,actualSourceStarted:presentAudio,
                    actualAudibleDeviceOutputProved:false,nativeWin32IAT:false});
            }
            if(packet.operation!==5||packet.durationFrames!==180||!source||stopped)throw new Error('Original ending fade owner rejected');
            fadeRemaining=fadeDuration=packet.durationFrames;
            return receipts.commit('media/'+packet.serial,{...packet,actualPCMFrameCursor:cursor,fadeRemaining,fadeDuration,
                ...sourceIdentity,actualAudibleDeviceOutputProved:false});
        },
        async tickAndCommit(state){
            if(closed||!source||!isCurrent())throw new Error('Original ending PCM tick owner is unavailable');
            let gainValue=1;
            if(stopped)gainValue=0;
            else if(fadeDuration){
                fadeRemaining=Math.max(0,fadeRemaining-1);
                if(fadeRemaining===0){stopped=true;gainValue=0;}
                else gainValue=10**(Math.trunc(fadeRemaining*5000/fadeDuration-5000)/2000);
            }
            const bytes=new Uint8Array(735*4),view=new DataView(bytes.buffer);
            for(let frame=0;frame<735;frame++){
                if(cursor>=pcm.frames)cursor=pcm.loopStart;
                for(let channel=0;channel<2;channel++)view.setInt16(frame*4+channel*2,Math.max(-32768,Math.min(32767,
                    Math.round(pcm.view.getInt16(pcm.offset+cursor*4+channel*2,true)*gainValue))),true);
                cursor++;
            }
            lastPCM=bytes;output.gain.setValueAtTime(gainValue,audio.currentTime);
            if(stopped&&presentAudio&&!deviceStopIssued){source.stop();deviceStopIssued=true;}
            return receipts.commit('pcm/'+(++ticks),{domain:'original-ending-pcm',generation:state.generation,index:state.index,
                logicalTick:ticks,actualPCMFrameCursor:cursor,fadeRemaining,fadeDuration,stopped,originalFrames:735,
                ...sourceIdentity,audibleDeviceCursorProved:false},bytes);
        }
    };
    const sound={
        async consumeAndCommit(packet){
            if(closed||!isCurrent())throw new Error('Original ending sound generation changed');
            soundQueue.assertIdentityHost();const requests=[];
            for(const command of packet.requests){
                const accepted=soundQueue.consumeNativeRequest(command.identifier,command.position,command.operation,{present:presentAudio});
                requests.push({...command,accepted});
            }
            soundQueue.flush({present:presentAudio});
            return receipts.commit('sound/'+packet.epoch+'/'+packet.serial,{...packet,requests,
                actualUnchangedIdentityQueueConsumedAndFlushed:true,audibleDeviceOutputProved:false});
        }
    };
    let owner;
    try{owner=new OriginalEndingBridge({module,resources,isCurrent,services:{readAsset,media,sound,prepareOrdinary,
        async present(pixels,state){
            if(!isCurrent())throw new Error('Original ending renderer generation changed');
            renderer.renderEndingPixels(pixels,receipts.database.name);lastPixels=Uint8Array.from(pixels);
            const world=JSON.parse(module.UTF8ToString(module._th128_world_state()));
            if(world.normalTerminal?.endingStarted===true)renderer.renderOriginalTerminalFade(world);
            return{actualRenderCallIssued:true,generation:state.generation,visiblePixelsProved:false};
        }}});}catch(error){output.disconnect();receipts.close();throw error;}
    return{owner,
        redraw(){if(lastPixels){
            renderer.renderEndingPixels(lastPixels,receipts.database.name);
            const world=JSON.parse(module.UTF8ToString(module._th128_world_state()));
            if(world.normalTerminal?.endingStarted===true)renderer.renderOriginalTerminalFade(world);
        }},
        close(){
            if(closed)return;
            if(isCurrent()&&!['return-requested','failed'].includes(owner.phase))throw new Error('Active original ending cannot be silently disposed');
            owner.input.release();if(source){
                if(presentAudio&&!deviceStopIssued){source.stop();deviceStopIssued=true;}
                source.disconnect();source=null;
            }
            output.disconnect();receipts.close();closed=true;renderer.releaseEndingPixels(receipts.database.name);lastPixels=null;
        },
        snapshot(){return{...owner.status(),durableReceiptDatabase:receipts.database.name,durableReceipts:receipts.count,
            PCMLogicalTicks:ticks,actualPCMFrameCursor:cursor,fadeRemaining,fadeDuration,
            strictIndexedDBUsed:true,audibleDeviceOutputProved:false,visiblePixelsProved:false};}
    };
}

export { endingWavPCM, endingOggPCM, endingMusicPCM };
