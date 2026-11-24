import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {tmpdir} from 'node:os';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
// This inventory is also used by the Launcher source inspection.
const sources=['src/runtime/Exports.cpp','src/runtime/EclExports.cpp','src/animation/AnmLogic.cpp',
  'src/animation/AnmScene.cpp','src/game/FreezeRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp',
  'src/runtime/WorldExports.cpp','src/game/StageWorld.cpp','src/game/StageIntroLogos.cpp','src/game/EnemyRules.cpp','src/game/EnemyPhaseRules.cpp','src/game/BulletRules.cpp','src/game/BulletManager.cpp','src/game/EnemyBulletCallbackRules.cpp',
  'src/runtime/PlayerExports.cpp','src/game/PlayerRules.cpp','src/game/BulletVisuals.cpp','src/game/AppearanceBounds.cpp',
  'src/game/BulletTickRules.cpp','src/game/BulletCollisionRules.cpp','src/game/BulletUpdateHost.cpp','src/game/BulletExtension8192.cpp','src/game/Extension16384.cpp','src/game/Extension1048576.cpp','src/game/Extension512.cpp','src/game/Extension4096.cpp','src/game/GrazeRules.cpp',
  'src/game/TransientEffects.cpp','src/game/EnemyVisuals.cpp','src/game/CancellationEffects.cpp',
  'src/game/PlayerHitRules.cpp','src/game/PlayerHitHost.cpp','src/game/PlayerShotBirthRules.cpp','src/game/PlayerShotMotion.cpp','src/game/PlayerShotTickRules.cpp','src/game/PlayerShotHost.cpp','src/game/PlayerShotGroups.cpp','src/game/ShotEnemyCollisionRules.cpp','src/game/ItemHost.cpp','src/game/ItemRules.cpp','src/game/PlayerDeathRules.cpp','src/game/PlayerDeathHost.cpp','src/game/DamageFieldPool.cpp','src/game/PlayerRespawnRules.cpp','src/game/EnemyDropRules.cpp','src/game/EnemyDropEmission.cpp','src/game/BulletExtension16.cpp','src/game/BulletExtension4.cpp','src/game/BulletExtension64.cpp','src/game/BulletExtension800000.cpp','src/game/BulletExtension256.cpp','src/game/PerfectHud.cpp'];
sources.push('src/game/Geometry526.cpp','src/game/Geometry526Host.cpp','src/game/EnemyRandomMovement.cpp','src/game/CircleFan.cpp');
sources.push('src/game/HeadPower.cpp');
sources.push('src/game/MessageHost.cpp','src/game/MessageVisuals.cpp','src/game/MessageTimeControl.cpp','src/game/RouteChoiceHost.cpp');
sources.push('src/game/StageSettlementHost.cpp','src/game/StageTransitionHost.cpp','src/game/RouteHandoffRules.cpp','src/game/StageControl.cpp','src/game/StageSessionIdentity.cpp');
sources.push('src/game/SpellVisuals.cpp');
sources.push('src/game/BossHud.cpp','src/game/RingGeometry.cpp');
sources.push('src/game/FocusIndicator.cpp');
sources.push('src/game/BulletExtension1.cpp');
sources.push('src/game/PlayerFireHost.cpp','src/game/PlayerFireRules.cpp','src/game/IceHost.cpp',
  'src/game/IceBirthRules.cpp','src/game/IceMaskRules.cpp','src/game/IceContact.cpp','src/game/FreezeAccounting.cpp',
  'src/game/PerfectFreezeHost.cpp');
sources.push('src/game/LaserLineHost.cpp');
sources.push('src/game/ending/EndingSceneHost.cpp','src/game/ending/host-text-vm-bridge/HostTextVmBridge.cpp');

