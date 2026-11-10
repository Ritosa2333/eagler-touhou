import {emscripten} from './toolchain.mjs';
import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';
import {runtimeSources,runtimeSourceFiles} from './runtime-sources.mjs';
import {createHash} from 'node:crypto';
import {practiceBuild} from './practice-build.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const {python,compiler,version}=emscripten(root);
if(process.env.TH12_DIAGNOSTIC!==undefined&&!['0','1'].includes(process.env.TH12_DIAGNOSTIC))throw Error('TH12_DIAGNOSTIC must be0 or1');
const diagnostic=process.env.TH12_DIAGNOSTIC==='1';
const environment=diagnostic?'web,node':process.env.TH12_BROWSER_BUILD==='1'?'web':'web,node';
const outputDirectory=path.resolve(root,process.env.TH12_CORE_OUTPUT_DIR||(diagnostic?'artifacts/diagnostic-runtime':'site/runtime'));
const ordinaryDirectory=path.resolve(root,'site/runtime');
if(diagnostic&&(outputDirectory===ordinaryDirectory||outputDirectory.startsWith(ordinaryDirectory+path.sep)))throw Error('Diagnostic artifacts must not overwrite the player Runtime');
if(!fs.existsSync(compiler))throw Error('Emscripten 4.0.22 is required; see docs/BUILD.md.');
fs.mkdirSync(outputDirectory,{recursive:true});
// State, poses and resolved motion are also used by the normal game/recording
// path. Dedicated verifier hooks and partial-world setters are separate.
const exports=['_malloc','_free','_th12_load_ecl','_th12_load_msg','_th12_select_stage','_th12_load_sht','_th12_start','_th12_state','_th12_state_size','_th12_tick','_th12_draw','_th12_draw_ptr','_th12_draw_stride','_th12_draw_version','_th12_hud','_th12_hud_size'];
exports.push(...['configure','connect','spectator_connect','start','restart','pump','step','stop','error','status','calibration_status','rollback_status','hash','state','stage_needed','stage_begin','stage_loaded','sounds','sounds_count','sounds_clear','replay_encode','replay_data','replay_validate','replay_load','replay_frames','replay_rewind','paced_elapsed','spectator_state','spectator_control','spectator_complete'].map(name=>'_th12_mp_'+name));
if(diagnostic)exports.push('_th12_diagnostic_build','_th12_diagnostic_state','_th12_diagnostic_state_size','_th12_oracle_fixture','_th12_set_initial','_th12_set_replay_economy','_th12_unsupported','_th12_rng16','_th12_rng32');
exports.push('_th12_mp_visual_owners','_th12_mp_visual_owners_count');
const cache=path.join(root,'artifacts/emscripten-cache');fs.mkdirSync(cache,{recursive:true});
const temporary=path.join(root,'artifacts/emscripten-temp',diagnostic?'diagnostic-core':'core');fs.mkdirSync(temporary,{recursive:true});
const thprac=process.env.TH12_THPRAC==='1',sharedSourceFiles={},buildFlags=[],sharedSources=[];
const commonRoot=path.resolve(root,'../../common');
function sharedInputs(folder){for(const entry of fs.readdirSync(path.join(commonRoot,folder),{withFileTypes:true})){const name=folder+'/'+entry.name;if(entry.isDirectory())sharedInputs(name);else sharedSourceFiles[name]=createHash('sha256').update(fs.readFileSync(path.join(commonRoot,name))).digest('hex');}}
for(const folder of ['include/eagler/netplay','src/netplay','include/eagler/gameplay','browser'])sharedInputs(folder);
buildFlags.push('-DTH12_MULTIPLAYER=1','-I'+path.join(commonRoot,'include'),'-I'+path.join(root,'src/multiplayer'));
for(const name of fs.readdirSync(path.join(commonRoot,'src/netplay')).filter(n=>n.endsWith('.cpp')))sharedSources.push(path.join(commonRoot,'src/netplay',name));
buildFlags.push('-DTH12_DIAGNOSTIC='+(diagnostic?'1':'0'));
if(thprac){
  const pin=JSON.parse(fs.readFileSync(path.join(root,'integration/common-dependency.json'),'utf8'));
  const candidates=[process.env.EAGLER_COMMON_ROOT,path.resolve(root,'../eagler-common'),path.resolve(root,'../common'),path.resolve(root,'../../common'),path.resolve(root,'../../eagler-touhou/common')].filter(Boolean);
  const common=candidates.find(candidate=>fs.existsSync(path.join(candidate,'include/eagler/thprac/PracticeSpeed.hpp')));
  if(!common)throw Error('完整适配构建需要固定版本的 common；请按 integration/common-dependency.json 准备并设置 EAGLER_COMMON_ROOT');
  for(const [name,expected]of Object.entries(pin.files)){const hash=createHash('sha256').update(fs.readFileSync(path.join(common,name))).digest('hex');if(hash!==expected)throw Error('共享源码与固定版本不一致：'+name);sharedSourceFiles[name]=hash;}
  const practice=practiceBuild(root,common);
  buildFlags.push(...practice.flags);sharedSources.push(...practice.sources);
  for(const file of practice.inputs)sharedSourceFiles[path.relative(common,file).split(path.sep).join('/')]=createHash('sha256').update(fs.readFileSync(file)).digest('hex');
  exports.push(...['init','font','key','mobile','context','params','parameters','capture','pointer','wheel','cancel','tick','draw','action','flags','clear_keys','behavior','shutdown'].map(name=>'_th12_practice_ui_'+name));
}
exports.push('_th12_message','_th12_message_size','_th12_spell','_th12_spell_size');
exports.push('_th12_ufo','_th12_ufo_size');
exports.push('_th12_start_replay','_th12_set_wall_time');
exports.push('_th12_boss_hud','_th12_boss_hud_size','_th12_music_track','_th12_music_serial');
exports.push('_th12_load_anm','_th12_load_std','_th12_background','_th12_background_size');
exports.push('_th12_unload_anm');
exports.push('_th12_sound_events','_th12_sound_events_ptr','_th12_sound_events_stride');
exports.push('_th12_anm_draw','_th12_anm_draw_ptr','_th12_anm_draw_stride');
exports.push('_th12_anm_schedule_count','_th12_anm_schedule_ptr','_th12_anm_schedule_stride');
exports.push('_th12_background_frame','_th12_background_visible');
exports.push('_th12_laser_draw','_th12_laser_draw_ptr','_th12_laser_draw_stride');
exports.push('_th12_mesh_draw','_th12_mesh_draw_ptr','_th12_mesh_draw_stride','_th12_mesh_vertices_ptr','_th12_mesh_vertices_count','_th12_mesh_vertex_stride');
exports.push('_th12_score_popups','_th12_score_popups_ptr','_th12_score_popups_stride');
exports.push('_th12_score_stored');
exports.push('_th12_start_full_game','_th12_set_full_game_mode','_th12_replay_stage_flow','_th12_start_next_stage','_th12_continue_game','_th12_stage_flow','_th12_stage_flow_size');
exports.push('_th12_motion','_th12_motion_result','_th12_player_state','_th12_player_instance','_th12_player_speed');
exports.push('_th12_presentation_motion','_th12_presentation_motion_size','_th12_presentation_motion_binary');
exports.push('_th12_practice_enabled','_th12_practice_reset','_th12_practice_configure','_th12_practice_start','_th12_practice_prepare_replay','_th12_practice_replay','_th12_practice_tools','_th12_practice_speed','_th12_practice_interval','_th12_practice_state','_th12_practice_state_size','_th12_practice_boss_music','_th12_practice_everlasting_bgm');
exports.push('_th12_practice_input');
exports.push('_th12_practice_damage_begin','_th12_practice_damage_reset','_th12_practice_damage_transition','_th12_practice_damage_load','_th12_practice_damage_stage','_th12_practice_damage_stage_words','_th12_practice_damage_state','_th12_practice_damage_state_size');
exports.push('_th12_load_spell_history','_th12_spell_history','_th12_spell_history_size');
const result=spawnSync(python,[compiler,...runtimeSources(root),...sharedSources,...buildFlags,'-std=c++20','-O2','-o',path.join(outputDirectory,'core.mjs'),'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT='+environment,'-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432','-sSTACK_SIZE=4194304','-sSTACK_OVERFLOW_CHECK=2','-sEXPORTED_FUNCTIONS='+JSON.stringify(exports),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8','HEAPU16','HEAPU32','HEAPF32']),'-sASSERTIONS=1'],{cwd:root,stdio:'inherit',env:{...process.env,EM_CACHE:cache,EMSDK_PYTHON:python,EMCC_TEMP_DIR:temporary,TMP:temporary,TEMP:temporary,TMPDIR:temporary,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
if(result.status!==0)process.exit(result.status||1);
const sources=runtimeSourceFiles(root);
const sourceFiles=Object.fromEntries(sources.map(name=>[name,createHash('sha256').update(fs.readFileSync(path.join(root,name))).digest('hex')]));
const sourceSha256=createHash('sha256').update(JSON.stringify({sourceFiles,sharedSourceFiles})).digest('hex');
const artifacts=Object.fromEntries(['core.mjs','core.wasm'].map(name=>{const bytes=fs.readFileSync(path.join(outputDirectory,name));return [name,{bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')}];}));
fs.writeFileSync(path.join(outputDirectory,'build.json'),JSON.stringify({version:'0.3.0-cpp',builtAt:new Date().toISOString(),compiler:'emscripten-'+version,environment,diagnostic,diagnosticApi:diagnostic?1:null,features:{multiplayer:true,thprac,languages:true,focusHitbox:false},sourceSha256,sourceFiles,sharedSourceFiles,artifacts},null,2)+'\n');
