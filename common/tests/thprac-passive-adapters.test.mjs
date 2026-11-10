
import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
const repo=resolve(import.meta.dirname,'../..'),out=resolve(repo,'common/artifacts/thprac-passive-adapters');
// Compile the actual platform routing functions. The seam replaces only SDL,
// UI hit-testing and final pointer sinks; gesture ownership uses the real header.
function fixture(name,body,entry){return `namespace ${name}{
struct Event{int type,id;float x,y;};std::vector<Event> game,ui;
bool popup=false;
struct State{int context=1;};State touch_state(){return {};}
void sync_touch_context(State){} unsigned SDL_GetTicks(){return 100;}
struct Sink{void pointer(int t,int id,float x,float y,unsigned,State,bool){game.push_back({t,id,x,y});}} gestures;${entry==='pointer'?'Sink touch;':''}
struct Practice{bool enabled=false,flip_screen_y=false;};
struct App{Practice practice;};App app_value;App* app=&app_value;
struct Motion{void touch_event(int,int,int,float,float){}};
struct Runtime{Motion motion;int keys[256]{};int* keyboard_state(){return keys;}} runtime_value;Runtime* runtime=&runtime_value;
int touch_stage(){return 1;}
struct World{struct{struct{int stage=1;} game;} state;Motion motion;} world_value;World* touch_world(){return &world_value;}
struct Input{struct{int virtual_keys[256]{};}snapshot;};Input input_value;
struct Session{App* state=&app_value;App* app=&app_value;Input* input=&input_value;} session_value;Session* session=&session_value;
eagler::thprac::PracticePointer practice_pointers;
namespace ThpracUi{
bool captures_pointer(float x,float y){return popup||(x<200&&y<100)||(x>=400&&y>=200);}
bool passive_pointer(float x,float y){return !popup&&x>=400&&y>=200;}
void mouse(int t,float x,float y){ui.push_back({t,0,x,y});}void cancel_pointer(){}
}
namespace browser{namespace ThpracUi=::${name}::ThpracUi;}namespace th15::sdl{}
${body}
void send(unsigned t,int id,float x,float y){${entry}(t,id,x,y);}
void reset(){practice_pointers.reset();game.clear();ui.clear();popup=false;}
void check(){
 reset();send(0,1,.8f,.6f);assert(game.empty()&&ui.empty());send(1,1,.801f,.601f);assert(game.empty());
 send(1,1,.85f,.65f);assert(game.size()==2&&game[0].type==0&&game[1].type==1);assert(std::abs(game[0].x-.8f)<.00001f&&std::abs(game[0].y-.6f)<.00001f);send(2,1,.9f,.7f);assert(game.size()==3&&ui.empty());
 reset();send(0,1,.8f,.6f);send(2,1,.801f,.601f);assert(game.empty()&&ui.size()==2&&ui[0].type==1&&ui[1].type==2);
 reset();send(0,1,.2f,.8f);send(1,1,.8f,.6f);send(2,1,.85f,.65f);assert(game.size()==3&&ui.empty());
 reset();send(0,1,.05f,.05f);send(1,1,.9f,.9f);send(2,1,.95f,.95f);assert(game.empty()&&ui.size()==3);
 reset();popup=true;send(0,1,.8f,.6f);send(1,1,.9f,.8f);send(2,1,.9f,.8f);assert(game.empty()&&ui.size()==3);
 reset();send(0,1,.8f,.6f);send(0,2,.2f,.8f);send(1,2,.8f,.6f);send(2,1,.8f,.6f);send(2,2,.85f,.65f);assert(game.size()==3&&game[0].id==2&&game[2].id==2&&ui.size()==2);
 reset();send(0,1,.8f,.6f);send(0,1,.2f,.8f);send(1,1,.8f,.6f);assert(game.size()==2&&ui.empty());
 std::puts("PASS ${name}: passive drag/tap, original down, crossing, widgets, popup, second finger, replacement down");
}
}
`;}
test('all normal and multiplayer SDL adapters restore passive tracker drags',{timeout:120000},()=>{
 mkdirSync(out,{recursive:true});let source='#include <eagler/thprac/PracticePointer.hpp>\n#include <vector>\n#include <cmath>\n#include <cassert>\n#include <cstdio>\n#define TH_ENABLE_THPRAC 1\n#define EMSCRIPTEN_KEEPALIVE\n';const names=[];
 for(const game of ['th08','th08-mp','th10','th10-mp','th11','th11-mp','th15','th15-mp']){
  const type=game.slice(0,4),file=type==='th11'?'Application.cpp':type==='th15'?'BrowserRuntime.cpp':'GameHost.cpp';
  const text=readFileSync(resolve(repo,'ports',game,'web/cpp/sdl',file),'utf8');
  const startText=type==='th08'?'void pointer(int type':type==='th10'?'void touch(int type':type==='th11'?'void touch_event(unsigned type':'EMSCRIPTEN_KEEPALIVE void th15_touch(unsigned type';
  const a=text.indexOf(startText);assert.ok(a>=0,game);const open=text.indexOf('{',a);let level=1,b=open+1;while(level&&b<text.length){if(text[b]==='{')level++;else if(text[b]==='}')level--;b++;}assert.equal(level,0,game);
  const name='route_'+game.replace('-','_');names.push(name);source+=fixture(name,text.slice(a,b),type==='th08'?'pointer':type==='th10'?'touch':type==='th11'?'touch_event':'th15_touch');
 }
 source+='int main(){'+names.map(n=>n+'::check();').join('')+'}\n';
 writeFileSync(resolve(out,'adapters.cpp'),source);writeFileSync(resolve(out,'CMakeLists.txt'),`cmake_minimum_required(VERSION 3.20)\nproject(passive_adapters LANGUAGES CXX)\nadd_executable(adapters adapters.cpp)\ntarget_compile_features(adapters PRIVATE cxx_std_17)\ntarget_include_directories(adapters PRIVATE "${resolve(repo,'common/include').replaceAll('\\','/')}")\nif(MSVC)\ntarget_compile_options(adapters PRIVATE /UNDEBUG)\nelse()\ntarget_compile_options(adapters PRIVATE -UNDEBUG)\nendif()\n`);
 const run=(cmd,args)=>execFileSync(cmd,args,{cwd:repo,windowsHide:true,encoding:'utf8',maxBuffer:2*1024*1024});run('cmake',['-S',out,'-B',resolve(out,'build')]);run('cmake',['--build',resolve(out,'build'),'--config','Release','-j','2']);
 console.log(run(resolve(out,'build',process.platform==='win32'?'Release/adapters.exe':'adapters'),[]).trim());
});
