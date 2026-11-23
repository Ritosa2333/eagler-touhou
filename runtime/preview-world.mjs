import {openTh128Dat} from '../../th128/site/runtime/th128-dat.mjs';
import {parseEclIncludes} from '../../th128/site/runtime/ecl-includes.mjs';
import {originalType33Bytes} from '../../th128/site/runtime/original-bullet-type33.mjs';
import {readWorldDrawRecords} from '../../th128/site/runtime/world-presentation.mjs';
import {coopWire} from './coop-rules.mjs';
import {bindCoopStage} from './coop-stage.mjs';
import {bindNativeRollbackWorld} from './native-world.mjs';

const text=new TextDecoder('utf-8',{fatal:true}),utf8=new TextEncoder();
const readJson=async(resources,key)=>JSON.parse(text.decode(await resources.read(key)));
const typeList=[4,6,10,13,16,18,20,21,23,24,25,29,30];

// Short local MP validation route. Its Worlds are independent of the normal
// recorder, title menus and persistent state. A separate host owns those.
export function prepareA13Preview(options){return prepareNativeStage(options,3);}
export async function prepareCoopStage(options){
  const {rules,resources,signal}=options;coopWire(rules);
  const first=rules.stageId,ids=first===16?[16]:[first,first+1,first+2,first+3,first+4];
  // Decode the verified resources once per run, not per World, and never read a file inside a rollback step.
  const cache=fn=>{const values=new Map();return key=>{if(!values.has(key))values.set(key,Promise.resolve().then(()=>fn(key)));return values.get(key);};};
  const read=cache(k=>resources.read(k)),archive=typeof resources.raw==='function'?null:openTh128Dat(await read('dat'));
  const raw=cache(name=>archive?archive.read(name):resources.raw(name));
  const shared={...options,resources:{build:resources.build,read,raw},original33:await originalType33Bytes()};
  const stages=new Map();for(const id of ids){signal?.throwIfAborted();stages.set(id,await prepareNativeStage(shared,id,rules));}
  const at=id=>{const stage=stages.get(id);if(!stage)throw Error('Native transition left the agreed campaign');return stage;};
  return Object.freeze({banks:at(first).banks,banksFor:id=>at(id).banks,
    openBackground:(id=first,ownerSignal)=>at(id).openBackground(ownerSignal),
    openCoopStage:(selection,ownerSignal)=>at(first).openCoopStage(selection,ownerSignal,at)});
}
const entries=new Map([
  [1,['st_a1_1.ecl','st_a1.msg','stage_a1']],[2,['st_a1_2.ecl','st_a1_2.msg','stage_b2']],[3,['st_a1_3.ecl','st_a1_3.msg','stage_c3']],
  [4,['st_a2_2.ecl','st_a2_2.msg','stage_c2']],[5,['st_a2_3.ecl','st_a2_3.msg','stage_b3']],
  [6,['st_b1_1.ecl','st_b1.msg','stage_b1']],[7,['st_b1_2.ecl','st_b1_2.msg','stage_a2']],[8,['st_b1_3.ecl','st_b1_3.msg','stage_c3']],
  [9,['st_b2_2.ecl','st_b2_2.msg','stage_c2']],[10,['st_b2_3.ecl','st_b2_3.msg','stage_a3']],
  [11,['st_c1_1.ecl','st_c1.msg','stage_c1']],[12,['st_c1_2.ecl','st_c1_2.msg','stage_a2']],[13,['st_c1_3.ecl','st_c1_3.msg','stage_b3']],
  [14,['st_c2_2.ecl','st_c2_2.msg','stage_b2']],[15,['st_c2_3.ecl','st_c2_3.msg','stage_a3']],[16,['st_ex.ecl','st_ex.msg','stage_ex']]]);
