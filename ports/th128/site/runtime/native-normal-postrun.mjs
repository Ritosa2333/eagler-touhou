import {OriginalEndingInput} from './native-ending-bridge-v6.mjs';
import {OwnedEndingReceipts} from './native-ending-browser-host-v4.mjs';
import {StageMusic,OwnedNativeTitleRecordStore,OwnedReplayStore,originalReplayPreviewPacket,nativeReplayToFile} from './sound.mjs';
import {decodeReplay} from './replay.mjs';
import {OwnedLastNameFieldStore} from './last-name-field.mjs';
import {orderDrawLayers} from './draw.mjs';
async function sha(bytes){return [...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(v=>v.toString(16).padStart(2,'0')).join('');}
export class NormalMenuProfileConsumer{
    constructor({module,generation,profiles,names,receipts,isCurrent,practice=false}){
        if(!module||!Number.isSafeInteger(generation)||generation<1||typeof profiles?.observe!=='function'||typeof profiles?.commit!=='function'
            ||typeof names?.observe!=='function'||typeof names?.saveStrict!=='function'||typeof receipts?.commit!=='function'||typeof isCurrent!=='function')
            throw new Error('Ordinary menu complete profile/name consumers unavailable');
        Object.assign(this,{module,generation,profiles,names,receipts,isCurrent,practice});this.busy=false;
        this.practiceRevision=null;this.practiceRunId=practice?'practice-menu:'+crypto.randomUUID():null;
    }
    async practiceProfile(){
        if(!this.practice||this.practiceRevision!==null||!this.isCurrent())throw new Error('Practice score profile already observed or cancelled');
        const record=await this.profiles.observe();if(!this.isCurrent())throw new DOMException('Practice score profile owner cancelled','AbortError');
        this.practiceRevision=record?.revision??0;return record?.profile??new Uint8Array();
    }
    packet(){
        const m=this.module,size=m._th128_world_normal_postrun_profile_size(),pointer=m._th128_world_normal_postrun_profile_data();
        if(size===0)return null;
        if(size!==52321||!Number.isSafeInteger(pointer)||pointer<1||pointer+size>m.HEAPU8.length)throw new Error('Ordinary menu full receipt packet unavailable');
        return m.HEAPU8.slice(pointer,pointer+size);
    }
    current(packet){
        if(!this.isCurrent())throw new DOMException('Ordinary menu profile owner cancelled','AbortError');
        const observed=this.packet();if(!observed||observed.length!==packet.length||!observed.every((v,i)=>v===packet[i]))
            throw new Error('Ordinary native menu profile packet changed during persistence');
    }
    acknowledge(generation,packet){
        this.current(packet);const pointer=this.module._malloc(packet.length);if(!pointer)throw new Error('Original profile ACK allocation failed');
        try{this.module.HEAPU8.set(packet,pointer);if(this.module._th128_world_normal_postrun_profile_ack(generation,pointer,packet.length)!==1)
            throw new Error(this.module.UTF8ToString(this.module._th128_world_error())||'Original complete menu receipt ACK rejected');}
        finally{this.module._free(pointer);}
    }
    async commit(){
        if(this.busy)throw new Error('Ordinary menu profile receipt already pending');const packet=this.packet();if(!packet)return null;
        this.busy=true;
        try{
            this.current(packet);const view=new DataView(packet.buffer,packet.byteOffset,packet.length),word=o=>view.getUint32(o,true);
            const generation=word(12),serial=word(16),tick=word(20),route=word(24),rank=word(28),position=word(32)-1,
                score=word(36),continues=word(40),profile=packet.slice(80,80+52232),name=packet.slice(80+52232),nameOffset=word(68),rowName=word(72),committed=Boolean(word(60)&0x300),scoreCommitted=Boolean(word(60)&0x100);
            let digest=2166136261;for(const value of profile)digest=Math.imul(digest^value,16777619)>>>0;
            const domain=word(76),practice=this.practice;
            if(word(0)!==0x31504d4e||word(4)!==1||word(8)!==packet.length||generation!==this.generation||!serial||!tick
                ||!(practice?domain===2&&route<6&&rank<4&&scoreCommitted&&position>=0:domain===0&&route<6&&rank<4||domain===1&&route===6&&rank===4)||position< -1||position>9
                ||score>999999999||continues>9||word(44)!==52232||word(48)!==digest||word(52)!==9||word(64)<1
                ||nameOffset!==13164||name[8]!==0||name.slice(0,8).some(c=>c<32||c>126)||!name.every((v,i)=>v===profile[nameOffset+i])
                ||rowName!==(position<0?0:route*1644+16+rank*320+position*32+6)
                ||scoreCommitted&&position>=0&&!name.every((v,i)=>v===profile[rowName+i]))throw new Error('Ordinary selected row/ST+12/independent9B native packet identity rejected');
            const extra=domain===1;
            const native={bound:true,started:true,prepared:true,acknowledged:false,tainted:extra||practice,normalClear:false,provenance:practice?'practice-native-postrun':extra?'extra-native-postrun':'ordinary-native-postrun',
                serial:1,bytes:52232,generation,frames:word(64),route,rank,digest,terminalStatsPrepared:true,terminalScore:score,terminalContinues:continues,
                menuReceiptSerial:serial,menuTick:tick,menuPhase:word(56),menuSubphase:word(60)&255,selectedScoreRow:position,nameCommitted:committed};
            if(practice){
                if(this.practiceRevision===null)throw new Error('Practice profile has no observed initial revision');
                const profileReceipt=await this.profiles.commit(this.practiceRevision,{profile,native,runId:this.practiceRunId});this.current(packet);
                if(profileReceipt?.strictFullReadback!==true||profileReceipt.strictDurability!==true||profileReceipt.revision!==this.practiceRevision+1
                    ||profileReceipt.runId!==this.practiceRunId||profileReceipt.sha256!==await sha(profile))throw new Error('Practice name and score durable readback rejected');
                this.acknowledge(generation,packet);this.practiceRevision=profileReceipt.revision;
                return {nativeAcknowledged:true,profileReceipt};
            }
            const previousName=await this.names.observe();this.current(packet);
            if(previousName?.strictRead!==true||!Number.isSafeInteger(previousName.revision)||previousName.revision<0
                ||!(previousName.bytes instanceof Uint8Array)||previousName.bytes.length!==9||previousName.bytes[8]!==0)
                throw new Error('Ordinary initial independent9B observation unavailable before any write');
            if(!committed&&!name.every((value,index)=>value===previousName.bytes[index]))
                throw new Error('Unconfirmed ordinary menu name differs from the actual independent9B observation; explicit native initial binding required');
            const capture={profile,native,runId:'normal-menu:'+generation+':'+serial},previous=await this.profiles.observe();this.current(packet);
            const profileReceipt=await this.profiles.commit(previous?.revision??0,capture);this.current(packet);
            if(profileReceipt?.strictFullReadback!==true||profileReceipt.strictDurability!==true||profileReceipt.runId!==capture.runId
                ||!profileReceipt.native||Reflect.ownKeys(profileReceipt.native).length!==Reflect.ownKeys(native).length
                ||Reflect.ownKeys(native).some(key=>!Object.hasOwn(profileReceipt.native,key)||profileReceipt.native[key]!==native[key])||!(profileReceipt.profile instanceof Uint8Array)
                ||profileReceipt.profile.length!==profile.length||!profileReceipt.profile.every((v,i)=>v===profile[i]))
                throw new Error('Ordinary complete current-menu profile receipt rejected');
            const nameReceipt=await this.names.saveStrict(name.slice(0,8),previousName);this.current(packet);
            if(nameReceipt?.strictFullReadback!==true||nameReceipt.strictDurability!==true||nameReceipt.fullFieldBytes!==9
                ||!(nameReceipt.bytes instanceof Uint8Array)||nameReceipt.bytes.length!==9||!nameReceipt.bytes.every((v,i)=>v===name[i]))
                throw new Error('Ordinary independent9B full strict receipt rejected');
            const metadata={domain:'ordinary-normal-menu-profile-name',generation,index:route,logicalTick:tick,serial,native,
                profileRevision:profileReceipt.revision,profileSHA256:profileReceipt.sha256,nameRevision:nameReceipt.revision,nameSHA256:nameReceipt.digest};
            const full=await this.receipts.commit('normal-menu/profile/'+generation+'/'+serial,metadata,packet);this.current(packet);
            if(full?.committed!==true||full.fullReadback!==true||full.durability!=='strict-indexeddb-transaction'||full.bytes!==packet.length
                ||full.sha256!==await sha(packet)||full.domain!==metadata.domain||full.generation!==generation||full.index!==route||full.logicalTick!==tick)
                throw new Error('Ordinary whole profile plus independent9B consensus receipt rejected');
            this.acknowledge(generation,packet);
            return{...full,nativeAcknowledged:true,profileReceipt,nameReceipt};
        }catch(error){if(!this.isCurrent())throw new DOMException('Ordinary menu profile owner cancelled','AbortError');throw error;}
        finally{this.busy=false;}
    }
}
export class NormalReplaySaveConsumer{
    constructor({module,generation,store,receipts,isCurrent,practice=false}){
        if(!module||!Number.isSafeInteger(generation)||generation<1||typeof store?.observe!=='function'||typeof store?.save!=='function'
            ||typeof receipts?.commitOrdinaryReplay!=='function'||typeof isCurrent!=='function')throw new Error('Ordinary occupied replay save consumers unavailable');
        if(practice&&typeof store.saveThprac!=='function')throw new Error('Practice replay writer unavailable');
        Object.assign(this,{module,generation,store,receipts,isCurrent,practice});this.rows=null;this.busy=false;this.practiceSeal=null;
    }
    sealPractice(recorder){
        if(!this.practice||this.practiceSeal||recorder?.generation!==this.generation||typeof recorder.sealForEnding!=='function')
            throw new Error('Practice replay screen requires the completed recording owner');
        const seal=recorder.sealForEnding();if(seal?.generation!==this.generation||typeof seal.encode!=='function')throw new Error('Practice replay seal identity rejected');
        this.practiceSeal=seal;
    }
    state(){if(!this.isCurrent())throw new DOMException('Ordinary replay owner cancelled','AbortError');
        const state=JSON.parse(this.module.UTF8ToString(this.module._th128_world_normal_postrun_state()));
        if(state.generation!==this.generation||state.current!==4||state.pending!==4||state.mode!==3||state.failed||state.normalClear!==false
            ||!(this.practice?state.provenance==='practice-ending16-name'&&state.ownerKind==='practice'&&!!this.practiceSeal:
                (state.provenance==='sealed-ordinary-ending16-return'&&state.extraSealed!==true)||(state.provenance==='sealed-extra-terminal23-return'&&state.extraSealed===true))
            ||state.menu?.phase!==15)throw new Error('Replay save lost its exact sealed menu owner');return state;}
    async observe(){const observed=await this.store.observe();this.state();if(!Array.isArray(observed)||observed.length!==25)throw new Error('Ordinary replay requires all25 observed slots');
        const rows=observed.map(row=>row===null?null:{...row,bytes:row.bytes?.slice(),digest:row.digest});
        for(const row of rows)if(row){if(!(row.bytes instanceof Uint8Array)||!/^[a-f0-9]{64}$/.test(row.digest)||await sha(row.bytes)!==row.digest)throw new Error('Ordinary occupied slot complete file/SHA256 differs');this.state();}
        originalReplayPreviewPacket(rows);return rows;}
    async bind(){
        if(this.busy||this.rows)throw new Error('Ordinary replay observation already bound or pending');this.busy=true;
        try{const before=this.state();if(before.menu.subphase!==0||before.menu.replayWriterBound!==false)throw new Error('Ordinary replay observation has no phase15 entry boundary');
            const rows=await this.observe(),after=this.state();if(after.serial!==before.serial||after.menu.ticks!==before.menu.ticks||after.menu.subphase!==0)throw new Error('Ordinary replay observation tick changed');
            const packet=new Uint8Array(4236),view=new DataView(packet.buffer),preview=originalReplayPreviewPacket(rows);
            [0x3150524e,1,this.generation,before.menu.ticks,1,3412].forEach((v,i)=>view.setUint32(i*4,v,true));packet.set(preview,24);
            for(let slot=0;slot<25;++slot)if(rows[slot])packet.set(Uint8Array.from(rows[slot].digest.match(/../g),v=>parseInt(v,16)),3436+slot*32);
            const pointer=this.module._malloc(packet.length);if(!pointer)throw new Error('Ordinary replay observation allocation failed');
            try{this.module.HEAPU8.set(packet,pointer);if(this.module._th128_world_normal_postrun_replay_observe(this.generation,pointer,packet.length)!==1)throw new Error('Ordinary complete occupied preview binding rejected');}
            finally{this.module._free(pointer);}this.rows=rows;return{slots:25,occupied:rows.filter(Boolean).length};
        }catch(error){if(!this.isCurrent())throw new DOMException('Ordinary replay owner cancelled','AbortError');throw error;}finally{this.busy=false;}
    }
    packet(){const m=this.module,size=m._th128_world_normal_postrun_replay_size(),pointer=m._th128_world_normal_postrun_replay_data();if(!size)return null;
        if(!Number.isSafeInteger(size)||size<(this.practice?426:427)||size>32*1024*1024+128||!Number.isSafeInteger(pointer)||pointer<1||pointer+size>m.HEAPU8.length)throw new Error('Full native replay packet unavailable');return m.HEAPU8.slice(pointer,pointer+size);}
    current(packet){const state=this.state(),view=new DataView(packet.buffer,packet.byteOffset,packet.length),after=this.packet();
        if(state.menu.ticks!==view.getUint32(20,true)||state.menu.subphase!==3||state.menu.replaySavePending!==true||!after||after.length!==packet.length||!after.every((v,i)=>v===packet[i]))throw new Error('Ordinary replay packet owner/tick/full bytes changed');}
    async commit(){
        if(this.busy)throw new Error('Ordinary replay save receipt already pending');const packet=this.packet();if(!packet)return null;if(!this.rows)throw new Error('Ordinary save has no actual25-slot observation');this.busy=true;
        try{this.current(packet);const view=new DataView(packet.buffer,packet.byteOffset,packet.length),word=o=>view.getUint32(o,true),slot=word(24),raw=packet.slice(128),hex=bytes=>Array.from(bytes,b=>b.toString(16).padStart(2,'0')).join('');
            const expectedDigest=word(28)?hex(packet.slice(64,96)):null,rawSHA256=hex(packet.slice(96,128));
            const route=word(40),rank=word(44),last=word(52),practice=this.practice;
            if(word(0)!==0x3153524e||word(4)!==1||word(8)!==packet.length||word(12)!==this.generation||!word(16)||!word(20)||slot>=25||word(28)>1
                ||word(32)!==raw.length||word(36)!==OwnedReplayStore.nativeChecksum(raw)
                ||!(practice?route<6&&rank<4&&last===[3,5,8,10,13,15][route]:((route<6&&rank<4)||(route===6&&rank===4))&&last===route+17)||!word(56)||!word(60)
                ||expectedDigest!==(this.rows[slot]?.digest??null)||!word(28)&&packet.slice(64,96).some(Boolean)||await sha(raw)!==rawSHA256)
                throw new Error('Ordinary immutable native slot/variable bytes/FNV/SHA256 identity rejected');this.current(packet);
            const nativeFile=nativeReplayToFile(raw),decoded=decodeReplay(nativeFile);
            if((practice?decoded.stages.length<1||decoded.stages.length>3||decoded.stages.at(-1).stage!==last:decoded.stages.length!==(route===6?1:3))
                ||decoded.stages.reduce((sum,s)=>sum+s.numFrames,0)!==word(56)||decoded.metadata.route!==route||decoded.metadata.rank!==rank
                ||decoded.metadata.scoreRaw!==word(48)||decoded.metadata.lastStage!==last)throw new Error('Replay metadata differs from the sealed command');
            const file=practice?this.practiceSeal.encode(raw,nativeFile):nativeFile;
            const options={expectedDigest,isCurrent:this.isCurrent};
            const saved=practice?await this.store.saveThprac(slot,file,options):await this.store.save(slot,raw,options);this.current(packet);
            const savedBytes=saved?.bytes?.slice(),fileSHA256=saved?.digest;
            if(saved?.strictFullReadback!==true||saved.strictDurability!==true||saved.slot!==slot
                ||(practice?saved.practice!==true:saved.rawBytes!==raw.length||saved.rawChecksum!==word(36))
                ||!(savedBytes instanceof Uint8Array)||savedBytes.length!==file.length||!savedBytes.every((v,i)=>v===file[i])||await sha(savedBytes)!==fileSHA256)
                throw new Error('Ordinary full variable replay strict save/readback rejected');this.current(packet);
            const rows=await this.observe();this.current(packet);
            for(let index=0;index<25;++index)if(index===slot){if(rows[index]?.digest!==fileSHA256||rows[index].bytes.length!==savedBytes.length||!rows[index].bytes.every((v,i)=>v===savedBytes[i]))throw new Error('Ordinary saved slot reread differs');}
                else if((rows[index]?.digest??null)!==(this.rows[index]?.digest??null))throw new Error('Ordinary other replay slot changed before native ACK');
            const preview=originalReplayPreviewPacket(rows),ack=new Uint8Array(packet.length+32+3412);ack.set(packet);ack.set(Uint8Array.from(fileSHA256.match(/../g),v=>parseInt(v,16)),packet.length);ack.set(preview,packet.length+32);
            const metadata={domain:practice?'practice-full-variable-replay-save':route===6?'extra-full-variable-replay-save':'ordinary-full-variable-replay-save',generation:this.generation,index:slot,logicalTick:word(20),serial:word(16),expectedDigest,rawBytes:raw.length,rawChecksum:word(36),rawSHA256,fileBytes:savedBytes.length,fileSHA256};
            const receipt=await this.receipts.commitOrdinaryReplay('normal-menu/replay/'+this.generation+'/'+word(16),metadata,ack);this.current(packet);
            if(receipt?.committed!==true||receipt.fullReadback!==true||receipt.durability!=='strict-indexeddb-transaction'||receipt.bytes!==ack.length||receipt.sha256!==await sha(ack)
                ||receipt.domain!==metadata.domain||receipt.generation!==metadata.generation||receipt.index!==slot||receipt.logicalTick!==metadata.logicalTick)throw new Error('Ordinary complete replay consensus receipt rejected');
            this.current(packet);const pointer=this.module._malloc(ack.length);if(!pointer)throw new Error('Ordinary replay ACK allocation failed');
            try{this.module.HEAPU8.set(ack,pointer);if(this.module._th128_world_normal_postrun_replay_ack(this.generation,pointer,ack.length)!==1)throw new Error('Ordinary exact native full replay ACK rejected');}finally{this.module._free(pointer);}
            this.rows=rows;return{...receipt,nativeAcknowledged:true,slot,fileSHA256};
        }catch(error){if(!this.isCurrent())throw new DOMException('Ordinary replay owner cancelled','AbortError');throw error;}finally{this.busy=false;}
    }
    close(){this.store.close?.();}
}
export class NormalMainReturnConsumer{
    constructor({module,generation,profiles,names,receipts,music,isCurrent}){if(!module||!Number.isSafeInteger(generation)||generation<1||typeof profiles?.observe!=='function'||typeof names?.observe!=='function'
        ||typeof receipts?.commit!=='function'||typeof music?.snapshot!=='function'||typeof isCurrent!=='function')throw new Error('Ordinary sealed Main consumers unavailable');
        Object.assign(this,{module,generation,profiles,names,receipts,music,isCurrent});this.busy=false;this.finished=false;}
    current(packet=null){if(!this.isCurrent())throw new DOMException('Ordinary Main owner cancelled','AbortError');
        const state=JSON.parse(this.module.UTF8ToString(this.module._th128_world_normal_postrun_state()));if(state.generation!==this.generation||state.failed||state.current!==4||state.pending!==4||state.mode!==3||state.normalClear!==false||state.menu?.mainHandoffReady!==true)
            throw new Error('Ordinary sealed Main lost its current4 boundary');if(packet){const after=this.packet();if(!after||after.length!==packet.length||!after.every((v,i)=>v===packet[i]))throw new Error('Ordinary full Main descriptor changed');}return state;}
    packet(){const m=this.module,n=m._th128_world_normal_postrun_main_size(),p=m._th128_world_normal_postrun_main_data();if(!n)return null;
        if(n!==52360||!Number.isSafeInteger(p)||p<1||p+n>m.HEAPU8.length)throw new Error('Ordinary Main full profile descriptor unavailable');return m.HEAPU8.slice(p,p+n);}
    memory(bytes,call){const p=this.module._malloc(bytes.length);if(!p)throw new Error('Ordinary Main ACK allocation failed');try{this.module.HEAPU8.set(bytes,p);return call(p,bytes.length);}finally{this.module._free(p);}}
    async commit(){if(this.busy||this.finished)throw new Error('Ordinary Main return is duplicate or pending');this.busy=true;
        try{this.current();if(this.module._th128_world_normal_postrun_main_prepare(this.generation)!==1)throw new Error('Ordinary native Main preparation rejected');const packet=this.packet();this.current(packet);
            const view=new DataView(packet.buffer,packet.byteOffset,packet.length),word=p=>view.getUint32(p,true),profile=packet.slice(128),name=profile.slice(13164,13173);let digest=2166136261;for(const b of profile)digest=Math.imul(digest^b,16777619)>>>0;
            if(word(0)!==0x314d4e4f||word(4)!==1||word(8)!==52360||word(12)!==this.generation||word(16)!==1||!word(20)||!word(24)||!word(28)||!((word(84)===0&&word(32)<6&&word(36)<4)||(word(84)===1&&word(32)===6&&word(36)===4))||word(40)>9||word(44)!==52232||word(48)!==digest
                ||!word(56)||word(60)!==3||word(64)>65535||word(72)>65535||word(80)!==1||word(84)>1||word(88)>1023||word(92)!==13164||packet.slice(96,128).some(Boolean)||name[8]!==0)
                throw new Error('Ordinary native Main full owner/profile/RNG identity rejected');
            const observed=await this.profiles.observe();this.current(packet);
            if(!observed||!(observed.profile instanceof Uint8Array)||observed.profile.length!==52232||!observed.profile.every((v,i)=>v===profile[i])||observed.native?.generation!==this.generation
                ||observed.native.route!==word(32)||observed.native.rank!==word(36)||observed.native.frames!==word(28)||observed.native.normalClear!==false||observed.native.tainted!==(word(84)===1)
                ||observed.native.provenance!==(word(84)===1?'extra-native-postrun':'ordinary-native-postrun'))
                throw new Error('Ordinary Main requires the exact complete persisted native profile');
            const last=await this.names.observe();this.current(packet);if(last?.strictRead!==true||!(last.bytes instanceof Uint8Array)||last.bytes.length!==9||!last.bytes.every((v,i)=>v===name[i]))throw new Error('Ordinary Main independent9B observation differs');
            const snapshot=this.music.snapshot(),source=this.music.source,buffer=this.music.buffer,track=this.music.track;
            if(snapshot.track!=='th128_07.wav'||snapshot.playing!==true||!source||!buffer||!track||this.music.disposed||this.music.context.state==='closed')throw new Error('Ordinary Main original Music7 source unavailable');
            const metadata={domain:'ordinary-postrun-sealed-main',generation:this.generation,index:word(32),logicalTick:word(20),serial:1,profileSHA256:observed.sha256,profileRevision:observed.revision,nameDigest:last.digest,nameRevision:last.revision,gameSeed:word(64),gameCalls:word(68),displaySeed:word(72),displayCalls:word(76),music:snapshot};
            const receipt=await this.receipts.commit('normal-menu/main/'+this.generation,metadata,packet);this.current(packet);
            if(receipt?.committed!==true||receipt.fullReadback!==true||receipt.durability!=='strict-indexeddb-transaction'||receipt.bytes!==packet.length||receipt.sha256!==await sha(packet)||receipt.domain!==metadata.domain||receipt.generation!==this.generation||receipt.index!==word(32)||receipt.logicalTick!==word(20))throw new Error('Ordinary Main full strict consensus receipt rejected');
            this.current(packet);if(this.music.source!==source||this.music.buffer!==buffer||this.music.track!==track||JSON.stringify(this.music.snapshot())!==JSON.stringify(snapshot)||this.music.context.state==='closed')throw new Error('Ordinary Main running Music7 owner changed during receipt');
            if(this.memory(packet,(p,n)=>this.module._th128_world_normal_postrun_main_ack(this.generation,p,n))!==1)throw new Error('Ordinary exact native Main receipt ACK rejected');
            if(this.module._th128_world_normal_postrun_main_commit(this.generation)!==1)throw new Error('Ordinary sealed Main commit rejected');
            this.finished=true;return{...receipt,nativeCommitted:true,generation:this.generation,profileDigest:digest};
        }catch(error){if(!this.isCurrent())throw new DOMException('Ordinary Main owner cancelled','AbortError');throw error;}finally{this.busy=false;}
    }
}
export class NormalPostrunController{
    constructor({module,renderer,generation,assets,readAsset,loadBank,music,soundQueue,receipts,profileConsumer,replayConsumer,mainConsumer,isCurrent,practice=false,practiceRecorder=null}){
        if(!module||typeof renderer?.render!=='function'||!Number.isSafeInteger(generation)||generation<1
            ||!Array.isArray(assets)||assets.length!==4||typeof readAsset!=='function'||typeof loadBank!=='function'
            ||typeof music?.switchTo!=='function'||typeof soundQueue?.consumeNativeRequest!=='function'
            ||typeof soundQueue?.flush!=='function'||typeof receipts?.commit!=='function'||typeof replayConsumer?.bind!=='function'||typeof replayConsumer?.commit!=='function'||typeof isCurrent!=='function')
            throw new Error('Ordinary sealed postrun consumers are unavailable');
        for(const name of ['asset','begin','input','media','media_ack','present','state','replay_observe','replay_size','replay_data','replay_ack','main_prepare','main_size','main_data','main_ack','main_commit'])
            if(typeof module['_th128_world_normal_postrun_'+name]!=='function')throw new Error('Ordinary postrun native API missing: '+name);
        if(practice&&typeof module._th128_world_practice_postrun_begin!=='function')throw new Error('Practice registration requires the native Ending16 consumer');
        Object.assign(this,{module,renderer,generation,assets,readAsset,loadBank,music,soundQueue,receipts,profileConsumer,replayConsumer,mainConsumer,isCurrent,practice,practiceRecorder});
        this.input=new OriginalEndingInput();this.serial=0;this.busy=false;this.paused=false;this.stepPromise=null;this.pausePromise=null;this.phase='new';this.closed=false;this.lastDraws=null;
    }
    current(){if(this.closed||!this.isCurrent())throw new DOMException('Ordinary postrun owner cancelled','AbortError');}
    accepted(value){if(value!==1)throw new Error(this.module.UTF8ToString(this.module._th128_world_error())||'Ordinary native postrun rejected');}
    memory(bytes,call){const pointer=this.module._malloc(bytes.length);if(!pointer)throw new Error('Ordinary postrun allocation failed');
        try{this.module.HEAPU8.set(bytes,pointer);return call(pointer,bytes.length);}finally{this.module._free(pointer);}}
    state(){this.current();const state=JSON.parse(this.module.UTF8ToString(this.module._th128_world_normal_postrun_state()));
        if(state.generation!==this.generation||state.failed||!(this.practice?state.provenance==='practice-ending16-name'&&state.ownerKind==='practice':
            (state.provenance==='sealed-ordinary-ending16-return'&&state.extraSealed!==true)||(state.provenance==='sealed-extra-terminal23-return'&&state.extraSealed===true))
            ||state.current!==4||state.pending!==4||state.mode!==3||state.normalClear!==false||!state.present)
            throw new Error('Ordinary postrun lost its sealed native owner');return state;}
    async start(ending){
        if(this.busy||this.phase!=='new'||ending?.phase!=='return-requested'||ending.ownerKind!==(this.practice?'practice':'ordinary')||ending.generation!==this.generation)
            throw new Error('Ordinary postrun requires the same actual ended source');
        this.busy=true;try{
            this.current();
            for(let index=0;index<4;++index){const asset=this.assets[index],bank=[0,24,25,11][index];
                if(asset.bank!==bank||!Number.isSafeInteger(asset.bytes)||asset.bytes<1||asset.bytes>16*1024*1024||!/^[a-f0-9]{64}$/.test(asset.sha256))throw new Error('Ordinary postrun original asset identity rejected');
                const bytes=await this.readAsset(asset);this.current();
                if(!(bytes instanceof Uint8Array)||bytes.length!==asset.bytes||await sha(bytes)!==asset.sha256)throw new Error('Ordinary postrun original full asset bytes differ');
                this.current();this.accepted(this.memory(bytes,(p,n)=>this.module._th128_world_normal_postrun_asset(this.generation,bank,p,n)));
                await this.loadBank(bank,asset);this.current();
            }
            if(this.practice){
                const profile=await this.profileConsumer.practiceProfile();this.current();
                this.replayConsumer.sealPractice(this.practiceRecorder);
                const begin=(p,n)=>this.module._th128_world_practice_postrun_begin(this.generation,p,n);
                this.accepted(profile.length?this.memory(profile,begin):begin(0,0));
            }else this.accepted(this.module._th128_world_normal_postrun_begin(this.generation));
            this.state();this.phase='active';
        }catch(error){this.phase='failed';throw error;}finally{this.busy=false;}
    }
    async startExtra(snapshot){
        if(this.busy||this.phase!=='new'||snapshot?.provenance!=='sealed-extra-terminal23-return'||snapshot?.extraSealed!==true||snapshot.generation!==this.generation)
            throw new Error("EX controller requires its actual sealed terminal23 owner");
        this.busy=true;try{this.current();for(let index=0;index<4;index++){const asset=this.assets[index],bytes=await this.readAsset(asset);this.current();if(!(bytes instanceof Uint8Array)||bytes.length!==asset.bytes||await sha(bytes)!==asset.sha256)throw new Error('EX original resource full bytes differ');this.current();await this.loadBank([0,24,25,11][index],asset);this.current();}this.state();this.phase='active';await this.media();}
        catch(e){this.phase='failed';throw e;}finally{this.busy=false;}
    }
    async media(){
        this.current();const packet=JSON.parse(this.module.UTF8ToString(this.module._th128_world_normal_postrun_media()));
        if(packet.epoch!==this.generation||!Array.isArray(packet.commands)||packet.commands.length>128)throw new Error('Ordinary postrun media window identity rejected');
        if(!packet.commands.length){if(packet.serial!==0)throw new Error('Ordinary empty media has a window');return;}
        const results=[];for(const command of packet.commands){
            if(command.kind===0&&command.operation===1&&[7,8].includes(command.identifier)){
                this.music.switchTo('th128_'+String(command.identifier).padStart(2,'0')+'.wav');results.push({...command,snapshot:this.music.snapshot()});
            }else if(command.kind===0&&command.operation===2&&[7,8].includes(command.identifier)){
                results.push({...command,nativeProfileHeardMarker:true,snapshot:this.music.snapshot()});
            }else if(command.kind===0&&[3,4].includes(command.operation)&&this.state().extraSealed===true){this.music.stopSource();results.push({...command,stopped:true});
            }else if(command.kind===1){const accepted=this.soundQueue.consumeNativeRequest(command.identifier,command.position,command.operation,{present:true});results.push({...command,accepted});}
            else throw new Error('Ordinary native postrun media operation unsupported');
        }
        const flushed=this.soundQueue.flush({present:true}),metadata={domain:'ordinary-postrun-media',generation:this.generation,frame:this.serial,packet,results,flushed},payload=new TextEncoder().encode(JSON.stringify(metadata));
        const receipt=await this.receipts.commit('normal-postrun/media/'+packet.serial,metadata,payload);this.current();
        const after=JSON.parse(this.module.UTF8ToString(this.module._th128_world_normal_postrun_media()));
        if(JSON.stringify(after)!==JSON.stringify(packet)||receipt?.committed!==true||receipt.fullReadback!==true
            ||receipt.durability!=='strict-indexeddb-transaction'||receipt.bytes!==payload.length||receipt.sha256!==await sha(payload))
            throw new Error('Ordinary native postrun media complete strict receipt differs');
        this.accepted(this.module._th128_world_normal_postrun_media_ack(this.generation,packet.serial,packet.commands.length,packet.digest));
    }
    async step(){
        if(this.stepPromise||this.paused)throw new Error('Ordinary postrun step already pending or paused');
        const pending=this.stepSerial();this.stepPromise=pending;
        try{return await pending;}finally{if(this.stepPromise===pending)this.stepPromise=null;}
    }
    async pause(){
        this.current();this.paused=true;this.input.release();const pending=this.stepPromise;
        const task=(async()=>{await pending;this.current();if(this.music.context.state==='running')await this.music.context.suspend();})();
        this.pausePromise=task;return task;
    }
    async resume(){
        await this.pausePromise;this.current();if(this.phase!=='active')return false;
        if(this.music.context.state==='suspended')await this.music.context.resume();this.current();this.paused=false;return true;
    }
    async stepSerial(){
        if(this.busy||this.phase!=='active')throw new Error('Ordinary postrun serial step is unavailable');
        this.current();const before=this.state();
        if(before.menu.phase===14&&before.menu.nameGlyphRendererBound!==true){this.phase='awaiting-name-owner';return before;}
        this.busy=true;try{
            if(before.menu.phase===15&&before.menu.replayWriterBound!==true){await this.replayConsumer.bind();this.current();if(this.state().menu.replayWriterBound!==true)throw new Error('Ordinary occupied replay owner did not bind');}
            const input=this.input.sample(),ticks=116444736000000000n+BigInt(Date.now())*10000n,serial=this.serial+1;
            this.accepted(this.module._th128_world_normal_postrun_input(this.generation,serial,input.holding,input.pressed,Number(ticks&0xffffffffn),Number(ticks>>32n)));
            this.serial=serial;await this.media();this.current();
            if(!this.profileConsumer)throw new Error('Ordinary complete menu profile/name owner unavailable');
            await this.profileConsumer.commit();this.current();
            await this.replayConsumer.commit();this.current();
            const frame=JSON.parse(this.module.UTF8ToString(this.module._th128_world_normal_postrun_present(this.generation)));
            if(frame.generation!==this.generation||frame.serial!==serial||!Array.isArray(frame.draws))throw new Error('Ordinary postrun native presentation identity rejected');
            this.lastDraws=orderDrawLayers(frame.draws);this.renderer.render(this.lastDraws,{nativeMenu:true});return this.state();
        }catch(error){this.phase='failed';throw error;}finally{this.busy=false;}
    }
    async finishMain(){
        await this.stepPromise;await this.pausePromise;this.current();
        if(this.practice){
            if(this.busy||this.phase!=='active'||this.state().menu?.mainHandoffReady!==true)throw new Error('Practice return waits for the committed name screen');
            this.accepted(this.module._th128_world_normal_postrun_main_commit(this.generation));this.phase='main-committed';
            return {nativeCommitted:true,generation:this.generation};
        }
        if(this.busy||this.phase!=='active'||!this.mainConsumer)throw new Error('Ordinary Main transition waits its exact completed step owner');
        this.busy=true;try{const receipt=await this.mainConsumer.commit();this.current();this.phase='main-committed';return receipt;}finally{this.busy=false;}
    }
    detachMainMusic(){this.current();if(this.phase!=='main-committed'||!this.music)throw new Error('Ordinary Main music transfer has no sealed native commit');const music=this.music;this.music=null;return music;}
    redraw(){if(this.lastDraws){this.current();this.renderer.render(this.lastDraws,{nativeMenu:true});}}
    close(){if(this.closed)return;this.closed=true;this.phase='closed';this.input.release();this.music?.dispose?.();this.profileConsumer?.names.close?.();this.replayConsumer.close?.();this.receipts.close();}
}
export async function createNormalPostrunController(options){
    const music=options.retainMusic??new StageMusic(options.audio,options.gain),receipts=await OwnedEndingReceipts.open();
    try{if(!options.retainMusic)await music.load(options.tracks,options.base,options.resolveUrl,{signal:options.signal,tracks:['th128_08.wav','th128_07.wav']});
        if(!options.isCurrent())throw new DOMException('Ordinary postrun preload cancelled','AbortError');
        const names=new OwnedLastNameFieldStore(),profiles=new OwnedNativeTitleRecordStore(undefined,{practice:options.practice});
        const profileConsumer=new NormalMenuProfileConsumer({module:options.module,generation:options.generation,profiles,names,receipts,isCurrent:options.isCurrent,practice:options.practice});
        const replayConsumer=new NormalReplaySaveConsumer({module:options.module,generation:options.generation,store:new OwnedReplayStore(),receipts,isCurrent:options.isCurrent,practice:options.practice});
        const mainConsumer=new NormalMainReturnConsumer({module:options.module,generation:options.generation,profiles,names,receipts,music,isCurrent:options.isCurrent});
        return new NormalPostrunController({...options,music,receipts,profileConsumer,replayConsumer,mainConsumer});
    }catch(error){music.dispose();receipts.close();throw error;}
}
