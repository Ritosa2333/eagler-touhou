// Private Application TU over the current production objects. Exact per-tick
// source hashes and the native transition fixture never enter a shipped module.
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync,spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {responseFileContents} from '../response-file.mjs';
const root=resolve(import.meta.dirname,'../..');
const initialStage=Number(process.argv.find(value=>value.startsWith('--stage='))?.slice(8)||1);
if(!Number.isInteger(initialStage)||initialStage<1||initialStage>7)throw Error('Spectator test stage must be 1..7');
const plan=JSON.parse(execFileSync(process.execPath,['portable/build.mjs','--multiplayer',...(process.argv.includes('--production')?[]:['--diagnostic']),'--print-plan'],{cwd:root,encoding:'utf8'}));
const build=JSON.parse(readFileSync(resolve(plan.outputDirectory,'build.json'),'utf8'));
const sha=b=>createHash('sha256').update(b).digest('hex');
for(const [file,expected] of Object.entries(build.sourceFiles))if(sha(readFileSync(resolve(root,file)))!==expected)throw Error('Rebuild changed production source '+file);
const out=resolve(plan.outputDirectory,'spectator-stage');mkdirSync(out,{recursive:true});
const sdl=resolve(root,'web/cpp/sdl'),sdk=resolve(process.env.EMSDK||resolve(root,'../../toolchains/emsdk'));
const emcc=['upstream','install'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);
if(!emcc)throw Error('Missing Emscripten');
let source=readFileSync(resolve(sdl,'Application.cpp'),'utf8').replace('#include "MultiplayerRollback.inc"',()=>readFileSync(resolve(sdl,'MultiplayerRollback.inc'),'utf8'));
const initialStageOwner='opts.stage=stage_override?stage_override:setup.initial_stage();';
if(!source.includes(initialStageOwner))throw Error('Missing native initial-stage owner');
source=source.replace(initialStageOwner,`opts.stage=stage_override?stage_override:(netplay.Spectator()?setup.initial_stage():${initialStage});`);
const trace='EM_ASM({Module.onSpectatorLabFrame?.($0,$1);},session.multiplayer_frame,session.multiplayer_hash());';
for(const needle of ['if(!netplay.MarkSimulated(frame,decision)){error=netplay.Error();return false;}',
 'if(!netplay.MarkSimulated(frame,decision))return rollback_fail(netplay.Error());',
 'multiplayer_spectator_capture_bytes=u32(candidate.size());']){
 if(!source.includes(needle))throw Error('Missing private trace anchor '+needle);
 source=source.replace(needle,()=>needle+'\n'+trace);
}
// Keep the controlled stage fixture alive, on sources and reconstructed ticks.
// These changes exist only in this private TU and never enter a packaged WASM.
source=source.replace('bool multiplayer_spectator_playback_tick(bool forward_due){',()=>
 'void spectator_lab_keep_alive(){if(!session.battle)return;for(unsigned i=0;i<session.multiplayer_options.seat_count;++i){auto* p=session.battle->pilots[i].get();if(p&&p->player)p->player->state.invincibility.set(120,&session.animations.rate);}}\n    bool multiplayer_spectator_playback_tick(bool forward_due){');
source=source.replaceAll('session.update_multiplayer(input)', '(spectator_lab_keep_alive(),session.update_multiplayer(input))')
 .replaceAll('session.update_multiplayer(inputs)', '(spectator_lab_keep_alive(),session.update_multiplayer(inputs))');
source=source.replace('const auto before=session.state.phase;const auto stage=session.state.stage;', 'spectator_lab_keep_alive();const auto before=session.state.phase;const auto stage=session.state.stage;');
const viewerTrace='if(!session.draw(renderer,&frame_text))return false;graphics.present();multiplayer_advanced=true;';
if(!source.includes(viewerTrace))throw Error('Missing private viewer frame hook');
source=source.replace(viewerTrace,()=>viewerTrace+'\n'+trace);
source=source.replace('multiplayer_spectator_capture_bytes=u32(candidate.size());',()=>
 'multiplayer_spectator_capture_bytes=u32(candidate.size());\nEM_ASM({Module.onSpectatorLabCheckpoint?.($0,$1);},session.multiplayer_frame,session.multiplayer_hash());');