async function prepareNativeStage({resources,factory,backgroundFactory,original33,onFactory,onWorldStart,onTick,signal},stageId,boundRules){
  const bounded=()=>signal?.throwIfAborted();
  if(typeof factory!=='function'||[onFactory,onWorldStart,onTick].some(fn=>typeof fn!=='function'))
    throw new TypeError('The multiplayer preview requires a bounded native factory');
  const [wasm,dat,catalog,baseBullet,iceMasks]=await Promise.all([
    resources.read(boundRules&&resources.build.gameplay==='same-stage-coop'?'coopWasm':'wasm'),typeof resources.raw==='function'?null:resources.read('dat'),readJson(resources,'atlas'),resources.read('baseBullet'),resources.read('iceMasks'),
  ]);
  bounded();
  const stage=catalog.stages.find(row=>row.id===stageId),[ecl,message,background]=entries.get(stageId);
  if(stage?.ecl!==ecl||stage.message!==message||stage.background!==background+'.std')throw new Error('Original stage resources differ from the selection');
  const archive=dat===null?null:openTh128Dat(dat),raw=name=>archive?archive.read(name):resources.raw(name),ecls=new Map();
  async function add(name){
    if(ecls.has(name))return;
    if(ecls.size>=32)throw new Error('Preview ECL include budget exhausted');
    const bytes=await raw(name);bounded();ecls.set(name,bytes);
    for(const child of parseEclIncludes(bytes).includes.ECLI)await add(child);
  }
  await add(stage.ecl);
  const enemies=parseEclIncludes(ecls.get(stage.ecl)).includes.ANIM;
  if(enemies.length<3||enemies.length>4)throw new Error('Unexpected A1-3 animation banks');
  const art=['bullet.anm','text.anm','pl00.anm','pl00a.sht','front.anm','ascii.anm','card.anm',enemies[2],'ice.anm',stage.message];
  const [bullet,textAnm,player,shot,front,font,card,boss,ice,msg]=await Promise.all(art.map(raw));
  const enemyAnm=await Promise.all(enemies.map(raw));
  let logo=null;
  if(boundRules){
    const tracks=stage.music?.map(track=>/^th128_(\d{2})\.wav$/.exec(track)?.[1]);
    if(!/^logo_[a-z0-9_]+\.anm$/.test(stage.logo??'')||tracks?.length!==2||tracks.some(t=>t===undefined))
      throw Error('Original stage logo or caption identity differs');
    logo={name:stage.logo,bytes:await raw(stage.logo),tracks:tracks.map(Number)};
  }
  const [backgroundData,backgroundAnm]=await Promise.all([raw(stage.background),raw(background+'.anm')]);
  const profiles=await Promise.all(typeList.map(async type=>[type,await resources.read(`bullet${type}`)]));
  for(const type of [9,32]){
    const row=await readJson(resources,`bullet${type}`);
    if(row.type!==type||!/^[0-9a-f]{576}$/.test(row.hex))throw new Error(`Unexpected typed profile ${type}`);
    profiles.push([type,Uint8Array.from(row.hex.match(/../g),pair=>parseInt(pair,16))]);
  }
  profiles.push([33,original33??await originalType33Bytes()]);
  const banks=Object.freeze([[0,'bullet'],...enemies.map((name,index)=>[index+1,name.slice(0,-4)]),
    [7,'pl00'],[6,'text'],[8,background],[5,'front'],[11,'ascii'],[12,'card'],[36,'ice'],...(logo?[[30,logo.name.slice(0,-4)]]:[])].map(Object.freeze));
  bounded();
  async function instantiate(kind,ownerSignal){
    bounded();ownerSignal?.throwIfAborted();onFactory(kind);
    const background=kind==='background'&&typeof backgroundFactory==='function';
    const selected=background?backgroundFactory:factory;const module=await selected({wasmBinary:background?await resources.read('wasm'):wasm,print(){},printErr:message=>{throw new Error(message);}});
    bounded();ownerSignal?.throwIfAborted();return module;
  }
  function withMemory(module,arrays,run){
    const pointers=[];
    try{
      const args=arrays.flatMap(bytes=>{
        const pointer=module._malloc(bytes.length);
        if(!pointer)throw new Error('Cannot allocate preview resource');
        pointers.push(pointer);module.HEAPU8.set(bytes,pointer);return [pointer,bytes.length];
      });
      return run(args);
    }finally{for(const pointer of pointers)module._free(pointer);}
  }
  async function configuredModule(rules,ownerSignal,cooperative=false){
    const m=await instantiate('game',ownerSignal);install(m,rules,ownerSignal,cooperative);return m;
  }
  function install(m,rules,ownerSignal,cooperative,carry=null){
    if((!cooperative&&(rules.route!=='A1-3'||rules.difficulty!==1))||(!carry&&rules.stageId!==stageId)||!Number.isInteger(rules.difficulty)||rules.difficulty<0||rules.difficulty>4||(rules.difficulty===4)!==(stageId===16)||rules.rollbackWindow>2||rules.coreSHA256!==resources.build.coreSHA256||
      rules.resourceSHA256!==resources.build.resourceSHA256)throw new Error('Preview World does not match the local A1-3 build');
    if(cooperative&&typeof m._th128_coop_bind!=='function')throw new Error('This is not a native co-op core');
    const ok=value=>{if(value!==1)throw new Error(m.UTF8ToString(m._th128_world_error())||'Native preview setup rejected');};
    const bind=(name,arrays,head=[],tail=[])=>withMemory(m,arrays,args=>ok(m[`_th128_world_${name}`](...head,...args,...tail)));
    m._th128_world_reset();
    for(const [name,bytes] of ecls)bind('load',[utf8.encode(name),bytes]);
    onWorldStart();bind('start',[utf8.encode(stage.ecl)],[],[rules.difficulty,rules.seed]);
    if(carry){bind('bind_stage_carry_target',[utf8.encode(stage.ecl)],[carry.token],[rules.difficulty]);ok(m._th128_world_restore_stage_carry_random(carry.token,stageId,carry.progress,rules.difficulty));}
    if(cooperative&&logo)bind('load_stage_intro',[utf8.encode(logo.name),logo.bytes],[stageId]);
    enemyAnm.forEach((bytes,index)=>bind('load_enemy_anm',[bytes],[index+1]));
    bind('enable_geometry526',[textAnm]);bind('enable_bullets',[baseBullet,bullet],[],[0,400]);bind('enable_bullet_updates',[shot]);
    ok(m._th128_world_enable_graze(0,10000,0x1234));
    bind('enable_player_context',[player],[],[1,0,120,0,0,0,0,0,0]);ok(m._th128_world_enable_player_motion(0,51200));
    withMemory(m,[new Uint8Array(8000)],([groups])=>bind('enable_player_shots',[shot,player],[],[groups,2000,0,rules.difficulty,0,0,0,0]));
    ok(m._th128_world_enable_shot_hits(cooperative?20000:100000,0,0,0,0));ok(m._th128_world_enable_rapid_shot(-1));ok(m._th128_world_enable_player_death(-1));
    bind('enable_perfect_hud',[front]);bind('load_score_font',[font]);bind('enable_spells',[bullet,boss,textAnm,font,card,player]);
    withMemory(m,[new Uint8Array(32)],([table])=>ok(m._th128_world_enable_player_respawn(1,0,table,8)));
    ok(m._th128_world_enable_head_power());ok(m._th128_world_enable_focus_indicator());bind('enable_z_fire',[ice,iceMasks]);
    for(const [type,bytes] of profiles)bind('add_bullet_profile',[bytes],[type]);
    bind('enable_messages',[msg,textAnm,player,boss]);bind('enable_message_front',[front]);
    if(cooperative&&[1,6,11].includes(stageId))bind('enable_route_choice',[front],[],[stageId,0,0]);
    if(cooperative&&[1,2,4,6,7,9,11,12,14].includes(stageId))bind('enable_stage_transition',[front],[],[stageId,carry?.progress??0,0,0,1,0]);
    bounded();ownerSignal?.throwIfAborted();
    if(cooperative)withMemory(m,[shot,player,ice,iceMasks],args=>ok(m._th128_coop_bind(rules.players,...args,Number(rules.challengeMode),(rules.freezeMotivationMultiplier??1)*4)));
    if(cooperative&&[3,5,8,10,13,15,16].includes(stageId))withMemory(m,[front],args=>ok(m._th128_coop_enable_clear(...args)));
    if(carry){ok(m._th128_world_restore_stage_carry(carry.token));ok(m._th128_coop_finish_stage(carry.token));}
  }
  async function openSeat(rules,seat,ownerSignal,holdGameOver){
    const m=await configuredModule(rules,ownerSignal);
    const engine=bindNativeRollbackWorld({module:m,seat,checkpointSlots:rules.rollbackWindow,maxCheckpointBytes:128*1024*1024,beforeTick:onTick,holdGameOver});
    return Object.freeze({...engine,
      state:()=>JSON.parse(m.UTF8ToString(m._th128_world_presentation_state())),
      draws:()=>readWorldDrawRecords(m,{fastNumbers:true}),
      stateText:()=>m.UTF8ToString(m._th128_world_state()),
    });
  }
  // Deliberately not the author-room default until every co-op gameplay mode is verified.
  async function openCoopStage(rules,ownerSignal,campaign){
    if(boundRules&&JSON.stringify(coopWire(rules))!==JSON.stringify(coopWire(boundRules)))throw Error('Prepared stage belongs to another cooperative match');
    if(rules.mode!=='same-stage-coop'||![2,3].includes(rules.players)||typeof rules.challengeMode!=='boolean'||rules.thpracEnabled!==undefined)throw Error('Unsupported co-op settings');
    const m=await configuredModule(rules,ownerSignal,true);
    const engine=bindCoopStage({module:m,players:rules.players,stageId,window:rules.rollbackWindow,beforeTick:onTick,
      caption(id,role){if(campaign)campaign(id).showCaption(m,role);},
      handoff(target){const next=campaign?.(target);if(!next)throw Error('No approved cooperative stage successor');
        const token=m._th128_coop_capture_stage(target);if(!token)throw Error(m.UTF8ToString(m._th128_world_error())||'Missing native cooperative carry');
        const progress=m._th128_coop_next_progress(token);if(progress<0)throw Error('Invalid cooperative carry progress');
        next.install(m,rules,ownerSignal,true,{token,progress});}});
    function startEnding(inputs){
      ownerSignal?.throwIfAborted();
      if(engine.outcome()!=='cleared'||!Array.isArray(inputs)||inputs.length!==4)throw Error('Original ending only opens on a sealed main campaign');
      const error=()=>m.UTF8ToString(m._th128_world_error())||'Original cooperative ending rejected';
      const ptr=[];try{
        for(const bytes of inputs){if(!(bytes instanceof Uint8Array)||!bytes.length||bytes.length>8*1024*1024)throw Error('Invalid ending input');const at=m._malloc(bytes.length);if(!at)throw Error('Native ending allocation failed');ptr.push(at);m.HEAPU8.set(bytes,at);}
        if(m._th128_coop_start_ending(...inputs.flatMap((bytes,i)=>[ptr[i],bytes.length]))!==1)throw Error(error());
      }finally{for(const at of ptr)m._free(at);}
      const snapshot=()=>JSON.parse(m.UTF8ToString(m._th128_world_ending_state()));
      return Object.freeze({snapshot,step({holding,pressed,released}){
        ownerSignal?.throwIfAborted();if(m._th128_coop_tick_ending(holding,pressed,released)!==1)throw Error(error());
        const state=snapshot(),n=m._th128_coop_sound_count(),p=m._th128_coop_sound_data();
        if(n>65536||n&&(p<1||p%4||p+12*n>m.HEAPU8.length))throw Error('Invalid original ending sound batch');
        const view=new DataView(m.HEAPU8.buffer);state.sounds=Array.from({length:n},(_,i)=>{const o=p+12*i;return {id:view.getInt32(o,true),x:view.getFloat32(o+4,true),op:view.getInt32(o+8,true)};});return state;
      },acknowledge({serial,operation}){if(m._th128_world_ending_ack_media(serial,operation)!==1)throw Error(error());}});
    }
    return Object.freeze({...engine,startEnding,draws:()=>{engine.state();return readWorldDrawRecords(m,{fastNumbers:true});},
      presentation:()=>JSON.parse(m.UTF8ToString(m._th128_world_presentation_state())),
      stageState:()=>{engine.state();return JSON.parse(m.UTF8ToString(m._th128_world_state()));}});
  }
  async function openPair(rules,ownerSignal,{holdGameOver=true}={}){
    const players=[];
    try{for(const seat of [0,1])players.push(await openSeat(rules,seat,ownerSignal,holdGameOver));}
    catch(error){for(const p of players)p.close();throw error;}
    return Object.freeze({
      checkpoint(){
        const tokens=[];
        try{for(const p of players)tokens.push(p.checkpoint());return tokens;}
        catch(error){for(let i=0;i<tokens.length;i++)players[i].release(tokens[i]);throw error;}
      },
      restore(tokens){players.forEach((p,i)=>p.restore(tokens[i]));},
      release(tokens){players.forEach((p,i)=>p.release(tokens[i]));},
      step(row){
        players.forEach(p=>p.step(row));
        if(players.every(p=>p.terminal()?.reason==='game-over'))return {bothGameOver:true};
      },
      terminal(seat){return players[seat].terminal();},state(seat){return players[seat].state();},
      draws(seat){return players[seat].draws();},stateText(seat){return players[seat].stateText();},
      close(){for(const p of players)p.close();players.length=0;},
    });
  }
  async function openBackground(ownerSignal){
    const m=await instantiate('background',ownerSignal);
    withMemory(m,[backgroundAnm],([pointer,length])=>{
      if(m._th128_load_anm(8,pointer,length)!==1)throw new Error(m.UTF8ToString(m._th128_error())||'Native background ANM rejected');
    });
    return {module:m,std:backgroundData.slice()};
  }
  function showCaption(module,role){
    if(module._th128_world_track_caption(logo.tracks[role],role)!==1)
      throw Error(module.UTF8ToString(module._th128_world_error())||'Original stage track caption rejected');
  }
  return boundRules?Object.freeze({banks,openCoopStage,openBackground,install,showCaption}):Object.freeze({banks,openPair,openCoopStage,openBackground});
}