// The Title, World and Ending consumers must share the complete game ABI.
const exports=[
  'malloc','free','th128_load_anm',
  'th128_boot_title','th128_tick','th128_select',
  'th128_selection','th128_title_menu_page','th128_title_menu_select',
  'th128_title_menu_state','th128_frame','th128_nodes',
  'th128_draw_count','th128_draw_ptr','th128_draw_stride',
  'th128_draw_enabled','th128_error','th128_irq',
  'th128_reset','th128_bind','th128_rng_calls',
  'th128_freeze_can_release','th128_freeze_duration','th128_freeze_add_power',
  'th128_load_ecl','th128_ecl_start','th128_ecl_tick',
  'th128_ecl_status','th128_ecl_error','th128_ecl_reset',
  'th128_world_load','th128_world_start','th128_world_tick',
  'th128_world_state','th128_world_presentation_state','th128_world_touch_state','th128_world_error','th128_world_reset',
  'th128_world_load_stage_intro','th128_world_track_caption','th128_world_stage_intro_state',
  'th128_world_stage_intro_draw','th128_world_stage_intro_draw_error','th128_world_enable_messages',
  'th128_world_message_state','th128_world_enable_message_front','th128_world_enable_route_choice',
  'th128_world_enable_stage_transition','th128_world_initialize_stage_control','th128_world_acknowledge_stage_transition',
  'th128_world_music_control_state','th128_world_route_choice_context','th128_world_flow_capabilities',
  'th128_world_acknowledge_music_control','th128_world_capture_stage_carry','th128_world_bind_stage_carry_target',
  'th128_world_restore_stage_carry_random','th128_world_restore_stage_carry','th128_world_discard_stage_carry',
  'th128_world_enable_spells','th128_world_enable_bullets','th128_world_bullet_draws',
  'th128_world_add_bullet_profile','th128_world_load_enemy_anm','th128_world_enable_geometry526',
  'th128_world_bullet_draw_error','th128_world_enable_bullet_updates','th128_world_enable_graze',
  'th128_world_enable_player_context','th128_world_enable_player_motion','th128_world_input',
  'th128_world_set_debug_infinite_respawn','th128_world_enable_player_shots','th128_world_emit_player_shots',
  'th128_world_enable_shot_hits','th128_world_enable_rapid_shot','th128_world_load_score_font',
  'th128_world_enable_player_death','th128_world_enable_player_respawn','th128_world_enable_perfect_hud',
  'th128_world_enable_head_power','th128_world_enable_focus_indicator','th128_world_enable_z_fire',
  'th128_world_sound_operation','th128_world_sound_count','th128_world_sound_id',
  'th128_world_sound_x','th128_world_sound_epoch','th128_world_sound_begin_window',
  'th128_world_sound_ack_window','th128_world_sound_window_state','th128_world_sound_restore_boundary',
  'th128_player_reset','th128_player_move','th128_player_x',
  'th128_player_y','th128_world_start_ending','th128_world_ending_tick',
  'th128_world_ending_state','th128_world_ending_ack_media','th128_world_ending_cleanup',
  'th128_world_ending_return_tail','th128_world_prepare_debug_record_prefix','th128_world_bind_debug_record_clock',
  'th128_world_debug_record_state','th128_world_debug_record_data','th128_world_debug_record_size',
  'th128_world_bind_debug_replay_record','th128_world_debug_replay_data','th128_world_debug_replay_size',
  'th128_world_bind_debug_record_driver','th128_world_debug_record_begin_update','th128_world_debug_record_presentation',
  'th128_world_debug_record_post_present','th128_world_debug_record_driver_state','th128_world_debug_postrun_record',
  'th128_world_bind_debug_postrun_asset','th128_world_debug_postrun_retire','th128_world_debug_postrun_state',
  'th128_world_debug_postrun_input','th128_world_debug_postrun_media_window','th128_world_debug_postrun_media_ack',
  'th128_world_debug_postrun_present','th128_world_debug_postrun_post_present','th128_world_debug_postrun_bind_previews',
  'th128_world_prepare_ordinary_record_prefix','th128_world_ordinary_ending_post_present','th128_world_commit_ordinary_ending_return',
  'th128_world_pause_active','th128_world_pause_action','th128_world_pause_open',
  'th128_world_pause_tick','th128_world_pause_state','th128_world_pause_commit',
  'th128_world_pause_load_manual','th128_world_replay_begin','th128_world_replay_clock',
  'th128_world_replay_previews','th128_world_replay_save_open','th128_world_replay_snapshot',
  'th128_world_replay_data','th128_world_replay_checksum','th128_world_replay_saved',
  'th128_title_music_catalog','th128_title_music_text_epoch','th128_title_music_text_count',
  'th128_title_music_text_request','th128_title_music_prepare','th128_title_music_input',
  'th128_title_sound_epoch','th128_title_sound_count','th128_title_sound_begin_window',
  'th128_title_sound_window_state','th128_title_sound_id','th128_title_sound_x',
  'th128_title_sound_operation','th128_title_sound_ack_window','th128_title_options_input',
  'th128_title_keyconfig_prepare','th128_title_keyconfig_input','th128_title_help_load',
  'th128_title_settings_restore','th128_title_replay_prepare','th128_title_music_bgm_request',
  'th128_title_music_bgm_ack','th128_title_results_prepare','th128_title_main_action_ack',
  'th128_title_main_input','th128_world_replay_playback_load','th128_world_replay_playback_state',
  'th128_world_replay_playback_cancel','th128_title_replay_stage_prepare','th128_title_extra_save',
  'th128_world_replay_playback_restore','th128_world_records_restore','th128_world_records_present',
  'th128_world_records_prepare','th128_world_records_data','th128_world_records_size',
  'th128_world_records_ack','th128_world_terminal_preupdate','th128_world_ending_set_pause',
  'th128_world_ending_return_prepare','th128_world_ending_return_packet_size','th128_world_ending_return_packet_data',
  'th128_world_ending_return_ack','th128_world_normal_postrun_asset','th128_world_normal_postrun_begin','th128_world_practice_postrun_begin',
  'th128_world_normal_postrun_input','th128_world_normal_postrun_profile_size','th128_world_normal_postrun_profile_data',
  'th128_world_normal_postrun_profile_ack','th128_world_normal_postrun_replay_observe','th128_world_normal_postrun_replay_size',
  'th128_world_normal_postrun_replay_data','th128_world_normal_postrun_replay_ack','th128_world_normal_postrun_media',
  'th128_world_normal_postrun_media_ack','th128_world_normal_postrun_present','th128_world_normal_postrun_main_prepare',
  'th128_world_normal_postrun_main_size','th128_world_normal_postrun_main_data','th128_world_normal_postrun_main_ack',
  'th128_world_normal_postrun_main_commit','th128_world_normal_postrun_state','th128_world_prepare_extra_record_profile',
  'th128_world_debug_record_accept_cpu_presentation','th128_world_draw_packet','th128_world_prepare_thprac128',
  'th128_world_thprac128_state','th128_world_thprac_replay_begin','th128_world_thprac_replay_clock',
  'th128_world_spell_medals_restore','th128_world_practice_medal_awards',
  'th128_world_thprac_replay_snapshot','th128_world_thprac_replay_data','th128_world_thprac_replay_state',
  'th128_world_thprac_replay_playback_load','th128_world_thprac_tools_set','th128_world_thprac_tools_state',
  'th128_world_thprac_tools_playback','th128_world_prepare_thprac_carry','th128_world_prepare_thprac_replay_initial',
  'th128_world_restore_thprac_replay_initial','th128_world_finish_thprac_ending','th128_world_touch_motion',
  'th128_world_touch_playback'
];
const flags=['-Isrc','-std=c++20','-O2','-fno-fast-math','-ffp-contract=off','-fexceptions','-sDISABLE_EXCEPTION_CATCHING=0','--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,node',
  '-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=67108864','-sMAXIMUM_MEMORY=268435456','-sASSERTIONS=1',
  '--js-library','tools/sound-identity-library.js',
  '-sEXPORTED_FUNCTIONS='+JSON.stringify(exports.map(name=>'_'+name)),
  '-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8','UTF8ToString'])];
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
function captureSources(){
  const names=fs.readdirSync(path.join(root,'src'),{recursive:true,withFileTypes:true})
    .filter(entry=>entry.isFile()&&/\.(?:hpp|cpp|txt)$/.test(entry.name))
    .map(entry=>path.relative(root,path.join(entry.parentPath,entry.name)).replaceAll('\\','/')).sort();
  return Object.fromEntries(names.map(name=>[name,hash(fs.readFileSync(path.join(root,name)))]));
}
export function describeCoreInputs() {
  const hostInputs={'tools/sound-identity-library.js':hash(fs.readFileSync(path.join(root,'tools/sound-identity-library.js')))};
  return {translationUnits:[...sources],compilerFlags:[...flags],sourceFiles:captureSources(),hostInputs};
}

