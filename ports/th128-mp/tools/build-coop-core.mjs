import {createHash} from 'node:crypto';
import {readFile,writeFile,mkdir,stat,mkdtemp,copyFile} from 'node:fs/promises';
import {dirname,join,resolve,relative,isAbsolute} from 'node:path';
import {spawn} from 'node:child_process';
import {describeCoreInputs} from '../../th128/tools/build-runtime.mjs';
// A separate out-of-tree MP translation unit. Never patches or replaces the ordinary core.
const [outArg,choice]=process.argv.slice(2);
if(!outArg||choice&&!['--base-objects'].includes(choice))throw Error('usage: build-coop-core.mjs <private output directory> [--base-objects]');
const mp=resolve(import.meta.dirname,'..'),normal=resolve(mp,'../th128'),tree=resolve(mp,'../..'),out=resolve(outArg),rel=relative(tree,out);
if(!rel||(!rel.startsWith('..')&&!isAbsolute(rel)))throw Error('MP build output must be outside source tree');
const sha=b=>createHash('sha256').update(b).digest('hex'),emcc=process.env.TH128_EMCC,python=process.env.EMSDK_PYTHON;
if(!emcc||!python)throw Error('EMSDK_PYTHON and TH128_EMCC required');
const run=(exe,args,cwd,capture=false)=>new Promise((accept,reject)=>{const ch=spawn(exe,args,{cwd,windowsHide:true,stdio:capture?['ignore','pipe','pipe']:'inherit'});let text='';if(capture){ch.stdout.on('data',x=>text+=x);ch.stderr.on('data',x=>text+=x);}ch.on('error',reject);ch.on('close',code=>code===0?accept(text):reject(Error(`${exe} failed (${code}) ${text}`)));});
const version=await run(python,[emcc,'--version'],normal,true);if(!version.includes('4.0.22'))throw Error('requires Emscripten 4.0.22');
const base=describeCoreInputs(),headers=Object.entries(base.sourceFiles).filter(([k])=>!k.endsWith('.cpp'));
const shared=['-std=c++20','-O2','-fno-fast-math','-ffp-contract=off','-fexceptions','-I'+join(normal,'src')];
const overlayText=await readFile(join(mp,'native/world-coop.patch'),'utf8');
// Every projected header can affect common translation units and their cache keys.
const projections=[...overlayText.matchAll(/^--- a\/(src\/[^\n]+)$/gm)].map(x=>x[1]);
if(!projections.includes('src/runtime/WorldExports.cpp')||!projections.includes('src/game/LaserWholeLineEffectsOwner.hpp')||new Set(projections).size!==projections.length)throw Error('Invalid cooperative native projection');
const headerPatch=overlayText.split(/(?=^--- a\/src\/)/m).filter(s=>/^--- a\/[^\n]+\.hpp\n/.test(s)).join('');
const salt=sha(JSON.stringify([version,shared,headers,headerPatch]));await mkdir(out,{recursive:true});const objects=join(out,'objects');await mkdir(objects,{recursive:true});
const stage=await mkdtemp(join(out,'compile-')),patchedUnits=projections.filter(file=>file.endsWith('.cpp'));const exists=async p=>!!(await stat(p).catch(()=>null))?.isFile();
let overlay='',native='';
{
 overlay=overlayText;native=(await Promise.all(['CoopSeats.inc','CoopClear.inc','CoopLife.inc','CoopReplaySave.inc','CoopItemHooks.hpp','CoopPhysics.hpp'].map(n=>readFile(join(mp,'native',n),'utf8')))).join('\n');
 for(const name of Object.keys(base.sourceFiles).filter(p=>p.startsWith('src/')&&!p.endsWith('.cpp'))){await mkdir(dirname(join(stage,name)),{recursive:true});await copyFile(join(normal,name),join(stage,name));}
 for(const u of patchedUnits){await mkdir(dirname(join(stage,u)),{recursive:true});await copyFile(join(normal,u),join(stage,u));}
 await run('git',['-c','core.autocrlf=false','apply','--ignore-space-change','--whitespace=nowarn',join(mp,'native/world-coop.patch')],stage,true);
}
const all=base.translationUnits.filter(u=>!choice||!patchedUnits.includes(u));const links=new Array(all.length);let cursor=0;
async function worker(){while(cursor<all.length){const i=cursor++,unit=all[i],patched=patchedUnits.includes(unit),source=resolve(stage,unit);
 if(!patched){await mkdir(dirname(source),{recursive:true});await copyFile(join(normal,unit),source);}
 const key=sha(JSON.stringify([salt,unit,base.sourceFiles[unit],patched?sha(overlay+native):''])),obj=join(objects,key+'.o');links[i]=obj;if(await exists(obj))continue;
 const tmp=join(stage,key+'.o');await run(python,[emcc,source,'-I'+join(stage,'src'),'-I'+join(mp,'native'),...shared,...(patched?['-I'+dirname(join(normal,unit)),'-I'+join(mp,'native')]:[]),'-c','-o',tmp],normal);await copyFile(tmp,obj);
}}
await Promise.all(Array.from({length:4},worker));if(choice){console.log(`${all.length} common units ready (no World linked)`);}else{
 const exposed=['malloc','free','th128_load_anm','th128_error','th128_world_reset','th128_world_load','th128_world_start','th128_world_error','th128_world_load_enemy_anm','th128_world_enable_geometry526','th128_world_enable_bullets','th128_world_enable_bullet_updates','th128_world_enable_graze','th128_world_enable_player_context','th128_world_enable_player_motion','th128_world_enable_player_shots','th128_world_enable_shot_hits','th128_world_enable_rapid_shot','th128_world_enable_player_death','th128_world_enable_perfect_hud','th128_world_load_score_font','th128_world_enable_spells','th128_world_enable_player_respawn','th128_world_enable_head_power','th128_world_enable_focus_indicator','th128_world_enable_z_fire','th128_world_add_bullet_profile','th128_world_enable_messages','th128_world_enable_message_front','th128_world_enable_route_choice','th128_world_enable_stage_transition','th128_world_state','th128_world_presentation_state','th128_world_bullet_draws','th128_world_bullet_draw_error','th128_world_draw_packet','th128_world_load_stage_intro','th128_world_track_caption','th128_coop_enable_clear','th128_coop_start_ending','th128_coop_tick_ending','th128_world_ending_state','th128_world_ending_ack_media','th128_coop_take_music_fade','th128_coop_message_music','th128_coop_sound_count','th128_coop_sound_data','th128_coop_bind','th128_coop_replay_save_open','th128_coop_replay_save_tick','th128_coop_replay_save_state','th128_coop_replay_save_ack','th128_coop_tick','th128_coop_state','th128_coop_next_stage','th128_coop_capture_stage','th128_coop_next_progress','th128_coop_finish_stage','th128_world_bind_stage_carry_target','th128_world_restore_stage_carry_random','th128_world_restore_stage_carry'];
 const flags=['-O2','-fexceptions','-sDISABLE_EXCEPTION_CATCHING=0','--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,node','-sALLOW_MEMORY_GROWTH=0','-sINITIAL_MEMORY=67108864','-sSTACK_SIZE=1048576','-sASSERTIONS=1','--js-library',join(normal,'tools/sound-identity-library.js'),'-sEXPORTED_FUNCTIONS='+JSON.stringify(exposed.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8","UTF8ToString"]'];
 const rsp=join(stage,'link.rsp');await writeFile(rsp,links.map(p=>'"'+p.replaceAll('\\','/')+'"').join('\n'));
 await run(python,[emcc,'@'+rsp,...flags,'-o',join(stage,'coop-core.mjs')],normal);
 const manifests={schema:'th128-coop-core/1',wasm:sha(await readFile(join(stage,'coop-core.wasm'))),js:sha(await readFile(join(stage,'coop-core.mjs'))),inputs:sha(JSON.stringify([base.sourceFiles,base.hostInputs,salt,overlay,native,flags]))};
 const dst=join(out,manifests.wasm);await mkdir(dst,{recursive:true});for(const n of ['coop-core.wasm','coop-core.mjs'])await copyFile(join(stage,n),join(dst,n));await writeFile(join(dst,'receipt.json'),JSON.stringify(manifests,null,2)+'\n');console.log(JSON.stringify({dst,...manifests}));
}