source=source.replace(/#include "([^"]+)"/g,(_,p)=>'#include "'+resolve(sdl,p).replaceAll('\\','/')+'"');
source+=`\nextern "C" EMSCRIPTEN_KEEPALIVE int th11_spectator_lab_next(){auto& a=th11::sdl::app;if(!a.session.battle||a.session.state.stage!=${initialStage})return 0;a.session.battle->completion.state.next_stage=${initialStage+1};a.session.battle->completion.state.exit=th11::StageExit::NextStage;return 1;}\n`;
// Project exactly the existing semantic hash inputs, including its portable
// ECL cursors. This diagnostic emits values, never native object bytes.
const nativeHash=readFileSync(resolve(root,'web/cpp/game/MultiplayerBattle.cpp'),'utf8');
let words=nativeHash.slice(nativeHash.indexOf('u32 GameBattle::mp_hash()const noexcept{'),nativeHash.indexOf('return hash;',nativeHash.indexOf('u32 GameBattle::mp_hash()const noexcept{')));
words=words.slice(words.indexOf('auto flt='));
const names=['mp_options','pilots','mp_tick','mp_fragments','mp_wipe_frames','mp_rank','economy','frame','resources','animations','spell_flags','spell_id','spell_bonus','spell_elapsed','enemy_environment','transitioning','transition_started','stage_active','transition_timer','stage','enemies','mp_damage_remainders','bullets','items','lasers'];
words=words.replace(new RegExp('(?<![.\\w])('+names.join('|')+')\\b','g'),'battle_owner.$1');
source+='\nextern "C" EMSCRIPTEN_KEEPALIVE const unsigned* th11_spectator_lab_words(){using namespace th11;auto& a=th11::sdl::app;static std::vector<u32> out;out={0};if(!a.session.battle)return out.data();auto& battle_owner=*a.session.battle;auto word=[&](u32 v){out.push_back(v);};'+words+'word(a.session.multiplayer_frame);word(u32(a.session.state.phase));out[0]=u32(out.size()-1);return out.data();}\n';
// Replay compares gameplay owners. Native stage jumps reconstruct background
// fades and start a fresh RNG call counter; neither controls future inputs.
let replayWords=words.replace('word(battle_owner.animations.script_rng.calls);','')
 .replace('word(battle_owner.transitioning);word(battle_owner.transition_started);word(battle_owner.stage_active);timer(battle_owner.transition_timer);','')
 .replace(/if\(battle_owner.stage\)\{[^\n]*\}/,'');
source+='\nextern "C" EMSCRIPTEN_KEEPALIVE unsigned th11_spectator_lab_replay_hash(){using namespace th11;auto& a=th11::sdl::app;if(!a.session.battle)return 0;auto& battle_owner=*a.session.battle;u32 hash=2166136261u;auto word=[&](u32 v){for(unsigned i=0;i<4;++i){hash^=(v>>(8*i))&255;hash*=16777619u;}};'+replayWords+'return hash;}\n';
const fixture=resolve(out,'Application.cpp');writeFileSync(fixture,source);
const objects=plan.sources.filter(p=>p!=='web/cpp/sdl/Application.cpp').map(p=>resolve(plan.outputDirectory,'objects',p.replaceAll('\\','_').replaceAll('/','_').replaceAll(':','_').replaceAll('.','_')+'.o'));
for(const file of objects)if(!existsSync(file))throw Error('Missing production object '+file);
const flags=[...plan.flags,...plan.linkFlags,'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0','-sEXPORTED_FUNCTIONS=_malloc,_free',...objects,fixture,'-o',resolve(out,'th11-sdl.mjs')];
const rsp=resolve(out,'link.rsp.utf-8');writeFileSync(rsp,responseFileContents(flags));
const result=spawnSync('python',[emcc,'@'+rsp],{cwd:root,env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten')},windowsHide:true,encoding:'utf8'});
if(result.status||result.error)throw result.error||Error(result.stdout+result.stderr);
writeFileSync(resolve(out,'build.json'),JSON.stringify({...build,privateSpectatorFixture:true,privateSpectatorInitialStage:initialStage,privateSpectatorKeepAlive:true,privateSpectatorMirroredTransition:false,privateSpectatorSourceOnlyTransition:true,productionWasm:build.sha256,sha256:sha(readFileSync(resolve(out,'th11-sdl.wasm'))),loaderSha256:sha(readFileSync(resolve(out,'th11-sdl.mjs')))},null,2)+'\n');
console.log('PASS private TH11 spectator fixture '+out);