export const packageDataLayout='sha256-58cd45fe7cac9af281d9de993a527ccfbbe6f27b89bcd0ca98a7031333fce200';
const receiptSchema='th128-web-core/1';
function coreGeneration(sourceFiles,hostInputs,artifacts) {
  return hash(Buffer.from(JSON.stringify({sourceFiles,hostInputs,artifacts})));
}

export function matchesBuiltCore(receipt) {
  if(receipt?.schema!==receiptSchema||receipt.game!=='th128'||receipt.compiler!=='emscripten4.0.22'
      ||receipt.packageDataLayout!==packageDataLayout||!receipt.artifacts||!receipt.sourceFiles)return false;
  const inputs=describeCoreInputs();
  if(JSON.stringify(receipt.sourceFiles)!==JSON.stringify(inputs.sourceFiles)
      ||JSON.stringify(receipt.hostInputs)!==JSON.stringify(inputs.hostInputs)
      ||JSON.stringify(receipt.compilerFlags)!==JSON.stringify(inputs.compilerFlags))return false;
  const {artifacts}=receipt;
  if(JSON.stringify(Object.keys(artifacts))!==JSON.stringify(['core.mjs','core.wasm'])||!Object.values(artifacts).every(file=>
      file&&Number.isSafeInteger(file.bytes)&&file.bytes>0&&/^[a-f0-9]{64}$/.test(file.sha256)))return false;
  const generation=coreGeneration(inputs.sourceFiles,inputs.hostInputs,artifacts);
  return receipt.generation===generation&&receipt.entry===`runtime/builds/${generation}/core.mjs`;
}

