import {OriginalEndingPixels,decodeEndingGlyph} from './native-ending-pixels.mjs';
const keyBits=Object.freeze({KeyZ:1,Enter:0x200,KeyC:0x200,ControlLeft:0x200,KeyX:2,Escape:0x100,ArrowUp:0x10,ArrowDown:0x20,ArrowLeft:0x40,ArrowRight:0x80});
const terminalStages=Object.freeze([3,5,8,10,13,15]);
async function digest(input){
    const bytes=input instanceof Uint8Array?input:new TextEncoder().encode(JSON.stringify(input));
    const value=new Uint8Array(await globalThis.crypto.subtle.digest('SHA-256',bytes));
    return [...value].map(byte=>byte.toString(16).padStart(2,'0')).join('');
}
function requireFunction(owner,name){
    if(typeof owner?.[name]!=='function')throw new Error('Original ending required API unavailable: '+name);
}
export function originalEndingRequest(world,{requireStats=false}={}){
    if(!world||typeof world!=='object')throw new Error('Original ending World snapshot is unavailable');
    const normal=world.normalTerminal,debug=world.debugTerminal;
    if(normal?.requested===true&&debug?.requested===true)throw new Error('Ambiguous original ending owner');
    const positive=value=>Number.isSafeInteger(value)&&value>0;
    const integer=(value,minimum,maximum)=>Number.isSafeInteger(value)&&value>=minimum&&value<=maximum;
    const generation=world.flow?.worldGeneration;
    if(normal?.requested===true){
        const record=world.normalRecord,control=world.stageControl,music=world.musicControl;
        if(normal.configured!==true||normal.begun!==true||normal.ownerBound!==true||normal.ownerClaimed!==true
            ||normal.ownerRecorded!==true||normal.ownerMatches!==true||!positive(generation)||normal.generation!==generation
            ||normal.runtimeScene!==15||normal.counter!==380||normal.messageDone!==true
            ||normal.normalClear!==false||normal.debugTainted!==false||normal.replayPlayback!==false
            ||normal.sessionRetired!==false||normal.endingStarted!==false||world.debug?.runTainted!==false
            ||world.debug?.infiniteRespawn===true||normal.provenance!=='ordinary-native-terminal-owner'
            ||!integer(normal.index,0,5)||terminalStages[normal.index]!==normal.stage||normal.selector!==normal.index+17
            ||world.flow.currentId!==normal.stage||!integer(world.flow.difficulty,0,3)
            ||control?.current!==7||control.pending!==15||control.gate!==false||(control.flags&0x10)!==0
            ||!positive(normal.musicSerial)||normal.musicAcknowledged!==true||!music
            ||music.operation!==5||music.acknowledged!==true||music.serial!==normal.musicSerial
            ||!record||record.bound!==true||record.started!==true||record.tainted!==false
            ||record.normalClear!==false||record.provenance!=='ordinary-native-postrun'||record.route!==normal.index
            ||record.rank!==world.flow.difficulty||!positive(record.frames)||!integer(normal.score,0,999999999)
            ||!integer(normal.continues,0,9)||!integer(world.frame,0,0xffffffff))
            throw new Error('Ordinary ending snapshot differs from its actual terminal owner');
        if(record.prepared&&(record.terminalStatsPrepared!==true||record.terminalScore!==normal.score||record.terminalContinues!==normal.continues))
            throw new Error('Ordinary ending profile was not prepared by the terminal owner');
        if(requireStats&&(record.prepared!==true||record.acknowledged!==true||record.terminalStatsPrepared!==true
            ||record.serial!==1||record.bytes!==52232||!integer(record.digest,0,0xffffffff)))
            throw new Error('Ordinary ending requires its exact acknowledged statistics payload');
        return{kind:'ordinary',index:normal.index,stage:normal.stage,generation,frame:world.frame,rank:record.rank,frames:record.frames,
            score:normal.score,continues:normal.continues,musicSerial:normal.musicSerial,recordSerial:record.serial,recordBytes:record.bytes,recordDigest:record.digest};
    }
    if(debug?.requested!==true)return null;
    if(debug.runtimeScene!==15||debug.counter!==380||debug.messageDone!==true||debug.normalClear!==false
        ||world.debug?.runTainted!==true||!positive(generation)||!integer(debug.index,0,5)||terminalStages[debug.index]!==debug.stage)
        throw new Error('Actual diagnostic runtime15 request is required before ending start');
    return{kind:debug.practice===true?'practice':'diagnostic',index:debug.index,stage:debug.stage,generation,frame:world.frame};
}
function sameEndingRequest(first,second,{statistics=true}={}){
    if(!first||!second)return false;
    const names=['kind','index','stage','generation','frame','rank','frames','score','continues','musicSerial'];
    if(statistics)names.push('recordSerial','recordBytes','recordDigest');
    return names.every(name=>first[name]===second[name]);
}
function nativeProfileDigest(bytes){let value=2166136261;for(const byte of bytes)value=Math.imul(value^byte,16777619)>>>0;return value;}
export class OriginalEndingInput{
    constructor(){this.held=new Set();this.pressed=0;this.released=0;}
    down(code){
        const bit=keyBits[code];if(bit===undefined||this.held.has(code))return false;
        if(![...this.held].some(key=>keyBits[key]===bit))this.pressed|=bit;
        this.held.add(code);return true;
    }
    up(code){
        const bit=keyBits[code];if(bit===undefined||!this.held.delete(code))return false;
        if(![...this.held].some(key=>keyBits[key]===bit))this.released|=bit;
        return true;
    }
    adoptHeld(codes){
        if(this.held.size||this.pressed||this.released||!(codes instanceof Set))throw new Error('Original ending initial held-input ownership rejected');
        for(const code of codes)if(keyBits[code]!==undefined)this.held.add(code);
    }
    release(){for(const code of this.held)this.released|=keyBits[code];this.held.clear();}
    sample(){
        let holding=0;for(const code of this.held)holding|=keyBits[code];
        const value={holding,pressed:this.pressed,released:this.released};this.pressed=this.released=0;return value;
    }
}
export class OriginalEndingBridge{
    constructor({module,resources,services,isCurrent}){
        if(!module||!resources||!services||typeof isCurrent!=='function')throw new Error('Original ending ownership is unavailable');
        for(const name of ['_malloc','_free','_th128_world_state','_th128_world_error','_th128_world_start_ending',
            '_th128_world_ending_tick','_th128_world_ending_state','_th128_world_ending_cleanup','_th128_world_ending_return_tail','_th128_world_ending_set_pause',
            '_th128_world_ending_ack_media','_th128_world_sound_epoch','_th128_world_sound_begin_window',
            '_th128_world_sound_window_state','_th128_world_sound_id','_th128_world_sound_x',
            '_th128_world_sound_operation','_th128_world_sound_ack_window'])requireFunction(module,name);
        requireFunction(services,'readAsset');requireFunction(services,'present');
        requireFunction(services.media,'consumeAndCommit');requireFunction(services.sound,'consumeAndCommit');
        requireFunction(services.media,'tickAndCommit');
        this.module=module;this.resources=resources;this.services=services;this.isCurrent=isCurrent;
        this.input=new OriginalEndingInput();this.phase='new';this.busy=false;this.failed=null;
        this.generation=0;this.index=-1;this.stage=0;this.endingTicks=0;this.mediaSerial=0;this.glyph=null;this.glyphOrdinal=-1;
        this.snapshot=null;this.originalReturn=null;this.renderer=null;this.mediaStopped=false;this.ownerKind=null;
    }
    error(){return this.module.UTF8ToString(this.module._th128_world_error());}
    current(){
        if(!this.isCurrent())throw new Error('Original ending asynchronous generation ownership changed');
        if(this.failed)throw this.failed;
    }
    state(){
        this.current();
        const value=JSON.parse(this.module.UTF8ToString(this.module._th128_world_ending_state()));
        if(!value||value.failed||value.index!==this.index||value.stage!==this.stage||value.generation!==this.generation
            ||this.ownerKind==='ordinary'&&(value.ordinaryOwner!==true||value.debugTainted!==false||value.normalClear!==false))
            throw new Error('Original ending native identity changed or faulted');
        this.snapshot=value;return value;
    }
    async asset(record){
        this.current();
        if(!record||!Number.isSafeInteger(record.bytes)||record.bytes<1||record.bytes>16*1024*1024||!/^[0-9a-f]{64}$/.test(record.sha256))
            throw new Error('Original ending asset pin is unavailable');
        const value=await this.services.readAsset(record);this.current();
        if(!(value instanceof Uint8Array)||value.length!==record.bytes||await digest(value)!==record.sha256)
            throw new Error('Original ending asset full-byte identity rejected');
        return value;
    }
    accepted(code,operation){if(code!==1)throw new Error(this.error()||'Original ending '+operation+' rejected');}
    async media(){
        const state=this.state();
        for(const command of state.media){
            if(command.acknowledged)continue;
            if(command.serial!==this.mediaSerial+1||command.path!=='bgm/th128_09.wav'
                ||command.operation!==8&&command.operation!==5
                ||command.operation===5&&command.durationFrames!==180)
                throw new Error('Original ending media serial or operation rejected');
            const packet={domain:'original-ending-media',generation:this.generation,index:this.index,serial:command.serial,
                operation:command.operation,path:command.path,durationFrames:command.durationFrames};
            packet.identitySHA256=await digest(packet);
            const receipt=await this.services.media.consumeAndCommit(packet);this.current();
            if(!receipt||receipt.committed!==true||receipt.fullReadback!==true||receipt.identitySHA256!==packet.identitySHA256)
                throw new Error('Original ending media durable identity receipt is unavailable');
            this.accepted(this.module._th128_world_ending_ack_media(command.serial,command.operation),'media ACK');
            this.mediaSerial=command.serial;
        }
    }
    async sound(){
        this.current();const epoch=this.module._th128_world_sound_epoch(),serial=this.module._th128_world_sound_begin_window();
        if(serial===0)return;
        const opened=JSON.parse(this.module.UTF8ToString(this.module._th128_world_sound_window_state()));
        if(!Number.isSafeInteger(epoch)||epoch<=0||serial<=0||opened.serial!==serial||opened.epoch!==epoch
            ||!Number.isSafeInteger(opened.count)||opened.count<0||opened.count>65536)
            throw new Error('Original ending sound native window rejected');
        const requests=[];
        for(let index=0;index<opened.count;index++){
            const identifier=this.module._th128_world_sound_id(index),position=this.module._th128_world_sound_x(index),
                operation=this.module._th128_world_sound_operation(index);
            if(!Number.isInteger(identifier)||identifier< -1||identifier>=76||!Number.isFinite(position)
                ||![0,1,2].includes(operation)||identifier<0&&operation!==0)
                throw new Error('Original ending sound request rejected');
            requests.push({identifier,position,operation});
        }
        const packet={domain:'original-ending-sound',generation:this.generation,epoch,serial,count:opened.count,
            baseLow:opened.baseLow,baseHigh:opened.baseHigh,requests};
        packet.identitySHA256=await digest(packet);
        const receipt=await this.services.sound.consumeAndCommit(packet);this.current();
        if(!receipt||receipt.committed!==true||receipt.fullReadback!==true||receipt.identitySHA256!==packet.identitySHA256)
            throw new Error('Original ending sound durable identity receipt is unavailable');
        const remaining=this.module._th128_world_sound_ack_window(serial,opened.count,opened.baseLow,opened.baseHigh,epoch);
        if(!Number.isInteger(remaining)||remaining<0)throw new Error(this.error()||'Original ending sound ACK rejected');
    }
    async present(state){
        const ordinal=state.executedOp3-1;
        if(ordinal>=0&&ordinal!==this.glyphOrdinal){
            const record=this.resources.glyphs[ordinal];
            if(!record||record.ordinal!==ordinal||record.route!==this.index||record.color!==state.textColor)
                throw new Error('Original ending authored glyph identity is unavailable');
            this.glyph=decodeEndingGlyph(await this.asset(record.texture));this.glyphOrdinal=ordinal;
        }
        const pixels=this.renderer.frame(state,this.glyph);
        await this.services.present(pixels,state);this.current();
    }
    async start(){
        if(this.busy||this.phase!=='new')throw new Error('Original ending start lifecycle rejected');
        this.busy=true;
        try{
            this.current();const before=JSON.parse(this.module.UTF8ToString(this.module._th128_world_state()));
            let request=originalEndingRequest(before);
            if(!request)throw new Error('Actual original runtime15 request is required before ending start');
            this.ownerKind=request.kind;this.index=request.index;this.stage=request.stage;this.generation=request.generation;
            if(this.resources.index!==this.index||!Array.isArray(this.resources.glyphs))
                throw new Error('Original ending route resource identity rejected');
            if(request.kind==='ordinary'){
                requireFunction(this.services,'prepareOrdinary');
                const receipt=await this.services.prepareOrdinary(request);this.current();
                const prepared=originalEndingRequest(JSON.parse(this.module.UTF8ToString(this.module._th128_world_state())),{requireStats:true});
                const native=receipt?.native;
                if(!sameEndingRequest(request,prepared,{statistics:false})||receipt?.strictFullReadback!==true
                    ||receipt.strictDurability!==true||receipt.nativeAcknowledged!==true||!native
                    ||!(receipt.profile instanceof Uint8Array)||receipt.profile.length!==52232
                    ||native.generation!==prepared.generation||native.route!==prepared.index||native.rank!==prepared.rank
                    ||native.frames!==prepared.frames||native.serial!==prepared.recordSerial||native.bytes!==prepared.recordBytes
                    ||native.terminalStatsPrepared!==true||native.terminalScore!==prepared.score||native.terminalContinues!==prepared.continues
                    ||native.acknowledged!==false||native.digest!==prepared.recordDigest||nativeProfileDigest(receipt.profile)!==prepared.recordDigest)
                    throw new Error('Ordinary ending full-profile durable acknowledgement identity is unavailable');
                request=prepared;
            }
            const inputs=await Promise.all([this.asset(this.resources.message),this.asset(this.resources.art),
                this.asset(this.resources.base),this.asset(this.resources.text)]);
            this.current();
            if(request.kind==='ordinary')await this.sound();
            const after=originalEndingRequest(JSON.parse(this.module.UTF8ToString(this.module._th128_world_state())),{requireStats:request.kind==='ordinary'});
            if(!sameEndingRequest(request,after))
                throw new Error('Gameplay advanced or terminal ownership changed while ending assets loaded');
            this.renderer=new OriginalEndingPixels(inputs[2],inputs[1]);
            const pointers=inputs.map(value=>{const pointer=this.module._malloc(value.length);this.module.HEAPU8.set(value,pointer);return pointer;});
            let code;
            try{code=this.module._th128_world_start_ending(...inputs.flatMap((value,index)=>[pointers[index],value.length]));}
            finally{for(const pointer of pointers)this.module._free(pointer);}
            this.accepted(code,'start');
            const first=JSON.parse(this.module.UTF8ToString(this.module._th128_world_ending_state()));
            if(first.generation!==this.generation)throw new Error('Original ending native constructor changed generation');this.phase='active';this.state();
            await this.sound();await this.media();await this.present(this.state());
        }catch(error){this.failed=error;this.phase='failed';throw error;}
        finally{this.busy=false;}
        return this.status();
    }
    async step(input=null){
        if(this.busy||!['active','tail'].includes(this.phase))throw new Error('Original ending step lifecycle rejected');
        if(input===null)input=this.input.sample();
        if(!input||!['holding','pressed','released'].every(name=>Number.isInteger(input[name])&&input[name]>=0&&input[name]<=0xffffffff))
            throw new Error('Original ending input bitfield rejected');
        this.busy=true;
        try{
            this.current();
            if(this.phase==='active')this.accepted(this.module._th128_world_ending_tick(input.holding,input.pressed,input.released),'input tick');
            else this.accepted(this.module._th128_world_ending_return_tail(),'return-tail tick');
            this.endingTicks++;await this.sound();await this.media();const state=this.state();
            const pcmReceipt=await this.services.media.tickAndCommit(state);this.current();
            if(!pcmReceipt||pcmReceipt.committed!==true||pcmReceipt.fullReadback!==true||pcmReceipt.domain!=='original-ending-pcm'
                ||pcmReceipt.generation!==this.generation||pcmReceipt.index!==this.index||pcmReceipt.logicalTick!==this.endingTicks
                ||typeof pcmReceipt.stopped!=='boolean')
                throw new Error('Original ending PCM full durable tick identity receipt is unavailable');
            this.mediaStopped=pcmReceipt.stopped;await this.present(state);
            if(this.phase==='active'&&state.originalReturnRequested){
                if(state.returnScene!==16)throw new Error('Original ending requested an unsupported next scene');
                this.originalReturn=state;this.accepted(this.module._th128_world_ending_cleanup(),'cleanup');
                const cleaned=this.state();if(!cleaned.cleaned||cleaned.loaded||cleaned.draws.length)
                    throw new Error('Original ending concrete cleanup is incomplete');
                this.phase='tail';
            }
            if(this.phase==='tail'&&!this.state().fade.active&&this.mediaStopped){
                if(this.ownerKind==='ordinary')await this.commitOrdinaryStop();
                this.phase='return-requested';
            }
        }catch(error){this.failed=error;this.phase='failed';throw error;}
        finally{this.busy=false;}
        return this.status();
    }
    async commitOrdinaryStop(){
        this.current();const module=this.module,state=this.state();
        if(this.ownerKind!=='ordinary'||!state.cleaned||state.loaded||!state.originalReturnRequested||state.returnScene!==16
            ||state.fade.active||!this.mediaStopped||this.mediaSerial!==2||state.endingTicks!==this.endingTicks)
            throw new Error('Ordinary original Ending16 cleanup/fade/media identity unavailable');
        for(const name of ['_th128_world_ending_return_prepare','_th128_world_ending_return_packet_size','_th128_world_ending_return_packet_data','_th128_world_ending_return_ack'])requireFunction(module,name);
        requireFunction(this.services.media,'stopAndCommit');
        this.accepted(module._th128_world_ending_return_prepare(),'ordinary stop descriptor');
        const size=module._th128_world_ending_return_packet_size(),pointer=module._th128_world_ending_return_packet_data();
        if(size!==128||!Number.isSafeInteger(pointer)||pointer<1||pointer+size>module.HEAPU8.length)throw new Error('Ordinary exact native return descriptor unavailable');
        const descriptor=module.HEAPU8.slice(pointer,pointer+size),packet={domain:'ordinary-ending-stop',generation:this.generation,index:this.index,logicalTick:this.endingTicks,descriptor};
        const receipt=await this.services.media.stopAndCommit(packet);this.current();
        if(receipt?.committed!==true||receipt.fullReadback!==true||receipt.durability!=='strict-indexeddb-transaction'
            ||receipt.domain!==packet.domain||receipt.generation!==packet.generation||receipt.index!==packet.index
            ||receipt.logicalTick!==packet.logicalTick||receipt.deviceStopIssued!==true||!(receipt.descriptor instanceof Uint8Array)
            ||receipt.descriptor.length!==size||!receipt.descriptor.every((v,i)=>v===descriptor[i])
            ||module._th128_world_ending_return_packet_size()!==size||module._th128_world_ending_return_packet_data()!==pointer
            ||!module.HEAPU8.slice(pointer,pointer+size).every((v,i)=>v===descriptor[i])||this.state().endingTicks!==packet.logicalTick)
            throw new Error('Ordinary exact stopped PCM receipt changed before native ACK');
        const copy=module._malloc(size);if(!copy)throw new Error('Ordinary stopped PCM ACK allocation failed');
        try{module.HEAPU8.set(descriptor,copy);this.accepted(module._th128_world_ending_return_ack(copy,size),'ordinary complete stopped PCM ACK');}
        finally{module._free(copy);}
        this.ordinaryStopReceipt=receipt;
    }
    setPaused(value){
        this.current();if(this.busy||!['active','tail'].includes(this.phase)||typeof value!=='boolean')throw new Error('Original ending pause owner is not ready');
        this.accepted(this.module._th128_world_ending_set_pause(value?1:0),'native global pause');return this.state();
    }
    status(){
        return{phase:this.phase,busy:this.busy,failed:this.failed?String(this.failed.message??this.failed):null,
            generation:this.generation,index:this.index,stage:this.stage,endingTicks:this.endingTicks,ownerKind:this.ownerKind,
            mediaSerial:this.mediaSerial,mediaStopped:this.mediaStopped,originalReturnRequested:!!this.originalReturn,
            requestedScene:this.originalReturn?.returnScene??0,postrunOwnerCommitted:false,native:this.snapshot};
    }
}