function build() {
  for(const source of sources)if(!fs.existsSync(path.join(root,source)))throw new Error(`Missing runtime implementation ${source}`);
  const requested=process.env.TH128_EMCC ?? (process.env.EMSDK && path.join(process.env.EMSDK,'upstream','emscripten','emcc.py'));
  if(!requested)throw new Error('Set EMSDK or TH128_EMCC to an activated Emscripten 4.0.22 SDK.');
  const compiler=path.resolve(requested), sdk=path.resolve(path.dirname(compiler),'../..');
  const bundledPython=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const python=process.env.TH128_PYTHON ?? process.env.EMSDK_PYTHON ?? (fs.existsSync(bundledPython)?bundledPython:process.platform==='win32'?'python':'python3');
  const target=path.resolve(process.env.TH128_CORE_OUTPUT ?? path.join(root,'site'));
  const output=path.join(target,'runtime');
  fs.mkdirSync(output,{recursive:true});
  const buildOutput=fs.mkdtempSync(path.join(tmpdir(),'th128-web-core-'));
  const environment={...process.env, EM_CONFIG:process.env.EM_CONFIG ?? path.join(sdk,'.emscripten'),
    EMSDK_PYTHON:python, PYTHONUTF8:'1', EMCC_CORES:process.env.EMCC_CORES ?? '2', BINARYEN_CORES:process.env.BINARYEN_CORES ?? '2'};
  const version=spawnSync(python,[compiler,'--version'],{cwd:root,encoding:'utf8',env:environment});
  if(version.status!==0||!version.stdout.includes('4.0.22'))throw new Error('TH128 requires Emscripten 4.0.22. '+(version.error?.message??version.stderr??''));
  const input=describeCoreInputs(),{sourceFiles,hostInputs,compilerFlags}=input;
  const built=spawnSync(python,[compiler,...sources,...compilerFlags,'-o',path.join(buildOutput,'core.mjs')],{cwd:root,stdio:'inherit',env:environment});
  if(built.status!==0){process.exitCode=built.status??1;return;}
  if(JSON.stringify(describeCoreInputs())!==JSON.stringify(input))throw new Error('TH128 sources changed during compilation.');
  const artifacts=Object.fromEntries(['core.mjs','core.wasm'].map(name=>{
    const bytes=fs.readFileSync(path.join(buildOutput,name));return [name,{bytes:bytes.length,sha256:hash(bytes)}];
  }));
  const generation=coreGeneration(sourceFiles,hostInputs,artifacts);
  const builds=path.join(output,'builds');fs.mkdirSync(builds,{recursive:true});
  if(path.relative(path.resolve(builds),fs.realpathSync(builds)))throw new Error('Runtime builds directory must not be redirected');
  const destination=path.join(builds,generation);
  if(!fs.existsSync(destination)){
    const stage=fs.mkdtempSync(path.join(builds,'.staging-'));
    for(const name of Object.keys(artifacts))fs.copyFileSync(path.join(buildOutput,name),path.join(stage,name));
    fs.renameSync(stage,destination);
  }
  for(const [name,record] of Object.entries(artifacts)){
    if(hash(fs.readFileSync(path.join(destination,name)))!==record.sha256)throw new Error('Runtime generation hash mismatch');
    if(target===path.join(root,'site'))fs.copyFileSync(path.join(buildOutput,name),path.join(output,name));
    fs.unlinkSync(path.join(buildOutput,name));
  }
  fs.rmdirSync(buildOutput);
  const manifest={schema:receiptSchema,game:'th128',compiler:'emscripten4.0.22',generation,
    entry:`runtime/builds/${generation}/core.mjs`,packageDataLayout,sourceFiles,hostInputs,compilerFlags,artifacts};
  const manifestStage=path.join(output,`.build-${process.pid}.json`);
  fs.writeFileSync(manifestStage,JSON.stringify(manifest,null,2)+'\n',{flag:'wx'});
  fs.renameSync(manifestStage,path.join(output,'build.json'));
  console.log(JSON.stringify({output:target,generation,artifacts}));
}

if(process.argv[1]&&path.resolve(process.argv[1])===path.resolve(fileURLToPath(import.meta.url)))build();
